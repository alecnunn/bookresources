/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Triangle Strip Creation, Optimizations, and Rendering Gem

Copyright (c) 2001 - 2002, Intel Corporation.
All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer. 
* Redistributions in binary form must reproduce the above copyright notice, this 
list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution. 
* Neither the name of Intel Corporation nor the names of its contributors may be 
used to endorse or promote products derived from this software without specific 
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS'' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR 
JURISDICTION. It is each licensee's responsibility to comply with any export 
regulations applicable in each licensee's jurisdiction. 
*/

#ifndef SURF_OF_REVOLUTION_H
#define SURF_OF_REVOLUTION_H

#include <stdlib.h>

//a structure of points to be rotated about the y-axis
	struct Vert { float x;float y;float z; };
	
#define PI 3.1415926535897932384626433832795

class SurfOfRevolution
{
public:
	SurfOfRevolution();
	~SurfOfRevolution();
		
	//note: must make vertex array before face array
	void MakeVertexArray();
	void MakeFaceArray();

	int GetNumPoints();
	int GetNumSteps();
	int *GetFaceArray();
	Vert *GetVertexArray();

private:
	struct Vert *VA;
	int *FA;
	int numPoints;
	int numSteps;
};

inline SurfOfRevolution::SurfOfRevolution()
{
	numPoints = 10;
	numSteps = 30;
	
	VA = (struct Vert *)malloc(sizeof(struct Vert)*numPoints*numSteps);
VA[0*numSteps].x = 0.8f;
	VA[0*numSteps].y = -2.0f;
	VA[0*numSteps].z = 0.0f;

VA[1*numSteps].x = 0.3f;
	VA[1*numSteps].y = -1.5f;
	VA[1*numSteps].z = 0.0f;

VA[2*numSteps].x = 0.2f;
	VA[2*numSteps].y = -1.0f;
	VA[2*numSteps].z = 0.0f;

VA[3*numSteps].x = 0.1f;
	VA[3*numSteps].y = -0.75f;
	VA[3*numSteps].z = 0.0f;

VA[4*numSteps].x = 0.2f;
	VA[4*numSteps].y = -0.50f;
	VA[4*numSteps].z = 0.0f;

VA[5*numSteps].x = 0.3f;
	VA[5*numSteps].y = 0.0f;
	VA[5*numSteps].z = 0.0f;

VA[6*numSteps].x = 0.8f;
	VA[6*numSteps].y = 0.5f;
	VA[6*numSteps].z = 0.0f;

VA[7*numSteps].x = 0.6f;
	VA[7*numSteps].y = 1.0f;
	VA[7*numSteps].z = 0.0f;

VA[8*numSteps].x = 0.6f;
	VA[8*numSteps].y = 1.5f;
	VA[8*numSteps].z = 0.0f;

VA[9*numSteps].x = 0.6f;
	VA[9*numSteps].y = 2.0f;
	VA[9*numSteps].z = 0.0f;
	
/*
	VA = (struct Vert *)malloc(sizeof(struct Vert)*numPoints*numSteps);
	VA[0*numSteps].x = .5;
	VA[0*numSteps].y = -2;
	VA[0*numSteps].z = 0;
	VA[1*numSteps].x = 1;
	VA[1*numSteps].y = -1;
	VA[1*numSteps].z = 0;
	VA[2*numSteps].x = 1;
	VA[2*numSteps].y = 0;
	VA[2*numSteps].z = 0;
	VA[3*numSteps].x = .5;
	VA[3*numSteps].y = 1;
	VA[3*numSteps].z = 0;
*/
}

inline void SurfOfRevolution::MakeVertexArray()
{
	int i,j;

	for(i=0;i<numPoints;i++)
	{
		for(j=0;j<numSteps-1;j++)
		{
			
			float degrees = (float)((360/numSteps)*(j+1));
			float rads = (float)(degrees*(PI/180));

			VA[i*numSteps+j+1].x = (VA[i*numSteps].x * (float)cos(rads)) - (VA[i*numSteps].z * (float)sin(rads));
			VA[i*numSteps+j+1].y =  VA[i*numSteps].y;
			VA[i*numSteps+j+1].z = (VA[i*numSteps].x * (float)sin(rads)) + (VA[i*numSteps].z * (float)cos(rads));

		}
	}
}

inline void SurfOfRevolution::MakeFaceArray()
{
	
	int nRows=numPoints;
	int nCols=numSteps;

	//int *FA;

	FA = (int *)malloc(sizeof(int)*(nRows*nCols*6));

	//note: two extra indices for the triangles that wrap around the edges
	//they are filled in after the inner loop is done
	int i,j;

	for(i=0;i<nRows-1;i++)
	{
		for(j=0;j<nCols-1;j++)
		{
			FA[i*nCols*6+j*6+0] = i*nCols+(j);
			FA[i*nCols*6+j*6+1] = i*nCols+(j)+1;
			FA[i*nCols*6+j*6+2] = (i+1)*nCols+j;
			FA[i*nCols*6+j*6+3] = i*nCols+(j)+1;
			FA[i*nCols*6+j*6+4] = (i+1)*nCols+(j)+1;
			FA[i*nCols*6+j*6+5] = (i+1)*nCols+(j);	
		}
		//j is now 1 larger, this gets the wrap complete
		FA[i*nCols*6+j*6+0] = i*nCols+(j);
		FA[i*nCols*6+j*6+1] = i*nCols;
		FA[i*nCols*6+j*6+2] = (i+1)*nCols+j;
		FA[i*nCols*6+j*6+3] = i*nCols;
		FA[i*nCols*6+j*6+4] = (i+1)*nCols;
		FA[i*nCols*6+j*6+5] = (i+1)*nCols+(j);
	}

}

inline int SurfOfRevolution::GetNumPoints()
{
	return numPoints;
}

inline int SurfOfRevolution::GetNumSteps()
{
	return numSteps;
}

inline Vert *SurfOfRevolution::GetVertexArray()
{
	return VA;
}

inline int *SurfOfRevolution::GetFaceArray()
{
	return FA;
}


inline SurfOfRevolution::~SurfOfRevolution()
{
	free(VA);
	free(FA);

}


#endif //SURF_OF_REVOLUTION_H