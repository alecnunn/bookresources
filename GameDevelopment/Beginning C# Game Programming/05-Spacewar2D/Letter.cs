namespace SpaceWar {
	using System;
	using System.Drawing;
	using System.Collections;
	using Microsoft.DirectX;
	using Microsoft.DirectX.DirectDraw;

	public enum LetterState {
		Bouncing,
		Normal,
		Exploding,
		Dead,
	}

	/// <summary>
	///    Summary description for Letter.
	/// </summary>
	public class Letter {
		char letter;
		LetterVector[] vectors;
		Point lastLocation;

		public Letter(char letter) {
			this.letter = letter;
		}

		public void SetLetter(float scale) {
			vectors = FontDraw.GetLetter(letter, scale);
		}

		public void Draw(Surface surface, int color, Point location) {
			if (vectors == null)
				return; 

			if (location != lastLocation) {
				foreach (LetterVector vector in vectors) {
					vector.Offset = location;
				}
				lastLocation = location;
			}

			surface.ForeColor = Color.FromArgb(color);
			foreach (LetterVector vector in vectors) {
				vector.Draw(surface);
			}
		}
	}
}
