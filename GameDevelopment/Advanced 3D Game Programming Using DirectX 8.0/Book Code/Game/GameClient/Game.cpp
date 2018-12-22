/*******************************************************************
 *				 Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	 Title: Game.cpp
 *    Desc: Town square for Mobots Attack!
 *          Runs init code, game loop, etc.
 * copyright (c) 2001 by Peter A Walsh, Adrian Perez, and Dan Royer
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
	
#include "..\NetLib\cDataExtract.h"
#include "..\NetLib\cDataCompress.h"
#include "..\NetLib\cNetError.h"

#include "..\\gamecommon\\netmsgs\\LoginNM.h"
#include "..\\gamecommon\\netmsgs\\EntNM.h"

#include "..\\gamecommon\\viewcone.h"

#include <deque>
using namespace std;

void PlayGameSound( resID id )
{
	cSoundWrapper* pSoundWrap = (cSoundWrapper*)Resources()->Get( id );
	pSoundWrap->Play();
}

bool IsClient()
{
	return true;
}

objID g_pPlayerID = -1;

objID GetClientID()
{
	return g_pPlayerID;
}

int g_useTextures = 1;

////////////////////////////////////////////////////////////////////////////////

#define MAX_PLAYERS			8	// max players

#define SERVER_LISTENPORT	4000	// the port the server is listening for data on.
#define CLIENT_LISTENPORT	4001	// the port the client is listening for data on.

/**
 * Drawing code is divorced from everything else so that 
 * the server doesn't need to include any of the DirectX
 * headers (makes it smaller and faster)
 */
extern bool DrawCell( cGameCell* pCell );
extern bool DrawEnt( cGameEnt* pEnt );
extern void InitLights();

////////////////////////////////////////////////////////////////////////////////

class cGameApp 
: public cApplication
, public iKeyboardReceiver
, public iMouseReceiver
, public iGameObject
{
	// how long the last frame took (the next one should take about as long)
	float m_timeDelta;

	// the client's player
	objID			m_clientID;
	cGamePlayerEnt*	m_pPlayer;

	// The world
	cGameWorld* m_pWorld;

	deque<string> m_console;

	bool		m_bConnected; // We are actually able to play.

	int			m_numPlayers;
	string		m_IPAddress,	// If server, what address do we connect to?
				m_localPlayerName;	// The player's name, as specified on the command line.
	bool		m_bInverseMouse;	// Invert the mouse for those player who prefer it?
	MTUDP		m_comm;	// the network comm class
	HOSTHANDLE	m_serverHost;	// the handle to the server host.

	sEntState	m_desiredState;
	bool		m_bInputTaken;

	float		m_deltaYaw, m_deltaPitch;
	point3		m_deltaLoc;

	bool		m_bDrawScoreboard;

	virtual void InitInput()
	{
		// We need to grab exclusive access to the input devices.  This is 
		// for two reasons... one, when we're windowed we want to avoid 
		// having out-of-window clicks cause a loss of focus.  Second, 
		// under Win2K, there is a problem with DirectInput that causes
		// 'sticky' input devices if they're not grabbed exclusively.  
		// Two birds with one stone.
		cInputLayer::Create( 
			AppInstance(), MainWindow()->GetHWnd(), 
			true, true, true );
	}

public:
	virtual void InitExtraSubsystems()
	{
		new cResourceMgr("media\\level1.res");
		MsgDaemon()->RegObject( g_gameID, this );
	}

	void InitClient();

	void ClientFrame( float timeDelta );
	void RenderFrame( float timeDelta );

	void DrawConsole();
	void AddTextLine( const char* text );

	void DrawScoreBoard();

	void HandleInput();

	//==========--------------------------	cApplication

	virtual void Run();
	virtual void DoFrame( float timeDelta );
	virtual void SceneInit();
	virtual void SceneEnd();

	cGameApp() 
	: cApplication()
	, m_console( 10, string("") )
	{
		m_pWorld = NULL;
		m_title = string( "Mobots Attack!!" );
		m_pPlayer = NULL;

		m_numPlayers = 0;
		m_bInverseMouse = false;
		m_bDrawScoreboard = false;
	}


	~cGameApp()
	{
		delete m_pWorld;
		delete Resources();
	}

	//==========--------------------------	iKeyboardReceiver

	virtual void KeyUp( int key );
	virtual void KeyDown( int key );

	//==========--------------------------	iMouseReceiver

	virtual void MouseMoved( int dx, int dy );
	virtual void MouseButtonUp( int button );
	virtual void MouseButtonDown( int button );

	//==========--------------------------	iGameObject
	virtual objID GetID(){	return g_gameID; }
	virtual void SetID( objID id ){}
	virtual uint ProcMsg( const sMsg& msg );

	//==========--------------------------	

	void ParseCmdLine( char* cmdLine );

	//==========--------------------------	Network message parsing
	
	void MakeFireRequest();

	void SendToServerR( cNetMessage& nMsg );
	void SendToServerUR( cNetMessage& nMsg );
};
////////////////////////////////////////////////////////////////////////////////

