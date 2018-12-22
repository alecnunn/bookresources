using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D=Microsoft.DirectX.Direct3D;


	public delegate void MessageDelegate(byte message); // Delegate for messages arriving via DirectPlay.
	public delegate void AudioDelegate(); // Delegate to handle audio playback.
	public delegate void PeerCloseCallback();           // This delegate will be called when the session terminated event is fired.

	public class GameClass : GraphicsSample
	{
		private System.Drawing.Font actualFont;
		private Direct3D.Font drawingFont;
		//private GraphicsFont drawingFont = null;
		private Point destination = new Point(0, 0);
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

		private bool networkEnabled;
		//private PlayClass peer;

		#region GameClass Constructor
		public GameClass(bool startFullScreen, Size size, bool enableNetwork)
		{
			this.startFullscreen = startFullScreen;
			this.Size = size;
			this.networkEnabled = enableNetwork;
			this.Text = "SpaceWar3D";

			actualFont = new System.Drawing.Font("Arial", 14.0f, FontStyle.Italic);

			input = new InputClass(this, play);

			if (networkEnabled)
			{
				play = new PlayClass(this);
			}

		}

		#endregion //GameClass Constructor


		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{

			input.GetInputState();

			//Clear the backbuffer to a Blue color 
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Blue, 1.0f, 0);
			//Begin the scene
			device.BeginScene();
			drawingFont.DrawText( null, "X: " + destination.X + " Y: " + destination.Y, 
				new Rectangle(5, 5, this.Width, this.Height), 
				DrawTextFormat.NoClip | DrawTextFormat.ExpandTabs | DrawTextFormat.WordBreak, Color.White);

			//
			// TODO: Insert application rendering code here.
			//
			device.EndScene();
		}

		/// <summary>
		/// Initialize scene objects.
		/// </summary>
		protected override void InitializeDeviceObjects()
		{
			drawingFont = new Direct3D.Font(device, actualFont);//.InitializeDeviceObjects(device);

		}

		/// <summary>
		/// Called when a device needs to be restored.
		/// </summary>
		protected override void RestoreDeviceObjects(System.Object sender, System.EventArgs e)
		{

		}
		protected override void Dispose(bool disposing)
		{
			if (networkEnabled)
                play.Dispose();
			base.Dispose(disposing);
		}
		public void MessageArrived(byte message)
		{
			switch (message)
			{
				case msgUp:
				{
					destination.X = 1;
					break;
				}
				case msgDown:
				{
					destination.X = -1;
					break;
				}
				case msgLeft:
				{
					destination.Y = 1;
					break;
				}
				case msgRight:
				{
					destination.Y = -1;
					break;
				}
				case msgCancelUp:
				case msgCancelDown:
				{
					destination.X = 0;
					break;
				}
				case msgCancelLeft:
				case msgCancelRight:
				{
					destination.Y = 0;
					break;
				}
			}
		}

		/// <summary>
		// When the peer closes, the code here is executed.
		/// </summary>
		public void PeerClose()
		{
			// The session was terminated, go ahead and shut down
			this.Dispose();
		}
	}
