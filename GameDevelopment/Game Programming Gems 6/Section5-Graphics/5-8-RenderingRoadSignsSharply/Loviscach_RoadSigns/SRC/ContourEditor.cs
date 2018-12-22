using System;
using System.Drawing;
using System.Windows.Forms;
using System.Drawing.Imaging;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization;
using System.Reflection;
using System.IO;

namespace BilevelTextureMagnification
{
	/// <summary>
	/// Main application class
	/// </summary>
	public class ContourEditor : System.Windows.Forms.Form
	{
		private System.ComponentModel.Container components = null;

		private System.Windows.Forms.MenuItem fileMenu;
		private System.Windows.Forms.MenuItem loadBitmapMenuItem;
		private System.Windows.Forms.MenuItem exportTextureMenuItem;
		private System.Windows.Forms.MenuItem exitMenuItem;
		private System.Windows.Forms.MenuItem saveProjectMenuItem;
		private System.Windows.Forms.MenuItem openProjectMenuItem;
		private System.Windows.Forms.MenuItem saveProjectAsMenuItem;
		private System.Windows.Forms.MainMenu mainMenu;
		private System.Windows.Forms.MenuItem viewMenu;
		private System.Windows.Forms.MenuItem viewInputMenuItem;
		private System.Windows.Forms.MenuItem viewOutputMenuItem;
		private System.Windows.Forms.MenuItem previewWithThresholdMenuItem;
		private System.Windows.Forms.MenuItem lineMenuItem1;
		private System.Windows.Forms.MenuItem lineMenuItem2;
		private System.Windows.Forms.MenuItem lineMenuItem3;
		private System.Windows.Forms.MenuItem showImageMenuItem;
		private System.Windows.Forms.MenuItem showSkeletonMenuItem;
		private System.Windows.Forms.MenuItem processMenu;
		private System.Windows.Forms.MenuItem downsampling4MenuItem;
		private System.Windows.Forms.MenuItem downsampling8MenuItem;
		private System.Windows.Forms.MenuItem downsampling16MenuItem;
		private System.Windows.Forms.MenuItem lineMenuItem4;
		private System.Windows.Forms.MenuItem zoomOffMenuItem;
		private System.Windows.Forms.MenuItem zoom2MenuItem;
		private System.Windows.Forms.MenuItem zoom4MenuItem;
		private System.Windows.Forms.MenuItem lineMenuItem5;
		private System.Windows.Forms.MenuItem adjustMetricMenuItem;
		private System.Windows.Forms.MenuItem lineMenuItem6;
		private System.Windows.Forms.MenuItem previewMenuItem;

		private OptimizedTexture myOptimizedTexture;
		private Bitmap displayBitmap;
		private bool displayBitmapIsDirty;
		private int oldBaseX;
		private int oldBaseY;
		private string theFile;

		private AdjustMetricForm myAdjustMetricForm;
		private System.Windows.Forms.MenuItem multicolorMenuItem;
		private System.Windows.Forms.MenuItem extrasMenu;
		private System.Windows.Forms.MenuItem helpMenu;
		private System.Windows.Forms.MenuItem infoMenuItelm;
		private RenderWindow myRenderWindow;

		public ContourEditor()
		{
			InitializeComponent();
			SetStyle(ControlStyles.DoubleBuffer | ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint, true);

			Assembly assem = Assembly.GetAssembly(typeof(ContourEditor));
			Stream st = assem.GetManifestResourceStream("BilievelTextureMagnification.Curvy.jpg");
			myOptimizedTexture = new OptimizedTexture(st, getDownsampling());
			st.Close();
			st = null;

			myAdjustMetricForm = new AdjustMetricForm();
			myRenderWindow = RenderWindow.Create();
			myOptimizedTexture.SetEditor(this, myAdjustMetricForm, myRenderWindow);
			AutoScrollMinSize = myOptimizedTexture.GetSize(getZoom());
			Dirty();
		}

