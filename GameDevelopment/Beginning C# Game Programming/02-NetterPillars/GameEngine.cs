using System;
using System.Drawing;
namespace Netterpillars {
	public class GameEngine {

		public enum GameFieldSizes {
			Small = 2,
			Medium = 1,
			Big = 0
		};

		public enum MushroomQuantity {
			Few = 2,
			JustRight = 1,
			Many = 0
		};

		public int Width = 25;
		public int Height = 25;
		private GameFieldSizes size = GameFieldSizes.Medium;
		private MushroomQuantity mushrooms = MushroomQuantity.JustRight;
		public static Image BackgroundImage;

		// This array and enum controls the object collision
		protected static GameObjects[,] arrGameField;
		protected enum GameObjects {
			Mushroom = 0,
			Empty = 1,
			//Spider = 2
			Branch = 3,
			Netterpillar = 4
		};


		private System.IntPtr ScreenWinHandle;

		// Game objects
		private Branch[] branches;

		private Mushroom objMushrooms;
		private int MushroomNumber = 75;

		public Netterpillar[] netterPillars = new Netterpillar[4];
		public int NetterpillarNumber = 1;
		public Netterpillar Player1;

		//Controls the game end
		public bool GameOver;
		public bool Paused;

		// This properties are defined as property procedures, and 
		//  they use the enumerations above as property types
		public GameFieldSizes Size {
			get { return size; }
			set {
				size = value;
				switch(value) {
					case GameFieldSizes.Small:
						Width = 15;
						Height = 15;
						break;
					case GameFieldSizes.Medium:
						Width = 25;
						Height = 25;
						break;
					case GameFieldSizes.Big:
						Width = 40;
						Height = 30;
						break;
				}
			}
		}

		public MushroomQuantity Mushrooms {
			get {
				return mushrooms;
			}
			set {
				mushrooms = value;
				switch(value) {
					case MushroomQuantity.Few:
						MushroomNumber = 25;
						break;
					case MushroomQuantity.JustRight:
						MushroomNumber = 75;
						break;
					case MushroomQuantity.Many:
						MushroomNumber = 125;
						break;
				}

				if (Size==GameFieldSizes.Medium) {
					MushroomNumber *= 2;
				}
				else if(Size==GameFieldSizes.Big) {
					MushroomNumber *= 3;
				}
			}
		}

		public void MoveNetterpillars() {
			int incX = 0; int incY = 0;

			for(int i=0; i<NetterpillarNumber; i++) {
				if (!netterPillars[i].IsDead) {
					// Moves all the Netterpillars
					switch(netterPillars[i].Direction) {
						case Sprite.CompassDirections.East:
							incX = 1;
							incY = 0;
							break;
						case Sprite.CompassDirections.West:
							incX = -1;
							incY = 0;
							break;
						case Sprite.CompassDirections.North:
							incX = 0;
							incY = -1;
							break;
						case Sprite.CompassDirections.South:
							incX = 0;
							incY = 1;
							break;
					}

					switch(arrGameField[netterPillars[i].Location.X+incX, netterPillars[i].Location.Y+incY]) {
						case GameObjects.Empty:
							// Update the Game Field - Empty the field after the Netterpillar
							arrGameField[netterPillars[i].NetterBody[netterPillars[i].NetterBodyLength-1].Location.X, netterPillars[i].NetterBody[netterPillars[i].NetterBodyLength-1].Location.Y] = GameObjects.Empty;
							// Move the Netterpillar
							netterPillars[i].Move(netterPillars[i].Location.X+incX, netterPillars[i].Location.Y+incY, ScreenWinHandle);
							// Update the Game Field - Sets the Netterpillar Head
							arrGameField[netterPillars[i].Location.X, netterPillars[i].Location.Y] = GameObjects.Netterpillar;
							break;
						case GameObjects.Mushroom:
							// Decrement the number of Mushrooms
							MushroomNumber--;
							netterPillars[i].EatAndMove(netterPillars[i].Location.X+incX, netterPillars[i].Location.Y+incY, ScreenWinHandle);
							// Update the Game Field - Sets the Netterpillar Head
							arrGameField[netterPillars[i].Location.X, netterPillars[i].Location.Y] = GameObjects.Netterpillar;
							break;
						default:
							KillNetterPillar(netterPillars[i]);
							break;
					}
				}
			}

		}

		public void KillNetterPillar(Netterpillar netterpillar) {
			netterpillar.IsDead = true;
			// Clears the game field
			arrGameField[netterpillar.Location.X, netterpillar.Location.Y] = GameObjects.Empty;
			netterpillar.Erase(ScreenWinHandle);

			for(int i=0; i<netterpillar.NetterBodyLength; i++) {
				arrGameField[netterpillar.NetterBody[i].Location.X, netterpillar.NetterBody[i].Location.Y] = GameObjects.Empty;
				netterpillar.NetterBody[i].Erase(ScreenWinHandle);
			}
		}



		public void Render() {
			// Move the Netterpillars
			MoveNetterpillars();

			// If all Netterpillars die - GameOver
			GameOver = true;
			for(int i=0; i<NetterpillarNumber; i++) {
				if (!netterPillars[i].IsDead) {
					GameOver = false;
				}
			}

			// If all mushrooms got eaten - Game Over
			if (MushroomNumber==0) {
				GameOver = true;
			}
			System.GC.Collect();
		}

