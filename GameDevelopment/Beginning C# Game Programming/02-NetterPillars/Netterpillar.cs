using System;
using System.Drawing;
using System.Windows.Forms;
namespace Netterpillars {
	public class Netterpillar : Sprite {
		private Bitmap NetterHeadN;
		private Bitmap NetterHeadS;
		private Bitmap NetterHeadE;
		private Bitmap NetterHeadW;
		public NetterBody[] NetterBody;
		public int NetterBodyLength = 4;

		public bool IsComputer = true; // Defaults to computer-controled netterpillar
		public bool IsDead = false; // Defaults to alive netterpillar

		// We can only set the direction once, until the 
		//  netterpillar moves, or we can run over our own tail
		private bool directionSet = false;

		private CompassDirections direction;
		public new CompassDirections Direction {
			get {
				return direction;
			}
			set {
				// Only set the direction once, until we receive the direction from
				//  the remote player
				if (!directionSet) {
					direction = value;
					directionSet = true;
				}
			}
		}


		public Netterpillar(int x, int y, Sprite.CompassDirections initialDirection, bool isComputer) {
			NetterBody = new NetterBody[25+1];
			int incX=0, incY=0;

			IsComputer = isComputer;
			NetterHeadN = Load(Application.StartupPath+"\\"+IMAGE_PATH+"\\"+(IsComputer ? "" : "Player")+"NetterHeadN.gif");
			NetterHeadS = Load(Application.StartupPath+"\\"+IMAGE_PATH+"\\"+(IsComputer ? "" : "Player")+"NetterHeadS.gif");
			NetterHeadE = Load(Application.StartupPath+"\\"+IMAGE_PATH+"\\"+(IsComputer ? "" : "Player")+"NetterHeadE.gif");
			NetterHeadW = Load(Application.StartupPath+"\\"+IMAGE_PATH+"\\"+(IsComputer ? "" : "Player")+"NetterHeadW.gif");
			for(int i=0; i<NetterBodyLength; i++) {
				NetterBody[i] = new NetterBody(IsComputer);
			}

			// Position the Netterpillar on the given point
			Direction = initialDirection;
			Location.X = x;
			Location.Y = y;
			// Position each of the body parts
			switch(Direction) {
				case Sprite.CompassDirections.East:
					incX = -1;
					break;
				case Sprite.CompassDirections.South:
					incY = -1;
					break;
				case Sprite.CompassDirections.West:
					incX = 1;
					break;
				case Sprite.CompassDirections.North:
					incY = 1;
					break;
			}
			for(int i=0; i<NetterBodyLength; i++) {
				x += incX;
				y += incY;
				NetterBody[i].Location.X = x;
				NetterBody[i].Location.Y = y;
			}

		}

		public void EatAndMove(int x, int y, System.IntPtr winHandle) {
			//  If the NetterBody array is full, allocate more space
			if (NetterBodyLength == NetterBody.Length) {
				NetterBody [] tempNetterBody = new NetterBody[NetterBody.Length+25+1];
				NetterBody.CopyTo(tempNetterBody, 0);
				NetterBody = tempNetterBody;
			}
			NetterBody[NetterBodyLength] = new NetterBody(IsComputer);
			NetterBody[NetterBodyLength].Location = NetterBody[NetterBodyLength-1].Location;

			// Updates the whole bodys position and then the head position
			for(int i=NetterBodyLength-1; i>=1; i--) {
				NetterBody[i].Location = NetterBody[i-1].Location;
			}

			NetterBody[0].Location = Location;
			NetterBody[0].Draw(winHandle);

			NetterBodyLength++;
			// Updates the Netterpillar head position
			Location = new Point(x, y);

			//Clear the mushroom
			Erase(winHandle);

			// Draw the Netterpillar head
			Draw(winHandle);
			// Reset the direction controller variable
			directionSet = false;
		}


		public void Move(int x, int y, System.IntPtr winHandle) {
			// Erase the last part of the body
			NetterBody[NetterBodyLength-1].Erase(winHandle);

			// Updates the whole bodys position and then the head position
			for(int i=NetterBodyLength-1; i>=1; i--) {
				NetterBody[i].Location = NetterBody[i-1].Location;
			}
			NetterBody[0].Location = Location;
			Location = new Point(x, y);

			// Redraws only the first part of the body and the head
			NetterBody[0].Draw(winHandle);

			//We don't need to erase the netterpillar head, since the body will cover it
			Draw(winHandle);
			// Reset the direction controller variable
			directionSet = false;
		}


		public new void Draw(System.IntPtr winHandle) {
			switch(Direction) {
				case Sprite.CompassDirections.East:
					base.Draw(NetterHeadE, winHandle);
					break;
				case Sprite.CompassDirections.South:
					base.Draw(NetterHeadS, winHandle);
					break;
				case Sprite.CompassDirections.West:
					base.Draw(NetterHeadW, winHandle);
					break;
				case Sprite.CompassDirections.North:
					base.Draw(NetterHeadN, winHandle);
					break;
			}

			for(int i=0; i<NetterBodyLength; i++) {
				NetterBody[i].Draw(winHandle);
			}
		}
	}
}