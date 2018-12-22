using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
namespace SpaceDonuts {
	public class TileSet {
		private Texture texture;

		public Texture Texture {
			get {
				return texture;
			}
		}
		private int xOrigin;

		public int XOrigin {
			get {
				return xOrigin;
			}
		}
		private int yOrigin;

		public int YOrigin {
			get {
				return yOrigin;
			}
		} 
		private int numberFrameRows;

		public int NumberFrameRows {
			get {
				return numberFrameRows;
			}
		}
		private int numberFrameColumns;

		public int NumberFrameColumns {
			get {
				return numberFrameColumns;
			}
		}
		private int xExtent;

		public int ExtentX {
			get {
				return xExtent;
			}
		}

		private int yExtent;
		public int ExtentY {
			get {
				return yExtent;
			}
		}

		public TileSet(Texture tex, int startX, int startY, int rowCount, int columnCount, int xWidth, int yHeight) {
			xOrigin = startX;
			yOrigin = startY;
			xExtent = xWidth;
			yExtent = yHeight;
			numberFrameRows = rowCount;
			numberFrameColumns = columnCount;
			texture = tex;
		}
	}
}