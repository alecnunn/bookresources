
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace EnterDirectX {
	public class MatrixTest:
		System.Windows.Forms.Form {
		#region " Windows Form Designer generated code "

		public MatrixTest() : base() {        

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
			// MatrixTest
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(362, 328);
			this.ControlBox = false;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.KeyPreview = true;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "MatrixTest";
			this.ShowInTaskbar = false;
			this.Text = "MatrixTestWindow";
			this.TopMost = true;
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.MatrixTest_KeyDown);
			this.Closing += new System.ComponentModel.CancelEventHandler(this.MatrixTest_Closing);

		}

		#endregion
		private bool endTest = false;

		public bool EndTest {
			get { return endTest; }
		}

		private void MatrixTest_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e) {
			if (e.KeyCode==Keys.Escape) {
				endTest = true;
			}
		}

		private void MatrixTest_Closing(object sender, System.ComponentModel.CancelEventArgs e) {
		
		}

	}
}
