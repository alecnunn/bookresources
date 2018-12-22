using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace SpaceWar {
	/// <summary>
	/// Drawing and updating routines for shots
	/// </summary>
	[Serializable]
	public class Shot {
		Vector2 position;
		[NonSerialized]
		Vector2 velocity;
		int age = -1;



		public Shot() {

		}


		public bool Alive {
			get {
				return (age != -1);
			}
			set {
				if (value == true)
					age = 0;
				else
					age = -1;
			}
		}

		public Vector2 Position {
			get {
				return position;
			}
			set {
				position = value;
			}
		}

		public int Age {
			get {
				return age;
			}
			set {
				age = value;
			}
		}

		public void SetShot(Vector2 position, Vector2 velocity) {
			age = 0;
			this.position = position;
			this.velocity = velocity;
		}


		public void UpdatePosition(Rectangle screenBounds, bool bounceBack, 
			Point sunLocation, bool inverseGravity, int gameGravity) {
			if (age == -1)
				return;

			position += velocity;

			if (bounceBack) {
				if (position.X > screenBounds.Right || position.X < screenBounds.Left)
					velocity.X = -velocity.X;
				if (position.Y > screenBounds.Bottom || position.Y < screenBounds.Top)
					velocity.Y = -velocity.Y;
			}
			else {
				if (position.X > screenBounds.Right)
					position.X = screenBounds.Left;

				if (position.X < screenBounds.Left)
					position.X = screenBounds.Right;

				if (position.Y > screenBounds.Bottom)
					position.Y = screenBounds.Top;

				if (position.Y < screenBounds.Top)
					position.Y = screenBounds.Bottom;
			}

			if (Constants.ShotGravity) {
				// update velocity due to the gravity of the sun...
				Vector2 gravity = new Vector2(position.X - sunLocation.X,position.Y - sunLocation.Y);  
				float length = Vector2.Length(gravity);
				gravity = (Vector2.Normalize(gravity) * gameGravity) * (1.0f / (length * length));

				if (inverseGravity)
					velocity += gravity;
				else
					velocity -= gravity;

				if (length < Constants.ShotSunCollisionLimit) {
					age = Constants.ShotLifetime;
				}
			}

			age++;

			if (age >= Constants.ShotLifetime) {
				age = -1;
			}
		}

		public void Draw(Surface surface) {
			if (age == -1)
				return;

			int x = (int) position.X;
			int y = (int) position.Y;
			surface.DrawLine(x, y, x + 1, y);
			surface.DrawLine(x, y + 1, x + 1, y + 1);
		}
	}
}
