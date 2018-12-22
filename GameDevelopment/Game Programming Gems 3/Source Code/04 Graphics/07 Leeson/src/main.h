/* Copyright (C) William Leeson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2001"
 */
/*
 *   GLUT - GLUT based program
 */
#include <GL/glut.h>
#include "GLScene.h"
#include "libarray/libarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// enum button_state = { NONE, LEFT, RIGHT MIDDLE };  // mouse buttons

//GLScene *gp_scene;                      // renderer
//IConsole *mp_console;                   // console for messages

/*
 * GLUT callback functions
 */
void reshape(int width,int height);          // window resize 
void keyboard(unsigned char key,int x,int y);// key press
void press(int button,int state,int x,int y);// mouse button press
void motion(int x,int y);                    // mouse motion
void display(void);                          // display function
void tick(int value);                        // timer tick
void idle(void);                             // idle function
