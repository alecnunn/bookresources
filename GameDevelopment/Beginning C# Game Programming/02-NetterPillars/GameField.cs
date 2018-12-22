using System;
using System.Drawing;
using System.Windows.Forms;
namespace Netterpillars {
	public class GameField : System.Windows.Forms.Form {
		public GameField() : base() {        

			//This call is required by the Windows Form Designer.
			InitializeComponent();

			//Add any initialization after the InitializeComponent() call

		}

		//Form overrides dispose to clean up the component list.
		protected override void Dispose(bool disposing) {
			if (disposing) {
				if (components != null) {
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code
		internal System.Windows.Forms.PictureBox PicGameField;
		private System.ComponentModel.IContainer components = new System.ComponentModel.Container();

		//Required by the Windows Form Designer

		//NOTE: The following procedure is required by the Windows Form Designer
		//It can be modified using the Windows Form Designer.  
		//Do not modify it using the code editor.
		[System.Diagnostics.DebuggerStepThrough]
		private void InitializeComponent() {
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(GameField));
			this.PicGameField = new System.Windows.Forms.PictureBox();
			this.SuspendLayout();
			// 
			// PicGameField
			// 
			this.PicGameField.BackgroundImage = ((System.Drawing.Bitmap)(resources.GetObject("PicGameField.BackgroundImage")));
			this.PicGameField.Image = ((System.Drawing.Bitmap)(resources.GetObject("PicGameField.Image")));
			this.PicGameField.Name = "PicGameField";
			this.PicGameField.Size = new System.Drawing.Size(592, 488);
			this.PicGameField.TabIndex = 0;
			this.PicGameField.TabStop = false;
			// 
			// GameField
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(594, 480);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.PicGameField});
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.KeyPreview = true;
			this.Name = "GameField";
			this.Text = ".Netterpillars";
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.GameField_KeyDown);
			this.Closing += new System.ComponentModel.CancelEventHandler(this.GameField_Closing);
			this.Load += new System.EventHandler(this.GameField_Load);
			this.Activated += new System.EventHandler(this.GameField_Activated);
			this.Paint += new System.Windows.Forms.PaintEventHandler(this.GameField_Paint);
			this.Deactivate += new System.EventHandler(this.GameField_Deactivate);
			this.ResumeLayout(false);

		}
		#endregion

		private void GameField_Load(System.Object sender, System.EventArgs e) {
			PicGameField.Location = new Point(0, 0);
			PicGameField.Size = new Size(MainGame.netterpillarGameEngine.Width*Sprite.IMAGE_SIZE, MainGame.netterpillarGameEngine.Height*Sprite.IMAGE_SIZE);
			this.ClientSize = PicGameField.Size;
		}


		private void GameField_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e) {
			// Just set the next direction for the player.
			//  We will not let the player go backwards from the current direction, because
			//    he would die if he does so, and may not understand why he died, what would not be a good game practice...
			switch(e.KeyCode) {
				case Keys.Right:
					if (MainGame.netterpillarGameEngine.Player1.Direction!=Sprite.CompassDirections.West) {
						MainGame.netterpillarGameEngine.Player1.Direction = Sprite.CompassDirections.East;
					}
					break;
				case Keys.Left:
					if (MainGame.netterpillarGameEngine.Player1.Direction!=Sprite.CompassDirections.East) {
						MainGame.netterpillarGameEngine.Player1.Direction = Sprite.CompassDirections.West;
					}
					break;
				case Keys.Up:
					if (MainGame.netterpillarGameEngine.Player1.Direction!=Sprite.CompassDirections.South) {
						MainGame.netterpillarGameEngine.Player1.Direction = Sprite.CompassDirections.North;
					}
					break;
				case Keys.Down:
					if (MainGame.netterpillarGameEngine.Player1.Direction!=Sprite.CompassDirections.North) {
						MainGame.netterpillarGameEngine.Player1.Direction = Sprite.CompassDirections.South;
					}
					break;
				case Keys.Escape:
					MainGame.netterpillarGameEngine.Paused = !MainGame.netterpillarGameEngine.Paused;
					if (MainGame.netterpillarGameEngine.Paused) {
						this.Text = ".Netterpillars - Press ESC to continue";
					}
					else {
						this.Text = ".Netterpillars";
					}
					break;
			}
		}



		private void GameField_Closing(object sender, System.ComponentModel.CancelEventArgs e) {
			MainGame.netterpillarGameEngine.GameOver = true;
		}

		private void GameField_Activated(object sender, System.EventArgs e) {
			// This event occurs when the window receives back the focus after had lost it to another window
			//   So, we redraw the whole game field
			// Clear the game field
			PicGameField.Invalidate();
			Application.DoEvents();
			MainGame.netterpillarGameEngine.Redraw();
		}

		private void GameField_Deactivate(object sender, System.EventArgs e) {
			// Pauses the game if it looses the focus
			MainGame.netterpillarGameEngine.Paused = true;
			this.Text = ".Netterpillars - Press ESC to continue";
		}

		private void GameField_Paint(object sender, System.Windows.Forms.PaintEventArgs e) {
		}

		protected override void OnPaint(System.Windows.Forms.PaintEventArgs e) {
			if (MainGame.netterpillarGameEngine!=null) {
				if (!MainGame.netterpillarGameEngine.GameOver) {
					MainGame.netterpillarGameEngine.Redraw();
				}
			}

		}
	}
}