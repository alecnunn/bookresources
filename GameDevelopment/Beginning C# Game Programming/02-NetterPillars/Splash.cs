using System;
using System.Drawing;
using System.Windows.Forms;
namespace Netterpillars {
	public class Splash : System.Windows.Forms.Form {
		internal System.Windows.Forms.Button cmdConfig;
		internal System.Windows.Forms.Button cmdExit;
		internal System.Windows.Forms.Button cmdStart;

		public Splash() : base() {        
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(Splash));
			this.PicGameField = new System.Windows.Forms.PictureBox();
			this.cmdConfig = new System.Windows.Forms.Button();
			this.cmdStart = new System.Windows.Forms.Button();
			this.cmdExit = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// PicGameField
			// 
			this.PicGameField.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.PicGameField.Image = ((System.Drawing.Bitmap)(resources.GetObject("PicGameField.Image")));
			this.PicGameField.Location = new System.Drawing.Point(9, 9);
			this.PicGameField.Name = "PicGameField";
			this.PicGameField.Size = new System.Drawing.Size(524, 370);
			this.PicGameField.TabIndex = 0;
			this.PicGameField.TabStop = false;
			// 
			// cmdConfig
			// 
			this.cmdConfig.Font = new System.Drawing.Font("Comic Sans MS", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.cmdConfig.Location = new System.Drawing.Point(205, 394);
			this.cmdConfig.Name = "cmdConfig";
			this.cmdConfig.Size = new System.Drawing.Size(112, 37);
			this.cmdConfig.TabIndex = 1;
			this.cmdConfig.Text = "&Config";
			this.cmdConfig.Click += new System.EventHandler(this.cmdConfig_Click);
			// 
			// cmdStart
			// 
			this.cmdStart.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.cmdStart.Font = new System.Drawing.Font("Comic Sans MS", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.cmdStart.Location = new System.Drawing.Point(51, 394);
			this.cmdStart.Name = "cmdStart";
			this.cmdStart.Size = new System.Drawing.Size(112, 37);
			this.cmdStart.TabIndex = 1;
			this.cmdStart.Text = "&Start !";
			// 
			// cmdExit
			// 
			this.cmdExit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cmdExit.Font = new System.Drawing.Font("Comic Sans MS", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.cmdExit.Location = new System.Drawing.Point(343, 394);
			this.cmdExit.Name = "cmdExit";
			this.cmdExit.Size = new System.Drawing.Size(113, 37);
			this.cmdExit.TabIndex = 2;
			this.cmdExit.Text = "&Exit";
			// 
			// Splash
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.cmdExit;
			this.ClientSize = new System.Drawing.Size(542, 440);
			this.ControlBox = false;
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.cmdExit,
																		  this.cmdStart,
																		  this.cmdConfig,
																		  this.PicGameField});
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.KeyPreview = true;
			this.Name = "Splash";
			this.Text = ".Netterpillars";
			this.ResumeLayout(false);

		}
		#endregion
    
		private void cmdConfig_Click(System.Object sender, System.EventArgs e) {
			Config WinConfig;
			WinConfig = new Config();
			WinConfig.ShowDialog();
			WinConfig.Dispose();
		}
	}
}