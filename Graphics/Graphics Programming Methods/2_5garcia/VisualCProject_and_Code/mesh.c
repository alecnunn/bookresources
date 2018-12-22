/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* Mesh related functions file          */
/* ---------------------------          */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#include "mesh.h"

/****************************************/
/* Copies m1 into m2                    */
/****************************************/
void copyMesh(mesh *m1, mesh *m2){
	int i,j;

	m2->id=m1->id;
	m2->numVertexs=m1->numVertexs;
	m2->numEdges=m1->numEdges;
	m2->numTriangles=m1->numTriangles;

	m2->vertexs=(vertex*)malloc(m1->numVertexs*sizeof(vertex));
	m2->triangles=(triangle*)malloc(m1->numTriangles*sizeof(triangle));
	m2->edges=(edge*)malloc(m1->numEdges*sizeof(edge));
		
	// Vertex copy
	for (i=0;i<m1->numVertexs;i++){
		m2->vertexs[i].id=m1->vertexs[i].id;
		m2->vertexs[i].position[0]=m1->vertexs[i].position[0];
		m2->vertexs[i].position[1]=m1->vertexs[i].position[1];
		m2->vertexs[i].position[2]=m1->vertexs[i].position[2];
		m2->vertexs[i].edgeCounter=m1->vertexs[i].edgeCounter;
		m2->vertexs[i].edges=(int*)malloc(m1->vertexs[i].edgeCounter*sizeof(int));
		for(j=0;j<m1->vertexs[i].edgeCounter;j++)
			m2->vertexs[i].edges[j]=m1->vertexs[i].edges[j];
		m2->vertexs[i].triangleCounter=m1->vertexs[i].triangleCounter;
		m2->vertexs[i].triangles=(int*)malloc(m1->vertexs[i].triangleCounter*sizeof(int));
		for(j=0;j<m1->vertexs[i].triangleCounter;j++)
			m2->vertexs[i].triangles[j]=m1->vertexs[i].triangles[j];
		m2->vertexs[i].normal[0]=m1->vertexs[i].normal[0];
		m2->vertexs[i].normal[1]=m1->vertexs[i].normal[1];
		m2->vertexs[i].normal[2]=m1->vertexs[i].normal[2];
	}
	
	// Edge copy
	for (i=0;i<m1->numEdges;i++){
		m2->edges[i].id=m1->edges[i].id;
		m2->edges[i].triangleCounter=m1->edges[i].triangleCounter;
		m2->edges[i].vertexs[0]=m1->edges[i].vertexs[0];
		m2->edges[i].vertexs[1]=m1->edges[i].vertexs[1];
		m2->edges[i].triangles=(int*)malloc(m2->edges[i].triangleCounter*sizeof(int));
		for(j=0;j<m1->edges[i].triangleCounter;j++)
			m2->edges[i].triangles[j]=m1->edges[i].triangles[j];
	}

	// Triangle copy
	for (i=0;i<m1->numTriangles;i++){
		m2->triangles[i].id=m1->triangles[i].id;
		m2->triangles[i].vertexs[0]=m1->triangles[i].vertexs[0];
		m2->triangles[i].vertexs[1]=m1->triangles[i].vertexs[1];
		m2->triangles[i].vertexs[2]=m1->triangles[i].vertexs[2];
		m2->triangles[i].edges[0]=m1->triangles[i].edges[0];
		m2->triangles[i].edges[1]=m1->triangles[i].edges[1];
		m2->triangles[i].edges[2]=m1->triangles[i].edges[2];
		m2->triangles[i].normal[0]=m1->triangles[i].normal[0];
		m2->triangles[i].normal[1]=m1->triangles[i].normal[1];
		m2->triangles[i].normal[2]=m1->triangles[i].normal[2];
	}
}

/****************************************/
/* Evaluation of the bounding box       */
/* for the input mesh                   */
/****************************************/
void boundingBoxMesh(mesh *m, float *dimensions){
	int i;

	// Initialize the array
	for(i=0;i<6;i++)
		dimensions[i]=0.0;

	// Get the max and min positions for every coordinate
	for(i=0;i<m->numVertexs;i++){
		if (dimensions[0]>m->vertexs[i].position[0])
			dimensions[0]=m->vertexs[i].position[0];
		if (dimensions[1]<m->vertexs[i].position[0])
			dimensions[1]=m->vertexs[i].position[0];

		if (dimensions[2]>m->vertexs[i].position[1])
			dimensions[2]=m->vertexs[i].position[1];
		if (dimensions[3]<m->vertexs[i].position[1])
			dimensions[3]=m->vertexs[i].position[1];

		if (dimensions[4]>m->vertexs[i].position[2])
			dimensions[4]=m->vertexs[i].position[2];
		if (dimensions[5]<m->vertexs[i].position[2])
			dimensions[5]=m->vertexs[i].position[2];
	}
}

