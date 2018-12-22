using System;
using System.Collections;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;



public struct Players
{
	public bool Active;
	public int dpnID;
	public string Name;
	public Players(int id, string n, bool active)
	{ dpnID = id; Name = n; Active = active;}
}       

	/// <summary>
	/// The class that houses DirectPlay code for the application.
	/// </summary>
public class PlayClass
{
	#region Fields and Properties
	private GameClass game = null;

	private  Peer peerObject = null;
		
	private bool isHost = false;
	public bool IsHost { get { return isHost; } }

	private bool inSession = false;
	public bool InSession { get { return inSession; } }

	private int localPlayerID = 0;
		

	private Players remotePlayer;
	public Players RemotePlayer { get {return remotePlayer; } }

	private ConnectWizard Connect = null;

	#endregion

	#region About Application Guids
	// This guid allows DirectPlay to find other instances of the same game on
	// the network, so it must be unique for every game, and the same for 
	// every instance of that game. You can use the guidgen.exe program to
	// generate a new guid.
	#endregion
	private Guid AppGuid = new Guid( 0xc134dcd4, 0xff30, 0x4c86,  0x9a, 0x2e, 0xe0, 0x46, 0xa2, 0x21, 0xe3, 0x86 );
		
	#region Constructor

	public PlayClass(GameClass Game)
	{
		this.game = Game;
			
		//Initialize our peer to peer network object
		peerObject = new Peer();
			

		// Set up our event handlers (We only need events for the ones we care about)
		peerObject.PlayerCreated += new PlayerCreatedEventHandler(PlayerCreated);
		peerObject.PlayerDestroyed += new PlayerDestroyedEventHandler(PlayerDestroyed);
		peerObject.Receive += new ReceiveEventHandler(game.DataReceived);
		peerObject.SessionTerminated += new SessionTerminatedEventHandler(SessionTerminated);
			
		// use the DirectPlay connection wizard to create our join sessions
		Connect = new ConnectWizard(peerObject, AppGuid, "Spacewar3D");
		Connect.StartWizard();
			
		inSession = Connect.InSession;

		if (inSession)
		{
			isHost = Connect.IsHost;
		}

	}
	#endregion //Constructor
			
	#region Network Message functions
	//  These routines handle the communication between the game peers.


	private void PlayerCreated(object sender, PlayerCreatedEventArgs dpMessage)
	{
		// Get the PlayerInformation and store it 
		int playerID = dpMessage.Message.PlayerID;
		PlayerInformation dpPeer = peerObject.GetPeerInformation(playerID);
		Players oPlayer = new Players(playerID, dpPeer.Name.ToUpper(), true);
		// Save this player id if it's ourselves
		if (dpPeer.Local)
			localPlayerID = dpMessage.Message.PlayerID;
		else
		{
			// We lock the data here since it is shared across multiple threads.
			lock (this)
			{
				remotePlayer = oPlayer;
			}
			game.RemotePlayerJoined(oPlayer.Name);
		}


	}
	private void PlayerDestroyed(object sender, PlayerDestroyedEventArgs dpMessage)
	{
		// Remove this player from our list
		// We lock the data here since it is shared across multiple threads.
		lock (this)
		{
			remotePlayer.Active = false;
		}
		game.RemotePlayerLeft(remotePlayer.Name);
	}


	public void SendPlayerUpdate(PlayerUpdate update, ShotUpdate shotUpdate)
	{
		if (inSession)
		{
			NetworkPacket packet = new NetworkPacket();
			packet.Write((byte)MessageType.PlayerUpdateID);
			packet.Write(update);

			for (int i = 0; i < Constants.NumShots; i++)
			{
				packet.Write(shotUpdate.ShotPosition[i]);
				packet.Write(shotUpdate.ShotAge[i]);
				packet.Write(shotUpdate.ShotAlive[i]);
			}

			peerObject.SendTo((int)PlayerID.AllPlayers, packet, 0, SendFlags.NoComplete | SendFlags.NoLoopback);
		}
	}
		
	public void SendGameState(GameStates gameState)
	{
		if (inSession)
		{
			byte messageType;
			switch (gameState)
			{
				case GameStates.Paused:
				{
					messageType = (byte)MessageType.GamePaused;
					break;
				}
				case GameStates.Running:
				{
					messageType = (byte)MessageType.GameRunning;
					break;
				}
				default:
				{
					return;
				}
			}
			NetworkPacket packet = new NetworkPacket();
			packet.Write(messageType);
			peerObject.SendTo((int)PlayerID.AllPlayers, packet, 0, SendFlags.Guaranteed | SendFlags.NoLoopback);
		}
	}

	public void SendScorePoint()
	{
		if (inSession)
		{
			NetworkPacket packet = new NetworkPacket();
			packet.Write((byte)MessageType.Add1ToScore);
			peerObject.SendTo(remotePlayer.dpnID, packet, 0, SendFlags.Guaranteed | SendFlags.NoLoopback);
		}
	}

	#endregion // Network message functions

	#region Cleanup
	private void SessionTerminated(object sender, SessionTerminatedEventArgs stea)
	{
		inSession = false;
		isHost = false;
	}

	#endregion //Cleanup
}
