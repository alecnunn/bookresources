using System;
using System.Drawing;


namespace SpaceWar {
	class FontDraw {
		public static LetterVector[] GetLetter(char letter, float scale) {
			int index = FindLetter(letter);		

			if (index == -1)
				return null;

			int vectorStart = GetVectorStart(index);
			int vectorEnd = vectorStart + VectorFontData.vectorCount[index] * 4;

			int size = (vectorEnd - vectorStart) / 4;
			LetterVector[] vectors = new LetterVector[size];

			int ind = 0;
			for (int vector = vectorStart; vector < vectorEnd; vector += 4) {
				int x1 = (int) Math.Round(VectorFontData.Vectors[vector] * scale);
				int y1 = (int) Math.Round(-VectorFontData.Vectors[vector + 1] * scale);
				int x2 = (int) Math.Round(VectorFontData.Vectors[vector + 2] * scale);
				int y2 = (int) Math.Round(-VectorFontData.Vectors[vector + 3] * scale);
				vectors[ind] = new LetterVector(new Point(x1, y1), new Point(x2, y2));
				ind++;
			}

			return(vectors);
		}

		public static void DrawLetter(Graphics g, char letter, float x, float y, float scale) {
			int index = FindLetter(letter);		

			if (index == -1)
				return;

		
			int vectorStart = GetVectorStart(index);
			int vectorEnd = vectorStart + VectorFontData.vectorCount[index] * 4;


			Pen pen = new Pen(Brushes.Red, 1.0f);
			for (int vector = vectorStart; vector < vectorEnd; vector += 4) {
				float x1 = x + VectorFontData.Vectors[vector] * scale;
				float y1 = y - VectorFontData.Vectors[vector + 1] * scale;
				float x2 = x + VectorFontData.Vectors[vector + 2] * scale;
				float y2 = y - VectorFontData.Vectors[vector + 3] * scale;

				g.DrawLine(pen, x1, y1, x2, y2);

			}

		}	

		public static int FindLetter(char letter) {
			for (int index = 0; index < VectorFontData.order.Length; index++) {
				if (VectorFontData.order[index] == letter)
					return(index);
			}
			return(-1);

		}

		public static int GetVectorStart(int max) {
			int start = 0;

			for (int index = 0; index < max; index++) {
				start += VectorFontData.vectorCount[index] * 4;
			}
			return(start);
		}

	}
}