using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace EnterDirectX {
	/// <summary>
	/// Summary description for wt.
	/// </summary>
	public class WindowTest : System.Windows.Forms.Form {
		private bool endTest = false;

		public bool EndTest {
			get { return endTest; }
		}

		private int numVerts = 4;
		private Device device = null;
		private VertexBuffer vertBuffer = null;
		private Texture[] textures = new Texture[10];
		private System.ComponentModel.Container components = null;
		private static int x = 0;
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

		/// <summary>
		/// Required designer variable.
		/// </summary>

		public WindowTest() {
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
				DisposeD3D();
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
			// 
			// WindowTest
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(292, 273);
			this.Name = "WindowTest";
			this.Text = "WindowTest";
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.WindowTest_KeyDown);
			this.Closing += new System.ComponentModel.CancelEventHandler(this.WindowTest_Closing);

		}
		#endregion

		private void WindowTest_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e) {
			if(e.KeyCode == Keys.Escape) {
				endTest = true;
			}
		}

		private void WindowTest_Closing(object sender, System.ComponentModel.CancelEventArgs e) {
			endTest = true;
			DisposeD3D();
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
			DisplayMode dispMode = 
				Manager.Adapters[Manager.Adapters.Default.Adapter].CurrentDisplayMode;
			PresentParameters presentParams= new PresentParameters();
			// Define the presentation parameters
			presentParams.Windowed = true;
			presentParams.SwapEffect = SwapEffect.Discard;
			presentParams.BackBufferFormat = dispMode.Format;
			// Try to create the device
			try {
				device = new Device(
					Manager.Adapters.Default.Adapter, 
					DeviceType.Hardware, 
					winHandle, 
					CreateFlags.SoftwareVertexProcessing, 
					presentParams);
				device.VertexFormat = customVertexFlags;
				return true;
			}	
			catch {
				return false;
			}
		}

		public bool CreateTextures() {
			CustomVertex[] verts;
			try {
				string textureFile;
				// Load the textures, named from "walk1.bmp" to "walk10.bmp"
				for(int i=1; i<=10; i++) {
					textureFile = Application.StartupPath + @"\..\..\Images\walk" + i.ToString() + ".bmp";
					textures[i-1] = TextureLoader.FromFile(device, textureFile);
				}
				// Define the vertex buffer to hold our custom vertices
				vertBuffer = new VertexBuffer(typeof(CustomVertex), 
					numVerts, device, Usage.WriteOnly, customVertexFlags, Pool.Default);
				// Locks the memory, which will return the array to be filled
				verts = vertBuffer.Lock(0, 0) as CustomVertex[];
				// Defines the vertices
				SquareVertices(verts);
				// Unlock the buffer, which will save our vertex information to the device
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

		private CustomVertex CreateFlexVertex(float X, float Y, float Z, float rhw, float tu, float tv) {
			CustomVertex cv = new CustomVertex();
			cv.X = X;
			cv.Y = Y;
			cv.Z = Z;
			cv.rhw = rhw;
			cv.tu = tu;
			cv.tv = tv;
			return cv;
		}

		public void Render() {		
			if(device == null) {
				return;
			}
			// Clears the device with blue color
			device.Clear(ClearFlags.Target, Color.FromArgb(0, 0, 255).ToArgb(), 1.0F, 0);
			device.BeginScene();

			// Show one texture a time, in order to create the illusion of a walking guy
			device.SetTexture(0, textures[x]);
			x = (x == 9) ? 0 : x+1; //If x is 9, set to 0, otherwise increment x
			// Define which vertex buffer should be used
			device.SetStreamSource(0, vertBuffer, 0);
			device.VertexFormat = customVertexFlags;
			// Draw the vertices of the vertex buffer, rendering them as a
			// triangle strip, using the given texture
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, numVerts - 2);
			device.EndScene();

			// Using an extra try-catch will prevent any errors if the device was disposed
			try {
				// Present the rendered scene
				device.Present();
			}
			catch {
			}
		}
	}
}
