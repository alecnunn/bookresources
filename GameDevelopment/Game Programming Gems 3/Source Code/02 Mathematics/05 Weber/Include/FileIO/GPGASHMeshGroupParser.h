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

#ifndef __GPGASHMeshGroupParser_h__
#define __GPGASHMeshGroupParser_h__

#include "Base/GPGDataTypes.h"
#include "GPGHParser.h"

#include "Base/GPGMeshFactory.h"
#include "FileIO/GPGFileHeaderUtil.h"
#include "FileIO/GPGProgressObserver.h"

/******************************************************************************
// Geometry parser class -- derives from ASH parser GPGParser
******************************************************************************/
class GPGASHMeshGroupParser: protected GPGParser {
public:
			GPGASHMeshGroupParser(void);
virtual		~GPGASHMeshGroupParser(void)	{};

	// Expects as inputs a filename with extension and a mesh factory.
	// The rest are (null) pointers which will be initialized by Readfile.
	// Deletion left to caller.
	GPGRESULT ReadFile(U8 *pFilename, GPGMeshFactory *pFactory,
		GPGMeshGroup **ppMeshGrp,
		GPGMaterial **ppMaterial, U32 *pNumMaterials,
		GPGTexture ***pppTexture, U32 *pNumTextures,
		GPGProgressObserver *pObserver = 0);

	// Inputs: filename with extension, meshgroup pointer, material array, material count
	// optionally:  unique ID, binary mode
	GPGRESULT WriteFile(U8 *pFilename, GPGMeshGroup *pMeshGrp,
		GPGMaterial *pMaterial, U32 numMaterials,
		U32 MeshUniqueID = 0, BOOL bBinaryMode = TRUE,
		GPGProgressObserver *pObserver = 0);

	GPGRESULT ReadFileHeader(U8 *pFilename, GPGFileHeaderUtil **ppGPGFileHeaderUtil);

private:
	GPGASH_ParseFormat	m_ash;
	GPGMeshFactory      *m_pMeshFactory;
	U32                 m_numMaterials;
	GPGMaterial         **m_ppMaterials;
	U32                 m_numTextures;
	GPGTexture          **m_ppTextures; // a pointer to an array of pointers
	GPGRESULT           m_status;
	GPGMesh             *m_pMesh;
	GPGProgressObserver *m_pProgressObserver;

	void OutputHeaderBlock(void);
	//===============================================================
	void TransferColor(GPGParseEntry &entry);
	void TransferFaces(GPGParseEntry &entry);
	void TransferMaterials(GPGParseEntry &entry);
	void TransferMesh(GPGParseEntry &entry);
	void TransferMeshAttribs(GPGParseEntry &entry);
	void TransferMeshGroup(GPGParseEntry &entry);
	void TransferTexCoord(GPGParseEntry &entry);
	void TransferTexture(GPGParseEntry &entry); // build the meshgroup texture array
	void TransferVector2Array(GPGParseEntry &entry);
	void TransferVector3Array(GPGParseEntry &entry);
	//===============================================================
	void TransferGeometryData(GPGMeshGroup *pmeshgroup);

	GPGRESULT BuildTextureArray(GPGTexture ***pppTexture);

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	int  m_fragmentTotal;
	int  m_fragmentCount;
	enum CompletionProgress {FACES, VERTICES, TEXCOORDS};

	void InitProgress(GPGMeshGroup *pMeshgroup);
	int  CalcProgress(GPGMesh *pMesh, CompletionProgress cprogress);
	void ReportProgress(GPGMesh *pMesh, CompletionProgress cprogress);
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
};

inline void GPGASHMeshGroupParser::ReportProgress(GPGMesh *pMesh, CompletionProgress cprogress) {
	if ((m_pProgressObserver) &&
		(GPG_OK != m_pProgressObserver->UpdateProgress(m_fragmentTotal,CalcProgress(pMesh,cprogress))))
	Abort();
}

#endif /* __GPGASHMeshGroupParser_h__ */
