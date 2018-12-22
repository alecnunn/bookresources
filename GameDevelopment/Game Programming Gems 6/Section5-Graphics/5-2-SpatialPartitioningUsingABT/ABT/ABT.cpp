#include "ABT.h"



/* ***************************************************************** *\
|  CONSTRUCTION / DESTRUCTION
\* ***************************************************************** */
cABT::cABT() :
	m_Root(NULL),
	m_NumRenderedTris(0),
	m_NumFinalTris(0)
{
}


cABT::~cABT()
{
	release();
}


/* ***************************************************************** *\
|  create
|  *****************************************************************
|  Creates the ABT tree by calling the recursive r_create method.
\* ***************************************************************** */
bool cABT::create()
{
	// check if VBO extension is supported ...
	if(!m_OGLExt.isExtensionSupported("GL_ARB_vertex_buffer_object"))
	{
		printf("GL_ARB_vertex_buffer_object not supported!!\n\n");
		return false;
	}

	// call init to be sure the extensions are loaded
	m_OGLExt.init();

	// load our ply files
	cDataLoader loader;
	if(!loader.readPlyFiles())
		return false;

	// get the loaded data
	vector<Vertex> *vertexData = loader.getVertexData();
	uint32 numLoadedTris = vertexData->size() / 3;
	
	// reset final tris counter
	m_NumFinalTris = 0;

	// create the tree
	m_Root = r_create((*vertexData));

	printf("ABT created from %d triangles (contains now %d triangles)\n", numLoadedTris, m_NumFinalTris);

	return true;
}


/* ***************************************************************** *\
|  render
|  *****************************************************************
|  Renders the ABT tree by calling the recursive r_render method.
\* ***************************************************************** */
uint32 cABT::render()
{
	// update frustum
	updateFrustum();

	// reset rendered tris counter
	m_NumRenderedTris = 0;

	// enable pointers
	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// render the tree
	r_render(m_Root);

	// disable pointers
	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	return m_NumRenderedTris;
}


/* ***************************************************************** *\
|  release
|  *****************************************************************
|  Releases the ABT tree by calling the recursive r_release method.
\* ***************************************************************** */
void cABT::release()
{
	r_release(m_Root);
	m_Root = NULL;
}


