using System;

namespace SpaceWar {
	/// <summary>
	/// GamePlay constants.  You can tune the game by adjusting these values.
	/// </summary>
	sealed public class Constants {
		static public Random random = new Random((int) DateTime.Now.Ticks);

		public const int RotateSteps = 60;				// number of steps to rotate ship.
		public const float ShipSize = 3f;
		public const float EngineThrust = 0.3f;
		public const int DyingCycle = 20;
		public const int DeadCycleWait = 30;
		public const int HyperChargeWait = 10;
		public const int HyperCycleWait = 2;
		public const float HyperSuccessFactor = 0.8f;		// initial success factor
		public const float HyperSuccessDegradation = 0.1f;	// degradation per use...
		public const float ShipCollisionLimit = 20f;

		public const int ShotLifetime = 35;
		public const int NumShots = 6;
		public const float ShotVelocity = 20f;
		public const int ShotDeltaCycles = 3;
		public const int ShotInitialMove = 3;		// times to update shot so it shows up outside the ship...
		public const int ShotCollisionLimit = 12;

		public const int SunSize = 20;
		public const float SunCollisionLimit = 25f;

		public const int LetterSize = 5;
		public const int LetterSpacing = 8;
		public const int ScoreVerticalSpacing = 20;
		public const int ScoreXOrigin = 30;
		public const int ScoreYOrigin = 30;
		public const int ScoreOffset = 100;		// offset of score from name...

		public const bool ShotGravity = true;
		public const bool ShotAddShipVelocity = false;
		public const int ShotSunCollisionLimit = 25;

		public const int ShipSunCreationDistance = 200;

		public const int NumStars = 60;
		public const int StarColorFull = 0x0000FF;
		public const int StarColorDim  = 0x000044;

		public const long RemoteTickTimeout = 5;	// five seconds...

		private Constants() {
		}
	}
}
