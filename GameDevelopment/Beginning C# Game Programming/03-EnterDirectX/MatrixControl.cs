using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace EnterDirectX {
	/// <summary>
	/// Summary description for MatrixControl.
	/// </summary>
	public class MatrixControl : System.Windows.Forms.Form {
		#region Form Components

		internal System.Windows.Forms.Label lblRotZ;
		internal System.Windows.Forms.Label lblRotY;
		internal System.Windows.Forms.Label LblRotX;
		internal System.Windows.Forms.CheckBox chkAuto;
		internal System.Windows.Forms.GroupBox grpTranslation;
		internal System.Windows.Forms.Label lblTranY;
		internal System.Windows.Forms.Label lblTranZ;
		internal System.Windows.Forms.Label lblTranX;
		internal System.Windows.Forms.Label lblScaX;
		internal System.Windows.Forms.Label lblScaY;
		internal System.Windows.Forms.Label lblScaZ;
		#endregion

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private bool endTest = false;

		public bool EndTest {
			get { return endTest; }
		}

		private Device device = null;
		private static int x = 0;

		private VertexBuffer vertBuffer = null;
		private Texture[] textures = new Texture[10];
		internal System.Windows.Forms.TrackBar RotationX;
		internal System.Windows.Forms.TrackBar RotationY;
		internal System.Windows.Forms.TrackBar RotationZ;
		internal System.Windows.Forms.TrackBar TranslationX;
		internal System.Windows.Forms.TrackBar TranslationY;
		internal System.Windows.Forms.TrackBar TranslationZ;
		internal System.Windows.Forms.TrackBar ScaleX;
		internal System.Windows.Forms.TrackBar ScaleY;
		internal System.Windows.Forms.TrackBar ScaleZ;
		private System.Windows.Forms.GroupBox grpRotation;
		private System.Windows.Forms.GroupBox grpScale;

		// Simple textured vertices constant and structure
		private const VertexFormats customVertex  = VertexFormats.Position | VertexFormats.Texture1;
		private int numVerts = 36;

		public MatrixControl() {
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

		public void DisposeD3D() {
			for(int i = 0; i < 10; i++) {
				if(textures[i] != null) {
					textures[i].Dispose();
					textures[i] = null;
				}
			}

			if(vertBuffer != null) {
				vertBuffer.Dispose();
				vertBuffer = null;
			}

			if(device != null) {
				device.Dispose();
				device = null;
			}
		}

		public bool InitD3D(IntPtr winHandle) {
			DisplayMode DispMode = 
				Manager.Adapters[Manager.Adapters.Default.Adapter].CurrentDisplayMode;
			PresentParameters presentParams = new PresentParameters();
			// Define the presentation parameters
			presentParams.Windowed = true;
			presentParams.SwapEffect = SwapEffect.Discard;
			presentParams.BackBufferFormat = DispMode.Format;
			presentParams.EnableAutoDepthStencil = true;
			presentParams.AutoDepthStencilFormat = DepthFormat.D16;
			// Try to create the device
			try {
				device = new Device(Manager.Adapters.Default.Adapter, 
					DeviceType.Hardware, winHandle, 
					CreateFlags.SoftwareVertexProcessing, presentParams);
				// Turn off culling => front and back of the triangles are visible
				device.RenderState.CullMode = Cull.None;
				// Turn off D3D lighting
				device.RenderState.Lighting = false;
				// Turn on ZBuffer
				device.RenderState.ZBufferEnable = true;
				device.VertexFormat = customVertex;
				//  Set the Projection Matrix to use a orthogonal view
				device.Transform.Projection = Matrix.OrthoLH(300, 200, -200, +200);
				return true;
			}
			catch {
				return false;
			}
		}

		public bool CreateCube() {
			try {
				string txName;
				for(int i=1; i<=10; i++) {
					txName = Application.StartupPath+@"\..\..\Images\Walk"+i.ToString()+".bmp";
					textures[i-1] = 
						TextureLoader.FromFile(device, txName);
				}
				vertBuffer = new VertexBuffer(typeof(CustomVertex.PositionTextured), numVerts, device, Usage.WriteOnly, CustomVertex.PositionTextured.Format, Pool.Default);
				vertBuffer.Created += new EventHandler(this.OnVertexBufferCreate);
				OnVertexBufferCreate(vertBuffer, null);
				return true;
			}
			catch {
				return false;
			}
		}


		private void OnVertexBufferCreate(object sender, EventArgs e) {
			VertexBuffer buffer = (VertexBuffer)sender;
			CustomVertex.PositionTextured[] verts = new CustomVertex.PositionTextured[numVerts];
		// 1st facet --------------------------------------------------------- 
			//triangle 1
			verts[0] = new CustomVertex.PositionTextured(0, 0, 0, 0, 0);
			verts[1] = new CustomVertex.PositionTextured(90, 0, 0, 1, 0);
			verts[2] = new CustomVertex.PositionTextured(0, 90, 0, 0, 1);

			//triangle 2
			verts[3] = new CustomVertex.PositionTextured(0, 90, 0, 0, 1);
			verts[4] = new CustomVertex.PositionTextured(90, 0, 0, 1, 0);
			verts[5] = new CustomVertex.PositionTextured(90, 90, 0, 1, 1);

			// 2nd facet --------------------------------------------------------- 
			//triangle 1
			verts[6] = new CustomVertex.PositionTextured(90, 0, 0, 0, 0);
			verts[7] = new CustomVertex.PositionTextured(90, 90, 0, 1, 0);
			verts[8] = new CustomVertex.PositionTextured(90, 0, 90, 0, 1);

			//triangle 2
			verts[9] = new CustomVertex.PositionTextured(90, 0, 90, 0, 1);
			verts[10] = new CustomVertex.PositionTextured(90, 90, 0, 1, 0);
			verts[11] = new CustomVertex.PositionTextured(90, 90, 90, 1, 1);

			// 3rd facet --------------------------------------------------------- 
			//triangle 1
			verts[12] = new CustomVertex.PositionTextured(0, 0, 0, 0, 0);
			verts[13] = new CustomVertex.PositionTextured(90, 0, 0, 1, 0);
			verts[14] = new CustomVertex.PositionTextured(0, 0, 90, 0, 1);

			//triangle 2
			verts[15] = new CustomVertex.PositionTextured(0, 0, 90, 0, 1);
			verts[16] = new CustomVertex.PositionTextured(90, 0, 0, 1, 0);
			verts[17] = new CustomVertex.PositionTextured(90, 0, 90, 1, 1);


			// 4th facet --------------------------------------------------------- 
			//triangle 1
			verts[18] = new CustomVertex.PositionTextured(0, 0, 0, 0, 0);
			verts[19] = new CustomVertex.PositionTextured(0, 90, 0, 1, 0);
			verts[20] = new CustomVertex.PositionTextured(0, 0, 90, 0, 1);

			//triangle 2
			verts[21] = new CustomVertex.PositionTextured(0, 0, 90, 0, 1);
			verts[22] = new CustomVertex.PositionTextured(0, 90, 0, 1, 0);
			verts[23] = new CustomVertex.PositionTextured(0, 90, 90, 1, 1);

			// 5th facet --------------------------------------------------------- 
			//triangle 1
			verts[24] = new CustomVertex.PositionTextured(0, 0, 90, 0, 0);
			verts[25] = new CustomVertex.PositionTextured(90, 0, 90, 1, 0);
			verts[26] = new CustomVertex.PositionTextured(0, 90, 90, 0, 1);

			//triangle 2
			verts[27] = new CustomVertex.PositionTextured(0, 90, 90, 0, 1);
			verts[28] = new CustomVertex.PositionTextured(90, 0, 90, 1, 0);
			verts[29] = new CustomVertex.PositionTextured(90, 90, 90, 1, 1);

			// 6th facet --------------------------------------------------------- 
			//triangle 1
			verts[30] = new CustomVertex.PositionTextured(0, 90, 0, 0, 0);
			verts[31] = new CustomVertex.PositionTextured(90, 90, 0, 1, 0);
			verts[32] = new CustomVertex.PositionTextured(0, 90, 90, 0, 1);

			//triangle 2
			verts[33] = new CustomVertex.PositionTextured(0, 90, 90, 0, 1);
			verts[34] = new CustomVertex.PositionTextured(90, 90, 0, 1, 0);
			verts[35] = new CustomVertex.PositionTextured(90, 90, 90, 1, 1);

			buffer.SetData(verts, 0, LockFlags.None);
		}

		public void Render() {
			int Tick;
			if ((device==null)) return;

			// Move the cube automatically
			if (chkAuto.Checked) {
				Tick = Environment.TickCount;
				device.Transform.World = Matrix.RotationAxis(
					new Vector3((float)Math.Cos((double)Tick/3000.0F), 1, 
					(float)Math.Sin((double)Tick/3000.0F)), Tick/3000.0F);
			}
			device.Clear(ClearFlags.Target|ClearFlags.ZBuffer, Color.FromArgb(255, 0, 0, 255), 1.0F, 0);
			device.BeginScene();

			// Show one texture a time, in order to create the illusion of a walking guy
			device.SetTexture(0, textures[x]);
			x = (x == 9) ? 0 : x+1; //If x is 9, set to 0, otherwise increment x
			device.SetStreamSource(0, vertBuffer, 0);

			device.DrawPrimitives(PrimitiveType.TriangleList, 0, numVerts/3);
			device.EndScene();
			try {
				device.Present();
			}
			catch {
				// This can lead to an error if the window is closed while the scene is being rendered
			}
		}
	
		private void Transformations_ValueChanged(object sender, System.EventArgs e) {
			if(device != null) {
				device.Transform.World = Matrix.Identity;
				RotationMatrices((float)RotationX.Value, (float)RotationY.Value, (float)RotationZ.Value);
				TranslationMatrices((float)TranslationX.Value, (float)TranslationY.Value, (float)TranslationZ.Value);
				ScaleMatrices((float)ScaleX.Value, (float)ScaleY.Value, (float)ScaleZ.Value);
			}
		}

		//The following functions create the transformation matrices for each operation
		public void RotationMatrices(float x, float y, float z) {
			device.Transform.World = Matrix.Multiply(device.Transform.World, Matrix.RotationX((float)(x * Math.PI / 180)));
			device.Transform.World = Matrix.Multiply(device.Transform.World, Matrix.RotationY((float)(y * Math.PI / 180)));
			device.Transform.World = Matrix.Multiply(device.Transform.World, Matrix.RotationZ((float)(z * Math.PI / 180)));
		}

		public void TranslationMatrices(float x, float y, float z) {
			device.Transform.World = Matrix.Multiply(device.Transform.World, Matrix.Translation(x, y, z));
		}

		public void ScaleMatrices(float x, float y, float z) {
			device.Transform.World = Matrix.Multiply(device.Transform.World, Matrix.Scaling(x / 100, y / 100, z / 100));
		}

		private void MatrixControl_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e) {
			if(e.KeyCode == Keys.Escape) {
				endTest = true;
			}
		}

		private void chkAuto_CheckedChanged(object sender, System.EventArgs e) {
			// Forces the execution of the Transformations_ValueChanged event
			Transformations_ValueChanged(sender, e);
		}

		private void MatrixControl_Closing(object sender, System.ComponentModel.CancelEventArgs e) {
			DisposeD3D();
			endTest = true;
		}


		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			this.lblRotZ = new System.Windows.Forms.Label();
			this.lblRotY = new System.Windows.Forms.Label();
			this.LblRotX = new System.Windows.Forms.Label();
			this.chkAuto = new System.Windows.Forms.CheckBox();
			this.grpTranslation = new System.Windows.Forms.GroupBox();
			this.lblTranY = new System.Windows.Forms.Label();
			this.lblTranZ = new System.Windows.Forms.Label();
			this.lblTranX = new System.Windows.Forms.Label();
			this.TranslationX = new System.Windows.Forms.TrackBar();
			this.TranslationY = new System.Windows.Forms.TrackBar();
			this.TranslationZ = new System.Windows.Forms.TrackBar();
			this.lblScaX = new System.Windows.Forms.Label();
			this.lblScaY = new System.Windows.Forms.Label();
			this.lblScaZ = new System.Windows.Forms.Label();
			this.RotationX = new System.Windows.Forms.TrackBar();
			this.RotationY = new System.Windows.Forms.TrackBar();
			this.RotationZ = new System.Windows.Forms.TrackBar();
			this.grpRotation = new System.Windows.Forms.GroupBox();
			this.ScaleX = new System.Windows.Forms.TrackBar();
			this.ScaleY = new System.Windows.Forms.TrackBar();
			this.ScaleZ = new System.Windows.Forms.TrackBar();
			this.grpScale = new System.Windows.Forms.GroupBox();
			this.grpTranslation.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.TranslationX)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.TranslationY)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.TranslationZ)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.RotationX)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.RotationY)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.RotationZ)).BeginInit();
			this.grpRotation.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.ScaleX)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ScaleY)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ScaleZ)).BeginInit();
			this.grpScale.SuspendLayout();
			this.SuspendLayout();
			// 
			// lblRotZ
			// 
			this.lblRotZ.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblRotZ.Location = new System.Drawing.Point(8, 128);
			this.lblRotZ.Name = "lblRotZ";
			this.lblRotZ.Size = new System.Drawing.Size(80, 24);
			this.lblRotZ.TabIndex = 5;
			this.lblRotZ.Text = "Z Axis";
			// 
			// lblRotY
			// 
			this.lblRotY.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblRotY.Location = new System.Drawing.Point(8, 80);
			this.lblRotY.Name = "lblRotY";
			this.lblRotY.Size = new System.Drawing.Size(80, 24);
			this.lblRotY.TabIndex = 3;
			this.lblRotY.Text = "Y Axis";
			// 
			// LblRotX
			// 
			this.LblRotX.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.LblRotX.Location = new System.Drawing.Point(8, 32);
			this.LblRotX.Name = "LblRotX";
			this.LblRotX.Size = new System.Drawing.Size(80, 24);
			this.LblRotX.TabIndex = 1;
			this.LblRotX.Text = "X Axis";
			// 
			// chkAuto
			// 
			this.chkAuto.Location = new System.Drawing.Point(16, 216);
			this.chkAuto.Name = "chkAuto";
			this.chkAuto.Size = new System.Drawing.Size(88, 24);
			this.chkAuto.TabIndex = 8;
			this.chkAuto.Text = "Auto Move";
			this.chkAuto.CheckedChanged += new System.EventHandler(this.chkAuto_CheckedChanged);
			// 
			// grpTranslation
			// 
			this.grpTranslation.Controls.Add(this.lblTranY);
			this.grpTranslation.Controls.Add(this.lblTranZ);
			this.grpTranslation.Controls.Add(this.lblTranX);
			this.grpTranslation.Controls.Add(this.TranslationX);
			this.grpTranslation.Controls.Add(this.TranslationY);
			this.grpTranslation.Controls.Add(this.TranslationZ);
			this.grpTranslation.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.grpTranslation.Location = new System.Drawing.Point(256, 0);
			this.grpTranslation.Name = "grpTranslation";
			this.grpTranslation.Size = new System.Drawing.Size(272, 176);
			this.grpTranslation.TabIndex = 5;
			this.grpTranslation.TabStop = false;
			this.grpTranslation.Text = "Translation";
			// 
			// lblTranY
			// 
			this.lblTranY.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblTranY.Location = new System.Drawing.Point(16, 80);
			this.lblTranY.Name = "lblTranY";
			this.lblTranY.Size = new System.Drawing.Size(80, 24);
			this.lblTranY.TabIndex = 9;
			this.lblTranY.Text = "Y Axis";
			// 
			// lblTranZ
			// 
			this.lblTranZ.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblTranZ.Location = new System.Drawing.Point(16, 128);
			this.lblTranZ.Name = "lblTranZ";
			this.lblTranZ.Size = new System.Drawing.Size(80, 24);
			this.lblTranZ.TabIndex = 11;
			this.lblTranZ.Text = "Z Axis";
			// 
			// lblTranX
			// 
			this.lblTranX.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblTranX.Location = new System.Drawing.Point(16, 32);
			this.lblTranX.Name = "lblTranX";
			this.lblTranX.Size = new System.Drawing.Size(80, 24);
			this.lblTranX.TabIndex = 7;
			this.lblTranX.Text = "X Axis";
			// 
			// TranslationX
			// 
			this.TranslationX.LargeChange = 90;
			this.TranslationX.Location = new System.Drawing.Point(88, 32);
			this.TranslationX.Maximum = 200;
			this.TranslationX.Minimum = -200;
			this.TranslationX.Name = "TranslationX";
			this.TranslationX.Size = new System.Drawing.Size(176, 45);
			this.TranslationX.TabIndex = 9;
			this.TranslationX.TickFrequency = 10;
			this.TranslationX.ValueChanged += new System.EventHandler(this.Transformations_ValueChanged);
			// 
			// TranslationY
			// 
			this.TranslationY.LargeChange = 90;
			this.TranslationY.Location = new System.Drawing.Point(88, 80);
			this.TranslationY.Maximum = 200;
			this.TranslationY.Minimum = -200;
			this.TranslationY.Name = "TranslationY";
			this.TranslationY.Size = new System.Drawing.Size(176, 45);
			this.TranslationY.TabIndex = 10;
			this.TranslationY.TickFrequency = 10;
			this.TranslationY.ValueChanged += new System.EventHandler(this.Transformations_ValueChanged);
			// 
			// TranslationZ
			// 
			this.TranslationZ.LargeChange = 10;
			this.TranslationZ.Location = new System.Drawing.Point(88, 128);
			this.TranslationZ.Maximum = 200;
			this.TranslationZ.Minimum = -200;
			this.TranslationZ.Name = "TranslationZ";
			this.TranslationZ.Size = new System.Drawing.Size(176, 45);
			this.TranslationZ.TabIndex = 10;
			this.TranslationZ.TickFrequency = 10;
			this.TranslationZ.ValueChanged += new System.EventHandler(this.Transformations_ValueChanged);
			// 
			// lblScaX
			// 
			this.lblScaX.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblScaX.Location = new System.Drawing.Point(8, 24);
			this.lblScaX.Name = "lblScaX";
			this.lblScaX.Size = new System.Drawing.Size(64, 24);
			this.lblScaX.TabIndex = 13;
			this.lblScaX.Text = "X Axis";
			// 
			// lblScaY
			// 
			this.lblScaY.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblScaY.Location = new System.Drawing.Point(80, 24);
			this.lblScaY.Name = "lblScaY";
			this.lblScaY.Size = new System.Drawing.Size(64, 24);
			this.lblScaY.TabIndex = 15;
			this.lblScaY.Text = "Y Axis";
			// 
			// lblScaZ
			// 
			this.lblScaZ.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblScaZ.Location = new System.Drawing.Point(152, 24);
			this.lblScaZ.Name = "lblScaZ";
			this.lblScaZ.Size = new System.Drawing.Size(64, 24);
			this.lblScaZ.TabIndex = 17;
			this.lblScaZ.Text = "Z Axis";
			// 
			// RotationX
			// 
			this.RotationX.LargeChange = 90;
			this.RotationX.Location = new System.Drawing.Point(80, 32);
			this.RotationX.Maximum = 360;
			this.RotationX.Name = "RotationX";
			this.RotationX.Size = new System.Drawing.Size(176, 45);
			this.RotationX.TabIndex = 9;
			this.RotationX.TickFrequency = 10;
			this.RotationX.ValueChanged += new System.EventHandler(this.Transformations_ValueChanged);
			// 
			// RotationY
			// 
			this.RotationY.LargeChange = 90;
			this.RotationY.Location = new System.Drawing.Point(80, 80);
			this.RotationY.Maximum = 360;
			this.RotationY.Name = "RotationY";
			this.RotationY.Size = new System.Drawing.Size(176, 45);
			this.RotationY.TabIndex = 10;
			this.RotationY.TickFrequency = 10;
			this.RotationY.ValueChanged += new System.EventHandler(this.Transformations_ValueChanged);
			// 
			// RotationZ
			// 
			this.RotationZ.CausesValidation = false;
			this.RotationZ.LargeChange = 90;
			this.RotationZ.Location = new System.Drawing.Point(80, 128);
			this.RotationZ.Maximum = 360;
			this.RotationZ.Name = "RotationZ";
			this.RotationZ.Size = new System.Drawing.Size(176, 45);
			this.RotationZ.TabIndex = 10;
			this.RotationZ.TabStop = false;
			this.RotationZ.TickFrequency = 10;
			this.RotationZ.ValueChanged += new System.EventHandler(this.Transformations_ValueChanged);
			// 
			// grpRotation
			// 
			this.grpRotation.Controls.Add(this.LblRotX);
			this.grpRotation.Controls.Add(this.RotationY);
			this.grpRotation.Controls.Add(this.RotationZ);
			this.grpRotation.Controls.Add(this.RotationX);
			this.grpRotation.Controls.Add(this.lblRotZ);
			this.grpRotation.Controls.Add(this.lblRotY);
			this.grpRotation.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.grpRotation.Location = new System.Drawing.Point(0, 0);
			this.grpRotation.Name = "grpRotation";
			this.grpRotation.Size = new System.Drawing.Size(256, 176);
			this.grpRotation.TabIndex = 11;
			this.grpRotation.TabStop = false;
			this.grpRotation.Text = "Rotation";
			// 
			// ScaleX
			// 
			this.ScaleX.Location = new System.Drawing.Point(16, 56);
			this.ScaleX.Maximum = 200;
			this.ScaleX.Minimum = 1;
			this.ScaleX.Name = "ScaleX";
			this.ScaleX.Orientation = System.Windows.Forms.Orientation.Vertical;
			this.ScaleX.Size = new System.Drawing.Size(45, 144);
			this.ScaleX.TabIndex = 12;
			this.ScaleX.TickFrequency = 10;
			this.ScaleX.Value = 1;
			this.ScaleX.ValueChanged += new System.EventHandler(this.Transformations_ValueChanged);
			// 
			// ScaleY
			// 
			this.ScaleY.Location = new System.Drawing.Point(88, 56);
			this.ScaleY.Maximum = 200;
			this.ScaleY.Minimum = 1;
			this.ScaleY.Name = "ScaleY";
			this.ScaleY.Orientation = System.Windows.Forms.Orientation.Vertical;
			this.ScaleY.Size = new System.Drawing.Size(45, 144);
			this.ScaleY.TabIndex = 12;
			this.ScaleY.TickFrequency = 10;
			this.ScaleY.Value = 1;
			this.ScaleY.ValueChanged += new System.EventHandler(this.Transformations_ValueChanged);
			// 
			// ScaleZ
			// 
			this.ScaleZ.Location = new System.Drawing.Point(160, 56);
			this.ScaleZ.Maximum = 200;
			this.ScaleZ.Minimum = 1;
			this.ScaleZ.Name = "ScaleZ";
			this.ScaleZ.Orientation = System.Windows.Forms.Orientation.Vertical;
			this.ScaleZ.Size = new System.Drawing.Size(45, 144);
			this.ScaleZ.TabIndex = 12;
			this.ScaleZ.TickFrequency = 10;
			this.ScaleZ.Value = 1;
			this.ScaleZ.ValueChanged += new System.EventHandler(this.Transformations_ValueChanged);
			// 
			// grpScale
			// 
			this.grpScale.Controls.Add(this.ScaleX);
			this.grpScale.Controls.Add(this.ScaleZ);
			this.grpScale.Controls.Add(this.lblScaX);
			this.grpScale.Controls.Add(this.lblScaY);
			this.grpScale.Controls.Add(this.ScaleY);
			this.grpScale.Controls.Add(this.lblScaZ);
			this.grpScale.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.grpScale.Location = new System.Drawing.Point(128, 176);
			this.grpScale.Name = "grpScale";
			this.grpScale.Size = new System.Drawing.Size(232, 208);
			this.grpScale.TabIndex = 18;
			this.grpScale.TabStop = false;
			this.grpScale.Text = "Scale";
			// 
			// MatrixControl
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(544, 398);
			this.Controls.Add(this.grpScale);
			this.Controls.Add(this.grpRotation);
			this.Controls.Add(this.chkAuto);
			this.Controls.Add(this.grpTranslation);
			this.Name = "MatrixControl";
			this.Text = "MatrixControl";
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.MatrixControl_KeyDown);
			this.Closing += new System.ComponentModel.CancelEventHandler(this.MatrixControl_Closing);
			this.grpTranslation.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.TranslationX)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.TranslationY)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.TranslationZ)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.RotationX)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.RotationY)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.RotationZ)).EndInit();
			this.grpRotation.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.ScaleX)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ScaleY)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ScaleZ)).EndInit();
			this.grpScale.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

	}
}