/* ***************************************************************** *\
|  r_create
|  *****************************************************************
|  Recursive ABT creation method.
\* ***************************************************************** */
ABTNode* cABT::r_create(vector<Vertex> &p_Vertex)
{
	// step1: determine the aabb of the current geometry set
	cAABoundingBox aabb;
	getAABB(p_Vertex, &aabb);

	// create a new node and assign the aabb
	ABTNode *pNewNode = new ABTNode;
	pNewNode->aabb = aabb;

	// step2: check if we reached the triangle threshold
	if(p_Vertex.size() > (ABT_MAX_NODEPRIMITIVES * 3))
	{
		// we don't have a leaf ...
		pNewNode->bIsLeaf = false;

		// find a new splitter for this node ...
		cPlane splitPlane;
		getSplitter(p_Vertex, &aabb, &splitPlane);

		// we define two planes that represent the maximum extend of a 
		// grown child bounding box
		cPlane splitPlaneGrow1, splitPlaneGrow2;

		splitPlaneGrow1 = splitPlane;
		splitPlaneGrow2 = splitPlane;

		// determine the axis perpendicular to our plane 
		uint32 iCurAxis = 0;
		for(uint32 i = 0; i < 3; i++)
		{
			if(splitPlane.n[i] == 1.0f)
				iCurAxis = i;
		}

		// calculate the grow planes
		float32 childAABBLen = (aabb.getSplitPercent(splitPlaneGrow1) * aabb.getAxisLen(iCurAxis));
		splitPlaneGrow1.p[iCurAxis] +=  childAABBLen * ABT_GROWTH_FACTOR;
		splitPlaneGrow2.p[iCurAxis] -= (aabb.getAxisLen(iCurAxis) - childAABBLen) * ABT_GROWTH_FACTOR;

		// split the node's geometry set
		vector<Vertex> childAVertex;
		vector<Vertex> childBVertex;

		// set the vectors' size to 3/4 of the original vector's size ...
		childAVertex.reserve(p_Vertex.size() * 3 / 4);
		childBVertex.reserve(p_Vertex.size() * 3 / 4);

		uint32 iNumSplits = 0;
		for(uint32 i = 0; i < p_Vertex.size(); i+=3)
		{
			// determine the current surface's location
			uint32 res = getTriangleSide(&splitPlane, &(p_Vertex[i]));

			// is a split required?
			if(res == ABT_SPANNING)
			{
				// check if we don't have to split if we grow the children's AABB
				res = getTriangleSide(&splitPlaneGrow1, &(p_Vertex[i]));
				if(res == ABT_SPANNING)
				{
					res = getTriangleSide(&splitPlaneGrow2, &(p_Vertex[i]));
					if(res == ABT_SPANNING)
					{
						// split the triangle
						vector<Vertex> frontFaces;
						vector<Vertex> backFaces;
						splitTriangle(&splitPlane, &(p_Vertex[i]), frontFaces, backFaces);

						if(frontFaces.size() % 3 || backFaces.size() % 3)
							DebugBreak();

						// add the new triangles to the list
						for(uint32 j = 0; j < frontFaces.size(); j++)
							childAVertex.push_back(frontFaces[j]);
						for(uint32 j = 0; j < backFaces.size(); j++)
							childBVertex.push_back(backFaces[j]);

						iNumSplits++;
						continue;
					}
				}
			}

			if(res == ABT_FRONT)
			{
				childAVertex.push_back(p_Vertex[i]);
				childAVertex.push_back(p_Vertex[i + 1]);
				childAVertex.push_back(p_Vertex[i + 2]);
			}
			else if(res == ABT_BACK)
			{
				childBVertex.push_back(p_Vertex[i]);
				childBVertex.push_back(p_Vertex[i + 1]);
				childBVertex.push_back(p_Vertex[i + 2]);
			}
			else
			{
				// coinciding ... add the triangle to the vector with the smaller
				// amount of vertices
				if(childAVertex.size() < childBVertex.size())
				{
					childAVertex.push_back(p_Vertex[i]);
					childAVertex.push_back(p_Vertex[i + 1]);
					childAVertex.push_back(p_Vertex[i + 2]);
				}
				else
				{
					childBVertex.push_back(p_Vertex[i]);
					childBVertex.push_back(p_Vertex[i + 1]);
					childBVertex.push_back(p_Vertex[i + 2]);
				}
			}
		}
		
		// now we can release the memory from our p_Vertex vector ..
		// little hack taken from: http://www.gotw.ca/gotw/054.htm
		vector<Vertex>().swap(p_Vertex);

		printf("Total splits: %d\n", iNumSplits);

		// continue recursion
		pNewNode->pChildA = r_create(childAVertex);
		pNewNode->pChildB = r_create(childBVertex);

	}else
	{
		// we have a leaf ...
		pNewNode->bIsLeaf = true;
		pNewNode->pChildA = pNewNode->pChildB = NULL;

		// create new vertex buffer objects
		m_OGLExt.glGenBuffersARB(1, &(pNewNode->iVertexBuffer));
		m_OGLExt.glBindBufferARB(GL_ARRAY_BUFFER_ARB, pNewNode->iVertexBuffer);

		// load the vertex data in our buffer
		m_OGLExt.glBufferDataARB(GL_ARRAY_BUFFER_ARB, p_Vertex.size() * sizeof(Vertex), 
			&(p_Vertex[0]), GL_STATIC_DRAW_ARB);

		// store the primitives number
		pNewNode->iVertexCount = (uint32)p_Vertex.size();

		// we don't need the vertex data further as it is already loaded
		// into the gfx card memory ...
		vector<Vertex>().swap(p_Vertex);

		printf("Leaf with %d faces created!\n", pNewNode->iVertexCount / 3);
		m_NumFinalTris += pNewNode->iVertexCount / 3;
	}

	return pNewNode;
}


