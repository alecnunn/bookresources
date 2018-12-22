/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
//===========================================================================
//===========================================================================
#include "stars.h"

#define MAX_RAND 		4096

//---------------------------------------------------------------------------
// Initialize stars
//---------------------------------------------------------------------------
void Stars::Init() {
	int step = cnt / 3;
	int i;

	glListID = glGenLists(1);

	glNewList(glListID, GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
	glPointSize(0.5);
	glBegin(GL_POINTS);
		for (i = 0; i < step; i++) {
			glColor3f(1.0, 1.0, 1.0);
			glVertex3f(
				radius * ((float)(rand() % MAX_RAND)/(MAX_RAND)) - radius / 2.0,
				radius * ((float)(rand() % MAX_RAND)/(MAX_RAND)) - radius / 2.0,
				radius * ((float)(rand() % MAX_RAND)/(MAX_RAND)) - radius / 2.0
			);
		}
	glEnd();
	glPointSize(1.0);
	glBegin(GL_POINTS);
		for (i = 0; i < step; i++) {
			glVertex3f(
				radius * ((float)(rand() % MAX_RAND)/(MAX_RAND)) - radius / 2.0,
				radius * ((float)(rand() % MAX_RAND)/(MAX_RAND)) - radius / 2.0,
				radius * ((float)(rand() % MAX_RAND)/(MAX_RAND)) - radius / 2.0
			);
		}
	glEnd();
	glPointSize(1.5);
	glBegin(GL_POINTS);
		for (i = 0; i < step; i++) {
			glVertex3f(
				radius * ((float)(rand() % MAX_RAND)/(MAX_RAND)) - radius / 2.0,
				radius * ((float)(rand() % MAX_RAND)/(MAX_RAND)) - radius / 2.0,
				radius * ((float)(rand() % MAX_RAND)/(MAX_RAND)) - radius / 2.0
			);
		}
	glEnd();
	glEndList();
}
