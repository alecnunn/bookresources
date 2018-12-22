using System;
using System.Collections;
using System.Windows.Forms;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;
using Microsoft.DirectX.DirectInput;
using Microsoft.DirectX.DirectPlay;

namespace SpaceWar {

	/// <summary>
	/// The main game functions
	/// </summary>
	public class GameClass {
		#region Fields and Properties

		private Control owner = null;
		private MainClass mainClass = null;

		private GameSettings gameSettings = null;

		private int gravity;
		private int gameSpeed;
		private bool bounceBack;
		private bool inverseGravity;
		private bool blackHole;

		public int Gravity { get { return gravity;} set { gravity = value;} }
		public int GameSpeed { get { return gameSpeed; } set { gameSpeed = value; } }
		public bool BounceBack { get { return bounceBack; } set { bounceBack = value; } }
		public bool InverseGravity { get { return inverseGravity; } set { inverseGravity = value; } }
		public bool BlackHole { get { return blackHole; }  set { blackHole = value; } }

		private Microsoft.DirectX.DirectDraw.Device localDevice = null;

		private float lastFrameTime = 0.0f;

		private Clipper localClipper = null;
		private Surface surfacePrimary = null;
		private Surface surfaceSecondary = null;

		private Stars stars;
		private Sun sun;
		private Point sunLocation = new Point(0,0);
		public Point SunLocation { get { return sunLocation; } }

		private SoundHandler soundHandler;
		private InputClass input;
		private PlayClass netPeer;
		private SplashScreen splash;


		private Rectangle windowBounds;

		private Hashtable	otherPlayers = new Hashtable();
		private Hashtable	wordCache = new Hashtable();

		public Color[] shipColors = new Color[] {Color.Purple, Color.Yellow, Color.Cyan,
													Color.Red, Color.Green, Color.Blue};
		private SpaceWar.Ship ship;

		private LocalPlayer localPlayer;

		private GameStates gameState;
		public GameStates GameState { get { return gameState; } set { gameState = value; } }


		#endregion //Fields and Properties


		#region Constructor and Initialization

		//  The GameClass constructor.  Here we create, but not show, the GameSettings form.
		// The 
		public GameClass(MainClass mainClass, Control owner) {
			gameState = GameStates.Loading;
			this.owner = owner;
			this.mainClass = mainClass;
			splash = new SplashScreen(this);
			splash.ShowDialog();
			gameSettings = new SpaceWar.GameSettings(this);
			gameSettings.Location = new Point(owner.Bounds.Right,owner.Bounds.Top);
			gravity = gameSettings.Gravity;
			gameSpeed = gameSettings.GameSpeed;
			bounceBack = gameSettings.Bounce;
			inverseGravity = gameSettings.InverseGravity;
			blackHole = gameSettings.BlackHole;

			localDevice = new Microsoft.DirectX.DirectDraw.Device();
			localDevice.SetCooperativeLevel(owner, Microsoft.DirectX.DirectDraw.CooperativeLevelFlags.Normal);

			DXUtil.Timer(DirectXTimer.Start);

			SpaceWar.RotatableShape.CreateShapes();

			input = new InputClass(this.owner);

			soundHandler = new SoundHandler(this.owner);

			try {
				netPeer = new PlayClass(this);
			}
			catch(DirectXException e) {
				MessageBox.Show(owner,e.ToString());
			}
		}

		public void Initialize(Rectangle bounds) {
			owner.Bounds = bounds;
			this.GameState = GameStates.Config;
			this.windowBounds = bounds;
			CreateSurfaces();

			sunLocation.X = windowBounds.Left + windowBounds.Width / 2;
			sunLocation.Y = windowBounds.Top + windowBounds.Height / 2;

			ship = new Ship(this);
			Random random = new Random((int) DateTime.Now.Ticks);
			ship.ScreenBounds = bounds;
			ship.SetRandomPosition(true, sunLocation);
			
			if ((null != localPlayer.Name) && (localPlayer.Name.Length > 0))
				ship.HostName = localPlayer.Name.ToUpper();
			else
				ship.HostName = System.Environment.MachineName.ToUpper();

			stars = new Stars(bounds, Constants.NumStars);
			sun = new Sun(sunLocation, Constants.SunSize);
			
			gameSettings.ControlsEnabled = true;
			if (netPeer.InSession) {
				if (netPeer.IsHost) {
					gameSettings.ControlsEnabled = true;
					netPeer.SendGameState(GameStates.Running);
				}
				else {
					gameSettings.ControlsEnabled = false;
				}
			}
			gameState = GameStates.Running;
		}

