//************************************************************************* //
// Camera class which is able to shake while follows a rough moving target
//
// Authors: Aszódi, Barnabás
//          Czuczor, Szabolcs
// Created: July, 2004., Budapest, Hungary
//
// Budapest University of Technology and Economics,
// Department of Control Engineering and Information Technology (BME-IIT)
//************************************************************************* //
#include <string.h>
#include "vector.h"

// Overriding the default abs() function to avoid the casting error from float to integer
#define abs(a) (((a)>=0)?(a):(-(a)))

// This "define" block is needed to be able to avoid the undesirable effect of variable frame rate
#ifndef FRAMERATECONTROL
#define FRAMERATECONTROL
struct __timeb64 timeStruct;

// This solution dies in 50 days
#define getCurrentTimeInto(LongIntTimeStamp) LongIntTimeStamp=timeGetTime()
// The same function, but it not dies in 50 days 
//#define getCurrentTimeInto(LongIntTimeStamp) { _ftime64(&timeStruct); LongIntTimeStamp = 1000 * timeStruct.time + timeStruct.millitm; }
#endif

#define MAXEXTRAPOLLENGTH 1000                    // Frame length that the inertia of the Camera is derived from
#define FPSTIMELISTLENGTH 2000                    // The length of the time measure list for the real-time frame rate calculation
#define FPSMAEASURETIMERANGE 1000                 // The maximum time (in ms) which between the FPS changing can be perceptible
//#define FRAMECOUNTTOAVERAGE 10                  // The number of frames that we use to calculate the last frame time

// For DEBUG
#define FIGLENGTH 2000                            // Needed just for the drawing of the graph


//===============================================================
class Camera {
//===============================================================
private:
	Vector3f eye;                                 // The eye position of the Camera object
	Vector3f target;                              // The point that the Camera look at (the view point)
	Vector3f up;                                  // The [up] vector of the Camera object (to know, where is "up" in the virtual world)
	float actualFov;                              // Field Of View in degree
	float fovToReach;                             // The camera zooms to reach this value
	
	// Originally saved parameters for RESET (These would be the parameters got through the constructor)
	Vector3f origEye;                             // Original eye point position
	Vector3f origTarget;                          // Original view point position
	Vector3f origUp;                              // Original up vector
	float origFov;                                // Original value of field of view

	bool autoZoom;                                // Should we change the FOV automatically?
    float targetSize;                             // How big is the watched object at the Camera.target that is need to be visible on the screen?

	// Class members for calculate time and the frame rate
	long int timeStamp, lastTimeStamp;            // [timeStamp] = actual time, [lastTimeStamp] = older time to calculate the FPS 
	float actualFPS;                              // The actual FPS value
	long int timeStampList[FPSTIMELISTLENGTH];    // Time measure list for the real-time frame rate calculation
	int tslToIdx, tslFromIdx;                     // Indices for the time measure list

	// For DEBUG /////////
	unsigned char screenMessage;                  // There can be 8 different messages to show on the screen (controlled by the 8 bit of this variable)
	char* screenMessageText[8];                   // The pointers for these messages which can be maximum 1023 char long
	Vector3f screenMessagePosition[8];            // A list for the screen messages
	Vector3f fig[FIGLENGTH];                      // This stores the points of the graphs
	int figX;                                     // Current x value of the drawing index along the X axis
	// End of "For DEBUG" /////

	// Parameters for shake-calculation
	Vector3f objPosList[MAXEXTRAPOLLENGTH];       // List for the object positions
	Vector3f objPosDifList[MAXEXTRAPOLLENGTH];    // List for the differences of the positions
	Vector3f objPosDifDifList[MAXEXTRAPOLLENGTH]; // List for the "differences of differences"
	Vector3f lpfObjPosList[MAXEXTRAPOLLENGTH];    // List for the object positions after low pass filter
	float objFrameWeightList[MAXEXTRAPOLLENGTH];  // List for the weighting of the position values
	float sumWeight[MAXEXTRAPOLLENGTH];           // Stores the sum of the weights

