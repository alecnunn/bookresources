using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D=Microsoft.DirectX.Direct3D;

	/// <summary>
	/// Summary description for Shockwave.
	/// </summary>
public class Shockwave {
	private PositionedMesh shockWaveMesh;
	private Device device;

	public Vector3 Location { 
		get {
			return shockWaveMesh.Position.Location;
		}
		set {
			shockWaveMesh.Position.Location = value;
		}
	}

	public Shockwave(Device device) {

		this.device = device;
		shockWaveMesh = new PositionedMesh(device, MediaUtilities.FindFile("shockwave.x"));
		shockWaveMesh.Position.Location = new Vector3(0, 0, 0);
		shockWaveMesh.Position.Rotate(0,0,10);
	}

	public void Reset(Vector3 location) {
		shockWaveMesh.Position.Location = location;
		shockWaveMesh.Position.Scale(1,1,1);
	}

	public void Update(float elapsedTime) {
		float scaleFactor = shockWaveMesh.Position.XScale;
		scaleFactor *= 1.2f + elapsedTime;
		shockWaveMesh.Position.Scale(scaleFactor, 1, scaleFactor);
	}

	public void Render() {

		device.RenderState.Lighting = false;
		device.RenderState.CullMode = Cull.None;
		device.RenderState.AlphaBlendOperation = BlendOperation.Add;
		device.RenderState.SourceBlend = Blend.One;
		device.RenderState.DestinationBlend = Blend.One;
		device.RenderState.AlphaBlendEnable = true;
		device.Transform.World = shockWaveMesh.Position.WorldMatrix;
		shockWaveMesh.Render(true, true);
		device.RenderState.AlphaBlendEnable = false;
		device.RenderState.SourceBlend = Blend.SourceAlpha;
		device.RenderState.DestinationBlend = Blend.InvSourceAlpha;
		device.RenderState.CullMode = Cull.CounterClockwise;
		device.RenderState.Lighting = true;
	}

}