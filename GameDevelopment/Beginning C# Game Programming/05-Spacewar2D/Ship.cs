using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;


namespace SpaceWar {

	/// <summary>
	/// This class handles the drawing and collision detection routines for our ship.
	/// </summary>
	public class Ship {
		#region Fields and Properties
		private GameClass game;
		private int[] flameFrame = new int[] {3, 4, 5, 4};

		private Vector2 position;
		private Vector2 velocity;
		private Shots shots = new Shots();
		private ShipState state = ShipState.Dead;		// current state
		public int State { get { return (int) state; } set { state = (ShipState) value; }}
		
		private string hostName;
		private RotatableShape outline;

		public int Outline { get { return outline.CurrentStep; } set { outline.CurrentStep = value; } }

		private Vector2 originalPosition;				// where we started...
		private Rectangle screenBounds;
		private int waitCount = 10;							// wait count before state transition.
		public int WaitCount { get { return waitCount; } set { waitCount = value; } }
		private int flameIndex = -1;					// no flame
		public int FlameIndex { get { return flameIndex; } set { flameIndex = value; } }
		private int score;								// our score
		
		private int deathCount;							// # of times we've died...
		private float hyperSuccess = Constants.HyperSuccessFactor;		// chance of a hyper being successful...

		private Word hostNameWord;
		private Sounds sounds;



		public Shots ShotHandler {
			get {
				return shots;
			}
		}
		public int DeathCount {
			get {
				return deathCount;
			}
			set {
				deathCount = value;
			}
		}

		public int Score {
			get {
				return score;
			}
			set {
				score = value;
			}
		}