		private void CreateSurfaces() {
			SurfaceDescription desc = new SurfaceDescription();
			SurfaceCaps caps = new SurfaceCaps(); 

			localClipper = new Clipper(localDevice);
			localClipper.Window = owner;

			desc.SurfaceCaps.PrimarySurface = true;
			if (null != surfacePrimary)
				surfacePrimary.Dispose();
			surfacePrimary = new Surface(desc, localDevice);
			surfacePrimary.Clipper = localClipper;
        
			desc.Clear();
			desc.SurfaceCaps.OffScreenPlain = true;
			desc.Width = surfacePrimary.SurfaceDescription.Width;
			desc.Height = surfacePrimary.SurfaceDescription.Height;
        
			if (null != surfaceSecondary)
				surfaceSecondary.Dispose();
			surfaceSecondary = new Surface(desc, localDevice);            
			surfaceSecondary.FillStyle = 0;
		}

		public Ship CreateNewShip(string hostName) {
			Ship newShip;
			newShip = new Ship(this);
			newShip.ScreenBounds = windowBounds;
			newShip.HostName = hostName;
			return newShip;
		}


		#endregion //Constructor and Initialization


		#region Network play message routines

		public void Pause() {
			if (gameState == GameStates.Running) {
				gameState = GameStates.Paused;
				if (netPeer.IsHost)
					netPeer.SendGameState(GameStates.Paused);
			}
		}

		public void UnPause() {
			if (gameState == GameStates.Paused) {
				gameState = GameStates.Running;
				if (netPeer.IsHost)
					netPeer.SendGameState(GameStates.Running);
			}
		}

		public void PlayerCreated(object sender, PlayerCreatedEventArgs pcea) {
			Peer peer = (Peer) sender;
			int playerID = pcea.Message.PlayerID;
			PlayerInformation playerInfo = peer.GetPeerInformation(playerID);

			// See if the player that was just created is us
			if (playerInfo.Local) {
				localPlayer.ID = playerID;
				localPlayer.Name = playerInfo.Name;
			}
				// If not, create a remote player
			else {		
				Ship newShip = new Ship(this);
				newShip.HostName = playerInfo.Name.ToUpper();
				newShip.State = (int)ShipState.Normal;
				newShip.ScreenBounds = this.windowBounds;
				RemotePlayer newPlayer = new RemotePlayer(playerID, playerInfo.Name, newShip);
				lock (otherPlayers) {
					otherPlayers.Add(playerID, newPlayer);
				}
			}
		}

		public void PlayerDestroyed(object sender, PlayerDestroyedEventArgs pdea) {
			// Remove this player from our list
			// We lock the data here since it is shared across multiple threads.
			int playerID = pdea.Message.PlayerID;
			if (playerID != localPlayer.ID) {
				lock (otherPlayers) {
					otherPlayers.Remove(playerID);
				}
			}
		}

