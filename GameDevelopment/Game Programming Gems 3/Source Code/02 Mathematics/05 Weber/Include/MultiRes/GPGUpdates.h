/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/
#ifndef GPGUPDATES_DOT_H
#define GPGUPDATES_DOT_H
 

// Data needed to update one face.
// one complete resolution change typically involves several face updates.
typedef struct
{
	U32 face;    // index of face to modify.
	U32 corner;  // which corner of the face to modify.
	U32 newUp;   // new vertex index to use when increasing the resolution.
	U32 newDown; // new vertex index to use when decreasing the resolution.
} GPGFaceUpdate;

// With GPGMesh a single resolution change may involve more than one vertex
class GPGResolutionChange
{
public:
	GPGResolutionChange() { deltaVerts = 0; deltaFaces = 0; numFaceUpdates = 0; };
	U32 deltaVerts;	  // number of vertices added or removed.
	U32 deltaFaces;	  // number of faces added or removed.
	U32 numFaceUpdates;	 // number of faces that need to be modified because of resolution change.
};


class GPGUpdates
{

public:
	
    GPGUpdates();
	~GPGUpdates();

	//	GPGRESULT Allocate();
	GPGRESULT Allocate(U32 rc, U32 fu);

	U32 numResChanges;
	GPGResolutionChange *pResChanges;

	U32 numFaceUpdates;
	GPGFaceUpdate *pFaceUpdates;
};


GPGINLINE GPGUpdates::GPGUpdates() 
{ 
    numResChanges = 0; 
    numFaceUpdates = 0; 
    pResChanges = NULL; 
    pFaceUpdates = NULL; 
};
	

GPGINLINE GPGUpdates::~GPGUpdates() 
{
    if (pFaceUpdates)
    {
        delete[] pFaceUpdates; 
        pFaceUpdates = NULL;
    }
    
    if (pResChanges)
    {
        delete[] pResChanges; 
        pResChanges = NULL;
    }
};

/*
GPGINLINE GPGRESULT GPGUpdates::Allocate() 
{ 
    if (pResChanges)
    {
        delete pResChanges;
        pResChanges = NULL;
    }
	pResChanges = new GPGResolutionChange[numResChanges];
    
    if (pFaceUpdates)
    {
        delete pFaceUpdates;
        pFaceUpdates = NULL;
    }
	pFaceUpdates = new GPGFaceUpdate[numFaceUpdates];
	
    if (pResChanges && pFaceUpdates) 
		return GPG_OK;
	else 
		return GPG_E_OUT_OF_MEMORY;
};
*/
	
GPGINLINE GPGRESULT GPGUpdates::Allocate(U32 rc, U32 fu) 
{ 
	GPGRESULT returnCode = GPG_OK;
    if (pResChanges)
    {
        delete [] pResChanges;
        pResChanges = NULL;
    }   
    if (pFaceUpdates)
    {
        delete [] pFaceUpdates;
        pFaceUpdates = NULL;
    }

	pResChanges     = new GPGResolutionChange[rc];
	pFaceUpdates    = new GPGFaceUpdate[fu];

	if(pResChanges && pFaceUpdates) 
    {
//		memset(pResChanges,0,rc*sizeof(GPGResolutionChange));
//		memset(pFaceUpdates,0,fu*sizeof(GPGFaceUpdate));
		numResChanges = rc;
		numFaceUpdates = fu;
	} 
	else 
	{
		if (pResChanges)
		{
			delete [] pResChanges;
			pResChanges = NULL;
		}   
		if (pFaceUpdates)
		{
			delete [] pFaceUpdates;
			pFaceUpdates = NULL;
		}
		
		returnCode = GPG_E_OUT_OF_MEMORY;
	}
	return returnCode;
};

#endif