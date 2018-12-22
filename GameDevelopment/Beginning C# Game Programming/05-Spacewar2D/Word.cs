using System;
using System.Collections;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace SpaceWar {
	/// <summary>
	/// Creates words from the letter vectors and draws complete words.
	/// </summary>
	public class Word {
		string word;
		ArrayList letters = new ArrayList();

		public Word(string word, float scale) {
			this.word = word;

			foreach (char c in word) {
				Letter letter = new Letter(c);
				letter.SetLetter(scale);
				letters.Add(letter);
			}
		}

		public void Draw(Surface surface, int color, int increment, Point location) {
			foreach (Letter letter in letters) {
				letter.Draw(surface, color, location);
				location.X += increment;
			}
		}
	}
}
