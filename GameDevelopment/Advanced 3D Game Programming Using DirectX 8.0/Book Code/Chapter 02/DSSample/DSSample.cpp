/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: DSSample.cpp
 *    Desc: DirectSound Sample app
 *          
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include <vector>
#include <string>
using namespace std;

class cDSSampleApp : public cApplication, public iKeyboardReceiver
{
	vector< cSound* > m_sounds[6];
	string m_names[6];

	int m_states[6]; // states of the keys 1-6

public:

	void PlaySound( int num );

	//==========--------------------------  cApplication

	virtual void DoFrame( float timeDelta );
	virtual void SceneInit();

	cDSSampleApp() :
		cApplication()
	{
		m_title = string( "DirectSound Sample" );
		m_width = 320;
		m_height = 200;

		for( int i=0; i<6; i++ ) m_states[i] = 0;
	}

	~cDSSampleApp()
	{
		for( int i=0; i<6; i++ )
		{
			for( int i2=0; i2< m_sounds[i].size(); i2++ )
			{
				delete m_sounds[i][i2];
			}
		}
	}

	virtual void KeyUp( int key );
	virtual void KeyDown( int key );

};


cApplication* CreateApplication()
{
	return new cDSSampleApp();
}


void DestroyApplication( cApplication* pApp )
{
	delete pApp;
}


void cDSSampleApp::SceneInit()
{
	m_names[0] = string("media\\keg.wav");
	m_names[1] = string("media\\crash1.wav");
	m_names[2] = string("media\\crash2.wav");
	m_names[3] = string("media\\bass.wav");
	m_names[4] = string("media\\snare.wav");
	m_names[5] = string("media\\hihat.wav");

	Input()->GetKeyboard()->SetReceiver( this );

	for( int i=0; i<6; i++ )
	{
		m_sounds[i].push_back( new cSound( (char*)m_names[i].c_str() ) );
	}
}


void cDSSampleApp::PlaySound( int num )
{
	/**
	 * iterate through the vector, looking
	 * for a sound that isn't currently playing.
	 */
	vector<cSound*>::iterator iter;
	for( iter = m_sounds[num].begin(); iter != m_sounds[num].end(); iter++ )
	{
		if( !(*iter)->IsPlaying() )
		{
			(*iter)->Play();
			return;
		}
	}

	/**
	 * A sound wasn't found.  Create a new one.
	 */
	DP("spawning a new sound\n");

	cSound* pNew = new cSound( *m_sounds[num][0] );
	m_sounds[num].push_back( pNew );
	m_sounds[num][ m_sounds[num].size() - 1 ]->Play();
}


void cDSSampleApp::DoFrame( float timeDelta )
{
	// Clear the previous contents of the backbuffer
	Graphics()->GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
													D3DCOLOR_XRGB( 0,0,200), 1.0f, 0 );
	
	// Set up the strings
	string help;
	help += "DirectSound Sample application\n";
	help += "Vocal Percussion with Adrian Perez\n";
	help += "  [1]: Keg drum\n";
	help += "  [2]: Crash 1\n";
	help += "  [3]: Crash 2\n";
	help += "  [4]: Bass drum\n";
	help += "  [5]: Snare drum\n";
	help += "  [6]: Hi-Hat\n";

	// Tell Direct3D we are about to start rendering
	Graphics()->GetDevice()->BeginScene();

	// Output the text
	Graphics()->DrawTextString( 1, 1, D3DCOLOR_XRGB( 0, 255, 0), help.c_str() );

	// Tell Direct3D we are done rendering
	Graphics()->GetDevice()->EndScene();

	// Present the back buffer to the primary surface
	Graphics()->Flip();
}


void cDSSampleApp::KeyDown( int key )
{
	switch( key )
	{
	case DIK_1:
		if( !m_states[0] )
		{
			m_states[0] = 1;
			PlaySound(0);
		}
		break;
	case DIK_2:
		if( !m_states[1] )
		{
			m_states[1] = 1;
			PlaySound(1);
		}
		break;
	case DIK_3:
		if( !m_states[2] )
		{
			m_states[2] = 1;
			PlaySound(2);
		}
		break;
	case DIK_4:
		if( !m_states[3] )
		{
			m_states[3] = 1;
			PlaySound(3);
		}
		break;
	case DIK_5:
		if( !m_states[4] )
		{
			m_states[4] = 1;
			PlaySound(4);
		}
		break;
	case DIK_6:
		if( !m_states[5] )
		{
			m_states[5] = 1;
			PlaySound(5);
		}
		break;
	}
}

void cDSSampleApp::KeyUp( int key )
{
	switch( key )
	{
	case DIK_1:
		m_states[0] = 0;
		break;
	case DIK_2:
		m_states[1] = 0;
		break;
	case DIK_3:
		m_states[2] = 0;
		break;
	case DIK_4:
		m_states[3] = 0;
		break;
	case DIK_5:
		m_states[4] = 0;
		break;
	case DIK_6:
		m_states[5] = 0;
		break;
	}
}