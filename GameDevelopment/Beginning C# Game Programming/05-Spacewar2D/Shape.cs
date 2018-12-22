using System;
using System.Drawing;
using System.Collections;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace SpaceWar {
	/// <summary>
	///Shape drawing class
	/// </summary>
	public class Shape {
		Point center;
		ArrayList lines = new ArrayList();

		public Shape() {
		}

		public Point Center {
			get {
				return center;
			}
			set {
				center = value;
			}
		}

		public Line this[int index] {
			get {
				return (Line) lines[index];
			}
		}

		public void AddLine(PointF end1, PointF end2, float scale) {
			Point e1 = new Point((int) (end1.X * scale), (int) (end1.Y * scale));
			Point e2 = new Point((int) (end2.X * scale), (int) (end2.Y * scale));

			lines.Add(new Line(e1, e2));
		}

		public void Draw(Surface surface, int startIndex, int endIndex) {
			for (int index = startIndex; index <= endIndex; index++) {
				Line line = (Line) lines[index];

				line.Offset = center;
				line.Draw(surface);
			}
		}
	}
}
