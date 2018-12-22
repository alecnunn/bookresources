using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing.Imaging;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using System.Reflection;
using System.IO;
using System.Threading;

namespace BilevelTextureMagnification
{
	/// <summary>
	/// Non-modal 3D preview window
	/// </summary>
	public class RenderWindow : System.Windows.Forms.Form
	{
		private System.ComponentModel.Container components = null;

		private Texture optimizedTexture, hiResTexture;
		private Device myDevice;
		private VertexBuffer myVertexBuffer;
		private Effect myEffect;
		private VertexDeclaration myVertexDeclaration;
		private RenderWindowThread myThread;
		private double time = 0.0;

		private System.Windows.Forms.MenuItem viewMenu;
		private System.Windows.Forms.MenuItem viewInputMenuItem;
		private System.Windows.Forms.MenuItem viewOutputMenuItem;
		private System.Windows.Forms.MenuItem viewThresholdedMenuItem;
		private System.Windows.Forms.MenuItem animateMenu;
		private System.Windows.Forms.MenuItem animateOnMenuItem;
		private System.Windows.Forms.MainMenu mainMenu;

		internal static RenderWindow Create()
		{
			RenderWindowThread t = new RenderWindowThread();
			return t.Window;
		}

		private class RenderWindowThread
		{
			private RenderWindow window;
			private bool isReady = false;
			private Thread t;

			public RenderWindowThread()
			{
				t = new Thread(new ThreadStart(start));
				t.Priority = ThreadPriority.Lowest;
				t.Start();
			}

			private void start()
			{
				window = new RenderWindow(this);
				isReady = true;
				Application.Run(window);
			}

			public RenderWindow Window
			{
				get
				{
					while(!isReady || !window.Created)
					{
						Thread.Sleep(10);
					}
					return window;
				}
			}

			public void Kill()
			{
				window.Close();
				t.Join();
			}
		}

		internal void Kill()
		{
			myThread.Kill();
		}

		private RenderWindow(RenderWindowThread t)
		{
			myThread = t;

			InitializeComponent();
			SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.Opaque, true);

			PresentParameters presentationParameters = new PresentParameters();
			presentationParameters.Windowed = true;
			presentationParameters.SwapEffect = SwapEffect.Discard;

			DeviceType dt = DeviceType.Hardware;
			if(Manager.GetDeviceCaps(0, DeviceType.Hardware).PixelShaderVersion.Major < 3)
			{
				dt = DeviceType.Reference;
				Text += " (emulated in SW)";
			}
			myDevice = new Device(0, dt, this, CreateFlags.HardwareVertexProcessing, presentationParameters);

			// gamma-correct rendering
			myDevice.RenderState.SrgbWriteEnable = true;

			Assembly assem = Assembly.GetAssembly(typeof(RenderWindow));
			Stream st = assem.GetManifestResourceStream("BilievelTextureMagnification.Shader.fx");

			myEffect = Effect.FromStream(myDevice, st, null, null, ShaderFlags.None, null);
			st.Close();
			st = null;
			myEffect.Technique = "threshold";
			
			VertexElement[] vertexElements = new VertexElement[]
				{
					new VertexElement(0, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 0),
					new VertexElement(0, 3*4, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 0),
					new VertexElement(0, 5*4, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 1),
					VertexElement.VertexDeclarationEnd
			};
			myVertexDeclaration = new VertexDeclaration(myDevice, vertexElements);
		}

