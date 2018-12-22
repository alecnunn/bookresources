/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* Mesh related functions file header   */
/* ----------------------------------   */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#ifndef MESH_H
#define MESH_H

/****************************************/
/* Include files                        */
/****************************************/
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "structs.h"
#include "vector.h"

/****************************************/
/* Function prototypes                  */
/****************************************/
void copyMesh(mesh *m1, mesh *m2);
void boundingBoxMesh(mesh *m, float *dimensions);
void findMeshNormalVectors(mesh *m);
void arrangeEdges(mesh *m);
int findEdge(int va, int vb, mesh *m);
void loadMesh(mesh *m, char *meshFile);
void deallocateMesh(mesh *m);

#endif