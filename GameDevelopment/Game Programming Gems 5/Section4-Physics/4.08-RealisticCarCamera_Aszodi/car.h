//************************************************************************* //
// 3D Car class inherited from 3D Movable Object class for OpenGL scenes
//
// Authors: Aszódi, Barnabás
//          Czuczor, Szabolcs
// Created: July, 2004., Budapest, Hungary
//
// Budapest University of Technology and Economics,
// Department of Control Engineering and Information Technology (BME-IIT)
//************************************************************************* //
#include "object.h"

#define MAXSTEERINGANGLE 50                       // The max value of steering angle in degree

// This class defines the object that we want to follow.
// This class is inherited from the MovableObject class to have the drawInScene() function.
// Currently, this car has a REALLY limited controllability.
//===============================================================
class Car : public MovableObject {
//===============================================================
private:
	float steeringAngle;                          // The angle of the steering-wheel
	bool steeringWasModified;                     // A flag, which tells us that the steering wheel was turned
	float frontWheelFromCenter, rearWheelFromCenter;  // The distance between front and rear wheels and the center point of the car
	float minMovementDuringSteering;              // The minimal movement during steering to calculate the orientation change properly

	// This function draws a wheel
	void drawWheel() {
		float y, z;
		glBegin(GL_POLYGON);
		for (float angle = 0; angle <= 360; angle += 20)
			glVertex3f(0.1, 0.35 * cos(M_PI * angle / 180.0), 0.35 * sin(M_PI * angle / 180.0));
		glEnd();
		glBegin(GL_POLYGON);
		for (float angle = 360; angle >= 0; angle -= 20)
			glVertex3f(-0.1, 0.35 * cos(M_PI * angle / 180.0), 0.35 * sin(M_PI * angle / 180.0));
		glEnd();
		glBegin(GL_QUAD_STRIP);
		for (float angle = 0; angle <= 360; angle += 20) {
			y = 0.35 * cos(M_PI * angle / 180.0);
			z = 0.35 * sin(M_PI * angle / 180.0);
			glVertex3f( 0.1, y, z);
			glVertex3f(-0.1, y, z);
		}
		glEnd();
	}
	
