/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Cartoon Shading in Real-Time

Copyright (c) 2001, Intel Corporation.
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

// Cartoon.h
/* This file includes all of the main files for painting 
	a model.  The main functions are listed below:

    * These functions do not include error handling, since it doesn't add to the understanding
	   of the Painter gem. 
 */

#include "Cartoon.h"

/****************************************************************************************/
// ***  PAINTER FUNCTIONS ***
// This sections includes all of the necessary Painter functions
/****************************************************************************************/

/****************************************************************************************/
//CalculateTextureCoordinates
//
//This function calculates the texture coordinates for each frame just before submitting
//the mesh to the graphics API
/****************************************************************************************/
void Cartoon::CalculateTextureCoordinates(Mesh *pMesh)
{
	//for painter
	//loop over vertices, assign texture coordinates, static for now.
	for(U32 i=0;i<pMesh->GetNumVertices();i++)
	{
		vector2 *texCoord;
		vector3 *vertNormalInModelSpace;
		
		texCoord = pMesh->GetVertexTexCoord(i);
		vertNormalInModelSpace = pMesh->GetVertexNormal(i);

		//okay, here is the dot product calculation
		//first, get the light vector
		//second, xlate it into model space
		//we will assume the directional light vector 1,0,0 in model space 
		//for this example
		//alternatively, we could calculate a point light vector per vertex, but
		//that is pretty expensive
		vector3 *lightInModelSpace = new vector3();
		lightInModelSpace->set(10,0,0);
		lightInModelSpace->normalize();
		texCoord->x = -DotProduct(*lightInModelSpace,*vertNormalInModelSpace);
		//printf("%f\n",texCoord->x);
		if(texCoord->x < 0) 
			texCoord->x = 0;
		else if(texCoord->x > 1.0)
			texCoord->x = 1.0;
		texCoord->y = 0.0;

	}

}

/****************************************************************************************/
//BuildTexture
//
//This function builds the texture that is submitted for the Painter
//For demonstration this is made very simple.
/****************************************************************************************/
void Cartoon::BuildTexture()
{
	vector3 materialAmbient;
	vector3 materialDiffuse;

	vector3 lightAmbient;
	vector3 lightDiffuse;

	vector3 globalAmbient;

	materialAmbient.set(0.6f,0.0f,0.0f);
	materialDiffuse.set(.8f,.0f,.0f);
	lightAmbient.set(.8f,.8f,.8f);
	lightDiffuse.set(1.0f,1.0f,1.0f);
	globalAmbient.set(0.4f,0.4f,0.4f);

	//Ci = illuminated diffuse (see Gem)
	vector3 Ci;
	Ci.x = globalAmbient.x * materialAmbient.x + lightAmbient.x * materialAmbient.x + lightDiffuse.x * materialDiffuse.x;
	Ci.y = globalAmbient.y * materialAmbient.y + lightAmbient.y * materialAmbient.y + lightDiffuse.y * materialDiffuse.y;
	Ci.z = globalAmbient.z * materialAmbient.z + lightAmbient.z * materialAmbient.z + lightDiffuse.z * materialDiffuse.z;
	//Cs = shadow color (see Gem)
	vector3 Cs;
	Cs.x = globalAmbient.x * materialAmbient.x + lightAmbient.x * materialAmbient.x;
	Cs.y = globalAmbient.y * materialAmbient.y + lightAmbient.y * materialAmbient.y;
	Cs.z = globalAmbient.z * materialAmbient.z + lightAmbient.z * materialAmbient.z;

	//clamps the color values to a max of 1.0
	if(Ci.x > 1.0f) Ci.x = 1.0f;
	if(Ci.y > 1.0f) Ci.y = 1.0f;
	if(Ci.z > 1.0f) Ci.z = 1.0f;
	if(Cs.x > 1.0f) Cs.x = 1.0f;
	if(Cs.y > 1.0f) Cs.y = 1.0f;
	if(Cs.z > 1.0f) Cs.z = 1.0f;
	
	//normally you can just use a 1x2 or 2x2 texture, but some older graphics cards
	//flake out on borders if you don't give them a big enough texture, so we will
	//make it a 4x4 texture in case you are using an old card
	#define NUMROWS 4
	#define NUMCOLS 4
	GLubyte toonImage[NUMROWS][NUMCOLS][4];

	for(int i=0;i<NUMROWS;i++)
	{
		for(int j=0;j<NUMCOLS;j++)
		{
			toonImage[i][j][0] = Ci.x * 255;
			toonImage[i][j][1] = Ci.y * 255;
			toonImage[i][j][2] = Ci.z * 255;
			toonImage[i][j][3] = 255;
		}
	}
	for(i=0;i<NUMROWS;i++)
	{
		for(int j=NUMCOLS/2;j<NUMCOLS;j++)
		{
			toonImage[i][j][0] = Cs.x * 255;
			toonImage[i][j][1] = Cs.y * 255;
			toonImage[i][j][2] = Cs.z * 255;
			toonImage[i][j][3] = 255;
		}
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, NUMROWS, NUMCOLS,0,GL_RGBA, GL_UNSIGNED_BYTE, toonImage);
}

/****************************************************************************************/
//RenderCartoon
//
//This function renders the Mesh
/****************************************************************************************/

void Cartoon::RenderCartoon(Mesh *pMesh)
{
	
	static int stride = sizeof(vector3) + sizeof(vector3) + (sizeof(vector2));

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	char *bytePtr = (char*) pMesh->GetVertexPtr();

	glVertexPointer(3, GL_FLOAT, stride, bytePtr);
	glNormalPointer(GL_FLOAT, stride, bytePtr + sizeof(vector3));
	glTexCoordPointer(2, GL_FLOAT, stride, bytePtr + (sizeof(vector3)*2));
	
	glDrawElements(GL_TRIANGLES, pMesh->GetNumFaces() * 3, GL_UNSIGNED_INT, pMesh->GetFacePtr());

}
/****************************************************************************************/
// End of the Painter Functions
/****************************************************************************************/


// Cartoon Constructor
Cartoon::Cartoon()
{

}

// Cartoon Destructor
Cartoon::~Cartoon()
{
	glDeleteTextures(1,&texName);
}