cApplication* CreateApplication()
{
	return new cGameApp();
}

void cGameApp::SceneInit()
{
	LogPrint("Enter cGameApp::SceneInit()");
	Graphics()->SetProjectionData( PI/3, 0.1f, 40.0f );
	Graphics()->MakeProjectionMatrix();

	Input()->GetKeyboard()->SetReceiver( this );
	if( Input()->GetMouse() )
		Input()->GetMouse()->SetReceiver( this );

	// initialize our scene
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();
	pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_DITHERENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	pDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	pDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	pDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	
	// Load the default world.
	cFile file;
	file.Open( "media\\level1.world" );
	m_pWorld = new cGameWorld( file );
	file.Close();

	Graphics()->SetWorldMatrix( matrix4::Identity );

	m_timeDelta = 0.03f; // a 30th of a second is a good first value

	InitClient();

	AddTextLine("[SceneInit]: Finished initializing\n");
}

void cGameApp::SceneEnd()
{
	/**
	 * Notify the server that we're outta here.
	 */
	if( m_pPlayer )
	{
		cNM_LogoutNotice msg( m_pPlayer->GetID() );
		SendToServerR( msg );
	}
}


void cGameApp::DoFrame( float timeDelta )
{
	// update the time
	m_timeDelta = timeDelta;

	// Our network thread has failed, abort now.
	if( m_comm.cThread::IsRunning() == false )
		throw cNetError( m_comm.cThread::Error() );

	ClientFrame( timeDelta );
	RenderFrame( timeDelta );

	m_comm.ResendPackets();


}



void cGameApp::KeyUp( int key )
{
	switch( key )
	{
	case DIK_TAB:
		m_bDrawScoreboard = false;
	}
}


void cGameApp::KeyDown( int key )
{
	if( !m_bConnected )
		return;

	m_pPlayer->RebuildMatrix();
	const matrix4 mat = m_pPlayer->GetMatrix();

	switch( key )
	{
	case DIK_GRAVE: // Clear the console
		{
			for( int i=0; i<10; i++ )
			{
				AddTextLine( "" );
			}
		}
		break;
		// fire rocket
	case DIK_LCONTROL:
		MakeFireRequest();
		break;

		// tilt up
	case DIK_PRIOR:
	case DIK_NUMPAD9:
		{
			m_deltaPitch += m_timeDelta * 1.8f;
			break;
		}
		
		// tilt down
	case DIK_NEXT:
	case DIK_NUMPAD3:
		{
			m_deltaPitch -= m_timeDelta * 1.8f;
			break;
		}

		// move forward
	case DIK_UP:
	case DIK_NUMPAD8:
		{
			point3 zAxis = mat.GetAxis(2);
			m_deltaLoc += (zAxis * 10.0f);
			break;
		}

		// move backward
	case DIK_DOWN:
	case DIK_NUMPAD2:
	case DIK_E:
		{
			point3 zAxis = mat.GetAxis(2);
			m_deltaLoc -= 10.0f * zAxis;
			break;
		}

		// strafe right
	case DIK_F:
		{
			point3 xAxis = mat.GetAxis(0);
			m_deltaLoc += 10.0f * xAxis;
			break;
		}

		// strafe left
	case DIK_S:
		{
			point3 xAxis = mat.GetAxis(0);
			m_deltaLoc -= 10.0f * xAxis;
			break;
		}

		// move up
	case DIK_SPACE:
		{
			point3 yAxis = mat.GetAxis(1);
			m_deltaLoc += 10.0f * yAxis;
			break;
		}

		// move down
	case DIK_D:
		{
			point3 yAxis = mat.GetAxis(1);
			m_deltaLoc -= 10.0f * yAxis;
			break;
		}

		// turn right
	case DIK_RIGHT:
	case DIK_NUMPAD6:
		{
			m_deltaYaw += m_timeDelta * 1.8f;
			break;
		}

		// turn left
	case DIK_LEFT:
	case DIK_NUMPAD4:
		{
			m_deltaYaw -= m_timeDelta * 1.8f;
			break;
		}

		// turn on scoreboard
	case DIK_TAB:
		m_bDrawScoreboard = true;
	}
}


