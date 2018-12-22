using System;
using System.Collections;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;


namespace SpaceWar {
	/// <summary>
	/// The class that houses DirectPlay code for the application.
	/// </summary>
	public class PlayClass {
		#region Fields and Properties
		private GameClass game = null;

		private  Peer peerObject = null;
		
		private bool isHost = false;
		public bool IsHost { get { return isHost; } }

		private bool inSession = false;
		public bool InSession { get { return inSession; } }

		private ConnectWizard Connect = null;
		private Hashtable playerList = new Hashtable();
		public Hashtable PlayerList {
			get {
				lock (playerList) {
					return playerList;
				}
			}
		}
		#endregion

		#region About Application Guids
		// This guid allows DirectPlay to find other instances of the same game on
		// the network, so it must be unique for every game, and the same for 
		// every instance of that game. You can use the guidgen.exe program to
		// generate a new guid.
		#endregion
		private Guid AppGuid = new Guid(0x147184df, 0x547d, 0x4d9e, 0xb6, 0x3c, 0x9f, 0x5c, 0xe9, 0x6a, 0xbf, 0x77);

		#region Constructor

		public PlayClass(GameClass game) {
			this.game = game;
			this.peerObject = peerObject;

			//Initialize our peer to peer network object
			peerObject = new Peer();

			// Set up our event handlers (We only need events for the ones we care about)
			peerObject.PlayerCreated += new PlayerCreatedEventHandler(game.PlayerCreated);
			peerObject.PlayerDestroyed += new PlayerDestroyedEventHandler(game.PlayerDestroyed);
			peerObject.Receive += new ReceiveEventHandler(game.DataReceived);
			peerObject.SessionTerminated += new SessionTerminatedEventHandler(SessionTerminated);
			
			// use the DirectPlay connection wizard to create our join sessions
			Connect = new ConnectWizard(peerObject, AppGuid, "SpacewarDX9");
			Connect.StartWizard();
			
			inSession = Connect.InSession;

			if (inSession) {
				isHost = Connect.IsHost;
			}

		}
		#endregion //Constructor
			
		#region Network Message functions
		//  These routines handle the communication between the game peers.
		public void SendPlayerUpdate(PlayerUpdate update, ShotUpdate shotUpdate) {
			if (inSession) {
				NetworkPacket packet = new NetworkPacket();
				packet.Write((byte)MessageType.PlayerUpdateID);
				packet.Write(update);

				for (int i = 0; i < Constants.NumShots; i++) {
					packet.Write(shotUpdate.ShotPosition[i]);
					packet.Write(shotUpdate.ShotAge[i]);
				}

				peerObject.SendTo((int)PlayerID.AllPlayers, packet, 0, SendFlags.NoComplete | SendFlags.NoLoopback);
			}
		}

		public void SendGameParamUpdate(GameParamUpdate update) {
			if (inSession) {
				NetworkPacket packet = new NetworkPacket();
				packet.Write((byte)MessageType.GameParamUpdateID);
				packet.Write(update);
				peerObject.SendTo((int)PlayerID.AllPlayers, packet, 0, SendFlags.Guaranteed | SendFlags.NoLoopback);
			}
		}
		
		public void SendGameState(GameStates gameState) {
			if (inSession) {
				byte messageType;
				switch (gameState) {
					case GameStates.Paused: {
						messageType = (byte)MessageType.GamePaused;
						break;
					}
					case GameStates.Running: {
						messageType = (byte)MessageType.GameRunning;
						break;
					}
					default: {
						return;
					}
				}
				NetworkPacket packet = new NetworkPacket();
				packet.Write(messageType);
				peerObject.SendTo((int)PlayerID.AllPlayers, packet, 0, SendFlags.Guaranteed | SendFlags.NoLoopback);
			}
		}

		public void SendScorePointToAll() {
			if (inSession) {
				NetworkPacket packet = new NetworkPacket();
				packet.Write((byte)MessageType.Add1ToScore);
				peerObject.SendTo((int)PlayerID.AllPlayers, packet, 0, SendFlags.Guaranteed | SendFlags.NoLoopback);
			}
		}

		public void SendTwoPointsToPlayer(int player) {
			if (inSession) {
				NetworkPacket packet = new NetworkPacket();
				packet.Write((byte)MessageType.Add2ToScore);
				peerObject.SendTo(player, packet, 0, SendFlags.Guaranteed);
			}
		}

		#endregion // Network message functions

		#region Cleanup
		private void SessionTerminated(object sender, SessionTerminatedEventArgs stea) {
			inSession = false;
			isHost = false;
		}

		#endregion //Cleanup
	}
}