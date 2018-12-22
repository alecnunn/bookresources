using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;
using Microsoft.DirectX.DirectPlay;
using Microsoft.DirectX.Direct3D;
using Direct3D=Microsoft.DirectX.Direct3D;


public delegate void MessageDelegate(byte message); // Delegate for messages arriving via DirectPlay.
public delegate void AudioDelegate(); // Delegate to handle audio playback.
public delegate void PeerCloseCallback();           // This delegate will be called when the session terminated event is fired.

public class GameClass : GraphicsSample {
	private GraphicsFont drawingFont = null;
	private Point destination = new Point(0, 0);
	private InputClass input = null;
	private MouseInput mouseInput = null;

	private bool networkEnabled;
	private PlayClass peer = null;
	private bool remotePlayerActive = false;
	private string statusMessage;
	private float statusMessageTimer;
	private PositionedMesh spaceSphere = null;
	private Ship playerShip = null;
	private Ship opponentShip = null;
	private HullColors hullColor = HullColors.White;
	private Camera camera = null;
	private CameraMode cameraMode = CameraMode.ChaseMode;
	private Matrix viewMatrix;
	private Vector2 screenCenter;
	private Vector2 mouseLoc;
	private SoundHandler soundHandler = null;
	private bool kbThrust = false;
	private bool cPressed = false;
	private bool f5Pressed = false;
	private bool f6Pressed = false;
	private bool f7Pressed = false;
	private bool f8Pressed = false;
	private bool f9Pressed = false;
	private bool f10Pressed = false;
	private bool spacePressed = false;

	private Texture bgPointerTexture = null;	
	private Texture vectorPanel = null;
	private RenderToSurface rts = null;
	private Surface renderSurface = null;
	private BGPointer bgPointer = null;
	private int range = 0;

	private GameStates gameState;
	public GameStates GameState { get { return gameState; } set { gameState = value; } }

	private string debugText;
	public string DebugText { get { return debugText; } set { debugText = value; } }
	private bool debugging = true;

	public GameClass(bool startFullScreen, Size size, bool enableNetwork) {
		this.startFullscreen = startFullScreen;
		this.Size = size;
		this.networkEnabled = enableNetwork;
		this.Text = "SpaceWar3D";
		statusMessageTimer = Constants.StatusMessageDisplayTime;

		drawingFont = new GraphicsFont( "Verdana", System.Drawing.FontStyle.Regular);

		input = new InputClass(this);
		mouseInput = new MouseInput(this);
		this.MouseMove +=new MouseEventHandler(GameClass_MouseMove);
		this.Cursor = Cursors.NoMove2D;

		camera = new Camera();
		soundHandler = new SoundHandler(this);

		if (networkEnabled) {
			peer = new PlayClass(this);
			if (peer.IsHost) {
				hullColor = HullColors.Red;
			}
		}

	}

	private void ProcessInput() {
		//Get input
		KeyboardState kbState = input.GetInputState();
		if (kbState == null)
			return;
		if (kbState[Key.W] || kbState[Key.Up]) {
			kbThrust = true;
		}
		else {
			kbThrust = false;
		}
		if (kbState[Key.C] && !cPressed) {
			cPressed = true;
			SelectNextCameraMode();
		}
		if (!kbState[Key.C]) {
			cPressed = false;
		}

		if (kbState[Key.F5] && !f5Pressed) {
			f5Pressed = true;
			playerShip.Sounds |= Sounds.Taunt;
		}
		if (!kbState[Key.F5]) {
			f5Pressed = false;
		}
		if (kbState[Key.F6] && !f6Pressed) {
			f6Pressed = true;
			playerShip.Sounds |= Sounds.Dude1;
		}
		if (!kbState[Key.F6]) {
			f6Pressed = false;
		}
		if (kbState[Key.F7] && !f7Pressed) {
			f7Pressed = true;
			playerShip.Sounds |= Sounds.Dude2;
		}
		if (!kbState[Key.F7]) {
			f7Pressed = false;
		}
		if (kbState[Key.F8] && !f8Pressed) {
			f8Pressed = true;
			playerShip.Sounds |= Sounds.Dude3;
		}
		if (!kbState[Key.F8]) {
			f8Pressed = false;
		}
		if (kbState[Key.F9] && !f9Pressed) {
			f9Pressed = true;
			playerShip.Sounds |= Sounds.Dude4;
		}
		if (!kbState[Key.F9]) {
			f9Pressed = false;
		}
		if (kbState[Key.F10] && !f10Pressed) {
			f10Pressed = true;
			playerShip.Sounds |= Sounds.Dude1;
		}
		if (!kbState[Key.F10]) {
			f10Pressed = false;
		}
		if (kbState[Key.Space] && !spacePressed) {
			spacePressed = true;
			playerShip.EnterHyper();
		}
		if (!kbState[Key.Space]) {
			spacePressed = false;
		}
	}

