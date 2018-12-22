using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
namespace PocketNettrix {
	public class GameField {
		// The game field witdth will be exactly 16 bits (2 bytes)
		public const int Width = 16;
		public const int Height = 30;
		// The Size of each square
		public const int SquareSize = 10;

		private static Square[,] arrGameField = new Square[Width, Height];
		private static int[] arrBitGameField = new int[Height];
		public static Color BackColor;

		private const int bitEmpty = 0x0;       //00000000 0000000
		private const int bitFull = 0xFFFF;     //11111111 1111111

		//Update to Pocket PC - Create rectangles to store the screen position 
		public static Rectangle rectBackground;
		public static Rectangle rectNextBlock;

		// Update to Pocket PC - New Global Graphics objects 
		public static Graphics GraphNextBlock;
		public static Graphics GraphBackground;
		
		// x goes from 0 to Width -1
		// y goes from 0 to Height -1
		public static bool IsEmpty(int x, int y) {
			// If the Y or X is beyond the game field, return false
			if ((y<0||y>=Height)||(x<0||x>=Width)) {
				return false;
			}
				//  Test the Xth bit of the Yth line of the game field
			else if((arrBitGameField[y] & (1<<x)) != 0) {
				return false;
			}
			return true;
		}

		public static int CheckLines() {
			int CheckLines_result = 0; //returns the number of lines completed
			int y = Height - 1;

			while ( y >= 0) {
				// stops the loop when the blank lines are reached
				if (arrBitGameField[y]==bitEmpty) y = 0;

				// If all the bits of the line are set, then increment the 
				//    counter to clear the line and move all above lines down
				if (arrBitGameField[y]==bitFull) {
					CheckLines_result++;

					// Move all next lines down
					for(int index = y; index >= 0; index--) {
						// if the current line is NOT the first of the game field,
						//  copy the line above
						if (index>0) {
							// Copy the bits from the line above 
							arrBitGameField[index] = arrBitGameField[index-1];

							// Copy each of the squares from the line above 
							for(int x=0; x<Width; x++) {
								// Copy the square
								arrGameField[x, index] = arrGameField[x, index-1];
								// update the Location property of the square
								if (arrGameField[x, index] != null)
									arrGameField[x, index].Location = 
										new Point(arrGameField[x, index].Location.X, arrGameField[x, index].Location.Y+SquareSize);
							}
						}
						else {
							// if the current line is the first of the game field
							//  just clear the line
							arrBitGameField[index] = bitEmpty;
							for(int x=0; x<Width; x++) {
								arrGameField[x, index] = null;
							}
						}
					}
				}
				else {
					y--;
				}
			}
			return CheckLines_result;
		}

		public static void StopSquare(Square square, int x, int y) {
			arrBitGameField[y] = arrBitGameField[y] | (1<<x);
			arrGameField[x, y] = square;
		}

		public static void Redraw() {
			for(int y=Height-1; y>=0; y--) 
				if (arrBitGameField[y]!=bitEmpty) 
					for(int x=Width-1; x>=0; x--) 
						if (arrGameField[x, y] != null) arrGameField[x, y].Show(GraphBackground);
		}

		public static void Reset() {
			// Clean the game field
			for(int i=Height; i<=0; i--) {
				//  Clear the lines
				arrBitGameField[i] = bitEmpty;
				for(int x=0; x<Width; x++) {
					arrGameField[x, i] = null;
				}
			}
		}

		// Update to Pocket PC: Create graphics objects that will be used throughout the application
		public static void Initialize(Form frmSource, PictureBox PicBackground, PictureBox PicNextBlock) {
			// Set the game field backcolor
			BackColor = Color.Black;

			// Update to Pocket PC - Create rectangles to help on drawing to screen
			rectBackground = new Rectangle(0, 0, PicBackground.Width, PicBackground.Height);
			rectNextBlock = new Rectangle(0, 0, PicNextBlock.Width, PicNextBlock.Height);

			GraphBackground = Graphics.FromImage(PicBackground.Image);
			GraphNextBlock = Graphics.FromImage(PicNextBlock.Image);
		}

		// Update to Pocket PC: Erase the screen
		public static void Clear() {
			//Since we are working in a solid background, we can just 
			//  draw a solid rectangle in order to "clear" the game field
			GraphBackground.FillRectangle(new SolidBrush(BackColor), rectBackground);
			// Clear the "next block" image
			GraphNextBlock.FillRectangle(new SolidBrush(BackColor), rectNextBlock);
		}

	}
}