		private static Random rand = new Random();
		public void CreateGameField(System.IntPtr winHandle) {
			branches = new Branch[5];
        
			arrGameField = new GameObjects[Width+1, Height+1];

			// Reset the game variables
			GameOver = false;
			Paused = false;
			// Reset the mushroomNumber, forcing a call to the property procedure
			Mushrooms = Mushrooms;

			ScreenWinHandle = winHandle;

			// Initialize the game array (for collision detection)
			for(int x=0; x<Width; x++) {
				for(int y=0; y<Height; y++) {
					arrGameField[x, y] = GameObjects.Empty;
				}
			}

			// Create the Netterpillars  
			for(int i=0; i<NetterpillarNumber; i++) {
				netterPillars[i] = null;
			}
			switch(NetterpillarNumber) {
				case 1:
					netterPillars[0] = new Netterpillar((int)(this.Width/2), (int)(this.Height)/2, Sprite.CompassDirections.South, false);
					break;
				case 2:
					netterPillars[0] = new Netterpillar((int)(this.Width/3), (int)(this.Height)/2, Sprite.CompassDirections.South, false);
					netterPillars[1] = new Netterpillar((int)(this.Width/3)*2, (int)(this.Height)/2, Sprite.CompassDirections.North, true);
					break;
				case 3:
					netterPillars[0] = new Netterpillar((int)(this.Width/4), (int)(this.Height)/2, Sprite.CompassDirections.South, false);
					netterPillars[1] = new Netterpillar((int)(this.Width/4)*2, (int)(this.Height)/2, Sprite.CompassDirections.North, true);
					netterPillars[2] = new Netterpillar((int)(this.Width/4)*3, (int)(this.Height)/2, Sprite.CompassDirections.South, true);
					break;
				case 4:
					netterPillars[0] = new Netterpillar((int)(this.Width/3), (int)(this.Height)/3, Sprite.CompassDirections.South, false);
					netterPillars[1] = new Netterpillar((int)(this.Width/3), (int)(this.Height)/3*2, Sprite.CompassDirections.East, true);
					netterPillars[2] = new Netterpillar((int)(this.Width/3)*2, (int)(this.Height)/3*2, Sprite.CompassDirections.North, true);
					netterPillars[3] = new Netterpillar((int)(this.Width/3)*2, (int)(this.Height)/3, Sprite.CompassDirections.West, true);
					break;
			}
			Player1 = netterPillars[0];

			// Populates the array with the netterpillars
			for(int i=0; i<NetterpillarNumber; i++) {
				arrGameField[netterPillars[i].Location.X, netterPillars[i].Location.Y] = GameObjects.Netterpillar;
				for(int j=0; j<netterPillars[i].NetterBodyLength; j++) {
					arrGameField[netterPillars[i].NetterBody[j].Location.X, netterPillars[i].NetterBody[j].Location.Y] = GameObjects.Netterpillar;
				}
			}

			// Create the branches
			//    We'll create four branches here just to generate a cleaner code,
			//    we could use the same approach used on the mushrooms below
			branches[0] = new Branch(Sprite.CompassDirections.North, this.Height);
			branches[1] = new Branch(Sprite.CompassDirections.North, this.Height);
			branches[2] = new Branch(Sprite.CompassDirections.East, this.Width-2);
			branches[3] = new Branch(Sprite.CompassDirections.East, this.Width-2);
			for(int x=0; x<Width; x++) {
				arrGameField[x, 0] = GameObjects.Branch;
				arrGameField[x, Height-1] = GameObjects.Branch;
			}
			for(int y=0; y<=Height; y++) {
				arrGameField[0, y] = GameObjects.Branch;
				arrGameField[Width-1, y] = GameObjects.Branch;
			}

			// Create the mushrooms
			//   Since all the mushroom control is on the array, we don't need to create one object per mushroom 
			//   in the screen, we'll rather draw one single mushroom in many positions (on the redraw sub)
			objMushrooms = new Mushroom();
			int randx, randy;
			for(int i=0; i<MushroomNumber; i++) {
				// Check to seek if we are not creating the mushrooms over other objects
				do {
					randx = rand.Next(0, this.Width-2)+1; 
					randy = rand.Next(0, this.Height-2)+1; 
				} while(arrGameField[randx, randy]!=GameObjects.Empty);
				arrGameField[randx, randy] = GameObjects.Mushroom;
			}

			Redraw();
		}

		// This sub will be called everytime the form is activated - because if it was behind other window,
		//  the game field must be redrawn
		public void Redraw() {
			// When the form is activated for the first time, it call the redraw procedure (before arrGameField creation)
			if ((arrGameField==null)||GameOver) {return;}

			for(int x=0; x<Width; x++) {
				for(int y=0; y<Height; y++) {
					if (arrGameField[x, y]==GameObjects.Mushroom) {
						objMushrooms.Location = new Point(x, y);
						objMushrooms.Draw(ScreenWinHandle);
					}
				}
			}

			branches[0].Draw(ScreenWinHandle, 0, 0);
			branches[1].Draw(ScreenWinHandle, (this.Width-1), 0);
			branches[2].Draw(ScreenWinHandle, 1, 0);
			branches[3].Draw(ScreenWinHandle, 1, (this.Height-1));

			for(int i=0; i<NetterpillarNumber; i++) {
				if (!netterPillars[i].IsDead) {
					netterPillars[i].Draw(ScreenWinHandle);
				}
			}

		}
	}
}