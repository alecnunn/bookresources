using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using D3D = Microsoft.DirectX.Direct3D;
using System.Drawing;
namespace SpaceDonuts {
	public class ShipSprite : BasicSprite {

		private float angleIncrement = 360f/40f; //40 frames in ship 
		private const float thrustAmount = 4f;

		public ShipSprite(TileSet ts) : base(ts) {
			this.AnimationSpeed = 0f; //ship only moves from user input
			this.Frame = 10; //aligns ship direction to 0 radians
		}

		public override void NextFrame() {
			currentFrame = ++currentFrame % totalFrames;
			this.Angle = this.Frame * angleIncrement - 90;
		}

		public override void PreviousFrame() {
			if (currentFrame == 0) 
				currentFrame = totalFrames-1;
			else
				currentFrame = --currentFrame % totalFrames;
			this.Angle = this.Frame * angleIncrement - 90;
		}

		public void Thrust() {	
			Vector2 thrustVector = new Vector2(
				(float)Math.Cos(zAngle),
				(float)Math.Sin(zAngle));
			velocity += thrustVector * thrustAmount;

		}
	}
}