		public string HostName {
			get {
				return hostName;
			}
			set {
				hostName = value;
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

		public Vector2 Velocity {
			get {
				return velocity;
			}
			set {
				velocity = value;
			}
		}

		public Rectangle ScreenBounds {
			get {
				return screenBounds;
			}
			set {
				screenBounds = value;
				shots.ScreenBounds = screenBounds;
			}
		}

		public Sounds Sounds {
			get {
				return sounds;
			}
			set {
				sounds = value;
			}
		}

		#endregion

		#region Constructors

		static Ship() {
			// outline of the ship, with 3 flames...
			MyPointF pointNose = new MyPointF(0f, 4f);
			MyPointF pointRight = new MyPointF(2f, -2f);
			MyPointF pointLeft = new MyPointF(-2f, -2f);
			MyPointF flameBase = new MyPointF(0f, -2f);
			MyPointF flameEnd1 = new MyPointF(0f, -4f);
			MyPointF flameEnd2 = new MyPointF(0.5f, -4f);
			MyPointF flameEnd3 = new MyPointF(-0.5f, -4f);

			RotatableShape.AddLine(pointNose, pointRight, Constants.ShipSize);								
			RotatableShape.AddLine(pointNose, pointLeft, Constants.ShipSize);								
			RotatableShape.AddLine(pointLeft, pointRight, Constants.ShipSize);								
			RotatableShape.AddLine(flameBase, flameEnd1, Constants.ShipSize);								
			RotatableShape.AddLine(flameBase, flameEnd2, Constants.ShipSize);								
			RotatableShape.AddLine(flameBase, flameEnd3, Constants.ShipSize);								
		}

		public Ship(GameClass game) {
			this.game = game;
			outline = new RotatableShape();
		}
		#endregion //Constructors

		#region Ship Functions

		public void Shoot() {
			if (this.state != ShipState.Normal) {
				return;
			}

			float angle = outline.Angle + 90f;

			// find out the location of the front of the ship...
			Line line = outline.GetCurrentLineByIndex(0);
			Vector2 noseOffset = new Vector2(line.End1.X, line.End1.Y);

			bool shot = shots.Shoot(position + noseOffset, velocity, angle);
			if (shot) {
				sounds |= Sounds.ShipFire;
			}
		}

		public void SetThrust(bool thrust) {
			SetFlame(thrust);

			if (thrust && (state == ShipState.Normal)) {

				float angle = outline.Angle + 90f;
				
				double angleInRadians = angle * (Math.PI / 180.0f);

				MyPointF acceleration = new MyPointF(Constants.EngineThrust, 0f);
				acceleration = acceleration.Rotate(angle);

				
				velocity += new Vector2(acceleration.X,acceleration.Y);
				
				sounds |= Sounds.ShipThrust;
				
			}
			else {
				if ((sounds & Sounds.ShipThrust) != 0)
					sounds ^= Sounds.ShipThrust;
			}
		}

		// set the flame on or off. 
		// if the is on and it was on last frame, we go to the next line
		// in the flicker sequence...
		void SetFlame(bool flameOn) {
			if (!flameOn) {
				flameIndex = -1;
			}
			else {
				flameIndex = ((flameIndex + 1) % 4);
			}
		}

		public void EnterHyper() {
			if (state != ShipState.Normal)
				return;

			SetState(ShipState.HyperCharge);
		}

		public void SetRandomPosition(bool setOriginal, Point sunLocation) {
			while (true) {
				position = new Vector2(screenBounds.Left + Constants.random.Next(screenBounds.Width), 
					screenBounds.Top + Constants.random.Next(screenBounds.Height));
				Vector2 dist = position - new Vector2(sunLocation.X, sunLocation.Y);
				if (Vector2.Length(dist) > Constants.ShipSunCreationDistance)
					break;
			}
			if (setOriginal) {
				originalPosition = position;
			}
		}

		void SetState(ShipState newState) {
			switch (newState) {
				case ShipState.Dying:
					deathCount++;
					waitCount = Constants.DyingCycle;
					sounds |= Sounds.ShipExplode;
					break;

				case ShipState.Dead:
					waitCount = Constants.DeadCycleWait;
					position = new Vector2(-1000f, -1000f);	// not on board
					break;

				case ShipState.Normal:
					if (state == ShipState.Hyper) {
						SetRandomPosition(false, game.SunLocation);
					}
					else {
						position = originalPosition;
						velocity = new Vector2(0.0f, 0.0f);
					}
					hyperSuccess = Constants.HyperSuccessFactor;
					shots.Clear();
					sounds |= Sounds.ShipAppear;
					break;

				case ShipState.HyperCharge:
					sounds |= Sounds.ShipHyper;
					waitCount = Constants.HyperChargeWait;
					break;

				case ShipState.Hyper:
					position = new Vector2( -1000f, -1000f);		// not on board...
					waitCount = Constants.HyperCycleWait;
					break;
			}
			state = newState;
		}

		public void UpdatePosition() {
			shots.UpdatePosition(game.BounceBack, game.SunLocation,
				game.InverseGravity, game.Gravity);

			if ((state != ShipState.Normal) &&
				(state != ShipState.HyperCharge))
				return;

			position += velocity;

			// Check the game boundaries.  If bounce back mode is enabled,
			// reverse the velocity when we reach the edge
			if (game.BounceBack) {
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


			// update velocity due to the gravity of the sun...
			Point sunLocation = game.SunLocation;
			Vector2 gravity =new Vector2( sunLocation.X - position.X,sunLocation.Y - position.Y);
			float length = Vector2.Length(gravity);
			gravity = Vector2.Normalize(gravity);
			gravity = (gravity * game.Gravity) * (1.0f / (length * length));

			if (game.InverseGravity)
				velocity -= gravity;
			else
				velocity += gravity;
			
			if (length < Constants.SunCollisionLimit) {
				SetState(ShipState.Dying);
				game.SendPointToAllPlayers();
			}
		}

		public void UpdateState() {
			if (state == ShipState.Dying) {
				this.waitCount--;
				if (waitCount == 0) {
					SetState(ShipState.Dead);
				}
			}

			if (state == ShipState.Dead) {
				this.waitCount--;
				if (waitCount == 0) {
					SetState(ShipState.Normal);
				}
			}

			if (state == ShipState.HyperCharge) {
				this.waitCount--;
				this.outline.ChangeCurrentStep(Constants.RotateSteps / 2);
				//this.outline.ChangeCurrentStep(5 - Constants.random.Next(10));
				if (waitCount == 0) {
					// if we didn't make it, we're dying...
					double randValue = Constants.random.NextDouble();
					if (hyperSuccess < randValue) {
						SetState(ShipState.Dying);
						game.SendPointToAllPlayers();
					}
					else {
						SetState(ShipState.Hyper);
					}
				}
			}
			if (state == ShipState.Hyper) {
				this.waitCount--;
				if (waitCount == 0) {
					hyperSuccess -= Constants.HyperSuccessDegradation;
					float save = this.hyperSuccess;
					SetState(ShipState.Normal);
					hyperSuccess = save;
				}
			}

		}

		// see if the other ship has hit us, or if the other shots have hit us
		public void TestShip(RemotePlayer player) {
			Ship otherShip = player.Ship;

			// if we're not alive, don't do any tests...
			if (state != ShipState.Normal)
				return;

			// Test if their shots are close enough to us. 
			if (otherShip.shots.TestShots(this)) {
				SetState(ShipState.Dying);
				game.SendTwoPointsToPlayer(player.PlayerID);	// give them 2 points
			}

			Vector2 delta = this.position - otherShip.position;
			if (Vector2.Length(delta) < Constants.ShipCollisionLimit) {
				SetState(ShipState.Dying);
				game.SendPointToAllPlayers(); // all others get a point
			}
		}

		public void Draw(Surface surface, int shipColor, int shotColor) {
			/*
			if ((state == ShipState.Dead) ||
				(state == ShipState.Hyper))
				return;
*/
			if (state == ShipState.Dying) {
				float factor = (float) this.waitCount / Constants.DyingCycle;

				int c1 = (int) ((shipColor & 0xFF0000) * factor) & 0xFF0000;
				int c2 = (int) ((shipColor & 0x00FF00) * factor) & 0x00FF00;
				int c3 = (int) ((shipColor & 0x0000FF) * factor) & 0x0000FF;

				shipColor = c1 + c2 + c3;
			}

			surface.ForeColor = Color.FromArgb(shipColor);
			surface.DrawWidth = 1;
			outline.Center = new Point((int) position.X, (int) position.Y);

			outline.Draw(surface, 0, 2);	// draw ship...
			if (flameIndex != -1) {
				outline.Draw(surface, flameFrame[flameIndex], flameFrame[flameIndex]);
			}

			surface.ForeColor = Color.FromArgb(shotColor);
			shots.Draw(surface);
		}

		public void DrawHostName(Surface surface, Point location, int color) {
			if (hostName == null)
				return;

			if (hostNameWord == null) {
				hostNameWord = new Word(hostName, Constants.LetterSize);
			}

			hostNameWord.Draw(surface, color, Constants.LetterSpacing, location);
		}

		public void RotateLeft() {
			outline.DecrementCurrentStep();
		}

		public void RotateRight() {
			outline.IncrementCurrentStep();
		}
		#endregion //Ship Functions
	}
}
