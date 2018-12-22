/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#include "spline.h"

#define STATE_BEGIN 0
#define STATE_SPLINE 1
#define STATE_POINTS 3
static int state = STATE_BEGIN;

#define COLOR_CONTROL 0xFF0000FF
#define COLOR_SPLINE_NO_GRAVITY 0xFFFFC000
#define COLOR_SPLINE_BREAK 0xFF0000E0
#define COLOR_SPLINE 0xFF00FF00
#define COLOR_TANGENT 0xFF0080FF
#define COLOR_NORMAL 0xFFFF00FF
#define COLOR_SIDE 0xFF00FF00
#define COLOR_ON 0xFFFFFFFF

#define PI 3.14159265358979323846

//---------------------------------------------------------------------------
// Contstruct a Spline using a Definition File
//---------------------------------------------------------------------------
Spline::Spline(char * fileName) {
	// Initialization
	controlCnt = 0;
	controlIndx = 0;
	defualtNrm = vector3(0.0, 1.0, 0.0);
	curveSubD = 0;
	curveCnt = 0;
	curveIndx = 0;
	curveType = 0;
	shortestDistance = 0;
	displayControlMode = 0;
	displayCurveMode = 0;
	displayCurveModulo = 0;
	displayOrientMode = 0;
	displayOrientModulo = 0;
	displayOrientLen = 0;

	// Colors
	SET_COLOR(controlClr, 0xFF, 0x00, 0x00, 0xFF);
	SET_COLOR(curveClr, 0x00, 0xFF, 0x00, 0xFF);
	SET_COLOR(tanClr, 0x80, 0x00, 0xFF, 0xFF);
	SET_COLOR(nrmClr, 0xFF, 0x00, 0xFF, 0xFF);
	SET_COLOR(sidClr, 0x00, 0xFF, 0x00, 0xFF);
	
	// Fill Structure
	ParseDefinitionFile(fileName);

	// Build Spline
	Build();
}

/*
//---------------------------------------------------------------------------
// Contstruct a Spline using Control Points
// For optimization reasons, the data is not copied here. You
// must make sure that the data remains allocated.
//---------------------------------------------------------------------------
Spline::Spline(SplineControlData * data, int controlCntIn, int curveSubDIn) {
	// Initialization
	controlIndx = 0;
	curveIndx = 0;
	
	// Fill Structure
	controlData = data;
	controlCnt = controlCntIn;
	curveSubD = curveSubDIn;

	curveType = 0;
	displayControlMode = 1;
	displayCurveMode = 1;
	displayCurveModulo = 1;
	displayOrientMode = 0;
	displayOrientModulo = 1;
	displayOrientLen = 0.25;

	// Build Spline
	Build();
}
*/

