using System;
using System.Drawing;
namespace Netterpillars {
	public class AINetterpillar : GameEngine {
		// Controls how random the netterpillar will move
		private int RandomPercent = 5;

		public Sprite.CompassDirections ChooseNetterpillarDirection(Point currentLocation, Sprite.CompassDirections currentDirection) {
			Sprite.CompassDirections ChooseNetterpillarDirection_result = (Sprite.CompassDirections)0;
			GameObjects BestObject;
			GameObjects NextObject = (GameObjects)0;

			switch(currentDirection) {
				case Sprite.CompassDirections.East:
					NextObject = arrGameField[currentLocation.X+1, currentLocation.Y];
					break;
				case Sprite.CompassDirections.West:
					NextObject = arrGameField[currentLocation.X-1, currentLocation.Y];
					break;
				case Sprite.CompassDirections.South:
					NextObject = arrGameField[currentLocation.X, currentLocation.Y+1];
					break;
				case Sprite.CompassDirections.North:
					NextObject = arrGameField[currentLocation.X, currentLocation.Y-1];
					break;
			}

			//Pick the lowest value - Mushroom or empty
			BestObject = (GameObjects)Math.Min(Math.Min(Math.Min((int)arrGameField[currentLocation.X+1, currentLocation.Y], (int)arrGameField[currentLocation.X-1, currentLocation.Y]), (int)arrGameField[currentLocation.X, currentLocation.Y+1]), (int)arrGameField[currentLocation.X, currentLocation.Y-1]);

			// If the current direction is equal the best direction, stay in current direction
			if (NextObject==BestObject) {
				ChooseNetterpillarDirection_result = currentDirection;
			}
			else {
				// Select the direction of the best object
				if (BestObject == arrGameField[currentLocation.X+1, currentLocation.Y]) {
					ChooseNetterpillarDirection_result = Sprite.CompassDirections.East;
				} 
				else if (BestObject == arrGameField[currentLocation.X-1, currentLocation.Y]) {
					ChooseNetterpillarDirection_result = Sprite.CompassDirections.West;
				}
				else if (BestObject == arrGameField[currentLocation.X, currentLocation.Y+1]) {
					ChooseNetterpillarDirection_result = Sprite.CompassDirections.South;
				}
				else if (BestObject == arrGameField[currentLocation.X, currentLocation.Y-1]) {
					ChooseNetterpillarDirection_result = Sprite.CompassDirections.North;
				}
			}

			ChooseNetterpillarDirection_result = RandomDirection(currentLocation, ChooseNetterpillarDirection_result);
			return ChooseNetterpillarDirection_result;
		}

		private static Random rand = new Random();
		public Sprite.CompassDirections RandomDirection(Point currentLocation, Sprite.CompassDirections chooseCompassDirections) {
			Sprite.CompassDirections RandomDirection_result;
			int x = rand.Next(0, 100); //Rnd(1)*100;

			RandomDirection_result = chooseCompassDirections;
			if (x<RandomPercent) {
				switch(chooseCompassDirections) {
					case Sprite.CompassDirections.East:
						// Try the other directions
						if (arrGameField[currentLocation.X, currentLocation.Y+1]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.South;
						}
						else if(arrGameField[currentLocation.X, currentLocation.Y-1]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.North;
						}
						else if(arrGameField[currentLocation.X-1, currentLocation.Y]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.West;
						}
						break;
					case Sprite.CompassDirections.West:
						// Try the other directions
						if (arrGameField[currentLocation.X, currentLocation.Y+1]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.South;
						}
						else if(arrGameField[currentLocation.X, currentLocation.Y-1]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.North;
						}
						else if(arrGameField[currentLocation.X+1, currentLocation.Y]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.East;
						}
						break;
					case Sprite.CompassDirections.North:
						// Try the other directions
						if (arrGameField[currentLocation.X, currentLocation.Y+1]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.South;
						}
						else if(arrGameField[currentLocation.X+1, currentLocation.Y]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.East;
						}
						else if(arrGameField[currentLocation.X-1, currentLocation.Y]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.West;
						}
						break;
					case Sprite.CompassDirections.South:
						// Try the other directions
						if (arrGameField[currentLocation.X, currentLocation.Y-1]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.North;
						}
						else if(arrGameField[currentLocation.X+1, currentLocation.Y]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.East;
						}
						else if(arrGameField[currentLocation.X-1, currentLocation.Y]<=GameObjects.Empty) {
							RandomDirection_result = Sprite.CompassDirections.West;
						}
						break;
				}
			}
			return RandomDirection_result;
		}
	}
}