/****************************************/
/* Evaluation of the normal vectors     */
/* associated to the triangles and      */
/* vertexs                              */
/****************************************/
void findMeshNormalVectors(mesh *m){
	int i, j, triangleIndex;
	float temporalVector[3], normalVector[3];

	// First let's find the normals associated to the triangles
	for(i=0;i<m->numTriangles;i++){
		findTriangleNormal(normalVector,
		m->vertexs[m->triangles[i].vertexs[0]].position,
		m->vertexs[m->triangles[i].vertexs[1]].position,
		m->vertexs[m->triangles[i].vertexs[2]].position);
		
		m->triangles[i].normal[0]=normalVector[0];
		m->triangles[i].normal[1]=normalVector[1];
		m->triangles[i].normal[2]=normalVector[2];
	}

	// Then the normal for a vertex is the average of the
	// normals from the triangles it belongs to
	// All the normals are forced to be unitary
	for(i=0;i<m->numVertexs;i++){
		m->vertexs[i].normal[0]=0.0;
		m->vertexs[i].normal[1]=0.0;
		m->vertexs[i].normal[2]=0.0;
		for(j=0;j<m->vertexs[i].triangleCounter;j++){
			triangleIndex=m->vertexs[i].triangles[j];
			m->vertexs[i].normal[0]+=m->triangles[triangleIndex].normal[0];
			m->vertexs[i].normal[1]+=m->triangles[triangleIndex].normal[1];
			m->vertexs[i].normal[2]+=m->triangles[triangleIndex].normal[2];
		}
		m->vertexs[i].normal[0]/=m->vertexs[i].triangleCounter;
		m->vertexs[i].normal[1]/=m->vertexs[i].triangleCounter;
		m->vertexs[i].normal[2]/=m->vertexs[i].triangleCounter;

		temporalVector[0]=m->vertexs[i].normal[0];
		temporalVector[1]=m->vertexs[i].normal[1];
		temporalVector[2]=m->vertexs[i].normal[2];
		normalizeVector(temporalVector);
		m->vertexs[i].normal[0]=temporalVector[0];
		m->vertexs[i].normal[1]=temporalVector[1];
		m->vertexs[i].normal[2]=temporalVector[2];
	}
}