//---------------------------------------------------------------------------
// Parse a Spline Definitions File
//---------------------------------------------------------------------------
void Spline::ParseDefinitionFile(char * fileName) {
	char buffer[256];
	double x, y, z, param;
	FILE * file;

	file = fopen(fileName, "r");
	assert(file != NULL && "Spline:: Couldn't Open Parse File");

	// Get track parameters
	while (1) { 
		// Grag Info
		fgets(buffer, 256, file);

		// EOF
		if (feof(file)) {
			assert(state == STATE_BEGIN && "Spline:: Couldn't Parse File");
			assert(displayOrientModulo && "Spline:: No Modulo Set!");
			assert(displayCurveModulo && "Spline:: No Modulo Set!");
			return;
		}

		// State Switching
		switch (state) {
			case STATE_BEGIN:
				if (strstr(buffer, "begin_spline")) state = STATE_SPLINE;
			break;
			case STATE_SPLINE:
				if (sscanf(buffer, " control_count %lf", &param) == 1) {
					controlCnt = (int)param;
				}
				else if (sscanf(buffer, " curve_subdivision %lf", &param) == 1) {
					curveSubD = (int)param;
				}
				else if (sscanf(buffer, " curve_type %lf", &param) == 1) {
					curveType = (int)param;
				}
				else if (sscanf(buffer, " display_control_mode %lf", &param) == 1) {
					displayControlMode = (int)param;
				}
				else if (sscanf(buffer, " display_curve_mode %lf", &param) == 1) {
					displayCurveMode = (int)param;
				}
				else if (sscanf(buffer, " display_curve_modulo %lf", &param) == 1) {
					displayCurveModulo = (int)param;
				}
				else if (sscanf(buffer, " display_orientation_mode %lf", &param) == 1) {
					displayOrientMode = (int)param;
				}
				else if (sscanf(buffer, " display_orientation_modulo %lf", &param) == 1) {
					displayOrientModulo = (int)param;
				}
				else if (sscanf(buffer, " display_orientation_len %lf", &param) == 1) {
					displayOrientLen = (float)param;
				}
				else if (strstr(buffer, "begin_points")) {
					// Allocate Point Space
					assert(curveSubD != 0 && "No curve subdivision!\n");
					assert(controlCnt != 0 && "No controlCnt");

					// Alloc Control Points
					controlData = (SplineControlData *)malloc(controlCnt * sizeof(SplineControlData));
					memset(controlData, 0, controlCnt * sizeof(SplineControlData));

					// Alloc Curve Points
					curveCnt = controlCnt * curveSubD;
					curveData = (SplineCurveData *)malloc(curveCnt * sizeof(SplineCurveData));
					memset(curveData, 0, curveCnt * sizeof(SplineCurveData));
					state = STATE_POINTS;
				}
				else if (strstr(buffer, "end_spline")) state = STATE_BEGIN;
			break;
			case STATE_POINTS:
				if (controlIndx == controlCnt) {
					state = STATE_SPLINE;
				}
				if (sscanf(buffer, " C: %lf %lf %lf", &x, &y, &z) == 3) {
					controlData[controlIndx].pos.x = (float)x;
					controlData[controlIndx].pos.y = (float)y;
					controlData[controlIndx].pos.z = (float)z;
					// Since there is no end indicator to the control point, I will increment here
					controlIndx ++;
				}
				if (strstr(buffer, "P:") && controlIndx >= 2) {
					// Since paramters are placed after the control point is defined, and I incremented
					// above, I need to subtract one from the index. PLUS, since the first control
					// point is not drawn, I need to subtract another one...
					if (strstr(buffer, "no_gravity")) {
						controlData[controlIndx - 2].physicsFlag = CONTROL_PHYSICS_NO_GRAVITY;
					}
					else if (sscanf(buffer, " P: break %lf", &x) == 1) {
						controlData[controlIndx - 2].physicsFlag = CONTROL_PHYSICS_BREAK;
						controlData[controlIndx - 2].physicsValue = (float)x;
					}
				}
				if (strstr(buffer, "N:") && controlIndx >= 2) {
					if (sscanf(buffer, " N: %lf %lf %lf ", &x, &y, &z) == 3) {
						controlData[controlIndx - 2].nrmFlag = CONTROL_NORMAL_GIVEN;
						controlData[controlIndx - 2].nrm.x = (float)x;
						controlData[controlIndx - 2].nrm.y = (float)y;
						controlData[controlIndx - 2].nrm.z = (float)z;
					}
					else if (sscanf(buffer, " N: rotate %lf ", &x) == 1) {
						controlData[controlIndx - 2].nrmFlag = CONTROL_NORMAL_ROT;
						controlData[controlIndx - 2].nrmValue = (float)x;
					}
				}
				else if (strstr(buffer, "end_points")) state = STATE_SPLINE;
			break;
		}
	}
}

//---------------------------------------------------------------------------
// Build a Spline
//---------------------------------------------------------------------------
void Spline::Build(vector3 defualtNrmIn) {
	defualtNrm = defualtNrmIn;
	Build();
}

