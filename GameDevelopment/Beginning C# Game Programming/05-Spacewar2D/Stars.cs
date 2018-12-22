using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace SpaceWar {
	/// <summary>
	/// Draw the starry background of the game screen
	/// </summary>
	public class Stars {
		Star[] stars;

		public Stars(Rectangle screenBounds, int count) {
			stars = new Star[count];

			for (int i = 0; i < count; i++) {
				stars[i] = new Star(screenBounds);
			}
		}

		public void Draw(Surface surface) {
			surface.ForeColor = Color.FromArgb(Constants.StarColorFull);
			foreach (Star star in stars) {
				star.Draw(surface);
			}
			int index = Constants.random.Next(stars.Length);
			surface.ForeColor = Color.FromArgb(Constants.StarColorDim);
			stars[index].Draw(surface);
		}
	}
}
