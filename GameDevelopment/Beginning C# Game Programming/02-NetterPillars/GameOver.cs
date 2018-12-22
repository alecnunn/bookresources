using System;
using System.Drawing;
using System.Windows.Forms;
namespace Netterpillars {
	public class GameOver : System.Windows.Forms.Form {
		public GameOver() : base() {        
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
		internal System.Windows.Forms.Button Button1;
		internal System.Windows.Forms.Label Label1;
		internal System.Windows.Forms.GroupBox grpStats;
		internal System.Windows.Forms.Label LblPlayer1;
		internal System.Windows.Forms.Label LblPlayer2;
		internal System.Windows.Forms.Label LblPlayer3;
		internal System.Windows.Forms.Label LblPlayer4;
		internal System.Windows.Forms.Label LblPlayer4Is;
		internal System.Windows.Forms.Label LblPlayer3Is;
		internal System.Windows.Forms.Label LblPlayer2Is;
		internal System.Windows.Forms.Label LblPlayer1Is;
		internal System.Windows.Forms.Label LblPlayer4Length;
		internal System.Windows.Forms.Label LblPlayer3Length;
		internal System.Windows.Forms.Label LblPlayer2Length;
		internal System.Windows.Forms.Label LblPlayer1Length;

		//Required by the Windows Form Designer
		private System.ComponentModel.IContainer components = new System.ComponentModel.Container();

		//NOTE: The following procedure is required by the Windows Form Designer
		//It can be modified using the Windows Form Designer.  
		//Do not modify it using the code editor.
		[System.Diagnostics.DebuggerStepThrough]
		private void InitializeComponent() {
			this.Button1 = new System.Windows.Forms.Button();
			this.Label1 = new System.Windows.Forms.Label();
			this.grpStats = new System.Windows.Forms.GroupBox();
			this.LblPlayer4Length = new System.Windows.Forms.Label();
			this.LblPlayer3Length = new System.Windows.Forms.Label();
			this.LblPlayer2Length = new System.Windows.Forms.Label();
			this.LblPlayer1Length = new System.Windows.Forms.Label();
			this.LblPlayer4Is = new System.Windows.Forms.Label();
			this.LblPlayer3Is = new System.Windows.Forms.Label();
			this.LblPlayer2Is = new System.Windows.Forms.Label();
			this.LblPlayer1Is = new System.Windows.Forms.Label();
			this.LblPlayer4 = new System.Windows.Forms.Label();
			this.LblPlayer3 = new System.Windows.Forms.Label();
			this.LblPlayer2 = new System.Windows.Forms.Label();
			this.LblPlayer1 = new System.Windows.Forms.Label();
			this.grpStats.SuspendLayout();
			this.SuspendLayout();
			// 
			// Button1
			// 
			this.Button1.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.Button1.Location = new System.Drawing.Point(120, 224);
			this.Button1.Name = "Button1";
			this.Button1.Size = new System.Drawing.Size(75, 24);
			this.Button1.TabIndex = 0;
			this.Button1.Text = "OK";
			// 
			// Label1
			// 
			this.Label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Label1.Location = new System.Drawing.Point(40, 24);
			this.Label1.Name = "Label1";
			this.Label1.Size = new System.Drawing.Size(240, 32);
			this.Label1.TabIndex = 1;
			this.Label1.Text = "Game Over !";
			this.Label1.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// grpStats
			// 
			this.grpStats.Controls.AddRange(new System.Windows.Forms.Control[] {
																				   this.LblPlayer4Length,
																				   this.LblPlayer3Length,
																				   this.LblPlayer2Length,
																				   this.LblPlayer1Length,
																				   this.LblPlayer4Is,
																				   this.LblPlayer3Is,
																				   this.LblPlayer2Is,
																				   this.LblPlayer1Is,
																				   this.LblPlayer4,
																				   this.LblPlayer3,
																				   this.LblPlayer2,
																				   this.LblPlayer1});
			this.grpStats.Location = new System.Drawing.Point(32, 64);
			this.grpStats.Name = "grpStats";
			this.grpStats.Size = new System.Drawing.Size(288, 152);
			this.grpStats.TabIndex = 2;
			this.grpStats.TabStop = false;
			this.grpStats.Text = "Stats";
			// 
			// LblPlayer4Length
			// 
			this.LblPlayer4Length.Location = new System.Drawing.Point(176, 120);
			this.LblPlayer4Length.Name = "LblPlayer4Length";
			this.LblPlayer4Length.Size = new System.Drawing.Size(88, 24);
			this.LblPlayer4Length.TabIndex = 11;
			this.LblPlayer4Length.Text = "0";
			// 
			// LblPlayer3Length
			// 
			this.LblPlayer3Length.Location = new System.Drawing.Point(176, 88);
			this.LblPlayer3Length.Name = "LblPlayer3Length";
			this.LblPlayer3Length.Size = new System.Drawing.Size(96, 24);
			this.LblPlayer3Length.TabIndex = 10;
			this.LblPlayer3Length.Text = "0";
			// 
			// LblPlayer2Length
			// 
			this.LblPlayer2Length.Location = new System.Drawing.Point(176, 56);
			this.LblPlayer2Length.Name = "LblPlayer2Length";
			this.LblPlayer2Length.Size = new System.Drawing.Size(80, 16);
			this.LblPlayer2Length.TabIndex = 9;
			this.LblPlayer2Length.Text = "0";
			// 
			// LblPlayer1Length
			// 
			this.LblPlayer1Length.Location = new System.Drawing.Point(176, 24);
			this.LblPlayer1Length.Name = "LblPlayer1Length";
			this.LblPlayer1Length.Size = new System.Drawing.Size(88, 16);
			this.LblPlayer1Length.TabIndex = 8;
			this.LblPlayer1Length.Text = "0";
			// 
			// LblPlayer4Is
			// 
			this.LblPlayer4Is.Location = new System.Drawing.Point(88, 120);
			this.LblPlayer4Is.Name = "LblPlayer4Is";
			this.LblPlayer4Is.Size = new System.Drawing.Size(96, 24);
			this.LblPlayer4Is.TabIndex = 7;
			this.LblPlayer4Is.Text = "Comp./Human";
			// 
			// LblPlayer3Is
			// 
			this.LblPlayer3Is.Location = new System.Drawing.Point(88, 88);
			this.LblPlayer3Is.Name = "LblPlayer3Is";
			this.LblPlayer3Is.Size = new System.Drawing.Size(104, 24);
			this.LblPlayer3Is.TabIndex = 6;
			this.LblPlayer3Is.Text = "Comp./Human";
			// 
			// LblPlayer2Is
			// 
			this.LblPlayer2Is.Location = new System.Drawing.Point(88, 56);
			this.LblPlayer2Is.Name = "LblPlayer2Is";
			this.LblPlayer2Is.Size = new System.Drawing.Size(88, 16);
			this.LblPlayer2Is.TabIndex = 5;
			this.LblPlayer2Is.Text = "Comp./Human";
			// 
			// LblPlayer1Is
			// 
			this.LblPlayer1Is.Location = new System.Drawing.Point(88, 24);
			this.LblPlayer1Is.Name = "LblPlayer1Is";
			this.LblPlayer1Is.Size = new System.Drawing.Size(96, 16);
			this.LblPlayer1Is.TabIndex = 4;
			this.LblPlayer1Is.Text = "Comp./Human";
			// 
			// LblPlayer4
			// 
			this.LblPlayer4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.LblPlayer4.Location = new System.Drawing.Point(16, 120);
			this.LblPlayer4.Name = "LblPlayer4";
			this.LblPlayer4.Size = new System.Drawing.Size(88, 24);
			this.LblPlayer4.TabIndex = 3;
			this.LblPlayer4.Text = "Player 4";
			// 
			// LblPlayer3
			// 
			this.LblPlayer3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.LblPlayer3.Location = new System.Drawing.Point(16, 88);
			this.LblPlayer3.Name = "LblPlayer3";
			this.LblPlayer3.Size = new System.Drawing.Size(96, 24);
			this.LblPlayer3.TabIndex = 2;
			this.LblPlayer3.Text = "Player 3";
			// 
			// LblPlayer2
			// 
			this.LblPlayer2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.LblPlayer2.Location = new System.Drawing.Point(16, 56);
			this.LblPlayer2.Name = "LblPlayer2";
			this.LblPlayer2.Size = new System.Drawing.Size(80, 16);
			this.LblPlayer2.TabIndex = 1;
			this.LblPlayer2.Text = "Player 2";
			// 
			// LblPlayer1
			// 
			this.LblPlayer1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.LblPlayer1.Location = new System.Drawing.Point(16, 24);
			this.LblPlayer1.Name = "LblPlayer1";
			this.LblPlayer1.Size = new System.Drawing.Size(88, 16);
			this.LblPlayer1.TabIndex = 0;
			this.LblPlayer1.Text = "Player 1";
			// 
			// GameOver
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(346, 266);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.grpStats,
																		  this.Label1,
																		  this.Button1});
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "GameOver";
			this.Text = ".Netterpillars";
			this.Load += new System.EventHandler(this.GameOver_Load);
			this.grpStats.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion


