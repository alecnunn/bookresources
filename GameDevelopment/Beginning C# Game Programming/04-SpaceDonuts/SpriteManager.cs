using System;
using System.Collections;
using System.Drawing;
using Microsoft.DirectX;
using D3D = Microsoft.DirectX.Direct3D;

namespace SpaceDonuts {
	/// <summary>
	/// Handles management (life/death/updating) of sprites, also handles simple collision detection
	/// </summary>
	public class SpriteManager {
		private ArrayList sprites;

		public delegate void HandleCollision(BasicSprite sprite1, BasicSprite sprite2);
		public event HandleCollision OnCollisionEventHandler;
	
		Rectangle world = new Rectangle(0,0,640,480);
		private bool bounceSprites = true;

		public SpriteManager() {
			sprites = new ArrayList();
		}

		public void Clear() {
			sprites = new ArrayList();
		}

		public void AddSprite(BasicSprite targetSprite) {
			sprites.Add(targetSprite);
		}

		public void Update(float deltaTime) {
			for (int i = 0; i < sprites.Count; i++) {
				BasicSprite sprite = (BasicSprite)sprites[i];
				sprite.Update(deltaTime);
				if (sprite.DurationOver) 
					sprites.RemoveAt(i);
				else
					if (bounceSprites) sprite.BoundaryCheck(world);
			}
		}

		public void Draw(D3D.Device device) {
			using (D3D.Sprite d3dSprite = new D3D.Sprite(device)) {
				d3dSprite.Begin(D3D.SpriteFlags.AlphaBlend);
				foreach (BasicSprite sprite in sprites) {
					sprite.Draw(d3dSprite);
				}
				d3dSprite.End();
			}
		}

		public void CollisionTest() {
			//iterate through first half of sprites for complete collision coverage
			for (int i = 0; i < sprites.Count ; i++) {
				BasicSprite sprite1 = (BasicSprite)sprites[i];
				if (sprite1.CanCollide && sprite1.Visible) {
					int sprite1Height = sprite1.CollisionyExtent;
					int sprite1Width = sprite1.CollisionxExtent;
					for (int j = 0; j < sprites.Count; j++) {
						BasicSprite sprite2 = (BasicSprite)sprites[j];
						if (sprite2.Visible && !sprite2.CanCollide) { //don't check two collidable sprites
							int sprite2Height = sprite2.CollisionyExtent;
							int sprite2Width = sprite2.CollisionxExtent;
							//Simple AABB Collision Check
							float deltaX = Math.Abs(sprite1.PositionX-sprite2.PositionX);
							float deltaY = Math.Abs(sprite1.PositionY-sprite2.PositionY);
							if ((deltaX <= (sprite2Width + sprite1Width) && 
								(deltaY <= (sprite2Height + sprite1Height) ) )) {
								OnCollisionEventHandler(sprite1, sprite2); //invoke delegate
							}
						}
					}
				}
			}
		}
	}
}
