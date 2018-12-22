using System;
using System.Drawing;
using System.Drawing.Drawing2D;
namespace PocketNettrix {
	public class Square {
		private Point squareLocation;

		public Point Location {
			get { return squareLocation; }
			set { squareLocation = value;	}
		}

		private Size squareSize;

		public Size Size {
			get { return squareSize; }
			set { squareSize = value;	}
		}

		private Color foregroundColor;

		public Color ForeColor {
			get { return foregroundColor;	}
			set { foregroundColor = value; }
		}

		private Color backgroundColor;

		public Color BackColor {
			get { return backgroundColor; }
			set { backgroundColor = value; }
		}


		public void Show(Graphics g) {
			g.FillRectangle(new SolidBrush(backgroundColor), Location.X, Location.Y, Size.Width, Size.Height);
			g.DrawRectangle(new Pen(foregroundColor),Location.X, Location.Y, Size.Width-1, Size.Height-1);
		}

		public void Hide(Graphics g) {
			g.FillRectangle(new SolidBrush(GameField.BackColor),
				new Rectangle(Location.X, Location.Y, Size.Width, Size.Height));
		}

		public Square(Size initialSize, Color initialBackColor, Color initialForeColor) {
			Size = initialSize;
			BackColor = initialBackColor;
			ForeColor = initialForeColor;
		}
	}
}