void cGameApp::MouseMoved( int dx, int dy )
{
	if( !m_bConnected )
		return;

	m_deltaYaw += 0.01f * (float)dx;
	m_deltaPitch += 0.01f * (float)dy * ( m_bInverseMouse == true ? -1 : 1 );
	
}

void cGameApp::MouseButtonUp( int button )
{
	if( !m_bConnected )
		return;

	// If we're dead, try to respawn.
	if( m_pPlayer->GetStatus() == gesDead ||
		m_pPlayer->GetStatus() == gesUnSpawned )
	{
		cNM_SpawnReq msg( m_pPlayer->GetID() );
		SendToServerR( msg );
	}
}

void cGameApp::MouseButtonDown( int button )
{
	if( !m_bConnected )
		return;

	if( button == 0 )
		MakeFireRequest();

	if( button == 1 )
	{
		m_bInputTaken = true;
		matrix4 mat = m_desiredState.BuildMatrix();
	
		point3 zAxis = mat.GetAxis(2);

		m_deltaLoc += 10.0f * zAxis;
	}
}


void cGameApp::Run()
{
	bool done = false;

	static float lastTime = (float)timeGetTime(); 

	while( !done )
	{
		if( Input()->GetKeyboard()->Poll( DIK_ESCAPE ) ||
			Input()->GetKeyboard()->Poll( DIK_Q ) )
		{
			PostMessage( MainWindow()->GetHWnd(), WM_CLOSE, 0, 0 );
		}

		while( !done && MainWindow()->HasMessages() )
		{
			if( resFalse == MainWindow()->Pump() )
				done = true;
		}

		float currTime = (float)timeGetTime();
		float delta = (currTime - lastTime)/1000.f;

		if( m_bActive )
		{
			DoFrame( delta );
		}
		else
		{
			DoIdleFrame( delta );
		}

		lastTime = currTime;
	}
}



void cGameApp::InitClient()
{

	/**
	 * Initially, we are not connected to the game
	 */
	m_bConnected = false;

	
	/* Player name is given in the command line, or read from the init file, 
	 * so we should have it.
	 * do any housekeeping, set up our sockets, and request a connection with the server.
	 * print some sort of ghetto "connecting" message on the screen.
	 */
	m_comm.Startup( CLIENT_LISTENPORT, SERVER_LISTENPORT );
	m_comm.StartClient();

	/**
	 * Client side I open a listening socket first so that I will not
	 * be able to send any data without being ready to read data.
	 * Internally to MTUDP it makes no difference.
	 */
	m_comm.StartListening();
	m_comm.StartSending();

	/**
	 * Open a connection to the server
	 */
	m_serverHost = m_comm.HostCreate( (char*)m_IPAddress.c_str(), SERVER_LISTENPORT );

	/**
	 * Attempt to log the player in.
	 */
	cNM_LoginRequest req( m_localPlayerName );
	SendToServerR( req );

	DebPrint("[InitClient]: Finished initializing");
}


