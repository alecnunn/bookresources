using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace SpaceWar {
	public class LetterVector {
		Point end1;
		Point end2;
		Point offset;		// offset from origin

		public LetterVector(Point point1, Point point2) {
			end1 = point1;
			end2 = point2;
			offset = new Point();
		}

		public override string ToString() {
			return
				end1.ToString() + " " +
				end2.ToString();
		}

		public Point Offset {
			get {
				return offset;
			}
			set {
				if (offset == value)
					return;

				// update the endpoints to the new offset...
				end1.X = end1.X - offset.X + value.X;
				end1.Y = end1.Y - offset.Y + value.Y;
				end2.X = end2.X - offset.X + value.X;
				end2.Y = end2.Y - offset.Y + value.Y;
				offset = value;
			}
		}

		public void Draw(Surface surface) {
			surface.DrawLine(end1.X, end1.Y, end2.X, end2.Y);
		}
	}
}