/* ***************************************************************** *\
|  r_render
|  *****************************************************************
|  Recursive method to render the ABT.
\* ***************************************************************** */
void cABT::r_render(ABTNode *p_Node)
{
	if(p_Node)
	{
		// check if the node is visible ...
		if(!isInFrustum(p_Node->aabb))
			return;	// no - reject it and all its children
		
		if(p_Node->bIsLeaf)
		{
			// setup the node's data for rendering
			m_OGLExt.glBindBufferARB(GL_ARRAY_BUFFER_ARB, p_Node->iVertexBuffer);
			glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (char*)(0));
			//glColorPointer(4, GL_BYTE, sizeof(Vertex), (char*)(6 * sizeof(float32)));
			glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)(3 * sizeof(float32)));
			
			// render
			glDrawArrays(GL_TRIANGLES, 0, p_Node->iVertexCount);

			// update counter
			m_NumRenderedTris += (p_Node->iVertexCount / 3);
		}else
		{
			// call r_render for the children
			r_render(p_Node->pChildA);
			r_render(p_Node->pChildB);
		}
	}
}


/* ***************************************************************** *\
|  r_release
|  *****************************************************************
|  Recursive method to free the allocated memory.
\* ***************************************************************** */
void cABT::r_release(ABTNode *p_Node)
{
	if(!p_Node)
		return;

	if(p_Node->bIsLeaf)
	{
		// release gl VBOs
		m_OGLExt.glDeleteBuffersARB(1, &(p_Node->iVertexBuffer));
	}
	else
	{
		// recursive call for children
		r_release(p_Node->pChildA);
		r_release(p_Node->pChildB);
	}

	// delete the node
	delete p_Node;
}


/* ***************************************************************** *\
|  getAABB
|  *****************************************************************
|  Calculates the AABB of the current geometry set
\* ***************************************************************** */
void cABT::getAABB(vector<Vertex> &p_Vertex, cAABoundingBox *p_AABB)
{
	float32 maxx, maxy, maxz;
	float32 minx, miny, minz;

	// check parameters
	if(!p_AABB)
	{
		printf("Invalid parameters passed to getAABB\n");
		return;
	}

	// init max/min values
	maxx = minx = p_Vertex[0].xyz[0];
	maxy = miny = p_Vertex[0].xyz[1];
	maxz = minz = p_Vertex[0].xyz[2];

	// determine size of the AABB
	vector<Vertex>::iterator i = p_Vertex.begin();
	for(i++; i != p_Vertex.end(); i++)
	{
		maxx = max(maxx, (*i).xyz[0]);
		maxy = max(maxy, (*i).xyz[1]);
		maxz = max(maxz, (*i).xyz[2]);

		minx = min(minx, (*i).xyz[0]);
		miny = min(miny, (*i).xyz[1]);
		minz = min(minz, (*i).xyz[2]);
	}

	// fill in aabb
	p_AABB->maxPt = cVector(maxx, maxy, maxz);
	p_AABB->minPt = cVector(minx, miny, minz);
	p_AABB->centerPt = cVector((minx + maxx) / 2.0f, (miny + maxy) / 2.0f, (minz + maxz) / 2.0f);
}


