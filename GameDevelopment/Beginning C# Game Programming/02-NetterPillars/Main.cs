using System;
using System.Drawing;
using System.Windows.Forms;
namespace Netterpillars {
	class MainGame {
		public static GameEngine netterpillarGameEngine;

		private static AINetterpillar objAINetterpillar = new AINetterpillar();

		public static void Main(string [] args) {
			Splash WinSplash;
			GameField WinGameField;
			GameOver WinGameOver = new GameOver();
			int LastTick=0; int DesiredFrameRate = 10;

			// Create the game engine object
			netterpillarGameEngine = new GameEngine();
			WinSplash = new Splash();

			while ( WinSplash.ShowDialog()==DialogResult.OK) {
				WinGameField = new GameField();
				WinGameField.Show();
				Application.DoEvents();
				//Creates a copy of the background image to allow erasing the sprites 
				GameEngine.BackgroundImage = (Image)WinGameField.PicGameField.Image.Clone();
				netterpillarGameEngine.CreateGameField(WinGameField.PicGameField.Handle);
				while ( !netterpillarGameEngine.GameOver) {
					if (!netterpillarGameEngine.Paused) {
						// EXTRA: Force a Frame rate of 10 frames to second on maximum
						if (System.Environment.TickCount-LastTick>=1000/DesiredFrameRate) {
							MoveComputerCharacters();
							netterpillarGameEngine.Render();
							LastTick = System.Environment.TickCount;
						}
					}
					Application.DoEvents();
				}
				WinGameOver.ShowDialog();
				WinGameField.Dispose();
			}
			netterpillarGameEngine = null;
			WinSplash.Dispose();
			WinGameOver.Dispose();
		}

		public static void MoveComputerCharacters() {
			//Move the Netterpillars
			for(int i=0; i<netterpillarGameEngine.NetterpillarNumber; i++) {
				if (!netterpillarGameEngine.netterPillars[i].IsDead) {
					// A.I. for the computer-controled Netterpillars
					if (netterpillarGameEngine.netterPillars[i].IsComputer) {
						netterpillarGameEngine.netterPillars[i].Direction = objAINetterpillar.ChooseNetterpillarDirection(netterpillarGameEngine.netterPillars[i].Location, netterpillarGameEngine.netterPillars[i].Direction);
					}
				}
			}
		}
	}
}