using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;


[Serializable]
public class WorldPosition {
	public float XPos {get { return xPos; } set { xPos = value; } }
	public float YPos {get { return yPos; } set { yPos = value; } }
	public float ZPos {get { return zPos; } set { zPos = value; } }

	public float XRotation { get { return xRotation; } set { xRotation = value; } }
	public float YRotation { get { return yRotation; } set { yRotation = value; } }
	public float ZRotation { get { return zRotation; } set { zRotation = value; } }

	public float XScale { get { return xScale; } set { xScale = value; } }
	public float YScale { get { return yScale; } set { yScale = value; } }
	public float ZScale { get { return zScale; } set { zScale = value; } }


	public Matrix WorldMatrix { get { Update(); return worldMatrix; } 
		set { 
			worldMatrix = value;
			Location = Vector3.TransformCoordinate(new Vector3(0,0,0), worldMatrix);
			Point(Location, Location+Direction);
			Roll(Location, Location+Up);
		} 
	}

	public Vector3 Direction {
		get {
			Vector3 directionVector= new Vector3(0,0,1);
			return Vector3.TransformNormal(directionVector, worldMatrix);
		}
	}

	public Vector3 Up {
		get {
			Vector3 upVector = new Vector3(0,1,0);
			return Vector3.TransformNormal(upVector,worldMatrix);
		}
	}
	public Vector3 Right {
		get {
			Vector3 rightVector = new Vector3(1,0,0);
			return Vector3.TransformNormal(rightVector,worldMatrix);
		}
	}

	public Vector3 Location {
		get {
			return new Vector3( xPos, yPos, zPos );
		}
		set {
			Move(value);
		}
	}

	public Vector3 Rotation {
		get {
			return new Vector3( xRotation, yRotation, zRotation);
		}
		set {
			Rotate(value);
		}
	}
				


	private float xPos, yPos, zPos = 0.0f;
	private float xRotation, yRotation, zRotation = 0.0f;
	private float xScale, yScale, zScale = 0.0f;

	private Matrix worldMatrix, scaleMatrix, rotationMatrix = Matrix.Identity;
	private Matrix translationMatrix = Matrix.Identity;
	private Matrix combineMatrix1, combineMatrix2;

	public Matrix CombineMatrix1 { get { return combineMatrix1; } set { combineMatrix1 = value; } }
	public Matrix CombineMatrix2 { get { return combineMatrix2; } set { combineMatrix2 = value; } }


	public WorldPosition() {
		
		Move(0.0f, 0.0f, 0.0f);
		Rotate(0.0f, 0.0f, 0.0f);
		Scale(1.0f, 1.0f, 1.0f);
		combineMatrix1 = combineMatrix2 = Matrix.Zero;

		Update();

	}
		
	public Vector3 CombinedLocation {
		get {
			Update();
			return new Vector3(worldMatrix.M41,worldMatrix.M42,worldMatrix.M43);
		}
	}

	public void Move( Vector3 newPosition ) {
		Move( newPosition.X, newPosition.Y, newPosition.Z );
	}

	public void Move( float x, float y, float z ) {
		xPos = x;
		yPos = y;
		zPos = z;

		translationMatrix = Matrix.Translation( xPos, yPos, zPos );
	}

	public void MoveRel( float x, float y, float z ) {
		Move(xPos + x, yPos + y, zPos + z );
	}

	public void Rotate( Vector3 newDirection ) {
		Rotate(newDirection.X, newDirection.Y, newDirection.Z);
	}

	public void Point(Vector3 pointAtLocation) {
		Point(xPos, yPos, zPos, pointAtLocation.X, pointAtLocation.Y, pointAtLocation.Z);
	}

	public void Point(Vector3 currentLocation, Vector3 pointAtLocation) {
		Point(currentLocation.X, currentLocation.Y, currentLocation.Z, pointAtLocation.X, pointAtLocation.Y, pointAtLocation.Z);
	}

	public void Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt) {

		float XRot, YRot, XDiff, YDiff, ZDiff;

		//Calculate angle between points
		XDiff = XAt - XEye;
		YDiff = YAt - YEye;
		ZDiff = ZAt - ZEye;
		XRot = (float) Math.Atan2(-YDiff, Math.Sqrt(XDiff*XDiff+ZDiff*ZDiff));
		YRot = (float) Math.Atan2(XDiff, ZDiff);

		Move(XEye, YEye, ZEye);
		Rotate(XRot, YRot, 0.0f);

	}

	public void Roll(Vector3 Location, Vector3 UpVector) {
		float xDiff, yDiff;
		xDiff = Location.X - UpVector.X;
		yDiff = Location.Y - UpVector.Y;
		zRotation = (float) Math.Atan2(xDiff, yDiff);
	}




	public void Rotate( float x, float y, float z ) {
		xRotation = x;
		yRotation = y;
		zRotation = z;

		rotationMatrix = Matrix.RotationYawPitchRoll( yRotation, xRotation, zRotation );
	}

	public void RotateRel( float x, float y, float z ) {
		Rotate( xRotation + x, yRotation + y, zRotation + z );
	}

	//YawPitchRoll rotates an object relative to its own orthonormal basis vectors
	public void YawPitchRoll( float yaw, float pitch, float roll) {

		Matrix yawMatrix = Matrix.RotationAxis(this.Up, yaw);
		Matrix pitchMatrix = Matrix.RotationAxis(this.Right, pitch);
		Matrix rollMatrix = Matrix.RotationAxis(this.Direction, roll);
			
		Matrix ypr = yawMatrix * pitchMatrix * rollMatrix;
		rotationMatrix *= ypr;


	}

	public void Scale( float x, float y, float z ) {
		xScale = x;
		yScale = y;
		zScale = z;

		scaleMatrix = Matrix.Scaling( xScale, yScale, zScale );

	}

	public void ScaleRel( float x, float y, float z ) {
		Scale( xScale + x, yScale + y, zScale + z );
	}


	public void Update() {

		Matrix m;

			
				
		m = scaleMatrix;
		m *= rotationMatrix;
		m *= translationMatrix;
		if (combineMatrix1 != Matrix.Zero)
			m *= combineMatrix1;
		if (combineMatrix2 != Matrix.Zero)
			m *= combineMatrix2;

		worldMatrix = m;
	}

}