/* ***************************************************************** *\
|  getSplitter
|  *****************************************************************
|  Finds the best splitter for the given geometry set using Andy
|  Campbell's statistically weighted multi sampling.
\* ***************************************************************** */
void cABT::getSplitter(vector<Vertex> &p_Vertex, cAABoundingBox *p_AABB, cPlane *p_Plane)
{
	// init vars
	float32 fBestSplitterScore = 100000000.0f;
	float32 fLargestAxis;

	// determine largest axis
	fLargestAxis = max(max(p_AABB->getAxisLen(0), p_AABB->getAxisLen(1)), p_AABB->getAxisLen(2));

	// we scan each axis for a plane
	for(uint32 iCurAxis = 0; iCurAxis < 3; iCurAxis++)
	{
		// init a new plane object
		cPlane testPlane(cVector(0.0f, 0.0f, 0.0f), cVector(0.0f, 0.0f, 0.0f));

		// init the plane's normal
		testPlane.n[iCurAxis] = 1.0f;

		// calculate the mean (only for the coordinates along our scanning axis)
		float64 fMeanSum = 0.0;
		for(uint32 i = 0; i < p_Vertex.size(); i++)
		{
			fMeanSum += p_Vertex[i].xyz[iCurAxis];
		}
		float32 fMean = (float32)(fMeanSum / (float64)p_Vertex.size());

		// calculate the standard deviation
		float64 fStdDevSum = 0.0f;
		for(uint32 i = 0; i < p_Vertex.size(); i++)
		{
			fStdDevSum += pow(fMean - p_Vertex[i].xyz[iCurAxis], 2);
		}
		float32 fStdDev = (float32)(fStdDevSum / (float64)(p_Vertex.size() - 1));
		fStdDev = sqrt(fStdDev);

		// we sample x samples doing uniform distance sampling within 
		// one standard deviation on each side of the mean. Also - we 
		// check boundaries if the standard deviation is too big and 
		// goes over our bounding box.
		testPlane.p[iCurAxis] = max(fMean - fStdDev, p_AABB->minPt[iCurAxis]);
		float32 fDist = abs(min(fMean + fStdDev, p_AABB->maxPt[iCurAxis]) 
								- testPlane.p[iCurAxis]) / (float32)(ABT_PLANE_SAMPLES * 2);

		for(uint32 i = 0; i < (ABT_PLANE_SAMPLES * 2 - 1); i++)
		{
			// update plane point
			testPlane.p[iCurAxis] += fDist;
			
			// determine the number of faces in front of and back of the plane
			// as well as the number of split faces
			uint32 iNumFrontFaces	= 0;
			uint32 iNumBackFaces	= 0;
			uint32 iNumSplitFaces	= 0;

			// determine each face's location
			bool bInsertFront = true;
			for(vector<Vertex>::iterator j = p_Vertex.begin(); j!= p_Vertex.end(); j += 3)
			{
				// determine the current surface's location
				uint32 res = getTriangleSide(&testPlane, &(*j));
				if(res == ABT_FRONT)
					iNumFrontFaces++;
				else if(res == ABT_BACK)
					iNumBackFaces++;
				else if(res == ABT_SPANNING)
					iNumSplitFaces++;
				else
				{
					// coinciding ... alternate between front/back
					if(bInsertFront)
						iNumFrontFaces++;
					else
						iNumBackFaces++;
					bInsertFront = !bInsertFront;
				}

				// calculate the plane's score ...
				float32 f1 = 1.0f - (p_AABB->getAxisLen(iCurAxis) / fLargestAxis);
				float32 f2 = abs(0.5f - p_AABB->getSplitPercent(testPlane)) * 2.0f;
				float32 f3 = (float32)abs((float)(iNumFrontFaces - iNumBackFaces)) / 
								(float32)(iNumFrontFaces + iNumBackFaces);
				float32 f4 = (float32)iNumSplitFaces / (float32)(iNumFrontFaces + iNumBackFaces);

				float32 fResult = f1 * ABT_WEIGHT_SPACELOCATION + 
								  f2 * ABT_WEIGHT_CHILDVOLUMES  +
								  f3 * ABT_WEIGHT_FACECOUNT	 +
								  f4 * ABT_WEIGHT_SPLITFACES;

				// check if we have a new "best" plane ...
				if(fResult < fBestSplitterScore)
				{
					(*p_Plane) = testPlane;
					fBestSplitterScore = fResult;
				}
			}
		}
	}
}