//---------------------------------------------------------------------------
// Build a Spline
//---------------------------------------------------------------------------
void Spline::Build() {
	float u, u_2, u_3;
	int i, j;
	int index;
	vector3 distVec;
	vector3 origin(0.0, 0.0, 0.0);
	matrix44 tempMtx;

	index = 0;
	// For each control Point (Minus the last three)
	for (i = 0; i < controlCnt - 3; i++) {

		// For each subdivision
		for(j = 0; j < curveSubD; j++) {

			u = (float)j / curveSubD;
			u_2 = u * u;
			u_3 = u_2 * u;

			if (curveType == 0) {
				// Position
				curveData[index].pos.x = B_SPLINE(u, u_2, u_3, 
									controlData[i  ].pos.x,
									controlData[i+1].pos.x,
									controlData[i+2].pos.x,
									controlData[i+3].pos.x);

				curveData[index].pos.y = B_SPLINE(u, u_2, u_3, 
									controlData[i  ].pos.y,
									controlData[i+1].pos.y,
									controlData[i+2].pos.y,
									controlData[i+3].pos.y);

				curveData[index].pos.z = B_SPLINE(u, u_2, u_3, 
									controlData[i  ].pos.z,
									controlData[i+1].pos.z,
									controlData[i+2].pos.z,
									controlData[i+3].pos.z);

				// Tangent
				curveData[index].tan.x = B_TANGENT(u, u_2,
									controlData[i  ].pos.x,
									controlData[i+1].pos.x,
									controlData[i+2].pos.x,
									controlData[i+3].pos.x);

				curveData[index].tan.y = B_TANGENT(u, u_2,
									controlData[i  ].pos.y,
									controlData[i+1].pos.y,
									controlData[i+2].pos.y,
									controlData[i+3].pos.y);

				curveData[index].tan.z = B_TANGENT(u, u_2,
									controlData[i  ].pos.z,
									controlData[i+1].pos.z,
									controlData[i+2].pos.z,
									controlData[i+3].pos.z);
				curveData[index].tan.normalize();
			}
			else {
				curveData[index].pos.x = CATMULL_ROM_SPLINE(u, u_2, u_3, 
									controlData[i  ].pos.x,
									controlData[i+1].pos.x,
									controlData[i+2].pos.x,
									controlData[i+3].pos.x);

				curveData[index].pos.y = CATMULL_ROM_SPLINE(u, u_2, u_3, 
									controlData[i  ].pos.y,
									controlData[i+1].pos.y,
									controlData[i+2].pos.y,
									controlData[i+3].pos.y);

				curveData[index].pos.z = CATMULL_ROM_SPLINE(u, u_2, u_3, 
									controlData[i  ].pos.z,
									controlData[i+1].pos.z,
									controlData[i+2].pos.z,
									controlData[i+3].pos.z);

				// Tangent
				curveData[index].tan.x = CATMULL_ROM_TANGENT(u, u_2,
									controlData[i  ].pos.x,
									controlData[i+1].pos.x,
									controlData[i+2].pos.x,
									controlData[i+3].pos.x);

				curveData[index].tan.y = CATMULL_ROM_TANGENT(u, u_2,
									controlData[i  ].pos.y,
									controlData[i+1].pos.y,
									controlData[i+2].pos.y,
									controlData[i+3].pos.y);

				curveData[index].tan.z = CATMULL_ROM_TANGENT(u, u_2,
									controlData[i  ].pos.z,
									controlData[i+1].pos.z,
									controlData[i+2].pos.z,
									controlData[i+3].pos.z);
				curveData[index].tan.normalize();
			}

			// Normal
			if (j == 0 && i == 0) {
				curveData[0].nrm = defualtNrm;
				curveData[0].sid = CrossProduct(curveData[0].nrm, curveData[0].tan);
			}
			else {
				// Cross two tangents To get a side vector
				curveData[index].sid = CrossProduct(curveData[index - 1].tan, curveData[index].tan);
				curveData[index].sid.normalize();
				// Modify side vector
#if 0
				if (controlData[i].nrmFlag == CONTROL_NORMAL_ROT) {
					MTXRotAxis(tempMtx, &curveData[index].tan, controlData[i].nrmValue);
					MTXMultVec(tempMtx, &curveData[index].sid, &curveData[index].sid);
				}
				if (controlData[i].nrmFlag == CONTROL_NORMAL_GIVEN) {
					MTXRotAxis(tempMtx, &curveData[index].tan, 90);
					MTXMultVec(tempMtx, &controlData[i].nrm, &curveData[index].sid);
					MTXLookAt(tempMtx, &origin,
						&curveData[index].sid,
						&curveData[index].tan);
					MTXMultVec(tempMtx, &curveData[index].sid, &curveData[index].sid);
					curveData[index].sid.z = 0;
					VECNormalize(&curveData[index].sid, &curveData[index].sid);
					MTXInverse(tempMtx, tempMtx);
					MTXMultVec(tempMtx, &curveData[index].sid, &curveData[index].sid);
					VECNormalize(&curveData[index].sid, &curveData[index].sid);
				}
#endif
				// Make sure there are no Zero vectors
				if (curveData[index].sid.isZero()) curveData[index - 1].sid = curveData[index].sid;
				// Check for Vector Flip
				if (DotProduct(curveData[index - 1].sid, curveData[index].sid) < 0) {
					curveData[index].sid *= -1.0;
				}
				// Cross side vector and tangent to get the normal
				curveData[index].nrm = CrossProduct(curveData[index].tan, curveData[index].sid);
				// Normalize
				curveData[index].nrm.normalize();
			}

			// Distance
			if (j == 0 && i == 0) {
				curveData[index].distance = 0;
				shortestDistance = 65536.0;
			}
			else {
				distVec = curveData[index].pos - curveData[index - 1].pos;
				curveData[index].distance = 
					curveData[index - 1].distance + distVec.length();
				if (curveData[index].distance < shortestDistance)
					shortestDistance = curveData[index].distance;
			}
								
			index++;
		}
	}
	
	curveCnt = index;
}

