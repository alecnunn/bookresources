using System;
using System.Drawing;
using Microsoft.DirectX.Direct3D;
namespace SpaceDonuts {
	public class BulletSprite : BasicSprite {

		public BulletSprite(TileSet ts) : base(ts) {
			this.VisuallyRotates = true;
		}

		public new float Angle {
			get {
				return Geometry.RadianToDegree(zAngle);
			}
			set {
				zAngle = Geometry.DegreeToRadian(value);
				visualAngle = zAngle;
			}
		}
	
		public override void BoundaryCheck(Rectangle boundingBox) {
			// Angle of reflection = angle of incidence, measured from the 
			// surface normal.  So for perpendicular surfaces, all we have to do 
			// is negate the current angle. 
			int height = boundingBox.Height - this.Tiles.ExtentY*2;
			int width = boundingBox.Width - this.Tiles.ExtentX*2;
			if (this.PositionX > (width) || this.PositionX < 0) {
				this.Angle = -this.Angle;
				this.VelocityX *= -1; 
			}
			if (this.PositionY > (height) || this.PositionY < 0) { 
				this.Angle = -this.Angle;
				this.VelocityY *= -1;
			}
		}
	}
}