void cGameApp::ClientFrame( float timeDelta )
{
	/**
	 * Any messages from the server?	if so process them.
	 */
	char			data[ MAX_UDPBUFFERSIZE ];
	HOSTHANDLE		srcHostID;
	cDataExtract	extract;
	unsigned short	amnt;

	/**
	 * Reliable messages new include logins, new disconnects,
	 * votes, chat messages, name/model changes,
	 * Chat messages, votes, name changes, spawns, deaths...
	 * We're not supporting all those options, but feel free to add them.
	 */
	while( ( amnt = m_comm.GetReliableData( data, MAX_UDPBUFFERSIZE, &srcHostID ) ) != 0 )
	{
		extract.SetBuffer( data, amnt * 8 );

		cNetMessage* pMsg = cNetMessageMaker::ConstructMessage( 
			(uchar*)extract.GetBuffer(), 
			extract.GetBufferLengthInBytes() );

		pMsg->SetFrom( srcHostID );

		pMsg->Exec();

		delete pMsg;

	}
	/**
	 * Unreliable messages are pretty much updates and that's it.
	 */
	while( ( amnt = m_comm.GetUnreliableData( data, MAX_UDPBUFFERSIZE, &srcHostID ) ) != 0 )
	{
		extract.SetBuffer( data, amnt * 8 );

		cNetMessage* pMsg = cNetMessageMaker::ConstructMessage( 
			(uchar*)extract.GetBuffer(), 
			extract.GetBufferLengthInBytes() );

		pMsg->SetFrom( srcHostID );

		pMsg->Exec();

		delete pMsg;
	}

	/**
	 * In here is where we should get input.
	 */
	HandleInput();

	/**
	 * Send our movement for this frame out on the wire.
	 */
	if( 
		m_pPlayer && 
		m_pPlayer->GetStatus() == gesAlive && 
		m_bInputTaken )
	{
		// Send the desired state to the server
		cNM_MiniStateChangeRequest msg( 
			m_pPlayer->GetID(), 
			m_desiredState );

		SendToServerUR( msg );

		m_bInputTaken = false;
	}

	/**
	 * Run a mortis check (kill any mortisable objects)
	 */
	m_pWorld->EntIterStart();
	while( !m_pWorld->EntIterDone() )
	{
		MortisCheck( m_pWorld->EntIterCurr() );
		m_pWorld->EntIterNext();
	}


	Sleep( 10 );

	

}

void cGameApp::DrawConsole()
{
	int nLines = m_console.size();
	for( int i=0; i<nLines; i++ )
	{
		Graphics()->DrawTextString( 
			1, 1+16 * i, D3DCOLOR_XRGB( 255, 0, 0), 
			m_console[i].c_str() );
	}
	for( i=0; i<nLines; i++ )
	{
		Graphics()->DrawTextString( 
			0, 16 * i, D3DCOLOR_XRGB( 255, 255, 255), 
			m_console[i].c_str() );
	}
		
}