		protected override void Dispose( bool disposing )
		{
			if(myOptimizedTexture != null)
			{
				myOptimizedTexture.Dispose(); // first stop this one because it does the updating
			}
			myOptimizedTexture = null;

			if(myRenderWindow != null)
			{
				myRenderWindow.Kill();
			}
			myRenderWindow = null;

			if(myAdjustMetricForm != null)
			{
				myAdjustMetricForm.Dispose();
			}
			myAdjustMetricForm = null;

			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(ContourEditor));
			this.mainMenu = new System.Windows.Forms.MainMenu();
			this.fileMenu = new System.Windows.Forms.MenuItem();
			this.openProjectMenuItem = new System.Windows.Forms.MenuItem();
			this.saveProjectMenuItem = new System.Windows.Forms.MenuItem();
			this.saveProjectAsMenuItem = new System.Windows.Forms.MenuItem();
			this.lineMenuItem1 = new System.Windows.Forms.MenuItem();
			this.loadBitmapMenuItem = new System.Windows.Forms.MenuItem();
			this.exportTextureMenuItem = new System.Windows.Forms.MenuItem();
			this.lineMenuItem2 = new System.Windows.Forms.MenuItem();
			this.exitMenuItem = new System.Windows.Forms.MenuItem();
			this.processMenu = new System.Windows.Forms.MenuItem();
			this.downsampling4MenuItem = new System.Windows.Forms.MenuItem();
			this.downsampling8MenuItem = new System.Windows.Forms.MenuItem();
			this.downsampling16MenuItem = new System.Windows.Forms.MenuItem();
			this.lineMenuItem5 = new System.Windows.Forms.MenuItem();
			this.adjustMetricMenuItem = new System.Windows.Forms.MenuItem();
			this.viewMenu = new System.Windows.Forms.MenuItem();
			this.viewInputMenuItem = new System.Windows.Forms.MenuItem();
			this.viewOutputMenuItem = new System.Windows.Forms.MenuItem();
			this.previewWithThresholdMenuItem = new System.Windows.Forms.MenuItem();
			this.lineMenuItem3 = new System.Windows.Forms.MenuItem();
			this.showImageMenuItem = new System.Windows.Forms.MenuItem();
			this.showSkeletonMenuItem = new System.Windows.Forms.MenuItem();
			this.lineMenuItem4 = new System.Windows.Forms.MenuItem();
			this.zoomOffMenuItem = new System.Windows.Forms.MenuItem();
			this.zoom2MenuItem = new System.Windows.Forms.MenuItem();
			this.zoom4MenuItem = new System.Windows.Forms.MenuItem();
			this.lineMenuItem6 = new System.Windows.Forms.MenuItem();
			this.previewMenuItem = new System.Windows.Forms.MenuItem();
			this.extrasMenu = new System.Windows.Forms.MenuItem();
			this.multicolorMenuItem = new System.Windows.Forms.MenuItem();
			this.helpMenu = new System.Windows.Forms.MenuItem();
			this.infoMenuItelm = new System.Windows.Forms.MenuItem();
			// 
			// mainMenu
			// 
			this.mainMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.fileMenu,
																					 this.processMenu,
																					 this.viewMenu,
																					 this.extrasMenu,
																					 this.helpMenu});
			// 
			// fileMenu
			// 
			this.fileMenu.Index = 0;
			this.fileMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.openProjectMenuItem,
																					 this.saveProjectMenuItem,
																					 this.saveProjectAsMenuItem,
																					 this.lineMenuItem1,
																					 this.loadBitmapMenuItem,
																					 this.exportTextureMenuItem,
																					 this.lineMenuItem2,
																					 this.exitMenuItem});
			this.fileMenu.Text = "&File";
			// 
			// openProjectMenuItem
			// 
			this.openProjectMenuItem.Index = 0;
			this.openProjectMenuItem.Text = "&Open Project ...";
			this.openProjectMenuItem.Click += new System.EventHandler(this.openProject);
			// 
			// saveProjectMenuItem
			// 
			this.saveProjectMenuItem.Index = 1;
			this.saveProjectMenuItem.Text = "&Save Project";
			this.saveProjectMenuItem.Click += new System.EventHandler(this.saveProject);
			// 
			// saveProjectAsMenuItem
			// 
			this.saveProjectAsMenuItem.Index = 2;
			this.saveProjectAsMenuItem.Text = "Save Project &As ...";
			this.saveProjectAsMenuItem.Click += new System.EventHandler(this.saveProjectAs);
			// 
			// lineMenuItem1
			// 
			this.lineMenuItem1.Index = 3;
			this.lineMenuItem1.Text = "-";
			// 
			// loadBitmapMenuItem
			// 
			this.loadBitmapMenuItem.Index = 4;
			this.loadBitmapMenuItem.Text = "Load &Bitmap ...";
			this.loadBitmapMenuItem.Click += new System.EventHandler(this.loadBitmap);
			// 
			// exportTextureMenuItem
			// 
			this.exportTextureMenuItem.Index = 5;
			this.exportTextureMenuItem.Text = "Export &Texture ...";
			this.exportTextureMenuItem.Click += new System.EventHandler(this.exportTexture);
			// 
			// lineMenuItem2
			// 
			this.lineMenuItem2.Index = 6;
			this.lineMenuItem2.Text = "-";
			// 
			// exitMenuItem
			// 
			this.exitMenuItem.Index = 7;
			this.exitMenuItem.Text = "E&xit";
			this.exitMenuItem.Click += new System.EventHandler(this.exit);
			// 
			// processMenu
			// 
			this.processMenu.Index = 1;
			this.processMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						this.downsampling4MenuItem,
																						this.downsampling8MenuItem,
																						this.downsampling16MenuItem,
																						this.lineMenuItem5,
																						this.adjustMetricMenuItem});
			this.processMenu.Text = "&Process";
			// 
			// downsampling4MenuItem
			// 
			this.downsampling4MenuItem.Index = 0;
			this.downsampling4MenuItem.RadioCheck = true;
			this.downsampling4MenuItem.Text = "Downsampling by &4";
			this.downsampling4MenuItem.Click += new System.EventHandler(this.changeDownsampling);
			// 
			// downsampling8MenuItem
			// 
			this.downsampling8MenuItem.Index = 1;
			this.downsampling8MenuItem.RadioCheck = true;
			this.downsampling8MenuItem.Text = "Downsampling by &8";
			this.downsampling8MenuItem.Click += new System.EventHandler(this.changeDownsampling);
			// 
			// downsampling16MenuItem
			// 
			this.downsampling16MenuItem.Checked = true;
			this.downsampling16MenuItem.Index = 2;
			this.downsampling16MenuItem.RadioCheck = true;
			this.downsampling16MenuItem.Text = "Downsampling by &16";
			this.downsampling16MenuItem.Click += new System.EventHandler(this.changeDownsampling);
			// 
			// lineMenuItem5
			// 
			this.lineMenuItem5.Index = 3;
			this.lineMenuItem5.Text = "-";
			// 
			// adjustMetricMenuItem
			// 
			this.adjustMetricMenuItem.Index = 4;
			this.adjustMetricMenuItem.Text = "Show/Hide &Adjust Metric Dialog";
			this.adjustMetricMenuItem.Click += new System.EventHandler(this.adjustMetric);
			// 
			// viewMenu
			// 
			this.viewMenu.Index = 2;
			this.viewMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.viewInputMenuItem,
																					 this.viewOutputMenuItem,
																					 this.previewWithThresholdMenuItem,
																					 this.lineMenuItem3,
																					 this.showImageMenuItem,
																					 this.showSkeletonMenuItem,
																					 this.lineMenuItem4,
																					 this.zoomOffMenuItem,
																					 this.zoom2MenuItem,
																					 this.zoom4MenuItem,
																					 this.lineMenuItem6,
																					 this.previewMenuItem});
			this.viewMenu.RadioCheck = true;
			this.viewMenu.Text = "&View";
			// 
			// viewInputMenuItem
			// 
			this.viewInputMenuItem.Index = 0;
			this.viewInputMenuItem.RadioCheck = true;
			this.viewInputMenuItem.Text = "&Input";
			this.viewInputMenuItem.Click += new System.EventHandler(this.changeViewType);
			// 
			// viewOutputMenuItem
			// 
			this.viewOutputMenuItem.Index = 1;
			this.viewOutputMenuItem.RadioCheck = true;
			this.viewOutputMenuItem.Text = "&Output";
			this.viewOutputMenuItem.Click += new System.EventHandler(this.changeViewType);
			// 
			// previewWithThresholdMenuItem
			// 
			this.previewWithThresholdMenuItem.Checked = true;
			this.previewWithThresholdMenuItem.Index = 2;
			this.previewWithThresholdMenuItem.RadioCheck = true;
			this.previewWithThresholdMenuItem.Text = "&Thresholded Output";
			this.previewWithThresholdMenuItem.Click += new System.EventHandler(this.changeViewType);
			// 
			// lineMenuItem3
			// 
			this.lineMenuItem3.Index = 3;
			this.lineMenuItem3.Text = "-";
			// 
			// showImageMenuItem
			// 
			this.showImageMenuItem.Checked = true;
			this.showImageMenuItem.Index = 4;
			this.showImageMenuItem.Text = "Show &Image";
			this.showImageMenuItem.Click += new System.EventHandler(this.showImage);
			// 
			// showSkeletonMenuItem
			// 
			this.showSkeletonMenuItem.Checked = true;
			this.showSkeletonMenuItem.Index = 5;
			this.showSkeletonMenuItem.Text = "Show Skeleton";
			this.showSkeletonMenuItem.Click += new System.EventHandler(this.showSkeleton);
			// 
			// lineMenuItem4
			// 
			this.lineMenuItem4.Index = 6;
			this.lineMenuItem4.Text = "-";
			// 
			// zoomOffMenuItem
			// 
			this.zoomOffMenuItem.Checked = true;
			this.zoomOffMenuItem.Index = 7;
			this.zoomOffMenuItem.RadioCheck = true;
			this.zoomOffMenuItem.Text = "Zoom &Off";
			this.zoomOffMenuItem.Click += new System.EventHandler(this.changeZoom);
			// 
			// zoom2MenuItem
			// 
			this.zoom2MenuItem.Index = 8;
			this.zoom2MenuItem.RadioCheck = true;
			this.zoom2MenuItem.Text = "Zoom &2x";
			this.zoom2MenuItem.Click += new System.EventHandler(this.changeZoom);
			// 
			// zoom4MenuItem
			// 
			this.zoom4MenuItem.Index = 9;
			this.zoom4MenuItem.RadioCheck = true;
			this.zoom4MenuItem.Text = "Zoom &4x";
			this.zoom4MenuItem.Click += new System.EventHandler(this.changeZoom);
			// 
			// lineMenuItem6
			// 
			this.lineMenuItem6.Index = 10;
			this.lineMenuItem6.Text = "-";
			// 
			// previewMenuItem
			// 
			this.previewMenuItem.Index = 11;
			this.previewMenuItem.Text = "Show/Hide &3D Preview";
			this.previewMenuItem.Click += new System.EventHandler(this.showHidePreview);
			// 
			// extrasMenu
			// 
			this.extrasMenu.Index = 3;
			this.extrasMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					   this.multicolorMenuItem});
			this.extrasMenu.Text = "&Extras";
			// 
			// multicolorMenuItem
			// 
			this.multicolorMenuItem.Index = 0;
			this.multicolorMenuItem.Text = "&Multicolor ...";
			this.multicolorMenuItem.Click += new System.EventHandler(this.multicolor);
			// 
			// helpMenu
			// 
			this.helpMenu.Index = 4;
			this.helpMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.infoMenuItelm});
			this.helpMenu.Text = "&Help";
			// 
			// infoMenuItelm
			// 
			this.infoMenuItelm.Index = 0;
			this.infoMenuItelm.Text = "Inf&o ...";
			this.infoMenuItelm.Click += new System.EventHandler(this.info);
			// 
			// ContourEditor
			// 
			this.AutoScale = false;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.AutoScroll = true;
			this.BackColor = System.Drawing.Color.White;
			this.ClientSize = new System.Drawing.Size(904, 726);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.KeyPreview = true;
			this.Menu = this.mainMenu;
			this.Name = "ContourEditor";
			this.Text = "Contour Editor";

		}
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new ContourEditor());
		}

		protected override void OnKeyDown(System.Windows.Forms.KeyEventArgs e)
		{
			switch(e.KeyCode)
			{
				case Keys.NumPad0:
				case Keys.N:
					myOptimizedTexture.ResetDirection();
					Invalidate();
					e.Handled = true;
					break;
				case Keys.NumPad2:
				case Keys.Down:
					myOptimizedTexture.MoveDown();
					Invalidate();
					e.Handled = true;
					break;
				case Keys.NumPad4:
				case Keys.Left:
					myOptimizedTexture.MoveLeft();
					Invalidate();
					e.Handled = true;
					break;
				case Keys.NumPad5:
				case Keys.M:
					myOptimizedTexture.ResetLocation();
					Invalidate();
					e.Handled = true;
					break;
				case Keys.NumPad6:
				case Keys.Right:
					myOptimizedTexture.MoveRight();
					Invalidate();
					e.Handled = true;
					break;
				case Keys.NumPad7:
				case Keys.PageUp:
					myOptimizedTexture.RotateCCW();
					Invalidate();
					e.Handled = true;
					break;
				case Keys.NumPad8:
				case Keys.Up:
					myOptimizedTexture.MoveUp();
					Invalidate();
					e.Handled = true;
					break;
				case Keys.NumPad9:
				case Keys.PageDown:
					myOptimizedTexture.RotateCW();
					Invalidate();
					e.Handled = true;
					break;
				default:
					break;
			}
		}

		protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
		{
			int baseX = AutoScrollPosition.X;
			int baseY = AutoScrollPosition.Y;

			Graphics g = e.Graphics;
			g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

			if(showImageMenuItem.Checked)
			{
				if(viewInputMenuItem.Checked)
				{
					System.Drawing.Drawing2D.Matrix oldTransform = g.Transform;
					g.TranslateTransform(baseX, baseY, System.Drawing.Drawing2D.MatrixOrder.Prepend);
					g.ScaleTransform(getZoom(), getZoom(), System.Drawing.Drawing2D.MatrixOrder.Prepend);
					g.DrawImage(myOptimizedTexture.HiresImage, 0, 0);
					g.Transform = oldTransform;
				}
				else
				{
					// rebuild if wrong size
					if(displayBitmap != null
						&& (displayBitmap.Width != ClientSize.Width || displayBitmap.Height != ClientSize.Height))
					{
						displayBitmap.Dispose();
						displayBitmap = null;
					}
					// rebuild if missing
					if(displayBitmap == null)
					{
						displayBitmap = new Bitmap(ClientSize.Width, ClientSize.Height, PixelFormat.Format24bppRgb);
						displayBitmapIsDirty = true;
						oldBaseX = baseX;
						oldBaseY = baseY;
					}
					
					if(oldBaseX != baseX || oldBaseY != baseY)
					{
						displayBitmapIsDirty = true;
						oldBaseX = baseX;
						oldBaseY = baseY;
					}

					if(displayBitmapIsDirty)
					{
						myOptimizedTexture.FillDisplayBitmap(displayBitmap, getZoom(), baseX, baseY, previewWithThresholdMenuItem.Checked);
						displayBitmapIsDirty = false;
					}
					g.DrawImageUnscaled(displayBitmap, 0, 0);
				}
			}

			if(showSkeletonMenuItem.Checked)
			{
				System.Drawing.Drawing2D.Matrix oldTransform = g.Transform;
				g.TranslateTransform(baseX, baseY, System.Drawing.Drawing2D.MatrixOrder.Prepend);
				g.ScaleTransform(getZoom(), getZoom(), System.Drawing.Drawing2D.MatrixOrder.Prepend);
				myOptimizedTexture.DrawSkeleton(g);
				g.Transform = oldTransform;
			}			
		}

		private void changeViewType(object sender, System.EventArgs e)
		{
			viewInputMenuItem.Checked = false;
			viewOutputMenuItem.Checked = false;
			previewWithThresholdMenuItem.Checked = false;
			((MenuItem)sender).Checked = true;
			Dirty();
		}

		private void showImage(object sender, System.EventArgs e)
		{
			showImageMenuItem.Checked = !showImageMenuItem.Checked;
			// don't show a blank screen but switch to other mode
			if(!showImageMenuItem.Checked && !showSkeletonMenuItem.Checked)
			{
				showSkeletonMenuItem.Checked = true;
			}
			Invalidate();
		}

		private void showSkeleton(object sender, System.EventArgs e)
		{
			showSkeletonMenuItem.Checked = !showSkeletonMenuItem.Checked;
			// don't show a blank screen but switch to other mode
			if(!showImageMenuItem.Checked && !showSkeletonMenuItem.Checked)
			{
				showImageMenuItem.Checked = true;
			}
			Invalidate();		
		}

		private void loadBitmap(object sender, System.EventArgs e)
		{
			DialogResult r = MessageBox.Show(this, "Discard the current bitmap and all editing done?", "Warning", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);
			if(r == DialogResult.OK)
			{
				using(OpenFileDialog fd = new OpenFileDialog())
				{
					fd.Filter = "Bitmap Files|*.bmp|JPEG Files|*.jpg|PNG Files|*.png";
					DialogResult dr = fd.ShowDialog();
					if(dr == DialogResult.OK)
					{
						try
						{
							OptimizedTexture newOptimizedTexture = new OptimizedTexture(fd.FileName, getDownsampling());
							if(!RenderWindow.IsPowerOfTwo(newOptimizedTexture.GetSize(1).Width) || !RenderWindow.IsPowerOfTwo(newOptimizedTexture.GetSize(1).Height))
							{
								throw new Exception("Width and height of bitmap must be powers of 2.");
							}
							myOptimizedTexture.Dispose();
							myOptimizedTexture = newOptimizedTexture;
							myOptimizedTexture.SetEditor(this, myAdjustMetricForm, myRenderWindow);
							AutoScrollMinSize = myOptimizedTexture.GetSize(getZoom());
							Dirty();
							theFile = null;
							Text = "untitled";
						}
						catch(Exception ex)
						{
							MessageBox.Show(this, "Error on loading:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
						}
					}
				}			
			}
		}

		private int getDownsampling()
		{
			if(downsampling4MenuItem.Checked)
			{
				return 4;
			}
			else if(downsampling8MenuItem.Checked)
			{
				return 8;
			}
			else
			{
				return 16;
			}
		}

		private void changeDownsampling(object sender, System.EventArgs e)
		{
			downsampling4MenuItem.Checked = false;
			downsampling8MenuItem.Checked = false;
			downsampling16MenuItem.Checked = false;
			((MenuItem)sender).Checked = true;
			DialogResult r = MessageBox.Show(this, "Discard all editing done?", "Warning", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);
			if(r == DialogResult.OK)
			{
				myOptimizedTexture.ChangeDownsampling(getDownsampling());
				Dirty();
			}
		}

		private int getZoom()
		{
			if(zoom2MenuItem.Checked)
			{
				return 2;
			}
			else if(zoom4MenuItem.Checked)
			{
				return 4;
			}
			else
			{
				return 1;
			}
		}

		private void changeZoom(object sender, System.EventArgs e)
		{
			zoomOffMenuItem.Checked = false;
			zoom2MenuItem.Checked = false;
			zoom4MenuItem.Checked = false;
			((MenuItem)sender).Checked = true;
			AutoScrollMinSize = myOptimizedTexture.GetSize(getZoom());
			Dirty();
		}
	
		protected override void OnMouseUp(MouseEventArgs e)
		{
			myOptimizedTexture.Select(e.X, e.Y, AutoScrollPosition.X, AutoScrollPosition.Y, getZoom());
			Invalidate();
			base.OnMouseUp(e);
		}

		internal void Dirty()
		{
			displayBitmapIsDirty = true;
			Invalidate();
		}

		private void exit(object sender, System.EventArgs e)
		{
			Application.Exit();
		}

		private void openProject(object sender, System.EventArgs e)
		{
			DialogResult r = MessageBox.Show(this, "Discard the current project and all editing done?", "Warning", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);
			if(r == DialogResult.OK)
			{
				using(OpenFileDialog fd = new OpenFileDialog())
				{
					fd.Filter = "Optimization Project Files|*.bilevelopt";
					DialogResult dr = fd.ShowDialog();
					if(dr == DialogResult.OK)
					{
						try
						{
							IFormatter formatter = new BinaryFormatter();
							Stream stream = new FileStream(fd.FileName, FileMode.Open, FileAccess.Read, FileShare.Read);
							OptimizedTexture newOptimizedTexture = (OptimizedTexture) formatter.Deserialize(stream);
							stream.Close();
							myOptimizedTexture.Dispose();
							myOptimizedTexture = newOptimizedTexture;
							myOptimizedTexture.SetEditor(this, myAdjustMetricForm, myRenderWindow);
							AutoScrollMinSize = myOptimizedTexture.GetSize(getZoom());
							Dirty();
							theFile = fd.FileName;
							Text = theFile;
						}
						catch(Exception ex)
						{
							MessageBox.Show(this, "Error on loading:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
						}				
					}
				}			
			}
		}

		private void saveProject(object sender, System.EventArgs e)
		{
			if(theFile == null)
			{
				saveProjectAs(sender, e);
			}
			else
			{
				commonSaveProjectAs(theFile);
			}
		}

		private void saveProjectAs(object sender, System.EventArgs e)
		{
			using(SaveFileDialog fd = new SaveFileDialog())
			{
				fd.Filter = "Optimization Project Files|*.bilevelopt";
				DialogResult dr = fd.ShowDialog();
				if(dr == DialogResult.OK)
				{
					commonSaveProjectAs(fd.FileName);
				}
			}		
		}

		private void commonSaveProjectAs(string fileName)
		{
			try
			{
				IFormatter formatter = new BinaryFormatter();
				Stream stream = new FileStream(fileName, FileMode.Create, FileAccess.Write, FileShare.None);
				formatter.Serialize(stream, myOptimizedTexture);
				stream.Close();
				theFile = fileName;
				Text = theFile;
			}
			catch(Exception ex)
			{
				MessageBox.Show(this, "Error on saving:\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void adjustMetric(object sender, System.EventArgs e)
		{
			if(!myAdjustMetricForm.Visible)
			{
				myAdjustMetricForm.Show();
			}
			else
			{
				myAdjustMetricForm.Hide();
			}
		}

		private void showHidePreview(object sender, System.EventArgs e)
		{
			if(!myRenderWindow.Visible)
			{
				myRenderWindow.Show();
			}
			else
			{
				myRenderWindow.Hide();
			}
		}

		private void exportTexture(object sender, System.EventArgs e)
		{
			using(SaveFileDialog fd = new SaveFileDialog())
			{
				fd.Filter = "Texture Files|*.dds";
				DialogResult dr = fd.ShowDialog();
				if(dr == DialogResult.OK)
				{
					myRenderWindow.SaveTexture(fd.FileName);
				}
			}
		}

		private void multicolor(object sender, System.EventArgs e)
		{
			using(MulticolorForm mf = new MulticolorForm(myRenderWindow))
			{
				mf.ShowDialog();
			}
		}

		private void info(object sender, System.EventArgs e)
		{
			MessageBox.Show(this, "Contour Editor V. 1.0\n2005 by Jörn Loviscach\nwww.l7h.cn\nFor details see Game Programming Gems Vol. 6", "Info");
		}
	}
}