/****************************************/
/* The edges are arranged for every     */
/* vertex. This is needed by the        */
/* smoothing algorithm                  */
/****************************************/
void arrangeEdges(mesh *m){
	float v1[3], v2[3], A, B, C, D, X, Y, Z;
	float temporalVector[3], temporalFloat;
	float *arrangement;
	planeCoordinates *coords;
	int i, j, k, vertexIndex[2];

	// Let's find the normal vector associated to all the vertexs
	findMeshNormalVectors(m);
	
	// The arrangement begins, vertex by vertex
	for(i=0;i<m->numVertexs;i++){

		// Memory allocation for the planeCoordinates structure
		coords=(planeCoordinates*)malloc(m->vertexs[i].edgeCounter*sizeof(planeCoordinates));
		arrangement=(float*)malloc(m->vertexs[i].edgeCounter*sizeof(float));

		// Creation of the plane that contains the 'i' vertex
		// Vertex and plane share the normal vector
		// The plane is from the form --> Ax+By+Cz+D=0
		A=m->vertexs[i].normal[0];
		B=m->vertexs[i].normal[1];
		C=m->vertexs[i].normal[2];

		X=m->vertexs[i].position[0];
		Y=m->vertexs[i].position[1];
		Z=m->vertexs[i].position[2];

		D=-A*X-B*Y-C*Z;

		// Neighbor vertexs are projected on the created plane
		for(j=0;j<m->vertexs[i].edgeCounter;j++){
			vertexIndex[0]=m->edges[m->vertexs[i].edges[j]].vertexs[0];
			vertexIndex[1]=m->edges[m->vertexs[i].edges[j]].vertexs[1];
			if (vertexIndex[0]!=i){
				temporalVector[0]=m->vertexs[vertexIndex[0]].position[0];
				temporalVector[1]=m->vertexs[vertexIndex[0]].position[1];
				temporalVector[2]=m->vertexs[vertexIndex[0]].position[2];
			}
			else{
				temporalVector[0]=m->vertexs[vertexIndex[1]].position[0];
				temporalVector[1]=m->vertexs[vertexIndex[1]].position[1];
				temporalVector[2]=m->vertexs[vertexIndex[1]].position[2];
			}

			temporalFloat=-(A*temporalVector[0]+B*temporalVector[1]+C*temporalVector[2]+D)/(A*A+B*B+C*C);

			coords[j].position[0]=temporalVector[0]+temporalFloat*A;
			coords[j].position[1]=temporalVector[1]+temporalFloat*B;
			coords[j].position[2]=temporalVector[2]+temporalFloat*C;

			coords[j].edgeIndex=m->vertexs[i].edges[j];
		}

		// Computation of v1 and v2, the director vectors for the plane

		// V1 is created from the vertex and the projection
		// of the first neighbor (edge labelled as 0)
		v1[0]=coords[0].position[0]-X;
		v1[1]=coords[0].position[1]-Y;
		v1[2]=coords[0].position[2]-Z;
		normalizeVector(v1);

		// V2 is created from the cross product between V1
		// and the plane normal vector
		crossProduct(m->vertexs[i].normal, v1, v2);
		normalizeVector(v2);

		// Let's fins the plane coordinates (lambda and mu) for every
		// projected neighbor.
		// lambda = projection of the vector vertex-neighbor over V1
		// mu = projection of the vector vertex-neighbor over V2
		// We go from 3D (space) to 2D (plane)
		for(j=0;j<m->vertexs[i].edgeCounter;j++){
			temporalVector[0]=coords[j].position[0]-X;
			temporalVector[1]=coords[j].position[1]-Y;
			temporalVector[2]=coords[j].position[2]-Z;

			coords[j].lambda=projectVectorOverVector(temporalVector,v1);
			coords[j].mu=projectVectorOverVector(temporalVector,v2);

			coords[j].angleR=(float)atan2((double)coords[j].mu, (double)coords[j].lambda);
			if (coords[j].angleR<0.0) coords[j].angleR+=2*PI;

			coords[j].angleG=radToDeg(coords[j].angleR);

			arrangement[j]=coords[j].angleG;
		}
		
		// Edges are arranged from min to max angle
		sort(arrangement,m->vertexs[i].edgeCounter);

		for(j=0;j<m->vertexs[i].edgeCounter;j++){
			for(k=0;k<m->vertexs[i].edgeCounter;k++){
				if (coords[k].angleG==arrangement[j]){
					m->vertexs[i].edges[j]=coords[k].edgeIndex;
				}
			}
		}
		free(coords);
		free(arrangement);
	}
}

/****************************************/
/* Creates a new edge or returns its    */
/* index                                */
/****************************************/
int findEdge(int va, int vb, mesh *m){
	int i;
	
	// Especial case for the first edge
	if (m->numEdges==0){
		m->edges[0].id=0;
		m->edges[0].vertexs[0]=va;
		m->edges[0].vertexs[1]=vb;
	}
	else{
		// Let's find out if this edge was already created
		for(i=0;i<m->numEdges;i++){
			if (((m->edges[i].vertexs[0]==va)&&(m->edges[i].vertexs[1]==vb))||
				((m->edges[i].vertexs[0]==vb)&&(m->edges[i].vertexs[1]==va))) return i;
		}
		
		// If here --> the edge is new
		m->edges[m->numEdges].id=m->numEdges;
		m->edges[m->numEdges].vertexs[0]=va;
		m->edges[m->numEdges].vertexs[1]=vb;
	}
	m->numEdges++;
	return (m->numEdges)-1;
}