	// This function draws the body of the car
	void draw() {
		glColor3f(0.3, 0.3, 0.3);
		// Undercarriage
		glBegin(GL_QUADS);
			glVertex3f(-1.0, 0.2, 2.0);
			glVertex3f(-1.0, 0.2,-2.0);
			glVertex3f( 1.0, 0.2,-2.0);
			glVertex3f( 1.0, 0.2, 2.0);
		glEnd();
		// Spoiler
		glBegin(GL_QUAD_STRIP);
			glVertex3f( 1.1, 0.4, 2.1);
			glVertex3f( 1.0, 0.2, 2.0);
			glVertex3f( 1.1, 0.4,-2.1);
			glVertex3f( 1.0, 0.2,-2.0);
			glVertex3f(-1.1, 0.4,-2.1);
			glVertex3f(-1.0, 0.2,-2.0);
			glVertex3f(-1.1, 0.4, 2.1);
			glVertex3f(-1.0, 0.2, 2.0);
			glVertex3f( 1.1, 0.4, 2.1);
			glVertex3f( 1.0, 0.2, 2.0);
		glEnd();
		// Bottom part of coachwork
		glColor3f(0.8, 0.8, 0.8);
		glBegin(GL_QUAD_STRIP);
			glVertex3f( 1.05, 1.0, 2.1);
			glVertex3f( 1.1, 0.4, 2.1);
			glVertex3f( 1.05, 1.0,-0.9);
			glVertex3f( 1.1, 0.4,-0.9);
			glVertex3f( 1.0, 0.8,-1.9);
			glVertex3f( 1.1, 0.4,-2.1);
			glVertex3f(-1.0, 0.8,-1.9);
			glVertex3f(-1.1, 0.4,-2.1);
			glVertex3f(-1.05, 1.0,-0.9);
			glVertex3f(-1.1, 0.4,-0.9);
			glVertex3f(-1.05, 1.0, 2.1);
			glVertex3f(-1.1, 0.4, 2.1);
			glVertex3f( 1.05, 1.0, 2.1);
			glVertex3f( 1.1, 0.4, 2.1);
		glEnd();
		// Roof and bonnet
		glColor3f(0.9, 0.9, 0.9);
		glBegin(GL_QUADS);
			glVertex3f( 0.8, 1.5, 1.8);
			glVertex3f( 0.8, 1.5,-0.2);
			glVertex3f(-0.8, 1.5,-0.2);
			glVertex3f(-0.8, 1.5, 1.8);
			glVertex3f( 1.05, 1.0,-0.9);
			glVertex3f( 1.0, 0.8,-1.9);
			glVertex3f(-1.0, 0.8,-1.9);
			glVertex3f(-1.05, 1.0,-0.9);
		glEnd();
		// Windows
		glColor3f(0.7, 0.7, 0.9);
		glBegin(GL_QUAD_STRIP);
			glVertex3f( 0.8, 1.5, 1.8);
			glVertex3f( 1.05, 1.0, 2.1);
			glVertex3f( 0.8, 1.5,-0.2);
			glVertex3f( 1.05, 1.0,-0.9);
			glVertex3f(-0.8, 1.5,-0.2);
			glVertex3f(-1.05, 1.0,-0.9);
			glVertex3f(-0.8, 1.5, 1.8);
			glVertex3f(-1.05, 1.0, 2.1);
			glVertex3f( 0.8, 1.5, 1.8);
			glVertex3f( 1.05, 1.0, 2.1);
		glEnd();
		// Wheels
		glColor3f(0.1, 0.1, 0.1);
		glPushMatrix();
			glTranslatef(-1.1, 0.35, -frontWheelFromCenter);
			glRotatef(steeringAngle, 0.0f, 1.0f, 0.0f);
			drawWheel();
		glPopMatrix();
		glPushMatrix();
			glTranslatef( 1.1, 0.35, -frontWheelFromCenter);
			glRotatef(steeringAngle, 0.0f, 1.0f, 0.0f);
			drawWheel();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-1.1, 0.35, -rearWheelFromCenter);
			drawWheel();
		glPopMatrix();
			glTranslatef( 1.1, 0.35, -rearWheelFromCenter);
			drawWheel();
		glPopMatrix();
	}

	// Reposition and orientate the car depending its steeringAngle and movement
	void driveWithSteering(float movement) {
		float movementMultiplier = abs(movement / minMovementDuringSteering);
		float movementSign = ((movement >= 0)?(1.0):(-1.0));
		if (movementMultiplier > 1.0) movement = abs(minMovementDuringSteering);
		else movement = abs(movement);
		Vector3f wheelFront(0.0, 0.0, -frontWheelFromCenter);
		Vector3f wheelRear(0.0, 0.0, -rearWheelFromCenter);
		Vector3f movementRear(0.0, 0.0, -movement);
		Vector3f movementFront = movementRear.Rotate(Vector3f(0.0, 1.0, 0.0), steeringAngle);
		wheelFront += movementFront;
		wheelRear += movementRear;
		Vector3f objectMovement = (wheelFront + wheelRear) * 0.5f;  // This is not precise solution (but fast and simple)

		Vector3f newWheelAxis = wheelFront - wheelRear;
		float orientationChange = movementSign * 180.0 * (float)atan((double)(newWheelAxis.X() / newWheelAxis.Z())) / M_PI;

		for (int i = 0; i < (int)movementMultiplier; i++) {
			position += (objectMovement.Rotate(Vector3f(0.0, 1.0, 0.0), orientation.Y()) * movementSign);
			orientation.Y() += orientationChange;
			objectMovement = objectMovement.Rotate(Vector3f(0.0, 1.0, 0.0), orientationChange);
		}
		objectMovement *= (movementMultiplier - (int)movementMultiplier);
		orientationChange *= (movementMultiplier - (int)movementMultiplier);
		position += (objectMovement.Rotate(Vector3f(0.0, 1.0, 0.0), orientation.Y()) * movementSign);
		orientation.Y() += orientationChange;
	}


public:
	Car(float posX = 0.0f, float posY = 0.0f, float posZ = 0.0f) : MovableObject(posX, posY, posZ) {
		steeringAngle = 0.0f;
		frontWheelFromCenter = 1.5f;
		rearWheelFromCenter = -1.5f;
		minMovementDuringSteering = (frontWheelFromCenter - rearWheelFromCenter) / 50.0f;
		allAroundSize = 5.0;
		steeringWasModified = false;
	}

	float getSteeringAngle() { return steeringAngle; }

	void setSteeringAngle(float newSA) {
		if (newSA > MAXSTEERINGANGLE) steeringAngle = MAXSTEERINGANGLE;
		else if (newSA < -MAXSTEERINGANGLE) steeringAngle = -MAXSTEERINGANGLE;
		else steeringAngle = newSA;
		steeringWasModified = true;
	}

	// This function updates the properties of the car
	// [deltaTime] : the time in milliseconds elapsed since the last animating sequence
	void animate(float deltaTime) {
		driveWithSteering(-speed.Z() * deltaTime / 1000.0f);
		speed += (acceleration * deltaTime / 1000.0f);
		orientation += (rotationSpeed * deltaTime / 1000.0f);
		if (steeringWasModified) steeringWasModified = false;
		else if (abs(steeringAngle) < (deltaTime * 0.12)) steeringAngle = 0.0;
		else {
			if (steeringAngle > 0) steeringAngle -= (deltaTime * 0.12);
			if (steeringAngle < 0) steeringAngle += (deltaTime * 0.12);
		}
	}

	// Tricky shake simulation of the car
	void simulateBumps(long int actualTime, float amplitude, double frequency) {
		position.Y() = amplitude * (float)sin(2 * M_PI * frequency * (double)actualTime / 1000.0);
	}
};