		internal void SetBaseTexture(int[,] optimizedArray, Bitmap hiResBitmap)
		{
			lock(this)
			{
				if(optimizedTexture != null)
				{
					optimizedTexture.Dispose();
					optimizedTexture = null;
				}

				if(hiResTexture != null)
				{
					hiResTexture.Dispose();
					hiResTexture = null;
				}
				if(hiResTexture == null)
				{
					// The obvious solution would be:
					// hiResTexture = new Texture(myDevice, hiResBitmap, Usage.AutoGenerateMipMap, Pool.Managed);
					// But that doesn't work for reading.
					
					int wHi = hiResBitmap.Width;
					int hHi = hiResBitmap.Height;
					int maxMipLevel = (int)(1.5 + Math.Log(Math.Min(wHi, hHi), 2.0));

					hiResTexture = new Texture(myDevice, wHi, hHi, maxMipLevel, Usage.None, Format.L8, Pool.Managed);
					byte[,] hiResTextureArray = (byte[,])hiResTexture.LockRectangle(typeof(byte), 0, LockFlags.None, new int[]{hHi, wHi});
					for(int j = 0; j < hHi; j++)
					{
						for(int i = 0; i < wHi; i++)
						{
							hiResTextureArray[j, i] = hiResBitmap.GetPixel(i, j).R;
						}
					}
					hiResTexture.UnlockRectangle(0);
					TextureLoader.FilterTexture(hiResTexture, 0, Filter.Box);
				}

				int w = optimizedArray.GetLength(1);
				int h = optimizedArray.GetLength(0);

				int downsampling = hiResTexture.GetLevelDescription(0).Width / w;
				int levelOffset = (int)(0.5 + Math.Log(downsampling, 2.0));
				optimizedTexture = new Texture(myDevice, w, h, hiResTexture.LevelCount - levelOffset, 0, Format.L8, Pool.Managed);

				// skip the first level; that's filled dynamically
				for(int level = 1; level < hiResTexture.LevelCount - levelOffset; level++)
				{
					int scale = (int)(0.5 + Math.Pow(2.0, level));

					byte[,] originalTextureArray = (byte[,])hiResTexture.LockRectangle(typeof(byte), level + levelOffset, LockFlags.ReadOnly, new int[]{h/scale, w/scale});
					byte[,] optimizedTextureArray = (byte[,])optimizedTexture.LockRectangle(typeof(byte), level, LockFlags.None, new int[]{h/scale, w/scale});
					
					int wLevel = w / scale;
					int hLevel = h / scale;

					for(int j = 0; j < hLevel; j++)
					{
						for(int i = 0; i < wLevel; i++)
						{
							int x = optimizedTextureArray[j, i];

							optimizedTextureArray[j, i] = (byte)originalTextureArray[j, i];
						}
					}

					optimizedTexture.UnlockRectangle(level);
					hiResTexture.UnlockRectangle(level + levelOffset);
				}

				if(myVertexBuffer != null)
				{
					myVertexBuffer.Dispose();
				}

				myVertexBuffer = new VertexBuffer(typeof(DualTextureVertex), 4, myDevice, Usage.WriteOnly, 0, Pool.Managed);
				DualTextureVertex[] myVertexData = new DualTextureVertex[4];
				float aspectRatio = h/(float)w;
				myVertexData[0] = new DualTextureVertex(-1.0f,  aspectRatio, 0.0f, 0.0f, 0.0f, 0, 0);
				myVertexData[1] = new DualTextureVertex(-1.0f, -aspectRatio, 0.0f, 0.0f, 1.0f, 0, h);
				myVertexData[2] = new DualTextureVertex( 1.0f,  aspectRatio, 0.0f, 1.0f, 0.0f, w, 0);
				myVertexData[3] = new DualTextureVertex( 1.0f, -aspectRatio, 0.0f, 1.0f, 1.0f, w, h);
				myVertexBuffer.SetData(myVertexData, 0, LockFlags.None);
			}
		}
		
		internal void SetTexture(int[,] optimizedArray, Bitmap hiResBitmap)
		{
			lock(this)
			{
				if(optimizedTexture == null || optimizedTexture.Disposed)
				{
					return; // something went wrong
				}

				int w = optimizedArray.GetLength(1);
				int h = optimizedArray.GetLength(0);

				SurfaceDescription sd = optimizedTexture.GetLevelDescription(0);
				if(w != sd.Width || h != sd.Height) // Out-of-date texture? (Didn't call SetBaseTexture first?)
				{
					return;
				}

				byte[,] optimizedtextureArray = (byte[,])optimizedTexture.LockRectangle(typeof(byte), 0, LockFlags.None, new int[]{h, w});
				int wLevel = optimizedtextureArray.GetLength(1);
				int hLevel = optimizedtextureArray.GetLength(0);
				for(int j = 0; j < hLevel; j++)
				{
					for(int i = 0; i < wLevel; i++)
					{
						optimizedtextureArray[j, i] = (byte)(optimizedArray[j, i]);
					}
				}
				optimizedTexture.UnlockRectangle(0);
			}
		}

