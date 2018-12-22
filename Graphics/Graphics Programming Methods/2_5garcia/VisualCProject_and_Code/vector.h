/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* The vector mini-library file header  */
/* -----------------------------------  */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#ifndef VECTOR_H
#define VECTOR_H

/****************************************/
/* Include files                        */
/****************************************/
#include <math.h>
#include "structs.h"

/****************************************/
/* Function prototypes                  */
/****************************************/
void sort(float *array, int counter);
float radToDeg(float rads);
float projectVectorOverVector(float vector1[3], float vector2[3]);
void findTriangleNormal(float *normalVector, float *v0, float *v1, float *v2);
void crossProduct(float a[3], float b[3], float c[3]);
float dotProduct(float vector1[3],float vector2[3]);
float magnitude(float vector[3]);
void normalizeVector(float v[3]);

#endif