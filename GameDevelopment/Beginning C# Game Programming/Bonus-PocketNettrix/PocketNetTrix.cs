using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace PocketNettrix {
	/// <summary>
	/// Summary description for .
	/// </summary>
	public class NetTrix : System.Windows.Forms.Form {
		internal System.Windows.Forms.PictureBox PicBackground;
		internal System.Windows.Forms.Label lblScore;
		internal System.Windows.Forms.Button CmdStart;
		internal System.Windows.Forms.PictureBox PicNextBlock;
		internal System.Windows.Forms.Label LblNextBlock;
		internal System.Windows.Forms.Label lblScoreValue;
		internal System.Windows.Forms.Timer tmrGameClock;
//		private System.ComponentModel.IContainer components;
		private bool stillProcessing = false;
		private int score = 0;
		private Block CurrentBlock;
		private System.Windows.Forms.Button leftButton;
		private System.Windows.Forms.Button rotateButton;
		private System.Windows.Forms.Button rightButton;
		private System.Windows.Forms.Button downButton;
		private Block NextBlock;

		public NetTrix() {
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing ) {
			if( disposing ) {
//				if(components != null) {
//					components.Dispose();
//				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			this.PicBackground = new System.Windows.Forms.PictureBox();
			this.lblScore = new System.Windows.Forms.Label();
			this.CmdStart = new System.Windows.Forms.Button();
			this.PicNextBlock = new System.Windows.Forms.PictureBox();
			this.LblNextBlock = new System.Windows.Forms.Label();
			this.lblScoreValue = new System.Windows.Forms.Label();
			this.tmrGameClock = new System.Windows.Forms.Timer();
			this.leftButton = new System.Windows.Forms.Button();
			this.rotateButton = new System.Windows.Forms.Button();
			this.rightButton = new System.Windows.Forms.Button();
			this.downButton = new System.Windows.Forms.Button();
			// 
			// PicBackground
			// 
			this.PicBackground.Size = new System.Drawing.Size(149, 321);
			// 
			// lblScore
			// 
			this.lblScore.Location = new System.Drawing.Point(161, 5);
			this.lblScore.Size = new System.Drawing.Size(76, 18);
			this.lblScore.Text = "Score:";
			// 
			// CmdStart
			// 
			this.CmdStart.Location = new System.Drawing.Point(168, 144);
			this.CmdStart.Size = new System.Drawing.Size(68, 25);
			this.CmdStart.Text = "Start!";
			this.CmdStart.Click += new System.EventHandler(this.CmdStart_Click);
			// 
			// PicNextBlock
			// 
			this.PicNextBlock.Location = new System.Drawing.Point(168, 72);
			this.PicNextBlock.Size = new System.Drawing.Size(75, 64);
			// 
			// LblNextBlock
			// 
			this.LblNextBlock.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
			this.LblNextBlock.Location = new System.Drawing.Point(160, 48);
			this.LblNextBlock.Size = new System.Drawing.Size(76, 16);
			this.LblNextBlock.Text = "Next Block:";
			// 
			// lblScoreValue
			// 
			this.lblScoreValue.Location = new System.Drawing.Point(160, 31);
			this.lblScoreValue.Size = new System.Drawing.Size(76, 18);
			this.lblScoreValue.Text = "0";
			this.lblScoreValue.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// tmrGameClock
			// 
			this.tmrGameClock.Tick += new System.EventHandler(this.tmrGameClock_Tick);
			// 
			// leftButton
			// 
			this.leftButton.Location = new System.Drawing.Point(168, 224);
			this.leftButton.Size = new System.Drawing.Size(24, 24);
			this.leftButton.Text = "<";
			this.leftButton.Click += new System.EventHandler(this.leftButton_Click);
			// 
			// rotateButton
			// 
			this.rotateButton.Location = new System.Drawing.Point(176, 192);
			this.rotateButton.Size = new System.Drawing.Size(56, 24);
			this.rotateButton.Text = "Rotate";
			this.rotateButton.Click += new System.EventHandler(this.rotateButton_Click);
			// 
			// rightButton
			// 
			this.rightButton.Location = new System.Drawing.Point(216, 224);
			this.rightButton.Size = new System.Drawing.Size(24, 24);
			this.rightButton.Text = ">";
			this.rightButton.Click += new System.EventHandler(this.rightButton_Click);
			// 
			// downButton
			// 
			this.downButton.Location = new System.Drawing.Point(192, 248);
			this.downButton.Size = new System.Drawing.Size(24, 24);
			this.downButton.Text = "\\/";
			this.downButton.Click += new System.EventHandler(this.downButton_Click);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			// 
			// NetTrix
			// 
			this.ClientSize = new System.Drawing.Size(246, 318);
			this.Controls.Add(this.leftButton);
			this.Controls.Add(this.CmdStart);
			this.Controls.Add(this.PicNextBlock);
			this.Controls.Add(this.LblNextBlock);
			this.Controls.Add(this.lblScoreValue);
			this.Controls.Add(this.lblScore);
			this.Controls.Add(this.PicBackground);
			this.Controls.Add(this.rotateButton);
			this.Controls.Add(this.rightButton);
			this.Controls.Add(this.downButton);
			this.Text = ".NETTrix";
			//this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.NetTrix_KeyDown);
			this.Load += new System.EventHandler(this.NetTrix_Load);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main() {
			Application.Run(new NetTrix());
		}

		private void tmrGameClock_Tick(object sender, System.EventArgs e) {
			if (stillProcessing) return;
			stillProcessing = true;
			//Manage the falling block
			if (!CurrentBlock.Down()) {
				//Update to Pocket PC: Update the score value
				score += 5;
				lblScoreValue.Text = score.ToString();
				if (CurrentBlock.Top() == 0) {
					tmrGameClock.Enabled = false;
					CmdStart.Enabled = true;
					//Update to Pocket PC: Changed box icon
					MessageBox.Show("GAME OVER", "NETTrix", MessageBoxButtons.OK, MessageBoxIcon.Hand,MessageBoxDefaultButton.Button1);
					stillProcessing = false;
					return;
				}
				//increase score based on # of deleted lines
				int erasedLines = GameField.CheckLines();
				if (erasedLines > 0) {
					score += 100 * erasedLines;
					lblScoreValue.Text = score.ToString();
					//Update to Pocket PC: Use Clear/Redraw
					GameField.Clear();
					GameField.Redraw();
				}
				//Replace the current block...
				CurrentBlock = new Block(new Point(GameField.SquareSize*6,0), NextBlock.BlockType);
				CurrentBlock.Show(GameField.GraphBackground);
				//Create the Next block
				NextBlock.Hide(GameField.GraphNextBlock);
				NextBlock = new Block(new Point(20,10), Block.BlockTypes.Undefined);
				NextBlock.Show(GameField.GraphNextBlock);
                PicNextBlock.Invalidate();
			}
			PicBackground.Invalidate();
			stillProcessing = false;
		}

		private void CmdStart_Click(object sender, System.EventArgs e) {
			tmrGameClock.Enabled = true;
			CmdStart.Enabled = false;
			lblScoreValue.Text = "0";

			// Clean the game field
			GameField.Reset();
            // Update to Pocket PC: we must draw the blank screen, instead of simply
            //   invalidating a picture box image
			GameField.Clear();

			// Create and show the current and next blocks
			CurrentBlock = new Block(new Point(GameField.SquareSize * 6, 50), Block.BlockTypes.Undefined);
			CurrentBlock.Show(GameField.GraphBackground);

			NextBlock = new Block(new Point(20, 10), Block.BlockTypes.Undefined);
			NextBlock.Show(GameField.GraphNextBlock);

			PicBackground.Invalidate();
			PicNextBlock.Invalidate();
		}

		private void NetTrix_Load(object sender, System.EventArgs e) {
			//Adjusts the Size of the form and position of controls based on the class constants
			//On the window height, sums the Size of the window title bar
			Height = GameField.Height * GameField.SquareSize + (Height - ClientSize.Height) + 3; //3=border width
			Width = GameField.Width * GameField.SquareSize + 120;
			PicBackground.Height = GameField.Height * GameField.SquareSize + 4;
			PicBackground.Width = GameField.Width * GameField.SquareSize + 4;
			PicNextBlock.Left = GameField.Width * GameField.SquareSize + 12;
			LblNextBlock.Left = GameField.Width * GameField.SquareSize + 12;
			lblScore.Left = GameField.Width * GameField.SquareSize + 12;
			lblScoreValue.Left = GameField.Width * GameField.SquareSize + 12;
			CmdStart.Left = GameField.Width * GameField.SquareSize + 12;

			//Update to Pocket PC: move the new buttons according to the square size too
			this.leftButton.Left = GameField.Width * GameField.SquareSize + 12;
			this.rightButton.Left = GameField.Width * GameField.SquareSize + 12 + leftButton.Width * 2;
            this.rotateButton.Left = GameField.Width * GameField.SquareSize + 12 + leftButton.Width;
			this.downButton.Left = GameField.Width * GameField.SquareSize + 12 + leftButton.Width;

			// Update to Pocket PC: The images must be explicitly created
			PicBackground.Image = new Bitmap(PicBackground.Width, PicBackground.Height);
			PicNextBlock.Image = new Bitmap(PicNextBlock.Width, PicNextBlock.Height);

			//Update to Pocket PC: The initialize function will create the 
			//   buffers and Graphics objects used to draw on the screen
			GameField.Initialize(this, PicBackground, PicNextBlock);
		}

		private void rotateButton_Click(object sender, System.EventArgs e) {
			//Don't do anything if timer isn't enabled
			if (tmrGameClock.Enabled) CurrentBlock.Rotate();
		}

		private void leftButton_Click(object sender, System.EventArgs e) {
			//Don't do anything if timer isn't enabled
			if (tmrGameClock.Enabled) CurrentBlock.Left();
		}

		private void rightButton_Click(object sender, System.EventArgs e) {
			//Don't do anything if timer isn't enabled
			if (tmrGameClock.Enabled) CurrentBlock.Right();
		}

		private void downButton_Click(object sender, System.EventArgs e) {
			//Don't do anything if timer isn't enabled
			if (tmrGameClock.Enabled) CurrentBlock.Down();
		}
	} 
}