/* ***************************************************************** *\
|  splitTriangle
|  *****************************************************************
|  Splits a triangle, using the plane p_Plane and returns a list of
|  vertices for the new front and back triangles.
\* ***************************************************************** */
void cABT::splitTriangle(cPlane *p_Plane, Vertex *p_Vertex, std::vector<Vertex> &p_Front, std::vector<Vertex> &p_Back)
{
	uint32 resA, resB;
	Vertex vA, vB;

	// initialize
	vA = p_Vertex[2];
	resA = classifyPoint(p_Plane, &vA);

	vector<Vertex> vecFront;
	vector<Vertex> vecBack;

	// check each edge of the triangle
	for(uint32 i = 0; i < 3; i++)	
	{
		// get the next point from our triangle
		vB = p_Vertex[i];
		resB = classifyPoint(p_Plane, &vB);

		if(resB == ABT_FRONT)
		{
			if(resA == ABT_BACK)
			{
				// compute intersection
				cVector vecIntersection;
				cVector vecA(vA);
				cVector vecB(vB);
				p_Plane->getPlaneLineIntersection(vecA, vecB, vecIntersection);

				Vertex newVert;
				newVert.xyz[0] = vecIntersection.x;
				newVert.xyz[1] = vecIntersection.y;
				newVert.xyz[2] = vecIntersection.z;

				// calc the interpolation factor
				float fFactor = cVector(vecA - cVector(newVert)).length() / cVector(vecA - vecB).length();			

				// interpolate the color
/*				newVert.rgba[0] = vA.rgba[0] + (byte)((vB.rgba[0] - vA.rgba[0]) * fFactor);
				newVert.rgba[1] = vA.rgba[1] + (byte)((vB.rgba[1] - vA.rgba[1]) * fFactor);
				newVert.rgba[2] = vA.rgba[2] + (byte)((vB.rgba[2] - vA.rgba[2]) * fFactor);
				newVert.rgba[3] = vA.rgba[3] + (byte)((vB.rgba[3] - vA.rgba[3]) * fFactor);
*/
				newVert.nxyz[0] = vA.nxyz[0] + (vB.nxyz[0] - vA.nxyz[0]) * fFactor;
				newVert.nxyz[1] = vA.nxyz[1] + (vB.nxyz[1] - vA.nxyz[1]) * fFactor;
				newVert.nxyz[2] = vA.nxyz[2] + (vB.nxyz[2] - vA.nxyz[2]) * fFactor;

				// add new vertex to both lists
				vecFront.push_back(newVert);
				vecBack.push_back(newVert);
			}

			vecFront.push_back(vB);
		
		}else if(resB == ABT_BACK)
		{
			if(resA == ABT_FRONT)
			{
				// compute intersection
				cVector vecIntersection;
				cVector vecA(vA);
				cVector vecB(vB);
				p_Plane->getPlaneLineIntersection(vecA, vecB, vecIntersection);

				Vertex newVert;
				newVert.xyz[0] = vecIntersection.x;
				newVert.xyz[1] = vecIntersection.y;
				newVert.xyz[2] = vecIntersection.z;

				// calc the interpolation factor
				float fFactor = cVector(vecA - cVector(newVert)).length() / cVector(vecA - vecB).length();			

/*				// interpolate the color
				newVert.rgba[0] = vA.rgba[0] + (byte)((vB.rgba[0] - vA.rgba[0]) * fFactor);
				newVert.rgba[1] = vA.rgba[1] + (byte)((vB.rgba[1] - vA.rgba[1]) * fFactor);
				newVert.rgba[2] = vA.rgba[2] + (byte)((vB.rgba[2] - vA.rgba[2]) * fFactor);
				newVert.rgba[3] = vA.rgba[3] + (byte)((vB.rgba[3] - vA.rgba[3]) * fFactor);
*/
				newVert.nxyz[0] = vA.nxyz[0] + (vB.nxyz[0] - vA.nxyz[0]) * fFactor;
				newVert.nxyz[1] = vA.nxyz[1] + (vB.nxyz[1] - vA.nxyz[1]) * fFactor;
				newVert.nxyz[2] = vA.nxyz[2] + (vB.nxyz[2] - vA.nxyz[2]) * fFactor;

				// add new vertex to both lists
				vecFront.push_back(newVert);
				vecBack.push_back(newVert);
			}
			
			vecBack.push_back(vB);
		
		}else
		{
			vecFront.push_back(vB);
			vecBack.push_back(vB);
		}

		// let's check the next edge
		vA = vB;
		resA = resB;
	}

	// make a triangle list out of the vertices in the vectors	
	for(uint32 i = 1; i < (vecFront.size() - 1); i++)
	{
		p_Front.push_back(vecFront[0]);
		p_Front.push_back(vecFront[i]);
		p_Front.push_back(vecFront[i + 1]);
	}

	for(uint32 i = 1; i < (vecBack.size() - 1); i++)
	{
		p_Back.push_back(vecBack[0]);
		p_Back.push_back(vecBack[i]);
		p_Back.push_back(vecBack[i + 1]);
	}
}


