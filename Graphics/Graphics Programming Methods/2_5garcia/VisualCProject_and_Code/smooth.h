/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* The smoothing algorithm file header  */
/* -----------------------------------  */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#ifndef SMOOTH_H
#define SMOOTH_H

/****************************************/
/* Include files                        */
/****************************************/
#include <malloc.h>
#include <math.h>
#include "structs.h"
#include "vector.h"

/****************************************/
/* Function prototypes                  */
/****************************************/
float returnNeighborhoodAngle(mesh *m, int id);
void smoothingAlgorithm(mesh *m, float alfa, float tolerance);

#endif