using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX.Direct3D;

namespace EnterDirectX {
	/// <summary>
	/// Summary description for FrmDirectX.
	/// </summary>
	public class FrmDirectX : System.Windows.Forms.Form {
		internal System.Windows.Forms.Button CmdClose;
		internal System.Windows.Forms.Button CmdTransparency;
		internal System.Windows.Forms.Button CmdLight;
		internal System.Windows.Forms.Button CmdMatrix;
		internal System.Windows.Forms.Button CmdFullScreen;
		internal System.Windows.Forms.Button CmdWindow;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.ListBox AdaptersListBox;
		private System.Windows.Forms.ListBox DevicesListBox;
		private System.Windows.Forms.ListBox DisplayModesListBox;
		private System.Windows.Forms.ListBox DeviceCapsListBox;
		private System.Windows.Forms.Label DeviceCapsLabel;
		private System.Windows.Forms.Label ResolutionsLabel;
		private System.Windows.Forms.Label DevicesLabel;
		private System.Windows.Forms.Label AdaptersLabel;
		internal System.Windows.Forms.GroupBox TestsGroup;

		Device device;

		public FrmDirectX() {
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
			this.CmdClose = new System.Windows.Forms.Button();
			this.AdaptersListBox = new System.Windows.Forms.ListBox();
			this.DevicesListBox = new System.Windows.Forms.ListBox();
			this.DisplayModesListBox = new System.Windows.Forms.ListBox();
			this.DeviceCapsLabel = new System.Windows.Forms.Label();
			this.ResolutionsLabel = new System.Windows.Forms.Label();
			this.DevicesLabel = new System.Windows.Forms.Label();
			this.AdaptersLabel = new System.Windows.Forms.Label();
			this.DeviceCapsListBox = new System.Windows.Forms.ListBox();
			this.TestsGroup = new System.Windows.Forms.GroupBox();
			this.CmdTransparency = new System.Windows.Forms.Button();
			this.CmdLight = new System.Windows.Forms.Button();
			this.CmdMatrix = new System.Windows.Forms.Button();
			this.CmdFullScreen = new System.Windows.Forms.Button();
			this.CmdWindow = new System.Windows.Forms.Button();
			this.TestsGroup.SuspendLayout();
			this.SuspendLayout();
			// 
			// CmdClose
			// 
			this.CmdClose.Location = new System.Drawing.Point(536, 296);
			this.CmdClose.Name = "CmdClose";
			this.CmdClose.Size = new System.Drawing.Size(88, 29);
			this.CmdClose.TabIndex = 34;
			this.CmdClose.Text = "Close";
			this.CmdClose.Click += new System.EventHandler(this.CmdClose_Click);
			// 
			// AdaptersListBox
			// 
			this.AdaptersListBox.Location = new System.Drawing.Point(8, 32);
			this.AdaptersListBox.Name = "AdaptersListBox";
			this.AdaptersListBox.Size = new System.Drawing.Size(160, 95);
			this.AdaptersListBox.TabIndex = 35;
			this.AdaptersListBox.SelectedIndexChanged += new System.EventHandler(this.AdaptersListBox_SelectedIndexChanged);
			// 
			// DevicesListBox
			// 
			this.DevicesListBox.Location = new System.Drawing.Point(8, 160);
			this.DevicesListBox.Name = "DevicesListBox";
			this.DevicesListBox.Size = new System.Drawing.Size(160, 95);
			this.DevicesListBox.TabIndex = 36;
			this.DevicesListBox.SelectedIndexChanged += new System.EventHandler(this.DevicesListBox_SelectedIndexChanged);
			// 
			// DisplayModesListBox
			// 
			this.DisplayModesListBox.HorizontalScrollbar = true;
			this.DisplayModesListBox.Location = new System.Drawing.Point(8, 288);
			this.DisplayModesListBox.Name = "DisplayModesListBox";
			this.DisplayModesListBox.Size = new System.Drawing.Size(160, 95);
			this.DisplayModesListBox.TabIndex = 37;
			// 
			// DeviceCapsLabel
			// 
			this.DeviceCapsLabel.AutoSize = true;
			this.DeviceCapsLabel.BackColor = System.Drawing.Color.Transparent;
			this.DeviceCapsLabel.Cursor = System.Windows.Forms.Cursors.Default;
			this.DeviceCapsLabel.Font = new System.Drawing.Font("Arial", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.DeviceCapsLabel.ForeColor = System.Drawing.SystemColors.ControlText;
			this.DeviceCapsLabel.Location = new System.Drawing.Point(184, 8);
			this.DeviceCapsLabel.Name = "DeviceCapsLabel";
			this.DeviceCapsLabel.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.DeviceCapsLabel.Size = new System.Drawing.Size(101, 16);
			this.DeviceCapsLabel.TabIndex = 29;
			this.DeviceCapsLabel.Text = "Device Capabilities:";
			// 
			// ResolutionsLabel
			// 
			this.ResolutionsLabel.AutoSize = true;
			this.ResolutionsLabel.BackColor = System.Drawing.Color.Transparent;
			this.ResolutionsLabel.Cursor = System.Windows.Forms.Cursors.Default;
			this.ResolutionsLabel.Font = new System.Drawing.Font("Arial", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.ResolutionsLabel.ForeColor = System.Drawing.SystemColors.ControlText;
			this.ResolutionsLabel.Location = new System.Drawing.Point(8, 264);
			this.ResolutionsLabel.Name = "ResolutionsLabel";
			this.ResolutionsLabel.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.ResolutionsLabel.Size = new System.Drawing.Size(79, 16);
			this.ResolutionsLabel.TabIndex = 28;
			this.ResolutionsLabel.Text = "Display Modes:";
			// 
			// DevicesLabel
			// 
			this.DevicesLabel.AutoSize = true;
			this.DevicesLabel.BackColor = System.Drawing.Color.Transparent;
			this.DevicesLabel.Cursor = System.Windows.Forms.Cursors.Default;
			this.DevicesLabel.Font = new System.Drawing.Font("Arial", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.DevicesLabel.ForeColor = System.Drawing.SystemColors.ControlText;
			this.DevicesLabel.Location = new System.Drawing.Point(3, 141);
			this.DevicesLabel.Name = "DevicesLabel";
			this.DevicesLabel.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.DevicesLabel.TabIndex = 27;
			this.DevicesLabel.Text = "Rendering Devices:";
			// 
			// AdaptersLabel
			// 
			this.AdaptersLabel.AutoSize = true;
			this.AdaptersLabel.BackColor = System.Drawing.Color.Transparent;
			this.AdaptersLabel.Cursor = System.Windows.Forms.Cursors.Default;
			this.AdaptersLabel.Font = new System.Drawing.Font("Arial", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.AdaptersLabel.ForeColor = System.Drawing.SystemColors.ControlText;
			this.AdaptersLabel.Location = new System.Drawing.Point(8, 8);
			this.AdaptersLabel.Name = "AdaptersLabel";
			this.AdaptersLabel.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.AdaptersLabel.Size = new System.Drawing.Size(51, 16);
			this.AdaptersLabel.TabIndex = 26;
			this.AdaptersLabel.Text = "Adapters:";
			// 
			// DeviceCapsListBox
			// 
			this.DeviceCapsListBox.BackColor = System.Drawing.SystemColors.Window;
			this.DeviceCapsListBox.Cursor = System.Windows.Forms.Cursors.Default;
			this.DeviceCapsListBox.Font = new System.Drawing.Font("Verdana", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.DeviceCapsListBox.ForeColor = System.Drawing.SystemColors.WindowText;
			this.DeviceCapsListBox.HorizontalScrollbar = true;
			this.DeviceCapsListBox.ItemHeight = 12;
			this.DeviceCapsListBox.Location = new System.Drawing.Point(184, 32);
			this.DeviceCapsListBox.Name = "DeviceCapsListBox";
			this.DeviceCapsListBox.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.DeviceCapsListBox.Size = new System.Drawing.Size(336, 352);
			this.DeviceCapsListBox.TabIndex = 30;
			// 
			// TestsGroup
			// 
			this.TestsGroup.Controls.Add(this.CmdTransparency);
			this.TestsGroup.Controls.Add(this.CmdLight);
			this.TestsGroup.Controls.Add(this.CmdMatrix);
			this.TestsGroup.Controls.Add(this.CmdFullScreen);
			this.TestsGroup.Controls.Add(this.CmdWindow);
			this.TestsGroup.Location = new System.Drawing.Point(528, 32);
			this.TestsGroup.Name = "TestsGroup";
			this.TestsGroup.Size = new System.Drawing.Size(106, 233);
			this.TestsGroup.TabIndex = 35;
			this.TestsGroup.TabStop = false;
			this.TestsGroup.Text = "Tests";
			// 
			// CmdTransparency
			// 
			this.CmdTransparency.Location = new System.Drawing.Point(13, 104);
			this.CmdTransparency.Name = "CmdTransparency";
			this.CmdTransparency.Size = new System.Drawing.Size(87, 37);
			this.CmdTransparency.TabIndex = 28;
			this.CmdTransparency.Text = "Transparency";
			this.CmdTransparency.Click += new System.EventHandler(this.CmdTransparency_Click);
			// 
			// CmdLight
			// 
			this.CmdLight.Location = new System.Drawing.Point(13, 187);
			this.CmdLight.Name = "CmdLight";
			this.CmdLight.Size = new System.Drawing.Size(87, 29);
			this.CmdLight.TabIndex = 27;
			this.CmdLight.Text = "Light";
			this.CmdLight.Click += new System.EventHandler(this.CmdLight_Click);
			// 
			// CmdMatrix
			// 
			this.CmdMatrix.Location = new System.Drawing.Point(13, 150);
			this.CmdMatrix.Name = "CmdMatrix";
			this.CmdMatrix.Size = new System.Drawing.Size(87, 28);
			this.CmdMatrix.TabIndex = 26;
			this.CmdMatrix.Text = "Matrix ";
			this.CmdMatrix.Click += new System.EventHandler(this.CmdMatrix_Click);
			// 
			// CmdFullScreen
			// 
			this.CmdFullScreen.Location = new System.Drawing.Point(13, 66);
			this.CmdFullScreen.Name = "CmdFullScreen";
			this.CmdFullScreen.Size = new System.Drawing.Size(87, 29);
			this.CmdFullScreen.TabIndex = 25;
			this.CmdFullScreen.Text = "Full Screen";
			this.CmdFullScreen.Click += new System.EventHandler(this.CmdFullScreen_Click);
			// 
			// CmdWindow
			// 
			this.CmdWindow.Location = new System.Drawing.Point(13, 27);
			this.CmdWindow.Name = "CmdWindow";
			this.CmdWindow.Size = new System.Drawing.Size(87, 27);
			this.CmdWindow.TabIndex = 24;
			this.CmdWindow.Text = "Window";
			this.CmdWindow.Click += new System.EventHandler(this.CmdWindow_Click);
			// 
			// FrmDirectX
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(640, 398);
			this.Controls.Add(this.CmdClose);
			this.Controls.Add(this.AdaptersListBox);
			this.Controls.Add(this.DevicesListBox);
			this.Controls.Add(this.DisplayModesListBox);
			this.Controls.Add(this.DeviceCapsLabel);
			this.Controls.Add(this.ResolutionsLabel);
			this.Controls.Add(this.DevicesLabel);
			this.Controls.Add(this.AdaptersLabel);
			this.Controls.Add(this.DeviceCapsListBox);
			this.Controls.Add(this.TestsGroup);
			this.Name = "FrmDirectX";
			this.Text = "FrmDirectX";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.FrmDirectX_Closing);
			this.Load += new System.EventHandler(this.FrmDirectX_Load);
			this.TestsGroup.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion


		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() {
			Application.Run(new FrmDirectX());
		}

		public void ListAdapters() {
			// Add each adapter to the LstAdapters listbox 
			foreach(AdapterInformation info in Manager.Adapters) {
				AdaptersListBox.Items.Add(info.Information.Description);
			}
			// Select the first availiable index, in order to fire the change event 
			AdaptersListBox.SelectedIndex = 0;
		}

		public void ListDevices(int adapter) {
			Caps deviceCaps = Manager.GetDeviceCaps(adapter, DeviceType.Reference);

			// Add each supported device to the DevicesListBox listbox 
			DevicesListBox.Items.Clear();
			// The Reference Rasterizer will always be supported
			DevicesListBox.Items.Add("Reference Rasterizer (REF)");

			// If there's no error when getting the HAL capabilities, 
			// then we have a hardware acceleration board installed
			try {
				deviceCaps = Manager.GetDeviceCaps(adapter, DeviceType.Hardware);
				DevicesListBox.Items.Add("Hardware Acceleration (HAL)");
			}
			catch {
			}
			// Select the first available index, in order to fire the change event 
			DevicesListBox.SelectedIndex = 0;
		}

		private void FrmDirectX_Load(object sender, System.EventArgs e) {
			// Fill the Adapters list
			ListAdapters();
		}

		private void FrmDirectX_Closing(object sender, CancelEventArgs e) {

			if(device != null) {
				device.Dispose();
			}
			device = null;
		}

		private void CmdWindow_Click(object sender, System.EventArgs e) {
			using (WindowTest windowTest = new WindowTest()) {
			
				windowTest.Show();

				// Initialize Direct3D and the device object
				if(!windowTest.InitD3D(windowTest.Handle)) {
					MessageBox.Show("Could not initialize Direct3D.");
					windowTest.Dispose();
					return;
				}			
				else {
					// Load the textures and create the square to show them
					if(!windowTest.CreateTextures()) {
						MessageBox.Show("Could not initialize vertices and textures.");
						return;
					}
				}

				// Uncomment the lines  below to see a smooth walking man
				//int desiredFrameRate = 10;
				//int lastTick = 0;
				while(!windowTest.EndTest) {
					// Force a Frame rate of 10 frames to second on maximum
					//if(System.Environment.TickCount - lastTick >= 1000 / desiredFrameRate) {
					windowTest.Render();
					// Frame rate calculation
					windowTest.Text = "Window Test.  Frame rate: " + DirectXLists.CalcFrameRate().ToString();
					//lastTick = System.Environment.TickCount;
					//}
					Application.DoEvents();
				}
			}
		}

		private void CmdFullScreen_Click(object sender, System.EventArgs e) {
			using (FullScreenTest fullScreenTest = new FullScreenTest()) {
				fullScreenTest.Show();

				// Initialize Direct3D and the device object
				if(!fullScreenTest.InitD3D(fullScreenTest.Handle)) {
					MessageBox.Show("Could not initialize Direct3D.");
					fullScreenTest.Dispose();
					return;
				}
				else {
					// Load the textures and create the square to show them
					if(!fullScreenTest.CreateTextures()) {
						MessageBox.Show("Could not initialize vertices and textures.");
						fullScreenTest.DisposeD3D();
						fullScreenTest.Dispose();
						return;
					}
				}

				// If we have no errors, then enter the rendering loop
				while(!fullScreenTest.EndTest) {
					try {
						fullScreenTest.Render();
						Application.DoEvents();
					}
					catch {
						// Ignore any errors that may arise when the window close
					}
				}
			}
		}

		private void CmdTransparency_Click(object sender, System.EventArgs e) {	
			using (TransparentTest transparentTest = new TransparentTest()) {
				transparentTest.Show();

				// Initialize Direct3D and the device object
				if(!transparentTest.InitD3D(transparentTest.Handle)) {
					MessageBox.Show("Could not initialize Direct3D.");
					transparentTest.Dispose();
					return;
				}
				else {
					// Load the textures and create the square to show them
					if(!(transparentTest.CreateTextures() && transparentTest.CreateTransparentVertices(0, 0))) {
						MessageBox.Show("Could not initialize vertices and textures.");
						transparentTest.DisposeD3D();
						transparentTest.Dispose();
						return;
					}
				}

				// If we have no errors, then enter the rendering loop
				while(!transparentTest.EndTest) {
					transparentTest.Render();
					// Frame rate calculation
					transparentTest.Text = 
						"Transparency Test.  Frame rate: " + DirectXLists.CalcFrameRate().ToString();
					Application.DoEvents();
				}
			}		
		}

		private void CmdMatrix_Click(object sender, System.EventArgs e) {
			using (MatrixControl matrixControl = new MatrixControl()) {
				MatrixTest matrixTest = new MatrixTest();
				matrixControl.Show();
				matrixTest.Show();

				// Initialize Direct3D and the device object
				if(!matrixControl.InitD3D(matrixTest.Handle)) {
					MessageBox.Show("Could not initialize Direct3D.");
					matrixControl.Dispose();
					return;
				}
				else {
					// Load the textures and create the cube to show them
					if(!matrixControl.CreateCube()) {
						MessageBox.Show("Could not initialize geometry.");
						matrixControl.DisposeD3D();
						matrixControl.Dispose();
						return;
					}
				}

				// Start with a simple rotation, to position the cube more nicely;
				//  and with no scale (100% of the original size)
			
				matrixControl.RotationX.Value = 45;
				matrixControl.RotationY.Value = 45;
				matrixControl.RotationZ.Value = 45;
				matrixControl.ScaleX.Value = 100;
				matrixControl.ScaleY.Value = 100;
				matrixControl.ScaleZ.Value = 100;

				// Ends the test if ESC is pressed in any of the 2 windows
				while(!matrixControl.EndTest && !matrixTest.EndTest) {
					matrixControl.Render();
					// Frame rate calculation
					matrixTest.Text = "Matrix Tests.  Frame Rate: " + DirectXLists.CalcFrameRate().ToString();
					Application.DoEvents();
				}
				matrixTest.Close();
			}		
		}

		private void CmdLight_Click(object sender, System.EventArgs e) {
			LightControl winLightControl = new LightControl();
			using (LightTest lightTest = new LightTest()) {

				winLightControl.Show();
				lightTest.Show();

				// Initialize Direct3D and the device object
				if(!winLightControl.InitD3D(lightTest.Handle)) {
					MessageBox.Show("Could not initialize Direct3D.");
					winLightControl.Dispose();
				}			
				else {
					// Load the textures and create the vertices
					if(!winLightControl.CreateTextures()) {
						MessageBox.Show("Could not initialize the textures and vertices.");
						winLightControl.DisposeD3D();
						winLightControl.Dispose();
					}
				}

				// Start with full white light in all vertices
			
				winLightControl.Red1.Value = 255;
				winLightControl.Green1.Value = 255;
				winLightControl.Blue1.Value = 255;
				winLightControl.Red2.Value = 255;
				winLightControl.Green2.Value = 255;
				winLightControl.Blue2.Value = 255;
				winLightControl.Red3.Value = 255;
				winLightControl.Green3.Value = 255;
				winLightControl.Blue3.Value = 255;
				winLightControl.Red4.Value = 255;
				winLightControl.Green4.Value = 255;
				winLightControl.Blue4.Value = 255;
			

				// Ends the test if ESC is pressed in any of the 2 windows
				while(!winLightControl.EndTest && !lightTest.EndTest) {
					winLightControl.Render();
					// Frame rate calculation
					lightTest.Text = "Light Test.  Frame Rate: " + DirectXLists.CalcFrameRate().ToString();
					Application.DoEvents();
				}
			}
		}

		private void CmdClose_Click(object sender, System.EventArgs e) {
			this.Close();
		}

		private void AdaptersListBox_SelectedIndexChanged(object sender, System.EventArgs e) {
			// Update the devices list every time a new adapter is chosen 
			ListDevices(AdaptersListBox.SelectedIndex);
		}

		private void DevicesListBox_SelectedIndexChanged(object sender, System.EventArgs e) {
			// The first entry in DevicesListBox is the Reference Rasterizer
			DeviceType deviceType = (DevicesListBox.SelectedIndex == 0) ? DeviceType.Reference : DeviceType.Hardware;
			ListDisplayModes(AdaptersListBox.SelectedIndex, deviceType, Format.X8R8G8B8);
			ListDisplayModes(AdaptersListBox.SelectedIndex, deviceType, Format.X1R5G5B5);
			ListDisplayModes(AdaptersListBox.SelectedIndex, deviceType, Format.R5G6B5);
			ListDeviceCaps(AdaptersListBox.SelectedIndex, deviceType);
		}

		private void ListDisplayModes(int adapter, DeviceType renderer, Format adapterFormat) {
			DisplayModesListBox.Items.Clear();
			foreach(DisplayMode dispMode in Manager.Adapters[adapter].SupportedDisplayModes) {
				// Check to see if the display mode is supported by the device
				if(Manager.CheckDeviceType(adapter, renderer, dispMode.Format, dispMode.Format, false)) {
					// Fill the display modes list with the width, height, the mode name and the refresh rate              
					DisplayModesListBox.Items.Add(dispMode.Width + "x" + dispMode.Height
						+ "   ( " + dispMode.Format.ToString() + " - " 
						+ dispMode.RefreshRate + "Khz)");
				}
			}
		}

		private void ListDeviceCaps(int adapter, DeviceType deviceType) {
			DeviceCapsListBox.Items.Clear();
			Caps caps = Manager.GetDeviceCaps(adapter, deviceType);
			DirectXLists.ListGeneralCaps(caps, DeviceCapsListBox);
			DirectXLists.ListDevCaps(caps.DeviceCaps, DeviceCapsListBox);
			DirectXLists.ListDriverCaps(caps.DriverCaps, DeviceCapsListBox);
			DirectXLists.ListRasterCaps(caps.RasterCaps, DeviceCapsListBox);
			DirectXLists.ListTextureCaps(caps.TextureCaps, DeviceCapsListBox);
		}
	}
}