void cGameApp::RenderFrame( float timeDelta )
{

	// Keep this static to try to keep memory problems at bay.
	static vector< cGameCell* > cellList;
	cellList.reserve( m_pWorld->m_nCells );
	cellList.clear();

	Graphics()->BeginScene();

	if( !m_bConnected )
	{
		Graphics()->Clear( true, true, 0, 1.f );

		DrawConsole();
		Graphics()->DrawTextString( 0, 160, D3DCOLOR_XRGB( 255, 0, 0), "Connecting to server..." );

		Graphics()->Flip();
		Sleep(100);
	}
	else
	{

		if( m_pPlayer->GetStatus() != gesUnSpawned )
		{
			// We can just use the yaw and pitch values.
			m_pPlayer->SetYaw( m_desiredState.m_yaw );
			m_pPlayer->SetPitch( m_desiredState.m_pitch );

			m_pPlayer->RebuildMatrix();
			matrix4 playerMat = m_pPlayer->GetMatrix();

			matrix4 viewMat;
			viewMat.ToInverse( playerMat );
			Graphics()->SetViewMatrix( viewMat );
			Graphics()->Clear( true, true, D3DCOLOR_XRGB( 255, 0, 0 ), 1.0f );
			

			// Generate a frustum for the camera, and use it to do a portal walk.

			// Some info about our rendering...
			char renderInfo[256];

			/**
			 * See if we can find the parent cell of the player
			 */
			sMsg msg( msgFindContainingCell, g_gameWorldID, 0 );
			msg.m_pt = m_pPlayer->GetLoc();
			int parentCell = MsgDaemon()->DeliverMessage( msg );

			if( -1 != parentCell )
			{
				// We found a valid cell.  Use it to do a portal walk.
				matrix4 viewMat = matrix4::Inverse( m_pPlayer->GetMatrix() );
				
				cViewCone cone( 
					PI/3, 
					Graphics()->Width(), 
					Graphics()->Height(), 
					viewMat );

				cGameCell* pCameraCell = (cGameCell*)MsgDaemon()->Get( 
					parentCell 
					);

				pCameraCell->PortalWalk( &cellList, cone );

				sprintf( renderInfo, "%d cells traversed", cellList.size() );
			}
			else
			{
				// We aren't in the bounds of a cell.  Draw
				// everything.
				sprintf( renderInfo, "Portal walk not performed" );
				m_pWorld->TraverseAll( &cellList );
			}

			// Draw all cells in the list
			for_each(
				cellList.begin(),
				cellList.end(),
				ptr_fun(DrawCell) );
			cellList.clear();

			Graphics()->GetDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );
			InitLights();

			// Draw all of the objects
			m_pWorld->EntIterStart();
			while( !m_pWorld->EntIterDone() )
			{
				DrawEnt( m_pWorld->EntIterCurr() );
				m_pWorld->EntIterNext();
			}
			Graphics()->GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
			
			if( m_bDrawScoreboard )
			{
				DrawScoreBoard();
			}
			else
			{
				DrawConsole();
			}

			Graphics()->DrawTextString( 0, 180, D3DCOLOR_XRGB( 0, 0, 255), renderInfo );
		}
		else if( m_pPlayer->GetStatus() == gesUnSpawned )
		{
			Graphics()->Clear( true, true, 0, 1.f );

			DrawConsole();
			Graphics()->DrawTextString( 0, 160, D3DCOLOR_XRGB( 0, 0, 255), "Press Fire key to spawn" );
		}
		else
		{
			Graphics()->Clear( true, true, 0, 1.f );

			DrawConsole();
			Graphics()->DrawTextString( 0, 160, D3DCOLOR_XRGB( 0, 0, 255), "Connected to server..." );
		}
		
		
		
	}

	Graphics()->EndScene();
	Graphics()->Flip();
}


void cGameApp::AddTextLine( const char* text )
{
	// Take the top line off
	m_console.pop_front();

	// Insert the line on the bottom
	m_console.push_back( string( text ) );
}


uint cGameApp::ProcMsg( const sMsg& msg )
{
	cNetMessage* pMsg;

	switch( msg.m_type )
	{

	case msgPrintString:
		assert( msg.m_pData );
		AddTextLine( (const char*)msg.m_pData );
		return 0;

	case msgPlayerCreated:
		{
			int pID = msg.m_i[0];
			m_bConnected = true;
			m_pPlayer = (cGamePlayerEnt*)MsgDaemon()->Get(pID);

			// Don't forget to init the global id
			g_pPlayerID = pID;
		}
		return 0;

	case msgSendToServerR:
		pMsg = (cNetMessage*)msg.m_pData;
		SendToServerR( *pMsg );
		return 0;

	case msgSendToServerUR:
		pMsg = (cNetMessage*)msg.m_pData;
		SendToServerUR( *pMsg );
		return 0;

	case msgGetServerTime:
		return m_comm.GetClock().GetTime();

	default:
		DP0("Bad Message got to cGame\n");
		return -1;
	}
}
////////////////////////////////////////////////////////////////////////////////

void cGameApp::ParseCmdLine( char *cmdLine )
{
	if( cmdLine == NULL )
		return;

	m_localPlayerName = string("Unnamed player");

	// Set up some of our options
	m_bInverseMouse = false;

	// Now scan it for game-related options
	char	pLclCmdLine[ 1024 ], *pTok;

	strcpy( pLclCmdLine, cmdLine );
	pTok = strtok( pLclCmdLine, " " );

	if( !pTok ) return;

	do
	{
		if( strcmp( pTok, "-client" ) == 0 )
		{
			// Grab the sever address
			pTok = strtok( NULL, " " );
			m_IPAddress = string( pTok );

			// Check the IP for correctness.
			if( strchr( m_IPAddress.c_str(), '.' ) == 0 )
				throw cGameError( "[cGameApp::ParseCmdLine]: Invalid IP address specified." );
		}

		else if( strcmp( pTok, "-name" ) == 0 )
		{
			pTok = strtok( NULL, " " );
			m_localPlayerName = string( pTok );
		}

		else if( strcmp( pTok, "-imouse" ) == 0 )
		{
			m_bInverseMouse = true;
		}

		else if( strcmp( pTok, "-noTex" ) == 0 )
		{
			g_useTextures = 0;
		}

	} while( pTok = strtok( NULL, " " ) );


}
////////////////////////////////////////////////////////////////////////////////