		public void DataReceived(object sender, ReceiveEventArgs rea) {
			int senderID = rea.Message.SenderID;
			
			//Ignore messages received before we are initialized
			if ((gameState == GameStates.Loading) || 
				(gameState == GameStates.Config)) {
				rea.Message.ReceiveData.Dispose();
				return;
			}

			byte mType = (byte)rea.Message.ReceiveData.Read(typeof(byte));
			MessageType messageType = (MessageType)mType;
			switch (messageType) {
				case MessageType.PlayerUpdateID: {	

					PlayerUpdate update = (PlayerUpdate)rea.
						Message.ReceiveData.Read(typeof(PlayerUpdate));
					ShotUpdate shotUpdate = new ShotUpdate();
					shotUpdate.ShotPosition = new Vector2[Constants.NumShots];
					shotUpdate.ShotAge = new int[Constants.NumShots];

					for (int i = 0; i < Constants.NumShots; i++) {
						shotUpdate.ShotPosition[i] = 
						  (Vector2)rea.Message.ReceiveData.Read(typeof(Vector2));
						shotUpdate.ShotAge[i] = 
							(int)rea.Message.ReceiveData.Read(typeof(int));
					}
					
					rea.Message.ReceiveData.Dispose();
					


					lock (otherPlayers) {
						object playerObject = otherPlayers[senderID];
						if (null == playerObject)
							return;
						RemotePlayer player = (RemotePlayer) playerObject;

						Shot[] shotArray = new Shot[Constants.NumShots];
						for (int i = 0; i < Constants.NumShots; i++) {
							shotArray[i] = new Shot();
							shotArray[i].Position = shotUpdate.ShotPosition[i]; 
							shotArray[i].Age = shotUpdate.ShotAge[i]; 
						}
						player.Ship.ShotHandler.SetShotArray(shotArray);

						player.Ship.Position	= update.ShipPosition;
						player.Ship.Outline		= update.Outline;
						player.Ship.Velocity	= update.ShipVelocity;
						player.Ship.State		= update.State;
						player.Ship.WaitCount	= update.WaitCount;
						player.Ship.DeathCount	= update.DeathCount;
						player.Ship.FlameIndex	= update.FlameIndex;
						player.Ship.Sounds		= (Sounds)update.Sounds;
						player.Ship.Score		= update.Score;

						player.UpdateTime		= DateTime.Now;
						player.Active				= true;

						otherPlayers[senderID] = player;
					}

					break;
				}
				case MessageType.GameParamUpdateID: {
					GameParamUpdate update = (GameParamUpdate)rea.Message.
						ReceiveData.Read(typeof(GameParamUpdate));
					rea.Message.ReceiveData.Dispose();
					gravity = update.Gravity;
					gameSpeed = update.GameSpeed;
					
					if (update.BounceBack != 0)
						bounceBack = true;
					else
						bounceBack = false;

					if (update.InverseGravity != 0)
						inverseGravity = true;
					else
						inverseGravity = false;

					if (update.BlackHole != 0)
						blackHole = true;
					else
						blackHole = false;
					Size newWindowSize = update.WindowSize;
					Rectangle newBounds = new 
						Rectangle(this.windowBounds.Location, newWindowSize);
					//Initialize(newBounds);
					break;
				}
				case MessageType.Add1ToScore: {
					rea.Message.ReceiveData.Dispose();
					ship.Score += 1;
					break;
				}
				case MessageType.Add2ToScore: {
					rea.Message.ReceiveData.Dispose();
					ship.Score += 2;
					break;
				}
				case MessageType.GamePaused: {
					rea.Message.ReceiveData.Dispose();
					gameState = GameStates.Paused;
					break;
				}
				case MessageType.GameRunning: {
					rea.Message.ReceiveData.Dispose();
					if (gameState == GameStates.Paused)
						gameState = GameStates.Running;
					break;
				}
			}
		}

		public void SendMyPlayerUpdate() {

			PlayerUpdate update = new PlayerUpdate();
			update.ShipPosition		= ship.Position;
			update.Outline			= ship.Outline;
			update.ShipVelocity		= ship.Velocity;
			update.State			= ship.State;
			update.WaitCount		= ship.WaitCount;
			update.DeathCount		= ship.DeathCount;
			update.FlameIndex		= ship.FlameIndex;
			update.Sounds			= (int)ship.Sounds;
			update.Score			= ship.Score;

			ShotUpdate shotUpdate = new ShotUpdate();
			shotUpdate.ShotAge = new int[Constants.NumShots];
			shotUpdate.ShotPosition = new Vector2[Constants.NumShots];

			Shot[] shotArray = ship.ShotHandler.GetShotArray();
			for (int i = 0; i < Constants.NumShots; i++) {
				shotUpdate.ShotPosition[i] = shotArray[i].Position;
				shotUpdate.ShotAge[i] = shotArray[i].Age;
			}
			netPeer.SendPlayerUpdate(update, shotUpdate);
		}