/* ***************************************************************** *\
|  getTriangleSide
|  *****************************************************************
|  Returns if the triangle pointed to by p_Vertex is in front, behind 
|  coinciding or spanning through the plane. Return values as defined in ABT.h
|  Implementation details at http://www.devmaster.net/articles/bsp-trees/
\* ***************************************************************** */
uint32 cABT::getTriangleSide(cPlane *p_Plane, Vertex *p_Vertex)
{
	uint32 numPositive = 0;
	uint32 numNegative = 0;

	// check each point in poly
	for(uint32 i = 0; i < 3; i++)
	{
		uint32 res = classifyPoint(p_Plane, &(p_Vertex[i]));
		if(res == ABT_FRONT)
			numPositive++;
		else if(res == ABT_BACK)
			numNegative++;
	}

	// only positive -> in front
	if(numPositive > 0 && numNegative == 0)
		return ABT_FRONT;
	// only negative -> back
	if(numPositive == 0 && numNegative > 0)
		return ABT_BACK;
	// both 0 -> coinciding
	if(numPositive == 0 && numNegative == 0)
		return ABT_COINCIDING;

	// if we'r here the only option left is spanning
	return ABT_SPANNING;
}


/* ***************************************************************** *\
|  classifyPoint
|  *****************************************************************
|  Classifies the point. Determines if the point is in front, back,
|  or coinciding with poly.
|  Return values as defined in ABT.h
|  Implementation details at http://www.devmaster.net/articles/bsp-trees/
\* ***************************************************************** */
uint32 cABT::classifyPoint(cPlane *p_Plane, Vertex *p_Vertex)
{
	// calc the direction vector from v1 to p
	cVector dir = cVector((*p_Vertex)) - p_Plane->p;

	// get the dot product of dir and polyNormal
	float32 res = dir * p_Plane->n;

	// positive result -> in front
	if(res > ABT_CLASSIFYPOINTDISTANCE_NULL)
		return ABT_FRONT;
	// negative result -> back
	if(res < -ABT_CLASSIFYPOINTDISTANCE_NULL)
		return ABT_BACK;

	// must be coinciding
	return ABT_COINCIDING;
}

/* ***************************************************************** *\
|  updateFrustum
|  *****************************************************************
|  Updates the viewing frustum
\* ***************************************************************** */

