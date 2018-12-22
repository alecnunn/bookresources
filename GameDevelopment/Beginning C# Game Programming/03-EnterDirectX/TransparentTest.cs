using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace EnterDirectX {
	/// <summary>
	/// Summary description for TransparentTest.
	/// </summary>
	public class TransparentTest : System.Windows.Forms.Form {
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private static int x = 0;
		private const int numVerts = 4;
		private const int numTextures = 10;
		private bool endTest = false;

		public bool EndTest {
			get { return endTest; }
		}

		private Device device = null;

		private VertexBuffer vertBuffer = null;
		private VertexBuffer TranspVertBuffer = null;
		private Texture[] textures = new Texture[10];
		private Texture TranspTexture;

		// Simple textured vertices constant and structure
		private const VertexFormats customVertexFlags  = VertexFormats.Transformed | VertexFormats.Texture1;

		private struct CustomVertex {
			public float X;
			public float Y;
			public float Z;
			public float rhw;
			public float tu;
			public float tv;
		}

		public TransparentTest() {
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

		//Dispose of any used resources
		public void DisposeD3D() {
			for (int i = 0; i < numTextures; i++)
				if (textures[i] != null) { 
					textures[i].Dispose();
					textures[i] = null;
				}
			if (vertBuffer != null) {
				vertBuffer.Dispose();
				vertBuffer = null;
			}
			if (TranspVertBuffer != null) {
				TranspVertBuffer.Dispose();
				TranspVertBuffer = null;
			}
			if (device != null) {
				device.Dispose();
				device = null;
			}
		}

		public bool InitD3D(IntPtr winHandle) {
			DisplayMode DispMode = 
				Manager.Adapters[Manager.Adapters.Default.Adapter].CurrentDisplayMode;
			PresentParameters presentParams = new PresentParameters();
			//Define the presentation parameters
			presentParams.Windowed = true;
			presentParams.SwapEffect = SwapEffect.Discard;
			presentParams.BackBufferFormat = DispMode.Format;
			//try to create the device
			try {
				device = new Device(Manager.Adapters.Default.Adapter,
					DeviceType.Hardware, winHandle, 
					CreateFlags.SoftwareVertexProcessing,presentParams);
				//Tells the device which is the format of our custom vertices
				device.VertexFormat = customVertexFlags;
				device.RenderState.Lighting = false;
				device.RenderState.SourceBlend = Blend.SourceAlpha;
				device.RenderState.DestinationBlend = Blend.InvSourceAlpha;
				device.RenderState.AlphaBlendEnable = true;
				return true;
			}	
			catch {
				return false;
			}
		}


		public bool CreateTextures() {
			CustomVertex[] verts;
			//We will use blue as the transparent color
			Color colorKeyVal  = Color.FromArgb(255, 0, 0, 255);
			try {
				//Load the walking guy textures
				string textureFile;
				for(int i=1; i<=10; i++) {
					textureFile = Application.StartupPath + @"\..\..\Images\walk" + i.ToString() + ".bmp";
					textures[i-1] = TextureLoader.FromFile(device, textureFile);
				}
				//Load the transparent texture
				TranspTexture = TextureLoader.FromFile(device, 
					Application.StartupPath + @"\..\..\Images\TranspSample.bmp", 
					64, 64, D3DX.Default, 0, Format.Unknown, Pool.Managed, Filter.Point, Filter.Point, 
					colorKeyVal.ToArgb());

				vertBuffer = new VertexBuffer(typeof(CustomVertex), 
					numVerts, device, Usage.WriteOnly, customVertexFlags, Pool.Default);
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
			vertices[0] = CreateFlexVertex(60, 60, 0, 1, 0, 0);
			vertices[1] = CreateFlexVertex(240, 60, 0, 1, 1, 0);
			vertices[2] = CreateFlexVertex(60, 240, 0, 1, 0, 1);
			vertices[3] = CreateFlexVertex(240, 240, 0, 1, 1, 1);
		}

		public bool CreateTransparentVertices(float x, float y) {
			CustomVertex[] verts;
			try {
				// If the vertex buffer was previously created, dispose them
				if(TranspVertBuffer != null) TranspVertBuffer.Dispose();

				TranspVertBuffer = new VertexBuffer(typeof(CustomVertex), 
					numVerts, device, Usage.WriteOnly, customVertexFlags, Pool.Default);

				verts = TranspVertBuffer.Lock(0, 0) as CustomVertex[];
				TranspVertices(x, y, verts);
				TranspVertBuffer.Unlock();
				return true;
			}
			catch {
				return false;
			}
		}

		private void TranspVertices(float X, float Y, CustomVertex[] vertices) {
			// Create a square, composed of 2 triangles.  Our transparent texture is 42 pixels wide and 60 long
			vertices[0] = CreateFlexVertex(X, Y, 0, 1, 0, 0);
			vertices[1] = CreateFlexVertex(X + 42, Y, 0, 1, 1, 0);
			vertices[2] = CreateFlexVertex(X, Y + 60, 0, 1, 0, 1);
			vertices[3] = CreateFlexVertex(X + 42, Y + 60, 0, 1, 1, 1);
		}

		private CustomVertex CreateFlexVertex(float X, float Y, float Z, float rhw, float tu, float tv) {
			CustomVertex custVertex = new CustomVertex();
			custVertex.X = X;
			custVertex.Y = Y;
			custVertex.Z = Z;
			custVertex.rhw = rhw;
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
			device.SetTexture(0, TranspTexture);
			device.SetStreamSource(0, TranspVertBuffer, 0);
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, numVerts - 2);

			device.EndScene();
			try {
				device.Present();
			}
			catch {
			}
		}

		private void TransparentTest_Closing(object sender, System.ComponentModel.CancelEventArgs e) {
			endTest = true;
			DisposeD3D();
		}

		private void TransparentTest_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e) {
			if(e.KeyCode == Keys.Escape) {
				endTest = true;
			}
		}

		private void TransparentTest_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e) {
			CreateTransparentVertices(e.X, e.Y);
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			// 
			// TransparentTest
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(346, 312);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "TransparentTest";
			this.Text = "TransparentTest";
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TransparentTest_KeyDown);
			this.Closing += new System.ComponentModel.CancelEventHandler(this.TransparentTest_Closing);
			this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.TransparentTest_MouseMove);

		}
		#endregion

	}
}
