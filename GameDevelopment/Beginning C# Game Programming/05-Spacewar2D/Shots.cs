using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace SpaceWar {
	/// <summary>
	///  Handles the collision detection and returns the array of shots for sending to other players 
	/// </summary>
	[Serializable]
	public class Shots {

		Shot[] shots;
		[NonSerialized]	
		int cyclesSinceShot = 0;
		[NonSerialized]	
		Rectangle screenBounds;	
		

		public Shots() {
			shots = new Shot[Constants.NumShots];
			for (int count = 0; count < Constants.NumShots; count++) {
				shots[count] = new Shot();
			}
		}

		public Rectangle ScreenBounds {
			get {
				return screenBounds;
			}
			set {
				screenBounds = value;
			}
		}
		public Shot[] GetShotArray() {
			return shots;
		}

		public void SetShotArray(Shot[] shotArray) {
			this.shots = shotArray;
		}

		public bool Shoot(Vector2 position, Vector2 velocity, float angle) {
			if (cyclesSinceShot < Constants.ShotDeltaCycles)
				return false;

			cyclesSinceShot = 0;

			MyPointF shotPoint = new MyPointF(Constants.ShotVelocity, 0f);

			MyPointF rotatedPoint = shotPoint.Rotate(angle);
			Vector2 shotVelocity = new Vector2(rotatedPoint.X,rotatedPoint.Y);
			if (Constants.ShotAddShipVelocity) {
				shotVelocity += velocity;
			}

			foreach (Shot shot in shots) {
				if (!shot.Alive) {
					shot.SetShot(position, shotVelocity);
					return true;
				}
			}
			return false;
		}

		public void UpdatePosition(bool bounceBack, Point sunLocation,
			bool inverseGravity, int gameGravity) {
			cyclesSinceShot++;

			foreach (Shot shot in shots) {
				shot.UpdatePosition(screenBounds, bounceBack,
					sunLocation, inverseGravity, gameGravity);
			}
		}

		public void Clear() {
			foreach (Shot shot in shots) {
				shot.Alive = false;
			}
		}
		public bool TestShots(Ship ship) {
			foreach (Shot shot in shots) {
				if (shot.Alive) {
					float distance = Vector2.Length(shot.Position - ship.Position);
					if (distance < Constants.ShotCollisionLimit) {
						shot.Alive = false;
						return true;
					}
				}
			}
			return false;
		}

		public void Draw(Surface surface) {
			foreach (Shot shot in shots) {
				shot.Draw(surface);
			}
		}
	}
}
