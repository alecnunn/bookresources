using System;
using System.Drawing;
using Microsoft.DirectX;
using Container = System.ComponentModel.Container;
using System.Windows.Forms;
using System.Threading;

namespace SpaceWar {

	/// <summary>
	/// The main windows form for the game.
	/// </summary>
	public class MainClass : System.Windows.Forms.Form {
		#region Fields
		private GameClass game = null;
		private Container components = null;
		private Control target = null; // Target for rendering operations.
		private bool moving = false;
		private Point mousePoint = new Point();

		#endregion //Fields

		/// <summary>
		// Main entry point of the application.
		/// </summary>
		#region Constructors
		[STAThread]
		public static void Main() {
			MainClass m = new MainClass();
			Application.Exit();
		}

		public MainClass() {
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			target = this;

			// Add event handlers to enable moving and resizing
			this.MouseDown += new MouseEventHandler(OnMouseDown);
			this.MouseMove +=new MouseEventHandler(OnMouseMove);
			this.MouseUp += new MouseEventHandler(OnMouseUp);
			this.Resize += new System.EventHandler(OnMoveResize);
			this.Move +=new EventHandler(OnMoveResize);


			// Set up Double buffering to eliminate flicker
			SetStyle(ControlStyles.DoubleBuffer, true);
			SetStyle(ControlStyles.AllPaintingInWmPaint, true);
			SetStyle(ControlStyles.UserPaint, true);

			//Initialize the main game class
			game = new GameClass(this, target);
			game.Initialize(this.Bounds);

			//Show our game form
			this.Show();

			//Start the main game loop
			StartLoop();
		}
		#endregion //Constructors

		#region GameLoop

		/// <summary>
		// This is the main loop of the application.
		/// </summary>
		private void StartLoop() {
			while(Created) {
				game.MainLoop();  //  Execute the game logic
				Application.DoEvents();  // Make sure we take care of all the windows event messages
				Thread.Sleep(2);  // Yield some CPU time to other applications.
			}
		}
		#endregion //GameLoop

		#region Cleanup

		public void EndGame() {
			this.Dispose(true);
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing ) {
			if( disposing ) {
				if (components != null) {
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}
		#endregion //Cleanup

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			// 
			// MainClass
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.BackColor = System.Drawing.Color.Black;
			this.ClientSize = new System.Drawing.Size(792, 592);
			this.ControlBox = false;
			this.KeyPreview = true;
			this.Location = new System.Drawing.Point(10, 10);
			this.MaximizeBox = false;
			this.Name = "MainClass";
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;

		}
		#endregion

		#region Event Handlers
		// Event Handlers for moving and resizing the game form
		private void OnMoveResize(object sender, System.EventArgs e) {
			if (moving)
				return;
			if (null != game) {
				game.GameState = GameStates.Paused;
				game.Initialize(this.DesktopBounds);
			}
		}

		private void OnMouseDown(object sender, MouseEventArgs mea) {
			if (null != game) {
				game.GameState = GameStates.Paused;
			}
			moving = true;
			mousePoint = new Point(mea.X, mea.Y);
		}

		private void OnMouseMove(object sender, MouseEventArgs mea) {
			if (moving) {
				this.Left += mea.X - mousePoint.X;
				this.Top += mea.Y - mousePoint.Y;
			}
		}

		private void OnMouseUp(object sender, MouseEventArgs mea) {
			moving = false;

			if (null != game) {
				game.Initialize(this.DesktopBounds);
			}
		}
		#endregion
	}
}