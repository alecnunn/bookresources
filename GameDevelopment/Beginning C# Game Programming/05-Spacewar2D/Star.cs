using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace SpaceWar {
	/// <summary>
	/// Drawing routine for the star background.
	/// </summary>
	public class Star {
		Point location;

		public Star(Rectangle screenBounds) {
			location = new Point(0);
			location.X = Constants.random.Next(screenBounds.Width) + screenBounds.X;
			location.Y = Constants.random.Next(screenBounds.Height) + screenBounds.Y;
		}

		public void Draw(Surface surface) {
			surface.DrawLine(location.X, location.Y, location.X + 1, location.Y + 1);
		}
	}
}
