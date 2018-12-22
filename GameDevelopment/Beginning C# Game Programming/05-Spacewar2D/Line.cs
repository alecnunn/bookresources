using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace SpaceWar {
	/// <summary>
	/// Summary description for Line.
	/// </summary>
	public class Line {
		Point end1;
		Point end2;
		Point offset;
		bool visible = true;

		public Line(Point end1, Point end2) {
			this.end1 = end1;
			this.end2 = end2;
		}

		public Point End1 {
			get {
				return end1;
			}
		}

		public Point End2 {
			get {
				return end2;
			}
		}

		public Point Offset {
			get {
				return offset;
			}
			set {
				offset = value;
			}
		}

		public bool Visible {
			get {
				return visible;
			}
			set {
				visible = value;
			}
		}

		public void Draw(Surface surface) {
			if (!visible)
				return;

			surface.DrawLine(end1.X + offset.X, end1.Y + offset.Y,
				end2.X + offset.X, end2.Y + offset.Y);
		}
	}
}
