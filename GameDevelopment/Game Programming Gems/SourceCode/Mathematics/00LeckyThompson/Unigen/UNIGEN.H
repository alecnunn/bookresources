/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
#define UNIVERSE_SIDE 	25	// Dimension of Universe
#define STAR_THRESHOLD	95	// 70% empty / 30% chance of a star at x,y

// Function prototypes
void DisplayUniverse(long universe_serial_number,
							RECT * rcTarget,
							HDC hdcTarget);

int GenerateName(long universe_serial_number,
						int x, int y,
						char name[10]);

void DisplayFeatureInformation(long universe_serial_number,
										 RECT * rcMapWindow,
										 int mouse_x, int mouse_y,
										 RECT * rcMapInfo,HDC hdcTarget);

void	DisplaySolarSystem(long universe_serial_number,RECT * rcMapWindow,
												int mouse_x, int mouse_y,
											  RECT * rcSolarSystemWindow,
											  HDC hdcTarget);