/****************************************/
/* The mesh loader                      */
/* The input format is very simple:     */
/*                                      */
/* number of vertexs (int)              */
/* number of triangles (int)            */
/* list of vertexs as 3 floats (XYZ)    */
/* list of triangles as 3 integers      */
/* with the 3 associated vertex indexes */
/****************************************/
void loadMesh(mesh *m, char *meshFile){
	FILE *f;
	int v0,v1,v2;
	int i,j,counter=0;
	edge *temporalEdgeList;

	f=fopen(meshFile,"r");
	fscanf (f,"%d",&m->numVertexs);
	fscanf (f,"%d",&m->numTriangles);
	
	// The worst case: to have 3 * number of triangles edges
	// The array will be correctly allocated later
	m->vertexs=(vertex*)malloc(m->numVertexs*sizeof(vertex));
	m->triangles=(triangle*)malloc(m->numTriangles*sizeof(triangle));
	m->edges=(edge*)malloc((m->numTriangles*3)*sizeof(edge));
		
	// Vertex parsing
	for (i=0;i<m->numVertexs;i++){
		fscanf (f,"%f",&m->vertexs[i].position[0]);
		fscanf (f,"%f",&m->vertexs[i].position[1]);
		fscanf (f,"%f",&m->vertexs[i].position[2]);
		m->vertexs[i].id=i;
		m->vertexs[i].normal[0]=0.0;
		m->vertexs[i].normal[1]=0.0;
		m->vertexs[i].normal[2]=0.0;
	}
	
	// Triangles are created. Vertexs and edges are associated
	// Edges are created. Vertexs are associated

	// Triangle parsing
	for (i=0;i<m->numTriangles;i++){
		fscanf (f,"%d",&v0);
		fscanf (f,"%d",&v1);
		fscanf (f,"%d",&v2);
		m->triangles[i].id=i;
		m->triangles[i].vertexs[0]=v0;
		m->triangles[i].vertexs[1]=v1;
		m->triangles[i].vertexs[2]=v2;
		m->triangles[i].edges[0]=findEdge(v0, v1, m);
		m->triangles[i].edges[1]=findEdge(v1, v2, m);
		m->triangles[i].edges[2]=findEdge(v2, v0, m);
		m->triangles[i].normal[0]=0.0;
		m->triangles[i].normal[1]=0.0;
		m->triangles[i].normal[2]=0.0;
	}

	// The edge array is correctly allocated (with the real needed dimension)
	temporalEdgeList=(edge*)malloc(m->numEdges*sizeof(edge));

	for (i=0;i<m->numEdges;i++){
		temporalEdgeList[i].id=m->edges[i].id;
		temporalEdgeList[i].vertexs[0]=m->edges[i].vertexs[0];
		temporalEdgeList[i].vertexs[1]=m->edges[i].vertexs[1];
	}

	free(m->edges);
	m->edges=(edge*)malloc(m->numEdges*sizeof(edge));

	for(i=0;i<m->numEdges;i++){
		m->edges[i].id=temporalEdgeList[i].id;
		m->edges[i].vertexs[0]=temporalEdgeList[i].vertexs[0];
		m->edges[i].vertexs[1]=temporalEdgeList[i].vertexs[1];
	}
		
	free(temporalEdgeList);

	// Every edge is associated to the triangles it belongs to
	for(i=0;i<m->numEdges;i++){
		counter=0;
		for(j=0;j<m->numTriangles;j++){
			if (m->triangles[j].edges[0]==i ||
				m->triangles[j].edges[1]==i ||
				m->triangles[j].edges[2]==i) counter++;
		}
		m->edges[i].triangles=(int*)malloc(counter*sizeof(int));
		counter=0;
		for(j=0;j<m->numTriangles;j++){
			if (m->triangles[j].edges[0]==i ||
				m->triangles[j].edges[1]==i ||
				m->triangles[j].edges[2]==i){
				m->edges[i].triangles[counter]=j;
				counter++;
			}
		}
		m->edges[i].triangleCounter=counter;
	}

	// Every vertex is associated to the triangles it belongs to
	for(i=0;i<m->numVertexs;i++){
		counter=0;
		for(j=0;j<m->numTriangles;j++){
			if (m->triangles[j].vertexs[0]==i ||
				m->triangles[j].vertexs[1]==i ||
				m->triangles[j].vertexs[2]==i) counter++;
		}
		m->vertexs[i].triangles=(int*)malloc(counter*sizeof(int));
		counter=0;
		for(j=0;j<m->numTriangles;j++){
			if (m->triangles[j].vertexs[0]==i ||
				m->triangles[j].vertexs[1]==i ||
				m->triangles[j].vertexs[2]==i){
				m->vertexs[i].triangles[counter]=j;
				counter++;
			}
		}
		m->vertexs[i].triangleCounter=counter;
	}

	// Every vertex is associated to the edges it belongs to
	for(i=0;i<m->numVertexs;i++){
		counter=0;
		for(j=0;j<m->numEdges;j++){
			if (m->edges[j].vertexs[0]==i ||
				m->edges[j].vertexs[1]==i) counter++;
		}
		m->vertexs[i].edges=(int*)malloc(counter*sizeof(int));
		counter=0;
		for(j=0;j<m->numEdges;j++){
			if (m->edges[j].vertexs[0]==i ||
				m->edges[j].vertexs[1]==i){
				m->vertexs[i].edges[counter]=j;
				counter++;
			}
		}
		m->vertexs[i].edgeCounter=counter;
	}
	fclose(f);
}

/****************************************/
/* The mesh deallocator                 */
/*                                      */
/* Free's all the memory when exitting  */
/****************************************/
void deallocateMesh(mesh *m){
	int i;

	for(i=0;i<m->numVertexs;i++){
		free(m->vertexs[i].edges);
		free(m->vertexs[i].triangles);
	}

	for(i=0;i<m->numEdges;i++)
		free(m->edges[i].triangles);
	
	free(m->vertexs);
	free(m->edges);
	free(m->triangles);
}