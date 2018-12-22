using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace SpaceWar {
	/// <summary>
	/// Spacewar Splashscreen
	/// </summary>
	public class SplashScreen : System.Windows.Forms.Form {
		private GameClass gameClass;
		private System.Windows.Forms.Button helpButton;
		private System.Windows.Forms.Button startButton;
		private System.Windows.Forms.Button closeButton;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public SplashScreen(GameClass game) {
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			this.gameClass = game;

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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(SplashScreen));
			this.helpButton = new System.Windows.Forms.Button();
			this.startButton = new System.Windows.Forms.Button();
			this.closeButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// helpButton
			// 
			this.helpButton.ForeColor = System.Drawing.Color.White;
			this.helpButton.Location = new System.Drawing.Point(440, 312);
			this.helpButton.Name = "helpButton";
			this.helpButton.Size = new System.Drawing.Size(136, 32);
			this.helpButton.TabIndex = 0;
			this.helpButton.Text = "Show Help";
			this.helpButton.Click += new System.EventHandler(this.helpButton_Click);
			// 
			// startButton
			// 
			this.startButton.ForeColor = System.Drawing.Color.White;
			this.startButton.Location = new System.Drawing.Point(440, 360);
			this.startButton.Name = "startButton";
			this.startButton.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.startButton.Size = new System.Drawing.Size(136, 32);
			this.startButton.TabIndex = 1;
			this.startButton.Text = "Configure and Play!";
			this.startButton.Click += new System.EventHandler(this.startButton_Click);
			// 
			// closeButton
			// 
			this.closeButton.ForeColor = System.Drawing.Color.White;
			this.closeButton.Location = new System.Drawing.Point(440, 408);
			this.closeButton.Name = "closeButton";
			this.closeButton.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.closeButton.Size = new System.Drawing.Size(136, 32);
			this.closeButton.TabIndex = 2;
			this.closeButton.Text = "Exit";
			this.closeButton.Click += new System.EventHandler(this.closeButton_Click);
			// 
			// SplashScreen
			// 
			this.AutoScale = false;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.BackColor = System.Drawing.Color.Black;
			this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
			this.ClientSize = new System.Drawing.Size(594, 454);
			this.ControlBox = false;
			this.Controls.Add(this.closeButton);
			this.Controls.Add(this.startButton);
			this.Controls.Add(this.helpButton);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.MaximizeBox = false;
			this.MaximumSize = new System.Drawing.Size(600, 460);
			this.MinimizeBox = false;
			this.MinimumSize = new System.Drawing.Size(600, 460);
			this.Name = "SplashScreen";
			this.ShowInTaskbar = false;
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.ResumeLayout(false);

		}
		#endregion

		private void helpButton_Click(object sender, System.EventArgs e) {
			HelpScreen helpScreen = new HelpScreen();
			helpScreen.ShowDialog();
		}

		private void startButton_Click(object sender, System.EventArgs e) {
			this.Close();
		}

		private void closeButton_Click(object sender, System.EventArgs e) {
			Environment.Exit(0);
		}
	}
}
