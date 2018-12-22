using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;
using Microsoft.DirectX.Direct3D;
using Direct3D=Microsoft.DirectX.Direct3D;


public delegate void MessageDelegate(byte message); // Delegate for messages arriving via DirectPlay.
public delegate void AudioDelegate(); // Delegate to handle audio playback.
public delegate void PeerCloseCallback();           // This delegate will be called when the session terminated event is fired.

public class GameClass : GraphicsSample {
	private GraphicsFont drawingFont = null;
	private Point destination = new Point(0, 0);
	private InputClass input = null;

	private PlayClass play = null;
	private bool networkEnabled;
	private PlayClass peer = null;
	private PositionedMesh spaceSphere = null;
	private PositionedMesh playerShip = null;
	private Camera camera = null;

	private string debugText;
	public string DebugText { 
		get { return debugText; } 
		set { debugText = value; } 
	}
	private bool debugging = true;

	public GameClass(bool startFullScreen, Size size, bool enableNetwork) {
		this.startFullscreen = startFullScreen;
		this.Size = size;
		this.networkEnabled = enableNetwork;
		this.Text = "SpaceWar3D";

		drawingFont = new GraphicsFont( "Verdana", System.Drawing.FontStyle.Regular);

		input = new InputClass(this);
		camera = new Camera();

		if (networkEnabled) {
			peer = new PlayClass(this);
		}

	}

	private void ProcessInput() {
		//Get input
		KeyboardState kbState = input.GetInputState();
		if (kbState == null)
			return;
		if (kbState[Key.Up]) {
			camera.MoveRel(0,0,elapsedTime * 10);
		}
		if (kbState[Key.Down]) {
			camera.MoveRel(0,0,-elapsedTime * 10);
		}
		if (kbState[Key.Right]) {
			camera.RotateRel(0,elapsedTime,0);
		}
		if (kbState[Key.Left]) {
			camera.RotateRel(0,-elapsedTime,0);
		}
	}

	protected override void FrameMove() {
		ProcessInput();
		int fps = (int) framePerSecond;
		debugText = "FPS:  " + fps.ToString() + "\r\n";
		debugText += "Use the arrow keys to move or rotate the camera. \r\n\r\n";
		debugText += "Camera Location:  X: " + camera.Loc.X.ToString() +
			"  Y: " + camera.Loc.Y.ToString() + "  Z: " + camera.Loc.Z.ToString();
		//rotate space very slowly for that nice twinkly star effect
		spaceSphere.Position.RotateRel(-.001f * elapsedTime,-0.0001f * elapsedTime, 0);

		//rotate the ship
		playerShip.Position.RotateRel(0, -elapsedTime * .2f, 0);
	}


	/// <summary>
	/// Called once per frame, the call is the entry point for 3d rendering. This 
	/// function sets up render states, clears the viewport, and renders the scene.
	/// </summary>
	protected override void Render() {
		//Clear the backbuffer to a Blue color 
		device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Blue, 1.0f, 0);

		//Set the view matrix
		device.Transform.View = camera.ViewMatrix;

		//Begin the scene
		device.BeginScene();
		device.RenderState.ZBufferEnable = false;
		device.RenderState.Lighting = false;
		spaceSphere.Render();
		device.RenderState.Lighting = true;
		device.RenderState.ZBufferEnable = true;

		device.Transform.World = playerShip.Position.WorldMatrix;
		playerShip.Render();
			
		if (debugging) {
			drawingFont.DrawText(5,5,Color.Yellow,debugText);
		}
		device.EndScene();
	}

	/// <summary>
	/// Initialize scene objects.
	/// </summary>
	protected override void InitializeDeviceObjects() {
		drawingFont.InitializeDeviceObjects(device);
		camera.Point(0,5,-30,0,0,0);

		spaceSphere = new PositionedMesh(device, "SpaceSphere.x");
		playerShip = new PositionedMesh(device, "WhiteShip.x");
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
			presentParams.BackBufferWidth/presentParams.BackBufferHeight,
			1.5f, 20000.0f );
		device.Transform.View = camera.ViewMatrix;

		playerShip.Position.Move(0,0,0);
		playerShip.Position.Rotate(0,(float)Math.PI,0);
	}

	protected override void Dispose(bool disposing) {
		if (networkEnabled)
			play.Dispose();
		base.Dispose(disposing);
	}

	public void MessageArrived(byte message) {

	}

	/// <summary>
	// When the peer closes, the code here is executed.
	/// </summary>
	public void PeerClose() {
		// The session was terminated, go ahead and shut down
		this.Dispose();
	}
}