void MatrixMultiply(float32 *p_M1, float32 *p_M2, float32 *p_Out)
{
	p_Out[0] = p_M1[0] * p_M2[0] + p_M1[1] * p_M2[4] + p_M1[2] * p_M2[8] + p_M1[3] * p_M2[12];
	p_Out[1] = p_M1[0] * p_M2[1] + p_M1[1] * p_M2[5] + p_M1[2] * p_M2[9] + p_M1[3] * p_M2[13];
	p_Out[2] = p_M1[0] * p_M2[2] + p_M1[1] * p_M2[6] + p_M1[2] * p_M2[10] + p_M1[3] * p_M2[14];
	p_Out[3] = p_M1[0] * p_M2[3] + p_M1[1] * p_M2[7] + p_M1[2] * p_M2[11] + p_M1[3] * p_M2[15];

	p_Out[4] = p_M1[4] * p_M2[0] + p_M1[5] * p_M2[4] + p_M1[6] * p_M2[8] + p_M1[7] * p_M2[12];
	p_Out[5] = p_M1[4] * p_M2[1] + p_M1[5] * p_M2[5] + p_M1[6] * p_M2[9] + p_M1[7] * p_M2[13];
	p_Out[6] = p_M1[4] * p_M2[2] + p_M1[5] * p_M2[6] + p_M1[6] * p_M2[10] + p_M1[7] * p_M2[14];
	p_Out[7] = p_M1[4] * p_M2[3] + p_M1[5] * p_M2[7] + p_M1[6] * p_M2[11] + p_M1[7] * p_M2[15];

	p_Out[8] = p_M1[8] * p_M2[0] + p_M1[9] * p_M2[4] + p_M1[10] * p_M2[8] + p_M1[11] * p_M2[12];
	p_Out[9] = p_M1[8] * p_M2[1] + p_M1[9] * p_M2[5] + p_M1[10] * p_M2[9] + p_M1[11] * p_M2[13];
	p_Out[10] = p_M1[8] * p_M2[2] + p_M1[9] * p_M2[6] + p_M1[10] * p_M2[10] + p_M1[11] * p_M2[14];
	p_Out[11] = p_M1[8] * p_M2[3] + p_M1[9] * p_M2[7] + p_M1[10] * p_M2[11] + p_M1[11] * p_M2[15];

	p_Out[12] = p_M1[12] * p_M2[0] + p_M1[13] * p_M2[4] + p_M1[14] * p_M2[8] + p_M1[15] * p_M2[12];
	p_Out[13] = p_M1[12] * p_M2[1] + p_M1[13] * p_M2[5] + p_M1[14] * p_M2[9] + p_M1[15] * p_M2[13];
	p_Out[14] = p_M1[12] * p_M2[2] + p_M1[13] * p_M2[6] + p_M1[14] * p_M2[10] + p_M1[15] * p_M2[14];
	p_Out[15] = p_M1[12] * p_M2[3] + p_M1[13] * p_M2[7] + p_M1[14] * p_M2[11] + p_M1[15] * p_M2[15];
}


void cABT::updateFrustum()
{
	float32   proj[16];
	float32   modview[16];
	float32   clip[16];

	// get projection matrix
	glGetFloatv(GL_PROJECTION_MATRIX, proj);

	// get modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, modview);

	MatrixMultiply(modview, proj, clip);
	
	// build the frustum
	m_Frustum[0][0] = clip[ 3] - clip[ 0];
	m_Frustum[0][1] = clip[ 7] - clip[ 4];
	m_Frustum[0][2] = clip[11] - clip[ 8];
	m_Frustum[0][3] = clip[15] - clip[12];

	m_Frustum[1][0] = clip[ 3] + clip[ 0];
	m_Frustum[1][1] = clip[ 7] + clip[ 4];
	m_Frustum[1][2] = clip[11] + clip[ 8];
	m_Frustum[1][3] = clip[15] + clip[12];

	m_Frustum[2][0] = clip[ 3] + clip[ 1];
	m_Frustum[2][1] = clip[ 7] + clip[ 5];
	m_Frustum[2][2] = clip[11] + clip[ 9];
	m_Frustum[2][3] = clip[15] + clip[13];

	m_Frustum[3][0] = clip[ 3] - clip[ 1];
	m_Frustum[3][1] = clip[ 7] - clip[ 5];
	m_Frustum[3][2] = clip[11] - clip[ 9];
	m_Frustum[3][3] = clip[15] - clip[13];

	m_Frustum[4][0] = clip[ 3] - clip[ 2];
	m_Frustum[4][1] = clip[ 7] - clip[ 6];
	m_Frustum[4][2] = clip[11] - clip[10];
	m_Frustum[4][3] = clip[15] - clip[14];

	m_Frustum[5][0] = clip[ 3] + clip[ 2];
	m_Frustum[5][1] = clip[ 7] + clip[ 6];
	m_Frustum[5][2] = clip[11] + clip[10];
	m_Frustum[5][3] = clip[15] + clip[14];
}


