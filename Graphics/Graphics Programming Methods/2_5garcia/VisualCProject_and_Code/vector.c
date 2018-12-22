/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* The vector mini-library file         */
/* ----------------------------         */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#include "vector.h"

/****************************************/
/* The float sorter                     */
/****************************************/
void sort(float *array, int counter){
    int i, j;
	float swapFloat;

    for(i=0;i<counter;i++){
        for(j=1;j<(counter-i);j++){
			if( array[j-1]>array[j] ){
				swapFloat=array[j];
				array[j]=array[j-1];
				array[j-1]=swapFloat;
			}
        }
    }
}

/****************************************/
/* Normal vector for a triangle         */
/****************************************/
void findTriangleNormal(float *normalVector, float *v0, float *v1, float *v2){
	float a[3],b[3],c[3];

	// First edge (a)

	a[0]=v2[0]-v1[0];
	a[1]=v2[1]-v1[1];
	a[2]=v2[2]-v1[2];
	
	// Second edge (b)

	b[0]=v0[0]-v1[0];
	b[1]=v0[1]-v1[1];
	b[2]=v0[2]-v1[2];

	// Cross product of a and b (c)

	crossProduct(a,b,c);

	// Normalization of c

	normalizeVector(c);

	// Done

	normalVector[0]=c[0];
	normalVector[1]=c[1];
	normalVector[2]=c[2];
}

/****************************************/
/* Rad to Degrees conversion            */
/****************************************/
float radToDeg(float rads){
	return rads * 180.0 / PI;
}

/****************************************/
/* Projection vector over vector        */
/****************************************/
float projectVectorOverVector(float vector1[3], float vector2[3]){
	return dotProduct(vector1,vector2)/magnitude(vector2);
}

/****************************************/
/* Cross product                        */
/****************************************/
void crossProduct(float a[3], float b[3], float c[3]){
	c[0]=a[1]*b[2]-a[2]*b[1];
	c[1]=a[2]*b[0]-a[0]*b[2];
	c[2]=a[0]*b[1]-a[1]*b[0];
}

/****************************************/
/* Dot product                          */
/****************************************/
float dotProduct(float vector1[3],float vector2[3]){
  return (vector1[0]*vector2[0]+vector1[1]*vector2[1]+vector1[2]*vector2[2]);
}

/****************************************/
/* Magnitude for a vector               */
/****************************************/
float magnitude(float vector[3]){
  return (float)sqrt(dotProduct(vector,vector));
}

/****************************************/
/* Vector normalization                 */
/****************************************/
void normalizeVector(float v[3]){
  float module;
  
  module=magnitude(v);
  if (module>0.0){
    v[0]/=module;
    v[1]/=module;
    v[2]/=module;
  }
}