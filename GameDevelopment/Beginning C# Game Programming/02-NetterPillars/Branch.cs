using System;
using System.Drawing;
using System.Windows.Forms;
namespace Netterpillars {
	public class Branch : Sprite {
		private Bitmap BranchStart;
		private Bitmap[] BranchMiddle;
		private Bitmap BranchEnd;
		public int branchSize;

		public Branch(CompassDirections branchDirection, int initialSize) {
			BranchMiddle = new Bitmap[initialSize-2];
			string imagePrefix;

			branchSize = initialSize;
			Direction = branchDirection;
			// Picks the prefix for the branch - Horizontal or vertical?
			imagePrefix = "Hor"; // Default direction is east-west (Horizontal)
			if (Direction==Sprite.CompassDirections.North||Direction==Sprite.CompassDirections.South) {
				imagePrefix = "Vert";
			}
			// Load the top, the middle parts and the end of the branch
			//  Magenta is the colorkey (which will be transparent) for the Load Method 
			BranchStart = Load(Application.StartupPath+"\\"+IMAGE_PATH+"\\"+imagePrefix+"BranchStart.gif", Color.FromArgb(255, 255, 0, 204));
			for(int i=0; i<=branchSize-3; i++) {
				BranchMiddle[i] = Load(Application.StartupPath+"\\"+IMAGE_PATH+"\\"+imagePrefix+"BranchMiddle.gif", Color.FromArgb(255, 255, 0, 204));
			}
			BranchEnd = Load(Application.StartupPath+"\\"+IMAGE_PATH+"\\"+imagePrefix+"BranchEnd.gif", Color.FromArgb(255, 255, 0, 204));
		}

		public void Draw(System.IntPtr winHandle, int x, int y) {
			// Sets the location and draws the start of the branch
			Location = new Point(x, y);
			base.Draw(BranchStart, winHandle);
			// Sets the location and draws each of the branch middle parts
			if (Direction==Sprite.CompassDirections.North||Direction==Sprite.CompassDirections.South) {
				// it's a horizontal branch
				for(int i=0; i<=branchSize-3; i++) {
					y++;
					Location = new Point(x, y);
					base.Draw(BranchMiddle[i], winHandle);
				}
				y++;
			}
			else {
				// it's a vertical branch
				for(int i=0; i<=branchSize-3; i++) {
					x++;
					Location = new Point(x, y);
					base.Draw(BranchMiddle[i], winHandle);
				}
				x++;
			}
			// Sets the location and draws the start of the branch
			Location = new Point(x, y);
			base.Draw(BranchEnd, winHandle);
		}
	}
}
