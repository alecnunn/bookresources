//************************************************************************* //
// 3D Movable Dummy Object class for OpenGL scenes
//
// Authors: Aszódi, Barnabás
//          Czuczor, Szabolcs
// Created: July, 2004., Budapest, Hungary
//
// Budapest University of Technology and Economics,
// Department of Control Engineering and Information Technology (BME-IIT)
//************************************************************************* //
#include "vector.h"

#ifndef OBJECT_H
#define OBJECT_H

#define abs(a) (((a)>=0)?(a):(-(a)))


// This class is the base object class.
// This defines all of the common parameters for the objects.
//===============================================================
class MovableObject {
//===============================================================
protected:
	Vector3f position;                            // The position of the Object
	Vector3f speed;                               // The actual speed
	Vector3f acceleration;                        // The actual acceleration
	Vector3f orientation;                         // The actual orientation (Euler angles in degree, around X, Y and Z axes)
	Vector3f rotationSpeed;                       // The actual speed of rotation;
	float allAroundSize;                          // This is the diameter of the bounding sphere of the moveable object

	// You need to fill this function with another content to set your inherited object visible
	virtual void draw() {
		glColor3f(0.1f, 0.1f, 0.1f);
		glBegin(GL_QUADS);
			glVertex3f(0.5f, -0.5f, 0.5f);
			glVertex3f(0.5f, -0.5f, -0.5f);
			glVertex3f(-0.5f, -0.5f, -0.5f);
			glVertex3f(-0.5f, -0.5f, 0.5f);
		glEnd();
		glColor3f(0.5f, 0.5f, 0.5f);
		glBegin(GL_TRIANGLES);
			glVertex3f(0.5f, 0.5f, 0.5f);
			glVertex3f(0.0f, 0.4f, -0.3f);
			glVertex3f(-0.5f, 0.5f, 0.5f);
		glEnd();
		glColor3f(0.2f, 0.2f, 0.2f);
		glBegin(GL_QUADS);
			glVertex3f(0.5f, -0.5f, 0.5f);
			glVertex3f(0.5f, 0.5f, 0.5f);
			glVertex3f(-0.5f, 0.5f, 0.5f);
			glVertex3f(-0.5f, -0.5f, 0.5f);
		glEnd();
		glColor3f(0.3f, 0.3f, 0.3f);
		glBegin(GL_TRIANGLES);
			glVertex3f(0.5f, -0.5f, -0.5f);
			glVertex3f(0.0f, 0.4f, -0.3f);
			glVertex3f(-0.5f, -0.5f, -0.5f);
		glEnd();
	}

public:
	MovableObject(float posX = 0.0f, float posY = 0.0f, float posZ = 0.0f) {
		position.set(posX, posY, posZ);
		allAroundSize = 2.0;
	}

	// This function places the object into the scene
	void drawInScene() {
		glPushMatrix();
			glTranslatef(position.X(), position.Y(), position.Z());
			glRotatef(orientation.X(), 1.0f, 0.0f, 0.0f);
			glRotatef(orientation.Y(), 0.0f, 1.0f, 0.0f);
			glRotatef(orientation.Z(), 0.0f, 0.0f, 1.0f);
			draw();
		glPopMatrix();
	}

	Vector3f& Position() { return position; }
	Vector3f& Speed() { return speed; }
	Vector3f& Acceleration() { return acceleration; }
	Vector3f& Orientation() { return orientation; }
	Vector3f& RotationSpeed() { return rotationSpeed; }

	// This function stops the object at once
	void stopMotion() {
		acceleration.set(0.0, 0.0, 0.0);
		speed.set(0.0, 0.0, 0.0);
		rotationSpeed.set(0.0, 0.0, 0.0);
	}
    // This function stops accelerating
	void evenPace() {
		acceleration.set(0.0, 0.0, 0.0);
	}

	// This function sets a new position for the object
	void setPosition(float x = 0.0f, float y = 0.0f, float z = 0.0f) {
		position.set(x, y, z);
	}

	// This function sets a new speed for the object
	void setSpeed(float x = 0.0f, float y = 0.0f, float z = 0.0f) {
		speed.set(x, y, z);
	}

	// This function updates the properties of the object depending on time
	// [timeDelta] : the time elapsed since the last animating sequence (in milliseconds)
	void animate(float timeDelta) {
		position += (speed * timeDelta / 1000.0f);
		speed += (acceleration * timeDelta / 1000.0f);
		orientation += (rotationSpeed * timeDelta / 1000.0f);
	}

	float& AllAroundSize() { return allAroundSize; }

};

#endif