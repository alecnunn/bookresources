using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace SpaceWar {
	/// <summary>
	/// The GameSettings Form allows us to change the options of the game.
	/// </summary>
	public class GameSettings : System.Windows.Forms.Form {
		#region Fields and Properties

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TrackBar gravitySlider;
		private System.Windows.Forms.TrackBar gameSpeedSlider;
		private System.Windows.Forms.CheckBox bounceBack;
		private System.Windows.Forms.Button closeButton;
		private System.Windows.Forms.CheckBox inverseGravity;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public int Gravity { get { return gravitySlider.Value; } }
		public int GameSpeed { get { return gameSpeedSlider.Value; } }
		public bool Bounce { get { return bounceBack.Checked; } }
		public bool InverseGravity { get { return inverseGravity.Checked; } }
		public bool BlackHole { get { return blackHole.Checked; } }
		
		private bool controlsEnabled = true;
		private System.Windows.Forms.Label lockedLabel;
		private System.Windows.Forms.CheckBox blackHole;
	
		public bool ControlsEnabled { get { return controlsEnabled; } set { EnableControls(value);} }

		private GameClass game = null;

		#endregion

		#region Constructor

		public GameSettings(GameClass game) {
			this.game = game;
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			// Set up the event handler
			this.gameSpeedSlider.ValueChanged += new EventHandler(ControlChanged);
			this.gravitySlider.ValueChanged +=new EventHandler(ControlChanged);
			this.bounceBack.CheckedChanged += new EventHandler(ControlChanged);
			this.inverseGravity.CheckedChanged +=new EventHandler(ControlChanged);
			this.blackHole.CheckedChanged += new EventHandler(ControlChanged);
		}
		#endregion // Constructor

		#region Logic routines for the form's controls
		
		private void EnableControls (bool enable) {
			gravitySlider.Enabled = enable;
			gameSpeedSlider.Enabled = enable;
			bounceBack.Enabled = enable;
			inverseGravity.Enabled = enable;
			blackHole.Enabled = enable;
			controlsEnabled = enable;
			lockedLabel.Visible = !enable;
			if (enable) {
				label1.ForeColor = Color.Yellow;
				label2.ForeColor = Color.Yellow;
			}
			else {
				label1.ForeColor = Color.LightGray;
				label2.ForeColor = Color.LightGray;
			}
		}

		private void ControlChanged (object sender, EventArgs e) {
			if (sender == gravitySlider)
				game.Gravity = gravitySlider.Value;
			
			if (sender == gameSpeedSlider)
				game.GameSpeed = gameSpeedSlider.Value;

			if (sender == inverseGravity)
				game.InverseGravity = inverseGravity.Checked;

			if (sender == bounceBack)
				game.BounceBack = bounceBack.Checked;

			if (sender == blackHole)
				game.BlackHole = blackHole.Checked;
		}
		#endregion  // Control logic
		
		#region Cleanup
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

		private void Close_Click(object sender, System.EventArgs e) {
			this.Hide();

			// tell the other players about our settings change
			game.SendGameParamUpdate();
			game.UnPause();
		}

		#endregion

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			this.gravitySlider = new System.Windows.Forms.TrackBar();
			this.label1 = new System.Windows.Forms.Label();
			this.gameSpeedSlider = new System.Windows.Forms.TrackBar();
			this.bounceBack = new System.Windows.Forms.CheckBox();
			this.closeButton = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.inverseGravity = new System.Windows.Forms.CheckBox();
			this.lockedLabel = new System.Windows.Forms.Label();
			this.blackHole = new System.Windows.Forms.CheckBox();
			((System.ComponentModel.ISupportInitialize)(this.gravitySlider)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.gameSpeedSlider)).BeginInit();
			this.SuspendLayout();
			// 
			// gravitySlider
			// 
			this.gravitySlider.LargeChange = 1000;
			this.gravitySlider.Location = new System.Drawing.Point(24, 16);
			this.gravitySlider.Maximum = 10000;
			this.gravitySlider.Name = "gravitySlider";
			this.gravitySlider.Size = new System.Drawing.Size(144, 45);
			this.gravitySlider.SmallChange = 100;
			this.gravitySlider.TabIndex = 1;
			this.gravitySlider.TickFrequency = 1000;
			// 
			// label1
			// 
			this.label1.ForeColor = System.Drawing.Color.Yellow;
			this.label1.Location = new System.Drawing.Point(48, 56);
			this.label1.Name = "label1";
			this.label1.TabIndex = 2;
			this.label1.Text = "Gravity";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// gameSpeedSlider
			// 
			this.gameSpeedSlider.LargeChange = 2;
			this.gameSpeedSlider.Location = new System.Drawing.Point(200, 16);
			this.gameSpeedSlider.Minimum = 1;
			this.gameSpeedSlider.Name = "gameSpeedSlider";
			this.gameSpeedSlider.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.gameSpeedSlider.Size = new System.Drawing.Size(144, 45);
			this.gameSpeedSlider.TabIndex = 2;
			this.gameSpeedSlider.Value = 4;
			// 
			// bounceBack
			// 
			this.bounceBack.ForeColor = System.Drawing.Color.Yellow;
			this.bounceBack.Location = new System.Drawing.Point(48, 128);
			this.bounceBack.Name = "bounceBack";
			this.bounceBack.TabIndex = 4;
			this.bounceBack.Text = "Bounce Back";
			// 
			// closeButton
			// 
			this.closeButton.BackColor = System.Drawing.Color.Black;
			this.closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.closeButton.ForeColor = System.Drawing.Color.Yellow;
			this.closeButton.Location = new System.Drawing.Point(256, 153);
			this.closeButton.Name = "closeButton";
			this.closeButton.TabIndex = 5;
			this.closeButton.Text = "Close";
			this.closeButton.Click += new System.EventHandler(this.Close_Click);
			// 
			// label2
			// 
			this.label2.ForeColor = System.Drawing.Color.Yellow;
			this.label2.Location = new System.Drawing.Point(222, 56);
			this.label2.Name = "label2";
			this.label2.TabIndex = 6;
			this.label2.Text = "Game Speed";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// inverseGravity
			// 
			this.inverseGravity.ForeColor = System.Drawing.Color.Yellow;
			this.inverseGravity.Location = new System.Drawing.Point(48, 104);
			this.inverseGravity.Name = "inverseGravity";
			this.inverseGravity.TabIndex = 3;
			this.inverseGravity.Text = "Inverse Gravity";
			// 
			// lockedLabel
			// 
			this.lockedLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lockedLabel.ForeColor = System.Drawing.Color.FromArgb(((System.Byte)(192)), ((System.Byte)(0)), ((System.Byte)(0)));
			this.lockedLabel.Location = new System.Drawing.Point(40, 80);
			this.lockedLabel.Name = "lockedLabel";
			this.lockedLabel.Size = new System.Drawing.Size(296, 32);
			this.lockedLabel.TabIndex = 7;
			this.lockedLabel.Text = "Settings Locked - connected to host.";
			this.lockedLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// blackHole
			// 
			this.blackHole.ForeColor = System.Drawing.Color.Yellow;
			this.blackHole.Location = new System.Drawing.Point(48, 152);
			this.blackHole.Name = "blackHole";
			this.blackHole.Size = new System.Drawing.Size(168, 24);
			this.blackHole.TabIndex = 8;
			this.blackHole.Text = "Black Hole (Invisible Sun)";
			// 
			// GameSettings
			// 
			this.AcceptButton = this.closeButton;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.BackColor = System.Drawing.Color.Black;
			this.CancelButton = this.closeButton;
			this.ClientSize = new System.Drawing.Size(366, 194);
			this.ControlBox = false;
			this.Controls.Add(this.blackHole);
			this.Controls.Add(this.inverseGravity);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.closeButton);
			this.Controls.Add(this.bounceBack);
			this.Controls.Add(this.gameSpeedSlider);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.gravitySlider);
			this.Controls.Add(this.lockedLabel);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.MaximumSize = new System.Drawing.Size(376, 230);
			this.MinimizeBox = false;
			this.MinimumSize = new System.Drawing.Size(376, 230);
			this.Name = "GameSettings";
			this.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "Game Settings";
			((System.ComponentModel.ISupportInitialize)(this.gravitySlider)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.gameSpeedSlider)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion
	}
}
