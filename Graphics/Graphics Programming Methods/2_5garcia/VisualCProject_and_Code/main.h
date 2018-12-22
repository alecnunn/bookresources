/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* The main application file header     */
/* --------------------------------     */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#ifndef MAIN_H
#define MAIN_H

/****************************************/
/* Include files                        */
/****************************************/
#include <GL/glut.h>
#include "structs.h"
#include "smooth.h"
#include "mesh.h"
#include "render.h"

/****************************************/
/* Defined constants                    */
/****************************************/
#define DEFAULT_TOL 0.2
#define DEFAULT_ALFA 0.1

/****************************************/
/* Function prototypes                  */
/****************************************/
void display(void);
void keys(unsigned char key, int x, int y);
void initRender(void);
int main(int argc, char** argv);

#endif