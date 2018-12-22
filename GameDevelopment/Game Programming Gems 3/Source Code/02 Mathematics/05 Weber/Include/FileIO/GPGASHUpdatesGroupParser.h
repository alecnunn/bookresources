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
#ifndef __GPGASHUpdatesGroupParser_H
#define __GPGASHUpdatesGroupParser_H

#include "GPGHParser.h"
#include "FileIO/GPGFileHeaderUtil.h"
#include "FileIO/GPGProgressObserver.h"
class GPGUpdatesGroup;
class GPGUpdates;

class GPGASHUpdatesGroupParser:protected GPGParser {
public:
			GPGASHUpdatesGroupParser(void);
virtual		~GPGASHUpdatesGroupParser(void)		{};

	// Expects as an input the filename with extension.
	// The rest are (null) pointers which will be initialized by Readfile.
	// Deletion left to caller.
	GPGRESULT ReadFile(U8 *pFilename, GPGUpdatesGroup **ppUpdateGrp,
		GPGProgressObserver *pObserver = 0);
	// Inputs:  filename with extension, update group.
	// optionally:  unique ID, binary mode
	GPGRESULT WriteFile(U8 *pFilename, GPGUpdatesGroup *pUpdateGrp,
		U32 MeshUniqueID = 0, BOOL bBinaryMode = TRUE,
		GPGProgressObserver *pObserver = 0);

	GPGRESULT ReadFileHeader(U8 *pFilename, GPGFileHeaderUtil **ppGPGFileHeaderUtil);

private:
	GPGASH_ParseFormat	m_ash;
	GPGRESULT			m_status;
	GPGUpdates          *m_pUpdates;
	GPGProgressObserver *m_pProgressObserver;

	void OutputHeaderBlock();
	//===============================================================
	void TransferAllocationData(GPGParseEntry &entry);     // 
	void TransferAllocator(GPGParseEntry &entry);      // allocate the updates group
	void TransferFaceUpdate(GPGParseEntry &entry);     // transfer sync tables
	void TransferMruGroup(GPGParseEntry &entry);       // allocate mru group
	void TransferMru(GPGParseEntry &entry);            // transfer mru data
	void TransferResChange(GPGParseEntry &entry);      // transfer sync tables
	void TransferSyncTable(GPGParseEntry &entry);      // transfer sync tables
	void TransferUpdate(GPGParseEntry &entry);         // transfer face updates
	//===============================================================
	void TransferMruData(GPGUpdatesGroup *pUpdateGrp); // root

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	int  m_fragmentTotal;
	int  m_fragmentCount;
	enum CompletionProgress {FACE, RES, SYNC};

	void InitProgress(GPGUpdatesGroup *pUpdatesgroup);
	int  CalcProgress(GPGUpdates *pUpdates, CompletionProgress cprogress);
	void ReportProgress(GPGUpdates *pUpdates, CompletionProgress cprogress);
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
};

inline void GPGASHUpdatesGroupParser::ReportProgress(GPGUpdates *pUpdates, CompletionProgress cprogress) {
	if ((m_pProgressObserver) &&
		(GPG_OK != m_pProgressObserver->UpdateProgress(m_fragmentTotal,CalcProgress(pUpdates,cprogress))))
	Abort();
}

#endif
