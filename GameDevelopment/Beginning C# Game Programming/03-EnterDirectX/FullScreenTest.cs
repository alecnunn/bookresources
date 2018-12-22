using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace EnterDirectX {
	public class FullScreenTest: System.Windows.Forms.Form {
		private bool endTest = false;

		public bool EndTest {
			get { return endTest; }
		}

		int numVerts = 4;
		Device device = null;
		VertexBuffer vertBuffer = null;
		Texture[] textures = new Texture[10];
		private static int x = 0;

		// Simple textured vertices constant and structure
		private const VertexFormats customVertex = VertexFormats.Transformed|VertexFormats.Texture1;

		private struct CustomVertex {
			public float X;
			public float Y;
			public float Z;
			public float rhw;
			public float tu;
			public float tv;
		}


		#region " Windows Form Designer generated code "

		public FullScreenTest() : base() {        

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
			// FullScreenTest
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(1119, 741);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.KeyPreview = true;
			this.Name = "FullScreenTest";
			this.Text = "TestFullScreen";
			this.TopMost = true;
			this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FullScreenTest_KeyDown);
			this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.FullScreenTest_MouseDown);
			this.Closing += new System.ComponentModel.CancelEventHandler(this.FullScreenTest_Closing);

		}

		#endregion

		private void FullScreenTest_KeyDown(object sender, KeyEventArgs e) {
			if (e.KeyCode==Keys.Escape) {
				endTest = true;
			}
		}
		private void FullScreenTest_MouseDown(object sender, MouseEventArgs e) {
			endTest = true;
		}


		public void DisposeD3D() {
			for(int i=0; i<=9; i++) {
				if ((textures[i] != null)) {textures[i].Dispose();}
				textures[i] = null;
			}

			if ((vertBuffer != null)) {vertBuffer.Dispose();}
			vertBuffer = null;

			if ((device != null)) {device.Dispose();}
			device = null;
		}

		public bool InitD3D(System.IntPtr winHandle) {
			DisplayMode DispMode = Manager.Adapters[Manager.Adapters.Default.Adapter].CurrentDisplayMode;
			PresentParameters presentParams = new PresentParameters();
			// Define the presentation parameters
			presentParams.BackBufferFormat = DispMode.Format;
			presentParams.BackBufferWidth = DispMode.Width;
			presentParams.BackBufferHeight = DispMode.Height;
			presentParams.SwapEffect = SwapEffect.Discard;
			// Try to create the device
			try {
				device = new Device(Manager.Adapters.Default.Adapter, DeviceType.Hardware, winHandle, CreateFlags.SoftwareVertexProcessing, presentParams);
				device.VertexFormat = customVertex;
				return true;
			}
			catch {
				return false;
			}
		}

		public bool CreateTextures() {
			CustomVertex[] verts;
			try {
				for(int i=1; i<=10; i++) {
					textures[i-1] = TextureLoader.FromFile(device, Application.StartupPath+@"\..\..\Images\Walk"+i.ToString()+".bmp");
				}
				vertBuffer = new VertexBuffer(typeof(CustomVertex), numVerts, 
					device, Usage.WriteOnly, customVertex, Pool.Default);
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
			DisplayMode mode = Manager.Adapters[Manager.Adapters.Default.Adapter].CurrentDisplayMode;
			// Create a square, composed of 2 triangles, taking all the screen
			vertices[0] = CreateFlexVertex(0, 0, 0, 1, 0, 0);
			vertices[1] = CreateFlexVertex(mode.Width, 0, 0, 1, 1, 0);
			vertices[2] = CreateFlexVertex(0, mode.Height, 0, 1, 0, 1);
			vertices[3] = CreateFlexVertex(mode.Width, mode.Height, 0, 1, 1, 1);
		}

		private CustomVertex CreateFlexVertex(float X, float Y, float Z, float rhw, float tu, float tv) {
			CustomVertex cv;
			cv.X = X;
			cv.Y = Y;
			cv.Z = Z;
			cv.rhw = rhw;
			cv.tu = tu;
			cv.tv = tv;
			return cv;
		}

		public void Render() {
			if ((device==null)) {return;}

			device.Clear(ClearFlags.Target, Color.FromArgb(0, 0, 255).ToArgb(), 1.0F, 0);
			device.BeginScene();

			// Show one texture a time, in order to create the illusion of a walking guy
			device.SetTexture(0, textures[x]);
			x = (x == 9) ? 0 : x+1; //If x is 9, set to 0, otherwise increment x
			device.SetStreamSource(0, vertBuffer, 0);
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, numVerts-2);
			device.EndScene();
			try {
				device.Present();
			}
			catch {
			}
		}

		private void FullScreenTest_Closing(object sender, System.ComponentModel.CancelEventArgs e) {
			endTest = true;
			DisposeD3D();
		}
	}
}