	protected override void FrameMove() {
		statusMessageTimer += elapsedTime;

		int fps = (int) framePerSecond;
		debugText = "FPS:  " + fps.ToString() + "\r\n";
		ProcessInput();
		MouseControlValues v = mouseInput.Values;
		if (v.FireButtonPushed)
			playerShip.Shoot();
			
		float yawAmount = mouseLoc.X - screenCenter.X;
		float pitchAmount = mouseLoc.Y - screenCenter.Y;

		playerShip.YawPitchRoll(yawAmount, pitchAmount, elapsedTime);
		playerShip.SetThrust(v.ThrustButtonPushed | kbThrust, elapsedTime);
		playerShip.UpdatePosition(elapsedTime);
		playerShip.UpdateState(elapsedTime);
		playerShip.TestShip(opponentShip);
		//Send our ship update to the remote player
		SendMyPlayerUpdate();

		// If there is no remote player, fly the o ther ship around in a circle for target practice.
		// Ideally, we would derive an AI controlled ship from the ship class and use it instead.
		if (!remotePlayerActive) {
			opponentShip.SetThrust(true, elapsedTime);
			opponentShip.YawPitchRoll(250,0,elapsedTime);
			opponentShip.TestShip(playerShip);
		}	
		opponentShip.UpdatePosition(elapsedTime);
		opponentShip.UpdateState(elapsedTime);

		/*Here we set up the view matrix and space dome location.  The space dome moves but not rotates with the player
			 * and is alway drawn first, so it looks like it is infinitely distant.
			 * 
			 * In chase mode, the chaseMatrix determines the offset from the ship.  If you want to move our viewpoint
			 * back from the ship more, increase the negative z value.
			 * 
			 * The fixed mode camera sits at the origin and always tracks the player ship.  Very hard to control from 
			 * this viewpoint, but cool to watch.
			 */

		Vector3 spaceSphereLocation = new Vector3(0, 0, 0);
		switch (cameraMode) {
			case CameraMode.ChaseMode: {
				Matrix chaseMatrix = Matrix.Translation(0, 6, -14);
				chaseMatrix *= playerShip.Position.WorldMatrix;
				viewMatrix = Matrix.Invert(chaseMatrix);
				spaceSphereLocation = playerShip.Position.Location;
				break;
			}
			case CameraMode.CockpitMode: {
				viewMatrix = Matrix.Invert(playerShip.Position.WorldMatrix);
				spaceSphereLocation = playerShip.Position.Location;
				break;
			}
			case CameraMode.Fixed: {
				camera.Point(0,0,0, 
					playerShip.Position.XPos,
					playerShip.Position.YPos,
					playerShip.Position.ZPos);
				viewMatrix = camera.ViewMatrix;
				spaceSphereLocation = new Vector3(0, 0, 0);
				break;
			}
		}
		device.Transform.View = viewMatrix;
		spaceSphere.Position.Location = spaceSphereLocation;
			
		//rotate space very slowly for that nice twinkly star effect
		spaceSphere.Position.RotateRel(-.001f * elapsedTime,-0.0001f * elapsedTime, 0);

		//Calculate range and direction to target
		range = (int) Vector3.Length(
			playerShip.Position.Location - opponentShip.Position.Location); 
		bgPointer.Point(playerShip.Position, opponentShip.Position);

		//Play the sounds
		soundHandler.Play(opponentShip.Sounds | playerShip.Sounds);
		opponentShip.Sounds = (Sounds) 0;
		playerShip.Sounds = (Sounds) 0;
	}


	/// <summary>
	/// Called once per frame, the call is the entry point for 3d rendering. This 
	/// function sets up render states, clears the viewport, and renders the scene.
	/// </summary>
	protected override void Render() {
		RenderBGPointer();

		//Clear the backbuffer to a Blue color 
		device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Blue, 1.0f, 0);

		//Set the view matrix
		device.Transform.View = viewMatrix;

		//Begin the scene
		device.BeginScene();
		device.RenderState.ZBufferEnable = false;
		device.RenderState.Lighting = false;
		spaceSphere.Render();
		device.RenderState.Lighting = true;
		device.RenderState.ZBufferEnable = true;

		device.Transform.World = playerShip.Position.WorldMatrix;
		playerShip.Render();
		opponentShip.Render();