		private void GameOver_Load(System.Object sender, System.EventArgs e) {

			LblPlayer1Length.Text = MainGame.netterpillarGameEngine.netterPillars[0].NetterBodyLength.ToString();
			LblPlayer1Is.Text = (MainGame.netterpillarGameEngine.netterPillars[0].IsComputer ? "Computer" : "Human");

			if (MainGame.netterpillarGameEngine.netterPillars[1]!=null) {
				LblPlayer2Length.Text = MainGame.netterpillarGameEngine.netterPillars[1].NetterBodyLength.ToString();
				LblPlayer2Is.Text = (MainGame.netterpillarGameEngine.netterPillars[1].IsComputer ? "Computer" : "Human");
			}
			else {
				LblPlayer2Length.Text = "-";
				LblPlayer2Is.Text = "-";
			}

			if (MainGame.netterpillarGameEngine.netterPillars[2]!=null) {
				LblPlayer3Length.Text = MainGame.netterpillarGameEngine.netterPillars[2].NetterBodyLength.ToString();
				LblPlayer3Is.Text = (MainGame.netterpillarGameEngine.netterPillars[2].IsComputer ? "Computer" : "Human");
			}
			else {
				LblPlayer3Length.Text = "-";
				LblPlayer3Is.Text = "-";
			}

			if (MainGame.netterpillarGameEngine.netterPillars[3]!=null) {
				LblPlayer4Length.Text = MainGame.netterpillarGameEngine.netterPillars[3].NetterBodyLength.ToString();
				LblPlayer4Is.Text = (MainGame.netterpillarGameEngine.netterPillars[3].IsComputer ? "Computer" : "Human");
			}
			else {
				LblPlayer4Length.Text = "-";
				LblPlayer4Is.Text = "-";
			}
		}
	}
}