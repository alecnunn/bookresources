using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D=Microsoft.DirectX.Direct3D;

public delegate void MessageDelegate(byte message); // Delegate for messages arriving via DirectPlay.
public delegate void AudioDelegate(); // Delegate to handle audio playback.
public delegate void PeerCloseCallback(); // This delegate will be called when the session terminated event is fired.

public class GraphicsClass : GraphicsSample {
	private float x = 0;
	private float y = 0;
	private GraphicsFont drawingFont = null;
	private Mesh teapot = null;
	private Mesh box = null;
	private int LastTick = Environment.TickCount;
	private int Elapsed = Environment.TickCount;
	private Point destination = new Point(0, 0);
	private AudioClass audio = null;
	private InputClass input = null;

	private PlayClass play = null;

	private const byte msgUp = 0;
	private const byte msgDown = 1;
	private const byte msgLeft = 2;
	private const byte msgRight = 3;
	private const byte msgCancelUp = 4;
	private const byte msgCancelDown = 5;
	private const byte msgCancelLeft = 6;
	private const byte msgCancelRight = 7;
	private const byte msgAudio = 8;
	private const byte msgSound = 9;

	public GraphicsClass() {
		this.Text = "Direct3DTest";
		audio = new AudioClass(this);
		play = new PlayClass(this);
		input = new InputClass(this, audio, play);
		drawingFont = new GraphicsFont( "Arial", System.Drawing.FontStyle.Bold );
	}


	/// <summary>
	/// Called once per frame, the call is the entry point for 3d rendering. This 
	/// function sets up render states, clears the viewport, and renders the scene.
	/// </summary>
	protected override void Render() {
		input.GetInputState();

		//Clear the backbuffer to a Blue color 
		device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Gray, 1.0f, 0);
		//Begin the scene
		device.BeginScene();

		device.Lights[0].Enabled = true;

		// Setup the world, view, and projection matrices
		Matrix m = new Matrix();
        
		if( destination.Y != 0 )
			y += DXUtil.Timer(DirectXTimer.GetElapsedTime) * (destination.Y * 25); 

		if( destination.X != 0 )
			x += DXUtil.Timer(DirectXTimer.GetElapsedTime) * (destination.X * 25);

		m = Matrix.Translation(0.0f, 0.75f, 0.0f);
		m *= Matrix.RotationY(y);
		m *= Matrix.RotationX(x);

		device.Transform.World = m;
		// Render the teapot.
		teapot.DrawSubset(0);

		m = new Matrix();
		m = Matrix.Translation(0.0f, -0.75f, 0.0f);
		m *= Matrix.RotationY(y);
		m *= Matrix.RotationX(x);

		device.Transform.World = m;
		//render the box
		box.DrawSubset(0);

		device.Transform.View = Matrix.LookAtLH( new Vector3( 0.0f, 3.0f,-5.0f ), new Vector3( 0.0f, 0.0f, 0.0f ), new Vector3( 0.0f, 1.0f, 0.0f ) );   
		device.Transform.Projection = Matrix.PerspectiveFovLH( (float)Math.PI / 4, 1.0f, 1.0f, 100.0f );


		device.EndScene();
	}

	/// <summary>
	/// Called when a device needs to be restored.
	/// </summary>
	protected override void RestoreDeviceObjects(System.Object sender, System.EventArgs e) {
		Material mtrl = GraphicsUtility.InitMaterial(Color.White);
		device.Material = mtrl;
                    
		device.Lights[0].Type = LightType.Directional;
		device.Lights[0].Direction = new Vector3(0.3f, -0.5f, 0.2f);
		device.Lights[0].Diffuse = Color.White;

		device.RenderState.Lighting = true;
//		device.RenderState.FillMode = FillMode.WireFrame;
	}

	/// <summary>
	/// Initialize scene objects.
	/// </summary>
	protected override void InitializeDeviceObjects() {
		drawingFont.InitializeDeviceObjects(device);            
		teapot = Mesh.Teapot(device);
		box = Mesh.Box(device, 1.5f, 1.5f, 1.5f);
	}
	protected override void Dispose(bool disposing) {
		play.Dispose();
		base.Dispose(disposing);
	}
	public void MessageArrived(byte message) {
		switch (message) {
			case msgUp: {
				destination.X = 1;
				break;
			}
			case msgDown: {
				destination.X = -1;
				break;
			}
			case msgLeft: {
				destination.Y = 1;
				break;
			}
			case msgRight: {
				destination.Y = -1;
				break;
			}
			case msgCancelUp:
			case msgCancelDown: {
				destination.X = 0;
				break;
			}
			case msgCancelLeft:
			case msgCancelRight: {
				destination.Y = 0;
				break;
			}
			case msgAudio: {
				this.BeginInvoke(new AudioDelegate(audio.PlayAudio));
				break;
			}
			case msgSound: {
				audio.PlaySound();
				break;
			}
		}
	}

	/// <summary>
	// When the peer closes, the code here is executed.
	/// </summary>
	public void PeerClose() {
		// The session was terminated, go ahead and shut down
		this.Dispose();
	}
}