/* ***************************************************************** *\
|  isInFrustum
|  *****************************************************************
|  Checks if the given axis-aligned bounding box is inside the current
|  viewing frustum.
|  Code taken from DigiBen's OcTree tutorial
|  http://www.gametutorials.com/download/OpenGL/Octree2_OGL.zip
\* ***************************************************************** */
bool cABT::isInFrustum(cAABoundingBox &p_AABB)
{
	// Basically, what is going on is, that we are given the center of the cube,
	// and half the length.  Think of it like a radius.  Then we checking each point
	// in the cube and seeing if it is inside the frustum.  If a point is found in NEAR
	// of a side, then we skip to the next side.  If we get to a plane that does NOT have
	// a point in NEAR of it, then it will return false.

	// *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
	// This happens when all the corners of the bounding box are not behind any one plane.
	// This is rare and shouldn't effect the overall rendering speed.
	for(uint32 i = 0; i < 6; i++ )
	{
		if(m_Frustum[i][0] * (p_AABB.minPt.x) + m_Frustum[i][1] * (p_AABB.minPt.y) + m_Frustum[i][2] * (p_AABB.minPt.z) + m_Frustum[i][3] > 0)
			continue;
		if(m_Frustum[i][0] * (p_AABB.maxPt.x) + m_Frustum[i][1] * (p_AABB.minPt.y) + m_Frustum[i][2] * (p_AABB.minPt.z) + m_Frustum[i][3] > 0)
			continue;
		if(m_Frustum[i][0] * (p_AABB.minPt.x) + m_Frustum[i][1] * (p_AABB.maxPt.y) + m_Frustum[i][2] * (p_AABB.minPt.z) + m_Frustum[i][3] > 0)
			continue;
		if(m_Frustum[i][0] * (p_AABB.maxPt.x) + m_Frustum[i][1] * (p_AABB.maxPt.y) + m_Frustum[i][2] * (p_AABB.minPt.z) + m_Frustum[i][3] > 0)
			continue;
		if(m_Frustum[i][0] * (p_AABB.minPt.x) + m_Frustum[i][1] * (p_AABB.minPt.y) + m_Frustum[i][2] * (p_AABB.maxPt.z) + m_Frustum[i][3] > 0)
			continue;
		if(m_Frustum[i][0] * (p_AABB.maxPt.x) + m_Frustum[i][1] * (p_AABB.minPt.y) + m_Frustum[i][2] * (p_AABB.maxPt.z) + m_Frustum[i][3] > 0)
			continue;
		if(m_Frustum[i][0] * (p_AABB.minPt.x) + m_Frustum[i][1] * (p_AABB.maxPt.y) + m_Frustum[i][2] * (p_AABB.maxPt.z) + m_Frustum[i][3] > 0)
			continue;
		if(m_Frustum[i][0] * (p_AABB.maxPt.x) + m_Frustum[i][1] * (p_AABB.maxPt.y) + m_Frustum[i][2] * (p_AABB.maxPt.z) + m_Frustum[i][3] > 0)
			continue;

		// If we get here, it isn't in the frustum
		return false;
	}
	return true; 
}