	// Print the help messages
	void printScreenMessages(int w, int h) {
		for (int n = 0; n < 7; n++) if ((screenMessage >> n) & 0x01) {
			glColor3f(0.7, 0.8, 0.9);
			glRasterPos2f(2 * screenMessagePosition[n].X() / (float)w - 1.0, 2 * screenMessagePosition[n].Y() / (float)h - 1.0);
			int lineBreak = 1;
			for( int i = 0; i < 1024; i++ ) {
				if ( screenMessageText[n][i] == '\0' ) break;
				else if ( screenMessageText[n][i] == '\n' )	{
					glRasterPos2f(2 * screenMessagePosition[n].X() / (float)w - 1.0, 2 * (screenMessagePosition[n].Y() - (float)lineBreak * 16.0) / (float)h - 1.0);
					lineBreak++;
				} else if ( screenMessageText[n][i] == '\t' ) {
					for (int space = 0; space < 4; space++)
						glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ' ');
				} else glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, screenMessageText[n][i]);
			}
		}
		// DEBUG: Figure drawing
		if (!((screenMessage >> 7) & 0x01)) return;
		figX = figX % w;
		fig[figX].set(screenMessagePosition[n].X() / 100.0f, screenMessagePosition[n].Y() / 100.0f, screenMessagePosition[n].Z() / 100.0f);
		glBegin(GL_POINTS);
			for (int i = 0; i < FIGLENGTH; i++) {
				glColor3f(0.0, 0.0, 1.0);
				glVertex2f(2.0 * (float)i / (float)w - 1.0, fig[i].Z());
				glColor3f(0.0, 1.0, 0.0);
				glVertex2f(2.0 * (float)i / (float)w - 1.0, fig[i].Y());
				glColor3f(1.0, 0.0, 0.0);
				glVertex2f(2.0 * (float)i / (float)w - 1.0, fig[i].X());
			}
		glEnd();
	}

	// Animate the camera fov parameter
	void animate() {
		float fovDiff = fovToReach - actualFov;
		float oneBitFovAddition = 0.0;
		if (fovDiff) {
			float deltaTime = getLastFrameTime();
			oneBitFovAddition = ((fovDiff > 0)?(1.0):(-1.0)) * deltaTime * 0.06;
			if (abs(fovDiff) < 15.0) oneBitFovAddition *= (abs(fovDiff) / 15.0);
			if (abs(oneBitFovAddition) < abs(fovDiff)) actualFov += oneBitFovAddition;
			if (actualFov > 179.0) actualFov = 179.0;
			if (actualFov < 0.001) actualFov = 0.001;
		}
	}