// Timeout between weapon shots (in milliseconds)
const DWORD c_weapTimeout = 1500; 

void cGameApp::MakeFireRequest()
{
	// Don't fire if we're not alive
	if( m_pPlayer->GetStatus() != gesAlive )
	{
		return;
	}

	// First attempt should succeed.
	static DWORD lastFire = timeGetTime() - c_weapTimeout;

	DWORD currTime = timeGetTime();

	if( lastFire + c_weapTimeout > currTime )
	{
		// Not enough time has elapsed.
		return;
	}

	// Reset the last fire time, since we're firing now.
	lastFire = currTime;

	// Play the fire sound.
	cSoundWrapper *pSoundWrap = (cSoundWrapper*)Resources()->Get( 202 );
	pSoundWrap->Play();

	cNM_ProjFireRequest msg( m_pPlayer->GetID() );
	SendToServerUR( msg );
	
}

////////////////////////////////////////////////////////////////////////////////

void cGameApp::SendToServerR( cNetMessage& nMsg )
{
	uchar buff[MAX_UDPBUFFERSIZE];
	int len = nMsg.SerializeTo( buff );

	m_comm.ReliableSendTo( 
		(char*)buff,
		len,
		m_serverHost );
}

void cGameApp::SendToServerUR( cNetMessage& nMsg )
{
	uchar buff[MAX_UDPBUFFERSIZE];
	int len = nMsg.SerializeTo( buff );

	m_comm.UnreliableSendTo( 
		(char*)buff,
		len,
		m_serverHost );
}


void cGameApp::HandleInput()
{
	if( m_pPlayer )
		m_desiredState = m_pPlayer->GetState();

	/**
	 * Reset our deltas
	 */
	m_deltaYaw = 0;
	m_deltaPitch = 0;
	m_deltaLoc = point3::Zero;

	m_bInputTaken = false;

	/**
	 * Get input
	 */
	if( Input() )
		Input()->UpdateDevices();

	/**
	 * If we got input, activate the input flag
	 */
	if( m_deltaYaw != 0 ||
		m_deltaPitch != 0 ||
		!(m_deltaLoc == point3::Zero) )
	{
		m_bInputTaken = true;

		Snap( m_deltaPitch, (-PI/2)+EPSILON, (PI/2)-EPSILON );

		m_desiredState.m_yaw += m_deltaYaw;
		m_desiredState.m_pitch += m_deltaPitch;
		m_desiredState.m_vel = m_deltaLoc;
	}

}

bool PlayerCmp( cGamePlayerEnt* pA, cGamePlayerEnt* pB )
{
	// pA preceeds pB if the score is higher.
	if( pA->GetScore() > pB->GetScore() )
		return true;
	return false;
}

void cGameApp::DrawScoreBoard()
{
	// Get the list of players.
	vector< cGamePlayerEnt* > players;
	m_pWorld->ListPlayers( &players );

	// Sort the list
	sort( players.begin(), players.end(), PlayerCmp );

	// Display the list
	int nPlayers = players.size();

	int i;
	for( i=0; i<nPlayers; i++ )
	{
		Graphics()->DrawTextString( 
			21, 21+16 * i, D3DCOLOR_XRGB( 0, 0, 255), 
			players[i]->GetName().c_str() );

		Graphics()->DrawTextString( 
			20, 20+16*i, D3DCOLOR_XRGB( 0, 0, 255), 
			players[i]->GetName().c_str() );

		char buff[20];
		sprintf( buff, "%d", players[i]->GetScore() );

		Graphics()->DrawTextString( 
			201, 21+16 * i, D3DCOLOR_XRGB( 0, 0, 255), 
			buff );
		Graphics()->DrawTextString( 
			200, 20+16 * i, D3DCOLOR_XRGB( 0, 0, 255), 
			buff );
	}
}