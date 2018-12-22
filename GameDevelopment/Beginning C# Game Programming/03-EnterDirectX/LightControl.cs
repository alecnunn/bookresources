using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace EnterDirectX {
	/// <summary>
	/// Summary description for LC.
	/// </summary>
	public class LightControl : System.Windows.Forms.Form {
		#region class components for Windows Form
		internal System.Windows.Forms.TabControl TabControl1;
		internal System.Windows.Forms.TabPage Vertex1;
		internal System.Windows.Forms.GroupBox GroupBox1;
		internal System.Windows.Forms.Label lblBlue1;
		internal System.Windows.Forms.NumericUpDown Blue1;
		internal System.Windows.Forms.Label lblGreen1;
		internal System.Windows.Forms.NumericUpDown Green1;
		internal System.Windows.Forms.Label lblRed1;
		internal System.Windows.Forms.NumericUpDown Red1;
		internal System.Windows.Forms.TabPage Vertex2;
		internal System.Windows.Forms.GroupBox GroupBox2;
		internal System.Windows.Forms.Label lblBlue2;
		internal System.Windows.Forms.NumericUpDown Blue2;
		internal System.Windows.Forms.Label lblGreen2;
		internal System.Windows.Forms.NumericUpDown Green2;
		internal System.Windows.Forms.Label LblRed2;
		internal System.Windows.Forms.NumericUpDown Red2;
		internal System.Windows.Forms.TabPage Vertex3;
		internal System.Windows.Forms.GroupBox GroupBox3;
		internal System.Windows.Forms.Label lblBlue3;
		internal System.Windows.Forms.NumericUpDown Blue3;
		internal System.Windows.Forms.Label lblGreen3;
		internal System.Windows.Forms.NumericUpDown Green3;
		internal System.Windows.Forms.Label lblRed3;
		internal System.Windows.Forms.NumericUpDown Red3;
		internal System.Windows.Forms.TabPage Vertex4;
		internal System.Windows.Forms.GroupBox GroupBox4;
		internal System.Windows.Forms.Label lblBlue4;
		internal System.Windows.Forms.NumericUpDown Blue4;
		internal System.Windows.Forms.Label lblGreen4;
		internal System.Windows.Forms.NumericUpDown Green4;
		internal System.Windows.Forms.Label lblRed4;
		internal System.Windows.Forms.NumericUpDown Red4;
		#endregion

		private System.ComponentModel.Container components = null;
		private static int x = 0;
		private bool endTest = false;

		public bool EndTest {
			get { return endTest; }
		}

		private Device device;
		private int numVerts = 4;
		private VertexBuffer vertBuffer;
		private const int numTextures = 10;
		private Texture[] textures = new Texture[numTextures];
		private const VertexFormats customVertex = VertexFormats.Transformed | 
			VertexFormats.Diffuse | 
			VertexFormats.Texture1;
		private struct CustomVertex {
			public float X;
			public float Y;
			public float Z;
			public float rhw;
			public int color;
			public float tu;
			public float tv;
		}


		public LightControl() {
			InitializeComponent();
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
			this.TabControl1 = new System.Windows.Forms.TabControl();
			this.Vertex1 = new System.Windows.Forms.TabPage();
			this.GroupBox1 = new System.Windows.Forms.GroupBox();
			this.lblBlue1 = new System.Windows.Forms.Label();
			this.Blue1 = new System.Windows.Forms.NumericUpDown();
			this.lblGreen1 = new System.Windows.Forms.Label();
			this.Green1 = new System.Windows.Forms.NumericUpDown();
			this.lblRed1 = new System.Windows.Forms.Label();
			this.Red1 = new System.Windows.Forms.NumericUpDown();
			this.Vertex2 = new System.Windows.Forms.TabPage();
			this.GroupBox2 = new System.Windows.Forms.GroupBox();
			this.lblBlue2 = new System.Windows.Forms.Label();
			this.Blue2 = new System.Windows.Forms.NumericUpDown();
			this.lblGreen2 = new System.Windows.Forms.Label();
			this.Green2 = new System.Windows.Forms.NumericUpDown();
			this.LblRed2 = new System.Windows.Forms.Label();
			this.Red2 = new System.Windows.Forms.NumericUpDown();
			this.Vertex3 = new System.Windows.Forms.TabPage();
			this.GroupBox3 = new System.Windows.Forms.GroupBox();
			this.lblBlue3 = new System.Windows.Forms.Label();
			this.Blue3 = new System.Windows.Forms.NumericUpDown();
			this.lblGreen3 = new System.Windows.Forms.Label();
			this.Green3 = new System.Windows.Forms.NumericUpDown();
			this.lblRed3 = new System.Windows.Forms.Label();
			this.Red3 = new System.Windows.Forms.NumericUpDown();
			this.Vertex4 = new System.Windows.Forms.TabPage();
			this.GroupBox4 = new System.Windows.Forms.GroupBox();
			this.lblBlue4 = new System.Windows.Forms.Label();
			this.Blue4 = new System.Windows.Forms.NumericUpDown();
			this.lblGreen4 = new System.Windows.Forms.Label();
			this.Green4 = new System.Windows.Forms.NumericUpDown();
			this.lblRed4 = new System.Windows.Forms.Label();
			this.Red4 = new System.Windows.Forms.NumericUpDown();
			this.TabControl1.SuspendLayout();
			this.Vertex1.SuspendLayout();
			this.GroupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.Blue1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.Green1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.Red1)).BeginInit();
			this.Vertex2.SuspendLayout();
			this.GroupBox2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.Blue2)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.Green2)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.Red2)).BeginInit();
			this.Vertex3.SuspendLayout();
			this.GroupBox3.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.Blue3)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.Green3)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.Red3)).BeginInit();
			this.Vertex4.SuspendLayout();
			this.GroupBox4.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.Blue4)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.Green4)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.Red4)).BeginInit();
			this.SuspendLayout();
			// 
			// TabControl1
			// 
			this.TabControl1.Controls.Add(this.Vertex1);
			this.TabControl1.Controls.Add(this.Vertex2);
			this.TabControl1.Controls.Add(this.Vertex3);
			this.TabControl1.Controls.Add(this.Vertex4);
			this.TabControl1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.TabControl1.Location = new System.Drawing.Point(0, 0);
			this.TabControl1.Name = "TabControl1";
			this.TabControl1.SelectedIndex = 0;
			this.TabControl1.Size = new System.Drawing.Size(256, 184);
			this.TabControl1.TabIndex = 9;
			// 
			// Vertex1
			// 
			this.Vertex1.Controls.Add(this.GroupBox1);
			this.Vertex1.Location = new System.Drawing.Point(4, 24);
			this.Vertex1.Name = "Vertex1";
			this.Vertex1.Size = new System.Drawing.Size(248, 156);
			this.Vertex1.TabIndex = 0;
			this.Vertex1.Text = "Vertex 1";
			// 
			// GroupBox1
			// 
			this.GroupBox1.Controls.Add(this.lblBlue1);
			this.GroupBox1.Controls.Add(this.Blue1);
			this.GroupBox1.Controls.Add(this.lblGreen1);
			this.GroupBox1.Controls.Add(this.Green1);
			this.GroupBox1.Controls.Add(this.lblRed1);
			this.GroupBox1.Controls.Add(this.Red1);
			this.GroupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.GroupBox1.Location = new System.Drawing.Point(32, 8);
			this.GroupBox1.Name = "GroupBox1";
			this.GroupBox1.Size = new System.Drawing.Size(184, 144);
			this.GroupBox1.TabIndex = 2;
			this.GroupBox1.TabStop = false;
			this.GroupBox1.Text = "Color";
			// 
			// lblBlue1
			// 
			this.lblBlue1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblBlue1.Location = new System.Drawing.Point(8, 106);
			this.lblBlue1.Name = "lblBlue1";
			this.lblBlue1.Size = new System.Drawing.Size(80, 24);
			this.lblBlue1.TabIndex = 5;
			this.lblBlue1.Text = "Blue";
			// 
			// Blue1
			// 
			this.Blue1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Blue1.Increment = new System.Decimal(new int[] {
																	5,
																	0,
																	0,
																	0});
			this.Blue1.Location = new System.Drawing.Point(96, 104);
			this.Blue1.Maximum = new System.Decimal(new int[] {
																  255,
																  0,
																  0,
																  0});
			this.Blue1.Name = "Blue1";
			this.Blue1.Size = new System.Drawing.Size(56, 29);
			this.Blue1.TabIndex = 4;
			this.Blue1.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// lblGreen1
			// 
			this.lblGreen1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblGreen1.Location = new System.Drawing.Point(8, 66);
			this.lblGreen1.Name = "lblGreen1";
			this.lblGreen1.Size = new System.Drawing.Size(80, 24);
			this.lblGreen1.TabIndex = 3;
			this.lblGreen1.Text = "Green";
			// 
			// Green1
			// 
			this.Green1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Green1.Increment = new System.Decimal(new int[] {
																	 5,
																	 0,
																	 0,
																	 0});
			this.Green1.Location = new System.Drawing.Point(96, 64);
			this.Green1.Maximum = new System.Decimal(new int[] {
																   255,
																   0,
																   0,
																   0});
			this.Green1.Name = "Green1";
			this.Green1.Size = new System.Drawing.Size(56, 29);
			this.Green1.TabIndex = 2;
			this.Green1.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// lblRed1
			// 
			this.lblRed1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblRed1.Location = new System.Drawing.Point(8, 26);
			this.lblRed1.Name = "lblRed1";
			this.lblRed1.Size = new System.Drawing.Size(80, 24);
			this.lblRed1.TabIndex = 1;
			this.lblRed1.Text = "Red";
			// 
			// Red1
			// 
			this.Red1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Red1.Increment = new System.Decimal(new int[] {
																   5,
																   0,
																   0,
																   0});
			this.Red1.Location = new System.Drawing.Point(96, 24);
			this.Red1.Maximum = new System.Decimal(new int[] {
																 255,
																 0,
																 0,
																 0});
			this.Red1.Name = "Red1";
			this.Red1.Size = new System.Drawing.Size(56, 29);
			this.Red1.TabIndex = 0;
			this.Red1.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// Vertex2
			// 
			this.Vertex2.Controls.Add(this.GroupBox2);
			this.Vertex2.Location = new System.Drawing.Point(4, 24);
			this.Vertex2.Name = "Vertex2";
			this.Vertex2.Size = new System.Drawing.Size(248, 156);
			this.Vertex2.TabIndex = 1;
			this.Vertex2.Text = "Vertex2";
			this.Vertex2.Visible = false;
			// 
			// GroupBox2
			// 
			this.GroupBox2.Controls.Add(this.lblBlue2);
			this.GroupBox2.Controls.Add(this.Blue2);
			this.GroupBox2.Controls.Add(this.lblGreen2);
			this.GroupBox2.Controls.Add(this.Green2);
			this.GroupBox2.Controls.Add(this.LblRed2);
			this.GroupBox2.Controls.Add(this.Red2);
			this.GroupBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.GroupBox2.Location = new System.Drawing.Point(32, 8);
			this.GroupBox2.Name = "GroupBox2";
			this.GroupBox2.Size = new System.Drawing.Size(184, 144);
			this.GroupBox2.TabIndex = 3;
			this.GroupBox2.TabStop = false;
			this.GroupBox2.Text = "Color";
			// 
			// lblBlue2
			// 
			this.lblBlue2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblBlue2.Location = new System.Drawing.Point(8, 106);
			this.lblBlue2.Name = "lblBlue2";
			this.lblBlue2.Size = new System.Drawing.Size(80, 24);
			this.lblBlue2.TabIndex = 5;
			this.lblBlue2.Text = "Blue";
			// 
			// Blue2
			// 
			this.Blue2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Blue2.Increment = new System.Decimal(new int[] {
																	5,
																	0,
																	0,
																	0});
			this.Blue2.Location = new System.Drawing.Point(96, 104);
			this.Blue2.Maximum = new System.Decimal(new int[] {
																  255,
																  0,
																  0,
																  0});
			this.Blue2.Name = "Blue2";
			this.Blue2.Size = new System.Drawing.Size(56, 29);
			this.Blue2.TabIndex = 4;
			this.Blue2.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// lblGreen2
			// 
			this.lblGreen2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblGreen2.Location = new System.Drawing.Point(8, 66);
			this.lblGreen2.Name = "lblGreen2";
			this.lblGreen2.Size = new System.Drawing.Size(80, 24);
			this.lblGreen2.TabIndex = 3;
			this.lblGreen2.Text = "Green";
			// 
			// Green2
			// 
			this.Green2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Green2.Increment = new System.Decimal(new int[] {
																	 5,
																	 0,
																	 0,
																	 0});
			this.Green2.Location = new System.Drawing.Point(96, 64);
			this.Green2.Maximum = new System.Decimal(new int[] {
																   255,
																   0,
																   0,
																   0});
			this.Green2.Name = "Green2";
			this.Green2.Size = new System.Drawing.Size(56, 29);
			this.Green2.TabIndex = 2;
			this.Green2.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// LblRed2
			// 
			this.LblRed2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.LblRed2.Location = new System.Drawing.Point(8, 26);
			this.LblRed2.Name = "LblRed2";
			this.LblRed2.Size = new System.Drawing.Size(80, 24);
			this.LblRed2.TabIndex = 1;
			this.LblRed2.Text = "Red";
			// 
			// Red2
			// 
			this.Red2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Red2.Increment = new System.Decimal(new int[] {
																   5,
																   0,
																   0,
																   0});
			this.Red2.Location = new System.Drawing.Point(96, 24);
			this.Red2.Maximum = new System.Decimal(new int[] {
																 255,
																 0,
																 0,
																 0});
			this.Red2.Name = "Red2";
			this.Red2.Size = new System.Drawing.Size(56, 29);
			this.Red2.TabIndex = 0;
			this.Red2.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// Vertex3
			// 
			this.Vertex3.Controls.Add(this.GroupBox3);
			this.Vertex3.Location = new System.Drawing.Point(4, 24);
			this.Vertex3.Name = "Vertex3";
			this.Vertex3.Size = new System.Drawing.Size(248, 156);
			this.Vertex3.TabIndex = 2;
			this.Vertex3.Text = "Vertex 3";
			this.Vertex3.Visible = false;
			// 
			// GroupBox3
			// 
			this.GroupBox3.Controls.Add(this.lblBlue3);
			this.GroupBox3.Controls.Add(this.Blue3);
			this.GroupBox3.Controls.Add(this.lblGreen3);
			this.GroupBox3.Controls.Add(this.Green3);
			this.GroupBox3.Controls.Add(this.lblRed3);
			this.GroupBox3.Controls.Add(this.Red3);
			this.GroupBox3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.GroupBox3.Location = new System.Drawing.Point(32, 8);
			this.GroupBox3.Name = "GroupBox3";
			this.GroupBox3.Size = new System.Drawing.Size(184, 144);
			this.GroupBox3.TabIndex = 3;
			this.GroupBox3.TabStop = false;
			this.GroupBox3.Text = "Color";
			// 
			// lblBlue3
			// 
			this.lblBlue3.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblBlue3.Location = new System.Drawing.Point(8, 106);
			this.lblBlue3.Name = "lblBlue3";
			this.lblBlue3.Size = new System.Drawing.Size(80, 24);
			this.lblBlue3.TabIndex = 5;
			this.lblBlue3.Text = "Blue";
			// 
			// Blue3
			// 
			this.Blue3.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Blue3.Increment = new System.Decimal(new int[] {
																	5,
																	0,
																	0,
																	0});
			this.Blue3.Location = new System.Drawing.Point(96, 104);
			this.Blue3.Maximum = new System.Decimal(new int[] {
																  255,
																  0,
																  0,
																  0});
			this.Blue3.Name = "Blue3";
			this.Blue3.Size = new System.Drawing.Size(56, 29);
			this.Blue3.TabIndex = 4;
			this.Blue3.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// lblGreen3
			// 
			this.lblGreen3.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblGreen3.Location = new System.Drawing.Point(8, 66);
			this.lblGreen3.Name = "lblGreen3";
			this.lblGreen3.Size = new System.Drawing.Size(80, 24);
			this.lblGreen3.TabIndex = 3;
			this.lblGreen3.Text = "Green";
			// 
			// Green3
			// 
			this.Green3.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Green3.Increment = new System.Decimal(new int[] {
																	 5,
																	 0,
																	 0,
																	 0});
			this.Green3.Location = new System.Drawing.Point(96, 64);
			this.Green3.Maximum = new System.Decimal(new int[] {
																   255,
																   0,
																   0,
																   0});
			this.Green3.Name = "Green3";
			this.Green3.Size = new System.Drawing.Size(56, 29);
			this.Green3.TabIndex = 2;
			this.Green3.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// lblRed3
			// 
			this.lblRed3.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblRed3.Location = new System.Drawing.Point(8, 26);
			this.lblRed3.Name = "lblRed3";
			this.lblRed3.Size = new System.Drawing.Size(80, 24);
			this.lblRed3.TabIndex = 1;
			this.lblRed3.Text = "Red";
			// 
			// Red3
			// 
			this.Red3.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Red3.Increment = new System.Decimal(new int[] {
																   5,
																   0,
																   0,
																   0});
			this.Red3.Location = new System.Drawing.Point(96, 24);
			this.Red3.Maximum = new System.Decimal(new int[] {
																 255,
																 0,
																 0,
																 0});
			this.Red3.Name = "Red3";
			this.Red3.Size = new System.Drawing.Size(56, 29);
			this.Red3.TabIndex = 0;
			this.Red3.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// Vertex4
			// 
			this.Vertex4.Controls.Add(this.GroupBox4);
			this.Vertex4.Location = new System.Drawing.Point(4, 24);
			this.Vertex4.Name = "Vertex4";
			this.Vertex4.Size = new System.Drawing.Size(248, 156);
			this.Vertex4.TabIndex = 3;
			this.Vertex4.Text = "Vertex 4";
			this.Vertex4.Visible = false;
			// 
			// GroupBox4
			// 
			this.GroupBox4.Controls.Add(this.lblBlue4);
			this.GroupBox4.Controls.Add(this.Blue4);
			this.GroupBox4.Controls.Add(this.lblGreen4);
			this.GroupBox4.Controls.Add(this.Green4);
			this.GroupBox4.Controls.Add(this.lblRed4);
			this.GroupBox4.Controls.Add(this.Red4);
			this.GroupBox4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.GroupBox4.Location = new System.Drawing.Point(32, 8);
			this.GroupBox4.Name = "GroupBox4";
			this.GroupBox4.Size = new System.Drawing.Size(184, 144);
			this.GroupBox4.TabIndex = 3;
			this.GroupBox4.TabStop = false;
			this.GroupBox4.Text = "Color";
			// 
			// lblBlue4
			// 
			this.lblBlue4.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblBlue4.Location = new System.Drawing.Point(8, 106);
			this.lblBlue4.Name = "lblBlue4";
			this.lblBlue4.Size = new System.Drawing.Size(80, 24);
			this.lblBlue4.TabIndex = 5;
			this.lblBlue4.Text = "Blue";
			// 
			// Blue4
			// 
			this.Blue4.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Blue4.Increment = new System.Decimal(new int[] {
																	5,
																	0,
																	0,
																	0});
			this.Blue4.Location = new System.Drawing.Point(96, 104);
			this.Blue4.Maximum = new System.Decimal(new int[] {
																  255,
																  0,
																  0,
																  0});
			this.Blue4.Name = "Blue4";
			this.Blue4.Size = new System.Drawing.Size(56, 29);
			this.Blue4.TabIndex = 4;
			this.Blue4.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// lblGreen4
			// 
			this.lblGreen4.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblGreen4.Location = new System.Drawing.Point(8, 66);
			this.lblGreen4.Name = "lblGreen4";
			this.lblGreen4.Size = new System.Drawing.Size(80, 24);
			this.lblGreen4.TabIndex = 3;
			this.lblGreen4.Text = "Green";
			// 
			// Green4
			// 
			this.Green4.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Green4.Increment = new System.Decimal(new int[] {
																	 5,
																	 0,
																	 0,
																	 0});
			this.Green4.Location = new System.Drawing.Point(96, 64);
			this.Green4.Maximum = new System.Decimal(new int[] {
																   255,
																   0,
																   0,
																   0});
			this.Green4.Name = "Green4";
			this.Green4.Size = new System.Drawing.Size(56, 29);
			this.Green4.TabIndex = 2;
			this.Green4.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// lblRed4
			// 
			this.lblRed4.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblRed4.Location = new System.Drawing.Point(8, 26);
			this.lblRed4.Name = "lblRed4";
			this.lblRed4.Size = new System.Drawing.Size(80, 24);
			this.lblRed4.TabIndex = 1;
			this.lblRed4.Text = "Red";
			// 
			// Red4
			// 
			this.Red4.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Red4.Increment = new System.Decimal(new int[] {
																   5,
																   0,
																   0,
																   0});
			this.Red4.Location = new System.Drawing.Point(96, 24);
			this.Red4.Maximum = new System.Decimal(new int[] {
																 255,
																 0,
																 0,
																 0});
			this.Red4.Name = "Red4";
			this.Red4.Size = new System.Drawing.Size(56, 29);
			this.Red4.TabIndex = 0;
			this.Red4.ValueChanged += new System.EventHandler(this.Color_TextChanged);
			// 
			// LightControl
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(256, 182);
			this.Controls.Add(this.TabControl1);
			this.Name = "LightControl";
			this.Text = "LC";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.LightControl_Closing);
			this.TabControl1.ResumeLayout(false);
			this.Vertex1.ResumeLayout(false);
			this.GroupBox1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.Blue1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.Green1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.Red1)).EndInit();
			this.Vertex2.ResumeLayout(false);
			this.GroupBox2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.Blue2)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.Green2)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.Red2)).EndInit();
			this.Vertex3.ResumeLayout(false);
			this.GroupBox3.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.Blue3)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.Green3)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.Red3)).EndInit();
			this.Vertex4.ResumeLayout(false);
			this.GroupBox4.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.Blue4)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.Green4)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.Red4)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		//Dispose of any used resources
		public void DisposeD3D() {
			DisposeVertices();
			for (int i = 0; i < numTextures; i++)
				if (textures[i] != null) { 
					textures[i].Dispose();
					textures[i] = null;
				}
			if (device != null) {
				device.Dispose();
				device = null;
			}
		}

		public void DisposeVertices() {
			if (vertBuffer != null) { 
				vertBuffer.Dispose();
				vertBuffer = null;
			}
		}
			 
		public bool InitD3D(IntPtr winHandle) {
			DisplayMode dispMode = 
				Manager.Adapters[Manager.Adapters.Default.Adapter].CurrentDisplayMode;
			PresentParameters presentParams = new PresentParameters();
			//Define the presentation parameters
			presentParams.Windowed = true;
			presentParams.SwapEffect = SwapEffect.Discard;
			presentParams.BackBufferFormat = dispMode.Format;
			try {
				device = new Device(Manager.Adapters.Default.Adapter,
					DeviceType.Hardware, winHandle, 
					CreateFlags.SoftwareVertexProcessing,presentParams);
				device.VertexFormat = customVertex;
				return true;
			}	
			catch {
				return false;
			}
		}


		public bool CreateTextures() {
			try {
				for (int i = 1; i <= numTextures; i++)
					textures[i-1] = TextureLoader.FromFile(device, Application.StartupPath + @"\..\..\Images\Walk" + i.ToString() + ".bmp");
				return CreateVertices();
			}
			catch {
				return false;
			}
		}

		public bool CreateVertices() {
			CustomVertex[] verts;

			try {
				//If the vertex buffer was previously created, dispose them
				DisposeVertices();

				vertBuffer = new VertexBuffer(typeof(CustomVertex), numVerts, device, Usage.WriteOnly, customVertex, Pool.Default);
				verts = vertBuffer.Lock(0, 0) as CustomVertex[];
				SquareVertices(verts);
				vertBuffer.Unlock();
				return true;
			}
			catch {
				return false;
			}			
		}
		
		private void SquareVertices(CustomVertex[] vertices) {
			// Create a square, composed of 2 triangles
			vertices[0] = CreateFlexVertex(60, 60, 0, 1, Color.FromArgb((int)Red1.Value, (int)Green1.Value, (int)Blue1.Value), 0, 0);
			vertices[1] = CreateFlexVertex(240, 60, 0, 1, Color.FromArgb((int)Red2.Value, (int)Green2.Value, (int)Blue2.Value), 1, 0);
			vertices[2] = CreateFlexVertex(60, 240, 0, 1, Color.FromArgb((int)Red3.Value, (int)Green3.Value, (int)Blue3.Value), 0, 1);
			vertices[3] = CreateFlexVertex(240, 240, 0, 1, Color.FromArgb((int)Red4.Value, (int)Green4.Value, (int)Blue4.Value), 1, 1);
		}



		private CustomVertex CreateFlexVertex(float X, float Y, float Z, float rhw, Color color, float tu, float tv) {
			CustomVertex custVertex = new CustomVertex();
			custVertex.X = X;
			custVertex.Y = Y;
			custVertex.Z = Z;
			custVertex.rhw = rhw;
			custVertex.color = color.ToArgb();
			custVertex.tu = tu;
			custVertex.tv = tv;
			return custVertex;
		}

		public void Render() {		
			if(device == null) return;

			device.Clear(ClearFlags.Target, Color.FromArgb(0, 0, 255).ToArgb(), 1.0F, 0);
			device.BeginScene();
			// Show one texture a time, in order to create the illusion of a walking guy
			device.SetTexture(0, textures[x]);
			x = (x == 9) ? 0 : x+1; //If x is 9, set to 0, otherwise increment x
			device.SetStreamSource(0, vertBuffer, 0);
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, numVerts - 2);
			device.EndScene();
			try {
				device.Present();
			}
			catch {
			}
		}

		private void LightControl_Closing(object sender, System.ComponentModel.CancelEventArgs e) {
			DisposeD3D();
			endTest = true;

		}

		private void Color_TextChanged(object sender, System.EventArgs e) {
			CreateVertices();
		}

	}
}

