using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using D3D = Microsoft.DirectX.Direct3D;
using System.Drawing;

namespace SpaceDonuts {

	public class BasicSprite {
		protected TileSet tiles; //reference to the sprite's tile set
		protected float zAngle = 0.0f; //in angles, not radians
		protected float visualAngle = 0f; //used to rotate sprite visually (in radians)
		protected bool visuallyRotates = false; //indicates if the sprite rotates along the visualAngle
		protected int currentFrame=0;  //Current frame of the sprite's animation
		protected float scale = 1.0f; //used to scale the sprite
		protected Rectangle tilePosition; 
		protected Vector3 center = new Vector3( 0f, 0f, 0f );
		protected Vector3 position = new Vector3( 0f, 0f, 0f );
		protected int collisionxExtent;
		protected int collisionyExtent;
		protected bool canCollide = false;
		protected float frameRate = 1.0f/30.0f; //30 times per second
		protected float frameTrigger = 0.0f; //time until next frame is shown
		protected float animationSpeed = 0.0f; //how fast to show the frames
		protected int totalFrames;
		protected Vector2 velocity = new Vector2(0f, 0f);
		protected bool isVisible = true;
		protected float delay = 0f;  //sets sprite startup delay
		protected float duration = 0f; //sets sprite lifespan (in seconds)
		protected bool limitedDuration = false; //indicates that sprite has a limited lifespan
		protected bool durationEnded= false; //indicates that the life of the sprite is over

		public bool DurationOver {
			get {
				return durationEnded;
			}
			set {
				durationEnded = value;
			}
		}

		public bool VisuallyRotates {
			get {
				return visuallyRotates;
			}
			set {
				visuallyRotates = value;
			}
		}

		public bool Visible {
			get {
				return isVisible;
			}
			set {
				isVisible = value;
			}
		}
		public float StartDelay {
			get {
				return delay;
			}
			set {
				delay = value;
				isVisible = false;
			}
		}

		public int Frame {
			get {
				return currentFrame;
			}
			set {
				currentFrame = value;
			}
		}

		public float Scale {
			get {
				return scale;
			}
			set {
				scale = value;
			}
		}

		public TileSet Tiles {
			get {
				return tiles;
			}
			set {
				tiles = value;
			}
		}

		public float Angle {
			get {
				return Geometry.RadianToDegree(zAngle);
			}
			set {
				zAngle = Geometry.DegreeToRadian(value);
			}
		}

		public int CollisionxExtent {
			get {
				return collisionxExtent;
			}
			set {
				collisionxExtent = value;
			}
		}
		public int CollisionyExtent {
			get {
				return collisionyExtent;
			}
			set {
				collisionyExtent = value;
			}
		}

		public bool CanCollide {
			get {
				return canCollide;
			}
			set {
				canCollide = value;
			}
		}

		public float FrameRate {
			get {
				return frameRate;
			}
			set {
				frameRate = value;
			}
		}
		public float AnimationSpeed {
			get {
				return animationSpeed;
			}
			set {
				animationSpeed = value;
			}
		}

		public float Velocity {
			get {
				return velocity.Length();
			}
			set {
				velocity.X = (float)Math.Cos(zAngle) * value;
				velocity.Y = (float)Math.Sin(zAngle) * value;
			}
		}

		public float VelocityX {
			get {
				return velocity.X;
			}
			set {
				velocity.X = value;
			}
		}

		public float VelocityY {
			get {
				return velocity.Y;
			}
			set {
				velocity.Y = value;
			}
		}

		public float PositionX {
			get {
				return position.X;
			}
			set {
				position.X = value;
			}
		}
		public float PositionY {
			get {
				return position.Y;
			}
			set {
				position.Y = value;
			}
		}

		public BasicSprite() {}

		public BasicSprite(TileSet ts) {
			if (ts != null) {
				collisionxExtent = ts.ExtentX;
				collisionyExtent = ts.ExtentY;
				tilePosition = new Rectangle(ts.XOrigin,ts.YOrigin,ts.ExtentX*2,ts.ExtentY*2);
				tiles = ts;
				animationSpeed = 1.0f;
				totalFrames = ts.NumberFrameColumns * ts.NumberFrameRows;
			}
			else
				throw new Exception("Cannot create sprite without valid tile set");
		}

		public void LimitLifespan(float newDuration) {
			duration = newDuration;
			limitedDuration = true;
		}

		public virtual void NextFrame() {
			currentFrame = ++currentFrame % totalFrames;
		}

		public virtual void PreviousFrame() {
			if (currentFrame == 0) 
				currentFrame = totalFrames-1;
			else
				currentFrame = --currentFrame % totalFrames;
		}

		public virtual void Draw(Sprite d3dSprite) {
			if (isVisible) {

				//Set rotation center for sprite
				center.X = position.X + tiles.ExtentX;
				center.Y = position.Y + tiles.ExtentY;

				//Spin, Shift, Stretch :-)
				d3dSprite.Transform = Matrix.RotationZ(visualAngle) * Matrix.Translation(center) * Matrix.Scaling(scale,scale,1.0f);

				d3dSprite.Draw(tiles.Texture, tilePosition, 
					center, position, Color.FromArgb(255,255,255,255));
			}
		}

		public virtual void Update(float deltaTime) {
			//Handle any delay times that are set
			if (delay > 0f) {
				delay -= deltaTime;
				if (delay <= 0f) {
					delay = 0f;
					isVisible = true;
				}
			}
			else {
				if (limitedDuration) {
					duration -= deltaTime;
					if (duration <= 0f) { 
						durationEnded = true;
						return;
					}
				}

				frameTrigger += deltaTime * animationSpeed;
				//Do we move to the next frame?
				if (frameTrigger >= frameRate) {
					NextFrame();
					frameTrigger = 0f;
				}

				tilePosition.X = tiles.XOrigin + ( (int)currentFrame % tiles.NumberFrameColumns ) * tiles.ExtentX*2;
				tilePosition.Y = tiles.YOrigin + ( (int)currentFrame / tiles.NumberFrameColumns) * tiles.ExtentY*2;

				//Now apply motion
				position.X += velocity.X * deltaTime;
				position.Y += velocity.Y * deltaTime;
			}
		}

		public virtual void BoundaryCheck(Rectangle boundingBox) {
			// Angle of reflection = angle of incidence, measured from the 
			// surface normal.  So for perpendicular surfaces, all we have to do 
			// is negate the current angle. 
			int height = boundingBox.Height - this.Tiles.ExtentY*2;
			int width = boundingBox.Width - this.Tiles.ExtentX*2;
			if (this.PositionX > (width) || this.PositionX < 0) {
				this.VelocityX *= -1; 		
			}
			if (this.PositionY > (height) || this.PositionY < 0) { 
				this.VelocityY *= -1;
			}
		}

	}
}