		//Render our targeting pointer
		using (Sprite pointerSprite = new Sprite(device)) {
			pointerSprite.Begin(SpriteFlags.AlphaBlend);

			pointerSprite.Draw(bgPointerTexture, new Rectangle(0, 0, 128, 128),
				new Vector3(0, 0, 0), new Vector3(42, this.Height - 250, 0), Color.White);
			pointerSprite.Transform = Matrix.Identity;
			pointerSprite.Draw(vectorPanel,new Rectangle(0,0,193,173),
				new Vector3(0,0,0), new Vector3(10, this.Height- 282, 0), Color.White);

			pointerSprite.End();
		}
		//Output the scores
		drawingFont.DrawText(5, 20, Color.Red, playerShip.HostName.ToString() + ":");
		drawingFont.DrawText(100,20,Color.Red,playerShip.Score.ToString());
		if (remotePlayerActive) {
			drawingFont.DrawText(5,45,Color.White, peer.RemotePlayer.Name + ":");
			drawingFont.DrawText(100,45, Color.White, opponentShip.Score.ToString());
		}

		//Display any status messages
		if (statusMessageTimer < Constants.StatusMessageDisplayTime) {
			drawingFont.DrawText(200, screenCenter.Y, Color.FromArgb(200,220,220,255), statusMessage);
		}