public:
	// The constructor of the class
	Camera(float eyeX = 0.0f, float eyeY = 0.0f, float eyeZ = 0.0f, float targetX = 0.0f, float targetY = 0.0f, float targetZ = -1.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f, float fov0 = 28.0f) {
		// First store the parameters
		eye.set(eyeX, eyeY, eyeZ);   
		target.set(targetX, targetY, targetZ);
		up.set(upX, upY, upZ);
		fovToReach = actualFov = fov0;
		// Saving parameters as 'originals' for RESET
		origEye = eye;
		origTarget = target;
		origUp = up;
		origFov = actualFov;

		autoZoom = false;
		targetSize = 2.0f;
		sumWeight[0] = 0;
		for (int i = 0; i < MAXEXTRAPOLLENGTH; i++) {
			objPosList[i].set(target.X(), target.Y(), target.Z());
			lpfObjPosList[i] = objPosList[i];
			objPosDifList[i].set(0.0f, 0.0f, 0.0f);
			objPosDifDifList[i].set(0.0f, 0.0f, 0.0f);
			objFrameWeightList[i] = 1.0f + ((float)i / 50.0);
			if (i == 0) sumWeight[0] = objFrameWeightList[0];
			else sumWeight[i] = sumWeight[i - 1] + objFrameWeightList[i];
		}
		// FPS measuring
		getCurrentTimeInto(timeStamp);
		lastTimeStamp = timeStamp;
		for (int i = 0; i < FPSTIMELISTLENGTH; i++) timeStampList[i] = lastTimeStamp;
		tslFromIdx = 0;
		tslToIdx = 1;
		// Screen messages
		//screenMessage = 255;                      // All of the screen messages will be shown
		screenMessage = 1 + 32 + 64 + 128;          // Set the basic screen messages (1,5,6,7)
		for (int i = 0; i < 8; i++) {
			screenMessageText[i] = new char[1024];  // The available message length is 1023 characters
			sprintf(screenMessageText[i], "[%d] nothing yet...", i);
			screenMessagePosition[i].set(10.0, 20.0 * (8 - i), 0.0);
		}
		figX = 0;
	}

    // Saving actual parameters as originals
	void saveParametersToOriginals() {
		origEye = eye;
		origTarget = target;
		origUp = up;
		origFov = actualFov;
	}
    
	// Restoring parameters from originals
	void reset() {
		eye = origEye;
		target = origTarget;
		up = origUp;
		fovToReach = origFov;
	}

	float getActualFPS() { return actualFPS; }

	long int getCurrentTime() { return timeStamp; }

	Vector3f& Eye() { return eye; }
	Vector3f& Target() { return target; }
	float getFov() { return actualFov; }
	void setFov(float newFov) { actualFov = fovToReach = newFov; }
	float& FovToReach() { return fovToReach; }
	bool getAutoZoom() { return autoZoom; }
	void setAutoZoom(bool newAZ, float objectSize) { autoZoom = newAZ; targetSize = objectSize; }
	float& TargetSize() { return targetSize; }
	unsigned char& ScreenMessage() { return screenMessage; }

	// This function receives the actual position of the object,
	// stores it in the list and calculates the actual position of the camera's target  
	void placeObjectHereToFollow(Vector3f objPos, bool directFollow = false) {
		if (actualFPS == 0.0) return;
		// A cameraman is not so fast. He needs about half second to notice that the followed object moves away.
		float lengthForExtrpl = min(actualFPS, (float)MAXEXTRAPOLLENGTH) * 0.7;
		int iLengthForExtrpl = (int)lengthForExtrpl;  // To save many float->int casting time
		// Besides, a cameraman has inertia too, so he works like a Low Pass Filter
		float lengthForLPF = min(actualFPS, (float)MAXEXTRAPOLLENGTH) * 0.4;
		int iLengthForLPF = (int)lengthForLPF;  // To save many float->int casting time

		// Calculating of a new "low pass filtered object position"
		Vector3f lpfObjectLastPos = objPos;
		for (int i = 0; i < (iLengthForLPF - 1); i++) lpfObjectLastPos += objPosList[i];
		if (iLengthForLPF != 0) lpfObjectLastPos *= (1 / (float)iLengthForLPF);
		else lpfObjectLastPos = objPos;

		// Shifting and inserting the object positions and differences
		for (int i = MAXEXTRAPOLLENGTH - 1; i > 0; i--) {  // Shifting...
			objPosList[i] = objPosList[i - 1];
			lpfObjPosList[i] = lpfObjPosList[i - 1];
			objPosDifList[i] = objPosDifList[i - 1];
			objPosDifDifList[i] = objPosDifDifList[i - 1];
		}
		objPosList[0] = objPos;  // ...and inserting new element
		lpfObjPosList[0] = lpfObjectLastPos;
		objPosDifList[0] = lpfObjPosList[0] - lpfObjPosList[1];
		objPosDifDifList[0] = objPosDifList[0] - objPosDifList[1];

		// Calculating the camera position
		Vector3f averageTargetStep;
		Vector3f averagePosDifDif;
		// We need just [iLengthForExtrpl - 2] element because they are the differences
		for (int i = 0; i < iLengthForExtrpl; i++) {
			averageTargetStep += (objPosDifList[i] * objFrameWeightList[i]);
			averagePosDifDif += (objPosDifDifList[i] * objFrameWeightList[iLengthForExtrpl - i - 1]);
		}
		averageTargetStep *= (1 / sumWeight[iLengthForExtrpl - 1]);
		averagePosDifDif *= (1 / sumWeight[iLengthForExtrpl - 1]);
		// In case of "direct follow" we leave the above calculated data
		if (directFollow) {
			target = objPos;
			return;
		}
		target = lpfObjPosList[iLengthForExtrpl] + averageTargetStep * (lengthForExtrpl + lengthForLPF / 2.0) + averagePosDifDif;
	}
    // Fill this function to create your own scene
	void buildScene();

	void setScreenMessage(unsigned char number, char* message, int posX, int posY) {
		if ((number < 0) || (number > 7)) return;
		strcpy(screenMessageText[number], message);
		screenMessagePosition[number].set((float)posX, (float)posY, 0.0);
	}

	Vector3f& ScreenMessagePositionVector(unsigned char number) {
		if ((number < 0) || (number > 7)) return Vector3f();
		return screenMessagePosition[number];
	};

	// This function defines the matrices, the parameters of the camera and loads the scene 
	void Render(int width, int height) {
		glPushMatrix();
			glLoadIdentity();				
			glMatrixMode(GL_PROJECTION);							
			glLoadIdentity();
            printScreenMessages(width, height);
			if (autoZoom) fovToReach = 360.0 * (float)atan((targetSize / 2.0) / (target - eye).Length()) / M_PI;
			if (fovToReach > 179.0) fovToReach = 179.0;
			if (fovToReach < 0.001) fovToReach = 0.001;
			gluPerspective(actualFov, (GLfloat)width / (GLfloat)height, 0.1f, 10000.0f);
			gluLookAt(eye.X(), eye.Y(), eye.Z(), target.X(), target.Y(), target.Z(), up.X(), up.Y(), up.Z());
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);								
		glPushMatrix();
			glLoadIdentity();
			buildScene();                         // Sample scene inserted here. This function needs to be defined  
		glPopMatrix();

	}

	// Real-time calculation of the frame rate in FPS
	void recalculateTime() {
		getCurrentTimeInto(timeStamp);
		timeStampList[tslToIdx] = timeStamp;
		if ((timeStampList[tslToIdx] - timeStampList[tslFromIdx]) > 0) {
			if (tslFromIdx < tslToIdx) actualFPS = ((1000.0 * (float)(tslToIdx - tslFromIdx)) / (float)(timeStampList[tslToIdx] - timeStampList[tslFromIdx])) + 1.0;
			else actualFPS = ((1000.0 * (float)(tslToIdx + FPSTIMELISTLENGTH - tslFromIdx)) / (float)(timeStampList[tslToIdx] - timeStampList[tslFromIdx])) + 1.0;
		}
		while ((timeStampList[tslToIdx] - timeStampList[tslFromIdx] > FPSMAEASURETIMERANGE) && (tslFromIdx != tslToIdx - 1)) {
			tslFromIdx++;
			if (tslFromIdx == FPSTIMELISTLENGTH) {
				if (tslToIdx == 0) tslToIdx = 1;
				tslFromIdx = 0;
			}
		}
		if (tslToIdx == FPSTIMELISTLENGTH - 1) {
			if (tslFromIdx == 0) tslFromIdx++;
			tslToIdx = 0;
		} else {
			if (tslToIdx == tslFromIdx - 1) tslFromIdx++;
			tslToIdx++;
		}
		if (tslFromIdx == FPSTIMELISTLENGTH) tslFromIdx = 0;

		// Animate the camera parameters
		animate();

		figX++;
	}

	float getLastFrameTime() {
		if (actualFPS == 0.0) return 0.0;
		else return (1000.0 / actualFPS);
	}
};