		public void SendGameParamUpdate() {
			if ((null == netPeer) || (otherPlayers.Count < 1))
				return;
			GameParamUpdate update = new GameParamUpdate();
			update.Gravity = gravity;
			update.GameSpeed = gameSpeed;
			if (bounceBack)
				update.BounceBack = 1;
			else
				update.BounceBack = 0;

			if (inverseGravity)
				update.InverseGravity = 1;
			else
				update.InverseGravity = 0;

			if (blackHole)
				update.BlackHole = 1;
			else
				update.BlackHole = 0;

			update.WindowSize = windowBounds.Size;

			netPeer.SendGameParamUpdate(update);
		}

		public void SendPointToAllPlayers() {
			netPeer.SendScorePointToAll();
		}

		public void SendTwoPointsToPlayer(int player) {
			netPeer.SendTwoPointsToPlayer(player);
		}


		#endregion //Network


		#region Input and Audio
		private void HandleKeys() {
			KeyboardState keyboardState = input.GetKBState();

			
			if (null == keyboardState)
				return;

			if (keyboardState[Key.LeftArrow])
				ship.RotateLeft();
			if (keyboardState[Key.RightArrow])
				ship.RotateRight();
			
			ship.SetThrust(keyboardState[Key.UpArrow]);

			if (keyboardState[Key.LeftControl]  || keyboardState[Key.RightControl])
				ship.Shoot();

			if (keyboardState[Key.Space])
				ship.EnterHyper();

			// Game configuration / Pause key.  The configuration controls are disabled if we are connected to another host.
			if (keyboardState[Key.F2]) {
				Pause();

				if (!netPeer.InSession || netPeer.IsHost)
					gameSettings.ControlsEnabled = true;
				else
					gameSettings.ControlsEnabled = false;

				gameSettings.Show();
			}

			// Sound keys
			if (keyboardState[Key.F5]) {
				ship.Sounds |= Sounds.Taunt;
			}

			if (keyboardState[Key.F6]) {
				ship.Sounds |= Sounds.Dude1;
			}

			if (keyboardState[Key.F7]) {
				ship.Sounds |= Sounds.Dude2;
			}

			if (keyboardState[Key.F8]) {
				ship.Sounds |= Sounds.Dude3;
			}

			if (keyboardState[Key.F9]) {
				ship.Sounds |= Sounds.Dude4;
			}

			if (keyboardState[Key.F10]) {
				ship.Sounds |= Sounds.Dude5;
			}	

			//Exit if escape is pressed
			if (keyboardState[Key.Escape]) {
				End();				
			}
			
		}

		private void PlaySounds(Sounds otherShipSounds) {
			if (soundHandler != null) {
				Sounds soundsToPlay = ship.Sounds;

				if ((otherShipSounds & Sounds.ShipExplode) != 0)
					soundsToPlay |= Sounds.OtherShipExplode;

				if ((otherShipSounds & Sounds.ShipFire) != 0)
					soundsToPlay |= Sounds.OtherShipFire;

				if ((otherShipSounds & Sounds.ShipThrust) != 0)
					soundsToPlay |= Sounds.OtherShipThrust;

				if ((otherShipSounds & Sounds.ShipAppear) != 0)
					soundsToPlay |= Sounds.OtherShipAppear;

				Sounds directMap =
					Sounds.ShipHyper |
					Sounds.Taunt |
					Sounds.Dude1 |
					Sounds.Dude2 |
					Sounds.Dude3 |
					Sounds.Dude4 |
					Sounds.Dude5;

				Sounds soundsToCopy = otherShipSounds & directMap;

				soundsToPlay |= soundsToCopy;

				soundHandler.Play(soundsToPlay);
			}
		}

		#endregion Input and Audio


