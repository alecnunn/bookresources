using System;
using Microsoft.DirectX;


	/// <summary>
	/// Summary description for Camera.
	/// </summary>
public class Camera {

	protected float velocity;


	protected float xPos, yPos, zPos;
	protected float xRot, yRot, zRot;

	protected float startXPos, startYPos, startZPos;
	protected float startXRot, startYRot, startZRot;

	protected float endXPos, endYPos, endZPos;
	protected float endXRot, endYRot, endZRot;

	protected Matrix viewMatrix;
	protected Matrix translationMatrix;
	protected Matrix rotationMatrix;

	public float XPosition { get { return xPos; } }
	public float YPosition { get { return yPos; } }
	public float ZPosition { get { return zPos; } }

	public float XRotation { get { return xRot; } }
	public float YRotation { get { return yRot; } }
	public float ZRotation { get { return zRot; } }	


	public Vector3 Loc {
		get {
			return new Vector3(xPos,yPos,zPos);
		}
		set {
			Vector3 loc = value;
			xPos = loc.X;
			yPos = loc.Y;
			zPos = loc.Z;
		}

	}

	public Matrix ViewMatrix {
		get {
			Update();
			return viewMatrix;
		}
		set {
			viewMatrix = value;
		}

	}


	public Camera() {
		Move(0.0f,0.0f,0.0f);
		Rotate(0.0f,0.0f,0.0f);
		Update();
	}


	public void Update() {           // Update transformation matrix
		viewMatrix = translationMatrix * rotationMatrix;
	}

	public void Move(float XPos, float YPos, float ZPos) {
		xPos = XPos;
		yPos = YPos;
		zPos = ZPos;
		translationMatrix = Matrix.Translation(  -xPos, -yPos,  -zPos );
	}

	public void MoveRel(float XAdd, float YAdd, float ZAdd) {
		Move (xPos + XAdd, yPos + YAdd, zPos + ZAdd);

	}

	public void Rotate(float XRot, float YRot, float ZRot) {
		Matrix xRotation, yRotation, zRotation;
			
		xRot = XRot;
		yRot = YRot;
		zRot = ZRot;

		xRotation = Matrix.RotationX( - xRot );
		yRotation = Matrix.RotationY( - yRot );
		zRotation = Matrix.RotationZ( - zRot );

		rotationMatrix = zRotation;
		rotationMatrix *= yRotation;
		rotationMatrix *= xRotation;

	}

	public void RotateRel(float XAdd, float YAdd, float ZAdd) {
		Rotate (xRot + XAdd, yRot + YAdd, zRot + ZAdd);
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
}