		if (debugging) {
			drawingFont.DrawText(5,5,Color.Yellow,debugText);
		}
		device.EndScene();
	}

	private void RenderBGPointer() {
		Viewport view = new Viewport();
		view.Width = 128;
		view.Height = 128;
		view.MaxZ = 1.0f;

		Matrix currentViewMatrix = device.Transform.View;

		rts.BeginScene(renderSurface, view);
		device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Black, 1.0f, 0);

		if (playerShip.State == ShipState.Normal && 
			opponentShip.State == ShipState.Normal) {
			Matrix pointerViewMatrix = Matrix.Translation(0, 2, -15);
			pointerViewMatrix *= playerShip.Position.WorldMatrix;
			device.Transform.View = Matrix.Invert(pointerViewMatrix);

			bgPointer.Render();
			drawingFont.DrawText(2,2,Color.LimeGreen, "Range: " + range.ToString());
		}

		rts.EndScene(Filter.Linear);
		device.Transform.View = currentViewMatrix;

	}

	/// <summary>
	/// Initialize scene objects.
	/// </summary>
	protected override void InitializeDeviceObjects() {
		drawingFont.InitializeDeviceObjects(device);

		spaceSphere = new PositionedMesh(device, "SpaceSphere.x");

		playerShip = new Ship(device, this, hullColor);
		if (playerShip.HostName == null)
			playerShip.HostName = "Player";
		playerShip.State = ShipState.Normal;

		HullColors opponentHullColor;
		if (hullColor == HullColors.Red)
			opponentHullColor = HullColors.White;
		else
			opponentHullColor = HullColors.Red;

		opponentShip = new Ship(device, this, opponentHullColor);
		if (opponentShip.HostName == null)
			opponentShip.HostName = "Opponent";
		opponentShip.State = ShipState.Normal;

		bgPointer = new BGPointer(device);
		vectorPanel = TextureLoader.FromFile(device, MediaUtilities.FindFile("vectorPanel.png"));

		rts = new RenderToSurface(device, 128, 128, Format.X8R8G8B8, true, DepthFormat.D16);


	}

	/// <summary>
	/// Called when a device needs to be restored.
	/// </summary>
	protected override void RestoreDeviceObjects(System.Object sender, System.EventArgs e) {
		device.RenderState.Ambient = Color.FromArgb(150,150,150);
		device.RenderState.SpecularEnable = true;
		device.Lights[0].Type = LightType.Directional;
		device.Lights[0].Direction = new Vector3(0,-1, -1);
		device.Lights[0].Diffuse = Color.White;
		device.Lights[0].Specular = Color.White;
		device.Lights[0].Enabled = true;
		device.Lights[0].Commit();
		device.RenderState.Lighting = true;

		device.Transform.Projection = Matrix.PerspectiveFovLH((float)Math.PI / 4,
			(presentParams.BackBufferWidth/presentParams.BackBufferHeight),
			1.5f, 20000.0f );

		device.Transform.View = camera.ViewMatrix;

		screenCenter = new Vector2(
			presentParams.BackBufferWidth / 2, 
			presentParams.BackBufferHeight / 2);

		if (bgPointerTexture != null)
			bgPointerTexture.Dispose();
		bgPointerTexture = new Texture(device, 128, 128, 1, Usage.RenderTarget, 
			Format.X8R8G8B8, Pool.Default);
		renderSurface = bgPointerTexture.GetSurfaceLevel(0);

		gameState = GameStates.Running;

	}

	public void RemotePlayerJoined(string playerName) {
		remotePlayerActive = true;
		statusMessage = playerName + " has joined the game.";
		statusMessageTimer = 0;
			
	}

	public void RemotePlayerLeft(string playerName) {
		remotePlayerActive = false;
		statusMessage = playerName + " has left the game.";
		statusMessageTimer = 0;
	}

	public void DataReceived(object sender, ReceiveEventArgs rea) {

		int senderID = rea.Message.SenderID;
	
		//Ignore messages received before we are initialized
		if ((gameState == GameStates.Loading) || (gameState == GameStates.Config)) {
			rea.Message.ReceiveData.Dispose();
			return;
		}

		byte mType = (byte)rea.Message.ReceiveData.Read(typeof(byte));
		MessageType messageType = (MessageType)mType;

		switch (messageType) {
			case MessageType.PlayerUpdateID: {	
				PlayerUpdate update = (PlayerUpdate)rea.Message.ReceiveData.Read(typeof(PlayerUpdate));
				ShotUpdate shotUpdate = new ShotUpdate();
				shotUpdate.ShotPosition = new Vector3[Constants.NumShots];
				shotUpdate.ShotAge = new float[Constants.NumShots];
				shotUpdate.ShotAlive = new bool[Constants.NumShots];

				for (int i = 0; i < Constants.NumShots; i++) {
					shotUpdate.ShotPosition[i] = (Vector3)rea.Message.ReceiveData.Read(typeof(Vector3));
					shotUpdate.ShotAge[i] = (int)rea.Message.ReceiveData.Read(typeof(int));
					shotUpdate.ShotAlive[i] = (bool)rea.Message.ReceiveData.Read(typeof(bool));
				}
					
				rea.Message.ReceiveData.Dispose();
				lock(opponentShip) {
					opponentShip.Position.WorldMatrix = update.WorldMatrix;
					opponentShip.Score		= update.Score;
					opponentShip.Sounds		= (Sounds)update.Sounds;
					opponentShip.WaitCount	= update.WaitCount;
					opponentShip.SetState((ShipState)update.State);

					Photon[] shotArray = opponentShip.ShotHandler.GetShotArray();
					for (int i = 0; i < Constants.NumShots; i++) {
						shotArray[i].Location = shotUpdate.ShotPosition[i]; 
						shotArray[i].Age = shotUpdate.ShotAge[i]; 
						shotArray[i].Alive = shotUpdate.ShotAlive[i]; 
					}
					opponentShip.ShotHandler.SetShotArray(shotArray);
				}

				break;
			}

			case MessageType.Add1ToScore: {
				rea.Message.ReceiveData.Dispose();
				playerShip.Score += 1;
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

	public void SendPoint() {
		if (!remotePlayerActive) {
			playerShip.Score += 1;
			return;
		}

		peer.SendScorePoint();
	}

	public void SendMyPlayerUpdate() {
		if (!networkEnabled)
			return;

		PlayerUpdate update = new PlayerUpdate();
		update.WorldMatrix		= playerShip.Position.WorldMatrix;
		update.State			= (int)playerShip.State;
		update.WaitCount		= playerShip.WaitCount;
		update.DeathCount		= playerShip.DeathCount;
		update.Sounds			= (int)playerShip.Sounds;
		update.Score			= playerShip.Score;

		ShotUpdate shotUpdate = new ShotUpdate();
		shotUpdate.ShotAge = new float[Constants.NumShots];
		shotUpdate.ShotPosition = new Vector3[Constants.NumShots];
		shotUpdate.ShotAlive = new bool[Constants.NumShots];

		Photon[] shotArray = playerShip.ShotHandler.GetShotArray();
		for (int i = 0; i < Constants.NumShots; i++) {
			shotUpdate.ShotPosition[i] = shotArray[i].Location;
			shotUpdate.ShotAge[i] = shotArray[i].Age;
			shotUpdate.ShotAlive[i] = shotArray[i].Alive;
		}
		peer.SendPlayerUpdate(update, shotUpdate);
	}



	/// <summary>
	// When the peer closes, the code here is executed.
	/// </summary>
	public void PeerClose() {
		// The session was terminated, go ahead and shut down
		this.Dispose();
	}

	private void SelectNextCameraMode() {
		int mode = (int) cameraMode;
		mode++;
		if (mode > 2)
			mode = 0;
		cameraMode = (CameraMode) mode;
	}

	private void GameClass_MouseMove(object sender, MouseEventArgs e) {
		mouseLoc.X = e.X;
		mouseLoc.Y = e.Y;
	}
}
