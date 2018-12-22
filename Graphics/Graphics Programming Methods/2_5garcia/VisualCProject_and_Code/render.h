/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* Render related functions file header */
/* ------------------------------------ */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#ifndef RENDER_H
#define RENDER_H

/****************************************/
/* Include files                        */
/****************************************/
#include <GL/glut.h>
#include "structs.h"

/****************************************/
/* Function prototypes                  */
/****************************************/
void drawEdges(mesh *m);
void drawTriangles(mesh *m);

#endif