//---------------------------------------------------------------------------
// Draw the Spline 
//---------------------------------------------------------------------------
void Spline::DrawPrimitives() {
	int i, j;
	vector3 * vec0P;
	vector3 * vec1P;
	vector3 temp;
	COLOR color;

	// Draw Control Spline
	if (displayControlMode) {
		// Control Lines
		if (displayControlMode == DISPLAY_MODE_LINES) {
			glBegin(GL_LINE_STRIP);
			for (i = 0; i < controlCnt; i++) {
				glColor4ub(
					controlClr.r,
					controlClr.g,
					controlClr.b,
					controlClr.a
				);
				glVertex3f(
					controlData[i].pos.x,
					controlData[i].pos.y,
					controlData[i].pos.z
				);
			}
			glEnd();
		}
		// Control Points
		glPointSize(3.0);
		glBegin(GL_POINTS);
		for (i = 0; i < controlCnt; i++) {
			glColor4ub(
				controlClr.r,
				controlClr.g,
				controlClr.b,
				controlClr.a
			);
			glVertex3f(
				controlData[i].pos.x,
				controlData[i].pos.y,
				controlData[i].pos.z
			);
		}
		glEnd();
		glPointSize(1.0);
	}

	// Draw Spline
	if (displayCurveMode) {
		if (displayCurveMode == DISPLAY_MODE_LINES) glBegin(GL_LINE_STRIP);
		else glBegin(GL_POINTS);
		for (i = 0; i < curveCnt; i += displayCurveModulo) {
			glColor4ub(
				curveClr.r,
				curveClr.g,
				curveClr.b,
				curveClr.a
			);
			glVertex3f(
				curveData[i].pos.x,
				curveData[i].pos.y,
				curveData[i].pos.z
			);
		}
		glEnd();
	}

	// Draw Orientations
	if (displayOrientMode) {
		for (j = 1; j < displayOrientMode + 1; j++) {
			glBegin(GL_LINES);
			for (i = 0; i < curveCnt; i += displayOrientModulo) {
				vec0P = &curveData[i].pos;
				switch (j) {
					// Draw Normal
					case 1:
						vec1P = &curveData[i].nrm;
						color = nrmClr;
					break;
					// Draw Tangent
					case 2:
						vec1P = &curveData[i].tan;
						color = tanClr;
					break;
					// Draw Side
					case 3:
						vec1P = &curveData[i].sid;
						color = sidClr;
					break;
				}
				temp = (*vec1P) * displayOrientLen;
				temp += (*vec0P);
				glColor4ub(
					color.r,
					color.g,
					color.b,
					color.a
				);
				glVertex3f(
					vec0P->x,
					vec0P->y,
					vec0P->z
				);
				glVertex3f(
					temp.x,
					temp.y,
					temp.z
				);
			}
			glEnd();
		}
	}
}

//---------------------------------------------------------------------------
// Compute a spline index at distance
//---------------------------------------------------------------------------
int Spline::GetIndexAtDistance(float distance) {
	int index = 0;

	if (distance < 0.0) return -1;

	// Forward Push
	while (index < curveCnt && distance > curveData[index].distance) {
		index++;
	}

	if (index >= curveCnt) return -1;

	return index;
}

//---------------------------------------------------------------------------
// Compute a spline index at distance
//---------------------------------------------------------------------------
int Spline::GetIndexAtDistanceFromIndex(float distance, int startIndex) {
	int index = startIndex;

	// Forward Push
	while (index < curveCnt && distance > curveData[index].distance) {
		index++;
	}
	// Backward Push
	while (index > 0 && distance <= curveData[index - 1].distance) {
		index--;
	}

	return index;
}