		internal void SaveTexture(string fileName)
		{
			lock(this)
			{
				TextureLoader.Save(fileName, ImageFileFormat.Dds, optimizedTexture);
			}
		}

		// Called by RenderWindowThread. Don't call from somewhere else.
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
		{
			lock(this)
			{
				if(myDevice == null || myVertexBuffer == null)
				{
					return;
				}

				else if(animateOnMenuItem.Checked)
				{
					time += 0.5; // in theory, this does _not_ work indefinitely long
				}

				// float s = (float)(3.0 + 2.95*Math.Sin(1.4+0.2941*time));
				myDevice.Transform.World =
					//Matrix.Scaling(s, s, s) *
					  Matrix.RotationX((float)(0.9*Math.Sin(0.3+0.3175*time)))
					* Matrix.RotationY((float)(0.9*Math.Sin(0.2+0.2923*time)))
					* Matrix.RotationZ((float)(0.7*Math.Sin(0.1+0.2843*time)))
					* Matrix.Translation((float)(0.2*Math.Sin(0.3423*time)), (float)(0.2*Math.Sin(0.2712*time)), (float)(11.0-11.0*Math.Sin(0.2737*time)));

				myDevice.Clear(ClearFlags.Target, Color.LightBlue, 1.0f, 0);
				myDevice.BeginScene();

				myDevice.Transform.Projection = Matrix.PerspectiveFovLH(0.3f, ClientSize.Width/(float)((ClientSize.Height != 0) ? ClientSize.Height : 1), 0.01f, 100.0f);
				myDevice.Transform.View = Matrix.LookAtLH(new Vector3(0.0f, 0.0f, -2.0f), new Vector3(), new Vector3(0.0f, 1.0f, 0.0f));

				myDevice.SetStreamSource(0, myVertexBuffer, 0);
				myDevice.VertexDeclaration = myVertexDeclaration;

				myDevice.RenderState.CullMode = Cull.None;
				myDevice.RenderState.Lighting = false;

				myDevice.SamplerState[0].MinFilter = TextureFilter.Linear;
				myDevice.SamplerState[0].MagFilter = TextureFilter.Linear;
				myDevice.SamplerState[0].MipFilter = TextureFilter.Linear;
				myDevice.SamplerState[0].MaxAnisotropy = 1;
				myDevice.SamplerState[0].MipMapLevelOfDetailBias = 0.0f;
				myDevice.SamplerState[0].AddressU = TextureAddress.Clamp;
				myDevice.SamplerState[0].AddressV = TextureAddress.Clamp;

				if(viewInputMenuItem.Checked)
				{
					myDevice.SetTexture(0, hiResTexture);
				}
				else
				{
					myDevice.SetTexture(0, optimizedTexture);
				}
				
				if(viewThresholdedMenuItem.Checked)
				{
					int numberOfPasses = myEffect.Begin(0);
					for(int i = 0; i < numberOfPasses; i++)
					{
						myEffect.BeginPass(i);
						myDevice.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);
						myEffect.EndPass();
					}
					myEffect.End();
				}
				else
				{
					myDevice.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);
				}

