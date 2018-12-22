using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Nettrix {
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
		private System.ComponentModel.IContainer components;
		private bool stillProcessing = false;
		private int score = 0;
		private Block CurrentBlock;
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
				if(components != null) {
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			this.components = new System.ComponentModel.Container();
			this.PicBackground = new System.Windows.Forms.PictureBox();
			this.lblScore = new System.Windows.Forms.Label();
			this.CmdStart = new System.Windows.Forms.Button();
			this.PicNextBlock = new System.Windows.Forms.PictureBox();
			this.LblNextBlock = new System.Windows.Forms.Label();
			this.lblScoreValue = new System.Windows.Forms.Label();
			this.tmrGameClock = new System.Windows.Forms.Timer(this.components);
			this.SuspendLayout();
			// 
			// PicBackground
			// 
			this.PicBackground.BackColor = System.Drawing.SystemColors.ControlDark;
			this.PicBackground.Location = new System.Drawing.Point(0, 0);
			this.PicBackground.Name = "PicBackground";
			this.PicBackground.Size = new System.Drawing.Size(149, 321);
			this.PicBackground.TabIndex = 0;
			this.PicBackground.TabStop = false;
			// 
			// lblScore
			// 
			this.lblScore.Location = new System.Drawing.Point(161, 5);
			this.lblScore.Name = "lblScore";
			this.lblScore.Size = new System.Drawing.Size(76, 18);
			this.lblScore.TabIndex = 3;
			this.lblScore.Text = "Score:";
			// 
			// CmdStart
			// 
			this.CmdStart.Location = new System.Drawing.Point(160, 144);
			this.CmdStart.Name = "CmdStart";
			this.CmdStart.Size = new System.Drawing.Size(68, 25);
			this.CmdStart.TabIndex = 9;
			this.CmdStart.Text = "Start!";
			this.CmdStart.Click += new System.EventHandler(this.CmdStart_Click);
			// 
			// PicNextBlock
			// 
			this.PicNextBlock.BackColor = System.Drawing.Color.DarkGray;
			this.PicNextBlock.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.PicNextBlock.Location = new System.Drawing.Point(160, 72);
			this.PicNextBlock.Name = "PicNextBlock";
			this.PicNextBlock.Size = new System.Drawing.Size(75, 64);
			this.PicNextBlock.TabIndex = 6;
			this.PicNextBlock.TabStop = false;
			// 
			// LblNextBlock
			// 
			this.LblNextBlock.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.LblNextBlock.Location = new System.Drawing.Point(160, 48);
			this.LblNextBlock.Name = "LblNextBlock";
			this.LblNextBlock.Size = new System.Drawing.Size(76, 16);
			this.LblNextBlock.TabIndex = 7;
			this.LblNextBlock.Text = "Next Block:";
			// 
			// lblScoreValue
			// 
			this.lblScoreValue.Location = new System.Drawing.Point(160, 31);
			this.lblScoreValue.Name = "lblScoreValue";
			this.lblScoreValue.Size = new System.Drawing.Size(76, 18);
			this.lblScoreValue.TabIndex = 8;
			this.lblScoreValue.Text = "0";
			this.lblScoreValue.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// tmrGameClock
			// 
			this.tmrGameClock.Interval = 330;
			this.tmrGameClock.Tick += new System.EventHandler(this.tmrGameClock_Tick);
			// 
			// NetTrix
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(246, 318);
			this.Controls.Add(this.CmdStart);
			this.Controls.Add(this.PicNextBlock);
			this.Controls.Add(this.LblNextBlock);
			this.Controls.Add(this.lblScoreValue);
			this.Controls.Add(this.lblScore);
			this.Controls.Add(this.PicBackground);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "NetTrix";
			this.Text = ".NETTrix";
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.NetTrix_KeyDown);
			this.Load += new System.EventHandler(this.NetTrix_Load);
			this.Activated += new System.EventHandler(this.NetTrix_Activated);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() {
			Application.Run(new NetTrix());
		}

		private void tmrGameClock_Tick(object sender, System.EventArgs e) {
			int erasedLines;

			if (stillProcessing) return;
			stillProcessing = true;

			//Manage the falling block
			if (!CurrentBlock.Down()) {
				if (CurrentBlock.Top() == 0) {
					tmrGameClock.Enabled = false;
					CmdStart.Enabled = true;
					MessageBox.Show("GAME OVER", ".NETTrix", MessageBoxButtons.OK, MessageBoxIcon.Stop);
					stillProcessing = false;
					return;
				}
				//increase score based on # of deleted lines
				erasedLines = GameField.CheckLines();
				if (erasedLines > 0) {
					score += 100 * erasedLines;
					lblScoreValue.Text = score.ToString();
					//Clear the game field and force the window to re-paint
					PicBackground.Invalidate();
					Application.DoEvents();
					GameField.Redraw();
				}

				//Replace the current block...
				CurrentBlock = new Block(new Point(GameField.SquareSize*6,0), NextBlock.BlockType);
				CurrentBlock.Show(PicBackground.Handle);
				
				//Create the Next block
				NextBlock.Hide(PicNextBlock.Handle);
				NextBlock = new Block(new Point(20,10), Block.BlockTypes.Undefined);
				NextBlock.Show(PicNextBlock.Handle);
			}
			stillProcessing = false;
		}

		private void CmdStart_Click(object sender, System.EventArgs e) {
			tmrGameClock.Enabled = true;
			CmdStart.Enabled = false;
			lblScoreValue.Text = "0";

			// Clean the game field
			PicBackground.Invalidate();
			Application.DoEvents();
			GameField.Reset();
			GameField.Redraw();

			// Create and show the current and next blocks
			CurrentBlock = new Block(new Point(GameField.SquareSize * 6, 50), Block.BlockTypes.Undefined);
			CurrentBlock.Show(PicBackground.Handle);

			NextBlock = new Block(new Point(20, 10), Block.BlockTypes.Undefined);
			NextBlock.Show(PicNextBlock.Handle);
		}

		private void NetTrix_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e) {
			switch(e.KeyCode) {
				case Keys.Right:  CurrentBlock.Right();break;
				case Keys.Left :  CurrentBlock.Left();break;
				case Keys.Up   :  CurrentBlock.Rotate();break;
				case Keys.Down :  CurrentBlock.Down();break;
				case Keys.Escape:
					tmrGameClock.Enabled = !tmrGameClock.Enabled;
					if (tmrGameClock.Enabled)
						this.Text = ".NETTrix";
					else
						this.Text = ".NETTrix -- Press 'Esc' to Continue";
					break;
				default: break;
			}
			Invalidate();
		}

		private void NetTrix_Load(object sender, System.EventArgs e) {
			GameField.BackColor = PicBackground.BackColor;
			GameField.WinHandle = PicBackground.Handle;
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
			//
			//Block - Creates the first block and the first NEXT block
			//
			GameField.WinHandle = PicBackground.Handle;
			GameField.BackColor = PicBackground.BackColor;
		}

		private void NetTrix_Activated(object sender, System.EventArgs e) {
			//        This event occurs when the window receives back the focus after losing it to another window
			//        So, we redraw the whole game field
			//        Clear the game field
			PicBackground.Invalidate();
			Application.DoEvents();
			GameField.Redraw();
			if (NextBlock != null)
				NextBlock.Show(PicNextBlock.Handle);
		}
	} 
}