		#region Main gameloop and rendering
		public void WriteScores() {
			Point location = new Point(Constants.ScoreXOrigin + windowBounds.X, 
				Constants.ScoreYOrigin + windowBounds.Y);
			Point scoreLocation = location;
			scoreLocation.X += Constants.ScoreOffset;

			ship.DrawHostName(surfaceSecondary, location, 0xFFFFFF);	// this ship is white...

			Word word = GetWordFromScore(ship.Score.ToString());
			int color = 0xFFFFFF;
			word.Draw(surfaceSecondary, color, Constants.LetterSpacing, scoreLocation);

			int shipIndex = 0;
			lock (otherPlayers) {
				foreach (RemotePlayer player in otherPlayers.Values) {
					if (!player.Active)
						continue;

					location.Y += Constants.ScoreVerticalSpacing;
					scoreLocation.Y += Constants.ScoreVerticalSpacing;
					player.Ship.DrawHostName(surfaceSecondary, location, shipColors[shipIndex].ToArgb());
					word = GetWordFromScore(player.Ship.Score.ToString());
					word.Draw(surfaceSecondary, shipColors[shipIndex].ToArgb(), Constants.LetterSpacing, scoreLocation);

					shipIndex = (shipIndex + 1) % shipColors.Length;
				}
			}
		}

		Word GetWordFromScore(string score) {
			Word word;

			word = (Word) wordCache[score];
			if (word != null)
				return word;

			word = new Word(score, Constants.LetterSize);
			wordCache.Add(score, word);
			return word;
		}

		public void MainLoop() {
			float minFrameTime = gameSettings.GameSpeed * 0.005f;

			if (lastFrameTime < minFrameTime) {
				lastFrameTime += DXUtil.Timer(DirectXTimer.GetElapsedTime);
				return;
			}
			lastFrameTime = 0.0f;
			try {				
				if (gameState == GameStates.Paused) {
					Word paused = new Word("PAUSED", Constants.LetterSize * 1.5f);
					paused.Draw(surfaceSecondary, Color.White.ToArgb(), 
						Constants.LetterSpacing * 2, 
						new Point(windowBounds.Left + 50, windowBounds.Top + 50));
					surfacePrimary.Draw(surfaceSecondary, DrawFlags.DoNotWait);
				}
				// Clear the ship's sound flags
				ship.Sounds = (Sounds) 0;
				
				// process input
				HandleKeys();

				
				if (gameState != GameStates.Running) return;

				surfaceSecondary.ColorFill(Color.Black);
				surfaceSecondary.DrawWidth = 1;
				
				// update my position, and tell others about it...
				ship.UpdatePosition();

				// update my state, and draw myself...
				ship.UpdateState();

				//If there are other players, send them our ship info
				if (netPeer.InSession  && otherPlayers.Count > 0)
					SendMyPlayerUpdate();

				WriteScores();
				stars.Draw(surfaceSecondary);

				int shipColor = Color.White.ToArgb();
				int shotColor = Color.White.ToArgb();
				ship.Draw(surfaceSecondary, shipColor, shotColor);

				// Handle other ships
				// walk through all other players. For each player
				// 1) draw the ship
				// 2) check to see whether the other ship has killed us
				// 3) figure the score
				// 4) see if we need to time-out this ship
				int shipIndex = 0;
				Sounds otherShipSounds = (Sounds) 0;
				DateTime now = DateTime.Now;
				lock (otherPlayers) {
					foreach (RemotePlayer player in otherPlayers.Values) {
						if (!player.Active)
							continue;

						player.Ship.Draw(surfaceSecondary, shipColors[shipIndex].ToArgb(), shotColor);
						shipIndex = (shipIndex + 1) % shipColors.Length;
						ship.TestShip(player);
						otherShipSounds |= player.Ship.Sounds;

						// if we haven't gotten an update in a while,
						// mark the player as inactive...
						TimeSpan delta = now - player.UpdateTime;
						if (delta.Seconds > Constants.RemoteTickTimeout) {
							player.Active = false;
						}
					}
				}

				// Draw the sun only if the "Black Hole" option isn't enabled
				if (!blackHole)
					sun.Draw(surfaceSecondary);

				surfacePrimary.Draw(surfaceSecondary, DrawFlags.DoNotWait);
				PlaySounds(otherShipSounds);
			}

			catch(SurfaceLostException) {
				// The surface can be lost if power saving
				// mode kicks in, or any other number of reasons.
				CreateSurfaces();
			}
		}

		#endregion


		#region Cleanup

		public void End() {
			gameState = GameStates.Exiting;
			mainClass.EndGame();
		}

		#endregion //cleanup
	}
}