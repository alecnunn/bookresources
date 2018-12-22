using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace EnterDirectX {

	public class LightTest:
		System.Windows.Forms.Form {
		#region " Windows Form Designer generated code "

		public LightTest() : base() {        

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

		//Required by the Windows Form Designer
		private System.ComponentModel.IContainer components;

		//NOTE: The following procedure is required by the Windows Form Designer
		//It can be modified using the Windows Form Designer.  
		//Do not modify it using the code editor.
		[System.Diagnostics.DebuggerStepThrough]
		private void InitializeComponent() {
			// 
			// LightTest
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(385, 350);
			this.ControlBox = false;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.KeyPreview = true;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "LightTest";
			this.ShowInTaskbar = false;
			this.Text = "Light Test Window";
			this.TopMost = true;
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.LightTestWindow_KeyDown);

		}

		#endregion
		private bool endTest = false;

		public bool EndTest {
			get { return endTest; }
		}

		private void LightTestWindow_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e) {
			if (e.KeyCode==Keys.Escape) {
				endTest = true;
			}
		}

	}

}