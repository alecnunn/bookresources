/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* The smoothing algorithm file         */
/* ----------------------------         */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#include "smooth.h"

/****************************************/
/* Returns the sum of angles between    */
/* the neighbors (edges)                */
/****************************************/
float returnNeighborhoodAngle(mesh *m, int id){
	int i, vertexIndex[2];
	float **vectorMatrix, *angleArray, xCoord, yCoord, zCoord, acumAngle=0.0;

	// Memory allocation for the structures that we need
	vectorMatrix=(float**)malloc(m->vertexs[id].edgeCounter*sizeof(float*));
	for(i=0;i<m->vertexs[id].edgeCounter;i++){
		vectorMatrix[i]=(float*)malloc(3*sizeof(float));
	}
	angleArray=(float*)malloc(m->vertexs[id].edgeCounter*sizeof(float));

	// Lets save the vertex position. Our vertex is defined by its 'id'
	xCoord=m->vertexs[id].position[0];
	yCoord=m->vertexs[id].position[1];
	zCoord=m->vertexs[id].position[2];

	// Vectors from the vertex to all the neighbors
	for(i=0;i<m->vertexs[id].edgeCounter;i++){
		vertexIndex[0]=m->edges[m->vertexs[id].edges[i]].vertexs[0];
		vertexIndex[1]=m->edges[m->vertexs[id].edges[i]].vertexs[1];
		if (vertexIndex[0]!=id){
			vectorMatrix[i][0]=m->vertexs[vertexIndex[0]].position[0]-xCoord;
			vectorMatrix[i][1]=m->vertexs[vertexIndex[0]].position[1]-yCoord;
			vectorMatrix[i][2]=m->vertexs[vertexIndex[0]].position[2]-zCoord;
		}
		else{
			vectorMatrix[i][0]=m->vertexs[vertexIndex[1]].position[0]-xCoord;
			vectorMatrix[i][1]=m->vertexs[vertexIndex[1]].position[1]-yCoord;
			vectorMatrix[i][2]=m->vertexs[vertexIndex[1]].position[2]-zCoord;
		}
		normalizeVector(vectorMatrix[i]);
	}

	// Evaluation of the angle between every vector and the next
	for(i=0;i<m->vertexs[id].edgeCounter;i++){
		if (i==m->vertexs[id].edgeCounter-1)
			angleArray[i]=(float)acos((double)dotProduct(vectorMatrix[i],vectorMatrix[0]));
		else angleArray[i]=(float)acos((double)dotProduct(vectorMatrix[i],vectorMatrix[i+1]));

		acumAngle+=angleArray[i];
	}

	// Let's free the memory
	for(i=0;i<m->vertexs[id].edgeCounter;i++)
		free(vectorMatrix[i]);
	free(vectorMatrix);
	free(angleArray);
	
	return acumAngle;
}

/****************************************/
/* An Easy-to-code Smoothing Algorithm  */
/* for triangle meshes.                 */
/****************************************/
void smoothingAlgorithm(mesh *m, float tolerance, float alfa){
	int i,j, vertexIndex[2];
	float centroidPosition[3], displacementVector[3];

	// The proces goes vertex from vertex
	for(i=0;i<m->numVertexs;i++){
		centroidPosition[0]=0.0;
		centroidPosition[1]=0.0;
		centroidPosition[2]=0.0;
		if ((2*PI-returnNeighborhoodAngle(m, i))>tolerance){
			// The neighborhood centroid is calculated
			// The displacement vector goes through it
			for(j=0;j<m->vertexs[i].edgeCounter;j++){
				vertexIndex[0]=m->edges[m->vertexs[i].edges[j]].vertexs[0];
				vertexIndex[1]=m->edges[m->vertexs[i].edges[j]].vertexs[1];
				if (vertexIndex[0]!=i){
					centroidPosition[0]+=m->vertexs[vertexIndex[0]].position[0];
					centroidPosition[1]+=m->vertexs[vertexIndex[0]].position[1];
					centroidPosition[2]+=m->vertexs[vertexIndex[0]].position[2];
				}
				else{
					centroidPosition[0]+=m->vertexs[vertexIndex[1]].position[0];
					centroidPosition[1]+=m->vertexs[vertexIndex[1]].position[1];
					centroidPosition[2]+=m->vertexs[vertexIndex[1]].position[2];
				}
			}
			centroidPosition[0]/=m->vertexs[i].edgeCounter;
			centroidPosition[1]/=m->vertexs[i].edgeCounter;
			centroidPosition[2]/=m->vertexs[i].edgeCounter;

			// Evaluation of the displacement vector
			displacementVector[0]=centroidPosition[0]-m->vertexs[i].position[0];
			displacementVector[1]=centroidPosition[1]-m->vertexs[i].position[1];
			displacementVector[2]=centroidPosition[2]-m->vertexs[i].position[2];
			normalizeVector(displacementVector);

			// The vertex is moved towards the centroid by the displacement vector
			while((2*PI-returnNeighborhoodAngle(m, i))>tolerance){
				m->vertexs[i].position[0]+=displacementVector[0]*alfa;
				m->vertexs[i].position[1]+=displacementVector[1]*alfa;
				m->vertexs[i].position[2]+=displacementVector[2]*alfa;
			}
		}
	}
}
