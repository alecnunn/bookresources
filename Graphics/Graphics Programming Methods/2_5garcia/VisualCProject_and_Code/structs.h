/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* The structures file header           */
/* --------------------------           */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#ifndef STRUCTS_H
#define STRUCTS_H

/****************************************/
/* Defined constants                    */
/****************************************/
#define PI 3.14159

/****************************************/
/* Data types                           */
/****************************************/
typedef struct{
	int edgeIndex;
	float position[3];
	float lambda;
	float mu;
	float angleR;
	float angleG;
}planeCoordinates;

typedef struct{
	int id;					// Vertex ID
	float position[3];		// XYZ values for the vertex
	int triangleCounter;	// It belongs to "triangleCounter" triangles
	int *triangles;			// Indexes of the triangles
	int edgeCounter;		// It belongs to "edgeCounter" edges
	int *edges;				// Indexes of the edges
	float normal[3];		// Normal vector associated to the vertex
}vertex;					// A vertex

typedef struct{
	int id;					// Edge ID
	int vertexs[2];			// Two vertexs for this edge
	int triangleCounter;	// It belongs to "triangleCounter" triangles
	int *triangles;			// Indexes of the triangles
}edge;						// An edge

typedef struct{
	int id;					// triangle ID
	int edges[3];			// Three edges for this triangle
	int vertexs[3];			// Three vertexs for this triangle
	float normal[3];		// Normal vector associated to the
							// triangle
}triangle;					// A triangle

typedef struct{
	int id;					// mesh ID
	int numVertexs;			// Total amount of vertexs in the global list
	vertex *vertexs;		// List of vertexs
	int numEdges;			// Total amount of edges in the global list
	edge *edges;			// List of edges
	int numTriangles;		// Total amount of triangles in the global list
	triangle *triangles;	// List of triangles
}mesh;						// A mesh

#endif

