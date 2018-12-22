using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D=Microsoft.DirectX.Direct3D;


	/// <summary>
	/// Summary description for BGPointer.
	/// </summary>
public class BGPointer {
	private PositionedMesh arrowMesh;
	private Device device;


	public BGPointer(Device device) {
		this.device = device;
		arrowMesh = new PositionedMesh(device,"arrow-yel.x");	
		arrowMesh.Position.Scale(2.0f, 2.0f, 2.0f);
	}

	public void Render() {	
		arrowMesh.Render();
	}

	public void Point(WorldPosition ourPosition, WorldPosition opponentWorldPosition) {
		arrowMesh.Position.Move(ourPosition.Location.X, ourPosition.Location.Y,
			ourPosition.Location.Z);
		Vector3 pointVector = opponentWorldPosition.Location - ourPosition.Location;

		
		float XRot, YRot;

		XRot = (float) Math.Atan2(-pointVector.Y, Math.Sqrt(pointVector.X * pointVector.X +
			pointVector.Z * pointVector.Z));
		YRot = (float) Math.Atan2(pointVector.X, pointVector.Z);

		arrowMesh.Position.Rotate(XRot, YRot, 0.0f);
	}
}