				myDevice.EndScene();
				myDevice.Present();
			}
			System.Threading.Thread.Sleep(200); // better than a timer because it can also deal with slow rendering
			Invalidate();
		}

		#region Windows Form Designer generated code
		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(RenderWindow));
			this.mainMenu = new System.Windows.Forms.MainMenu();
			this.viewMenu = new System.Windows.Forms.MenuItem();
			this.viewInputMenuItem = new System.Windows.Forms.MenuItem();
			this.viewOutputMenuItem = new System.Windows.Forms.MenuItem();
			this.viewThresholdedMenuItem = new System.Windows.Forms.MenuItem();
			this.animateMenu = new System.Windows.Forms.MenuItem();
			this.animateOnMenuItem = new System.Windows.Forms.MenuItem();
			// 
			// mainMenu
			// 
			this.mainMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.viewMenu,
																					 this.animateMenu});
			// 
			// viewMenu
			// 
			this.viewMenu.Index = 0;
			this.viewMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.viewInputMenuItem,
																					 this.viewOutputMenuItem,
																					 this.viewThresholdedMenuItem});
			this.viewMenu.Text = "&View";
			// 
			// viewInputMenuItem
			// 
			this.viewInputMenuItem.Index = 0;
			this.viewInputMenuItem.RadioCheck = true;
			this.viewInputMenuItem.Shortcut = System.Windows.Forms.Shortcut.F5;
			this.viewInputMenuItem.Text = "&Input";
			this.viewInputMenuItem.Click += new System.EventHandler(this.changeView);
			// 
			// viewOutputMenuItem
			// 
			this.viewOutputMenuItem.Index = 1;
			this.viewOutputMenuItem.RadioCheck = true;
			this.viewOutputMenuItem.Shortcut = System.Windows.Forms.Shortcut.F6;
			this.viewOutputMenuItem.Text = "&Output";
			this.viewOutputMenuItem.Click += new System.EventHandler(this.changeView);
			// 
			// viewThresholdedMenuItem
			// 
			this.viewThresholdedMenuItem.Checked = true;
			this.viewThresholdedMenuItem.Index = 2;
			this.viewThresholdedMenuItem.RadioCheck = true;
			this.viewThresholdedMenuItem.Shortcut = System.Windows.Forms.Shortcut.F7;
			this.viewThresholdedMenuItem.Text = "&Thresholded Output";
			this.viewThresholdedMenuItem.Click += new System.EventHandler(this.changeView);
			// 
			// animateMenu
			// 
			this.animateMenu.Index = 1;
			this.animateMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						this.animateOnMenuItem});
			this.animateMenu.Text = "&Animate";
			// 
			// animateOnMenuItem
			// 
			this.animateOnMenuItem.Checked = true;
			this.animateOnMenuItem.Index = 0;
			this.animateOnMenuItem.Shortcut = System.Windows.Forms.Shortcut.F8;
			this.animateOnMenuItem.Text = "&On";
			this.animateOnMenuItem.Click += new System.EventHandler(this.animateOnOff);
			// 
			// RenderWindow
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.BackColor = System.Drawing.Color.White;
			this.ClientSize = new System.Drawing.Size(200, 137);
			this.ControlBox = false;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Menu = this.mainMenu;
			this.Name = "RenderWindow";
			this.Text = "3D Preview";
			this.TopMost = true;

		}
		#endregion

		private void changeView(object sender, System.EventArgs e)
		{
			viewInputMenuItem.Checked = false;
			viewOutputMenuItem.Checked = false;
			viewThresholdedMenuItem.Checked = false;
			((MenuItem)sender).Checked = true;
		}

		private void animateOnOff(object sender, System.EventArgs e)
		{
			animateOnMenuItem.Checked = !animateOnMenuItem.Checked;
		}

		static internal bool IsPowerOfTwo(int n)
		{
			int i = 1;
			while(i < n)
			{
				i *= 2;
			}
			return i == n;
		}

		internal bool MulticolorCombine(out string errorMessage, bool twoChannels, string[] inputFileNames, string outputFileName)
		{
			if(myDevice == null)
			{
				errorMessage = "DirectX not initialized properly.";
				return false;
			}

			Texture[] inputTextures = new Texture[4];
			Texture outputTexture = null;
			try
			{
				for(int i = 0; i < (twoChannels ? 2 : 4); i++)
				{
					inputTextures[i] = TextureLoader.FromFile(myDevice, inputFileNames[i]);
				}

				int numLevels = inputTextures[0].LevelCount;
				int width = inputTextures[0].GetSurfaceLevel(0).Description.Width;
				int height = inputTextures[0].GetSurfaceLevel(0).Description.Height;

				if(!IsPowerOfTwo(width))
				{
					throw new Exception("The width of texture 1 is not a power of 2.");
				}
				if(!IsPowerOfTwo(height))
				{
					throw new Exception("The height of texture 1 is not a power of 2.");
				}

				// check consistency
				for(int i = 0; i < (twoChannels ? 2 : 4); i++)
				{
					if(inputTextures[i].GetSurfaceLevel(0).Description.Format != Format.L8)
					{
						throw new Exception("Wrong format in texture " + (i+1) + ".");
					}
					// actually, the following tests could be skipped for i == 0
					if(inputTextures[i].LevelCount != numLevels)
					{
						throw new Exception("Wrong number of levels in texture " + (i+1) + ".");
					}
					if(inputTextures[i].GetSurfaceLevel(0).Description.Width != width)
					{
						throw new Exception("Wrong width of texture " + (i+1) + ".");
					}
					if(inputTextures[i].GetSurfaceLevel(0).Description.Height != height)
					{
						throw new Exception("Wrong height of texture " + (i+1) + ".");
					}
				}

				outputTexture = new Texture(myDevice, width, height, numLevels, 0,
					twoChannels ? Format.A8L8 : Format.A8R8G8B8, Pool.Managed);

				for(int level = 0; level < numLevels; level++)
				{
					byte[][,] texData = new byte[twoChannels ? 2 : 4][,];
					int h = height >> level;
					int w = width >> level;
					int[] size = new int[]{h, w};
					for(int i = 0; i < (twoChannels ? 2 : 4); i++)
					{
						texData[i] = (byte[,])inputTextures[i].LockRectangle(typeof(byte), level, LockFlags.ReadOnly, size);
					}

					if(twoChannels)
					{
						short[,] outputTexData = (short[,])outputTexture.LockRectangle(typeof(short), level, LockFlags.None, size);
						
						for(int j = 0; j < h; j++)
						{
							for(int i = 0; i < w; i++)
							{
								outputTexData[j, i] = (short)((texData[1][j, i] << 8) | texData[0][j, i]);
							}
						}
					}
					else
					{
						int[,] outputTexData = (int[,])outputTexture.LockRectangle(typeof(int), level, LockFlags.None, size);
						
						for(int j = 0; j < h; j++)
						{
							for(int i = 0; i < w; i++)
							{
								outputTexData[j, i] = (texData[1][j, i]<<24) | (texData[1][j, i]<<16) | (texData[1][j, i]<<8) | texData[0][j, i];
							}
						}
					}

					outputTexture.UnlockRectangle(level);
					for(int i = 0; i < (twoChannels ? 2 : 4); i++)
					{
						inputTextures[i].UnlockRectangle(level);
					}
				}

				TextureLoader.Save(outputFileName, ImageFileFormat.Dds, outputTexture);
			}
			catch(Exception e)
			{
				errorMessage = e.Message;
				return false;
			}
			finally
			{
				if(outputTexture != null)
				{
					outputTexture.Dispose();
				}
				for(int i = 0; i < (twoChannels?2:4); i++)
				{
					if(inputTextures[i] != null)
					{
						inputTextures[i].Dispose();
					}
				}
			}

			errorMessage = "";
			return true;
		}
	}

	struct DualTextureVertex
	{
		public Vector3 Position;
		public Vector2 uv;
		public Vector2 uvScaled;
		
		public DualTextureVertex(float x, float y, float z, float u, float v, float uScaled, float vScaled)
		{
			Position = new Vector3(x, y, z);
			uv = new Vector2(u, v);
			uvScaled = new Vector2(uScaled, vScaled);
		}
	}
}
