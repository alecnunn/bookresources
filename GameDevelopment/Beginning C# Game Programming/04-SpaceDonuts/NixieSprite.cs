using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using D3D = Microsoft.DirectX.Direct3D;
using System.Drawing;

namespace SpaceDonuts {
	/// <summary>
	/// Summary description for CubeSprite.
	/// </summary>
	public class NixieSprite : BasicSprite {

		Rectangle nixiePosition;

		public enum NixieCharacters { 
			Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine, L, E, V, Mute
		}

		public NixieSprite(TileSet ts) : base(ts) {
			nixiePosition = new Rectangle(ts.XOrigin,ts.YOrigin,ts.ExtentX*2,ts.ExtentY*2);
			nixiePosition.Y = ts.YOrigin; //this value never changes
		}

		public void Draw(Sprite d3dSprite, NixieCharacters nixie, Vector3 displayPosition) {
			nixiePosition.X = tiles.XOrigin + ( (int)nixie % tiles.NumberFrameColumns ) * tiles.ExtentX*2;
			d3dSprite.Draw(tiles.Texture, nixiePosition,  
				new Vector3(), displayPosition, Color.FromArgb(255,255,255,255));
		}
	}
}