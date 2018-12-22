using System;
using System.Drawing;
using System.Windows.Forms;
using System.Drawing.Imaging;
namespace Netterpillars {
	public class Sprite : GameEngine {
		// Images path and size, to be used by the child classes
		public const string IMAGE_PATH = @"..\..\Images";
		public const int IMAGE_SIZE = 15;

		protected Bitmap Source;
		public CompassDirections Direction;
		public Point Location;
		public ScaleSizes Scale = ScaleSizes.Sprite;

		public enum ScaleSizes {
			Pixel = 1,
			Sprite = IMAGE_SIZE
		};

		public enum CompassDirections {
			North = 1,
			NorthEast = 2,
			East = 3,
			SouthEast = 4,
			South = 5,
			SouthWest = 6,
			West = 7,
			NorthWest = 8
		};

		public Sprite() {
			// this empty constructor is to be used by the child classes when they 
			//  want to implement everything from the ground up
		}
		//
		public Sprite(string strImageName) {
			Source = Load(strImageName);
		}

		public Sprite(string strImageName, Point point) {
			Source = Load(strImageName);
			Location = point;
		}

		public Bitmap Load(string strImageName) {
			Bitmap Load_result;
			Color BackColor;

			try {
				Load_result = (Bitmap)Bitmap.FromFile(strImageName);
				// The transparent color (keycolor) was not informed, then it will be the color of the first pixel
				BackColor = Load_result.GetPixel(0, 0);
				Load_result.MakeTransparent(BackColor);
			}
			catch {
				MessageBox.Show("An image file was not found."+Keys.Enter+"Please make sure that the file "+strImageName+" exists.", ".Netterpillars", MessageBoxButtons.OK, MessageBoxIcon.Stop);
				Load_result = null;
			}
			return Load_result;
		}

		public Bitmap Load(string strImageName, Color keycolor) {
			Bitmap Load_result;
			try {
				Load_result = (Bitmap)Bitmap.FromFile(strImageName);
				Load_result.MakeTransparent(keycolor);
			}
			catch {
				MessageBox.Show("An image file was not found."+Keys.Enter+"Please make sure that the file "+strImageName+" exists.", ".Netterpillars", MessageBoxButtons.OK, MessageBoxIcon.Stop);
				Load_result = null;
			}
			return Load_result;
		}

		public Sprite(string strImageNamem, Color keycolor) {
			Load(strImageNamem, keycolor);
		}


		// This overloads is intended to be used by objects that need more than one bitmap (animated sprites)
		public void Draw(Image Source, System.IntPtr winHandle) {
			Graphics graphBack = Graphics.FromHwnd(winHandle);
			graphBack.DrawImageUnscaled(Source, Location.X * (int)Scale, Location.Y*(int)Scale);
			graphBack.Dispose();
		}

		public void Erase(System.IntPtr winHandle) {
			Graphics graphBack = Graphics.FromHwnd(winHandle);
			graphBack.DrawImage(BackgroundImage, new Rectangle(Location.X*(int)Scale, Location.Y*(int)Scale, IMAGE_SIZE, IMAGE_SIZE), new Rectangle(Location.X*(int)Scale, Location.Y*(int)Scale, IMAGE_SIZE, IMAGE_SIZE), GraphicsUnit.Pixel);
			graphBack.Dispose();
		}

		public void Draw(System.IntPtr winHandle) {
			Graphics graphBack = Graphics.FromHwnd(winHandle);
			graphBack.DrawImageUnscaled(Source, Location.X*(int)Scale, Location.Y*(int)Scale);
			graphBack.Dispose();
		}
	}
}
