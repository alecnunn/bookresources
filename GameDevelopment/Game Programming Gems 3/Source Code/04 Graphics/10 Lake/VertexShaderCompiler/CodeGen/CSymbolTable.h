/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Game Programming Gem  sample code

Copyright (c) 2001-2002, Intel Corporation.
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
#ifndef CSYMBOLTABLE_H
#define CSYMBOLTABLE_H

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "windows.h"

enum RegType { eRegTypeConst, 
			   eRegTypeVertex, 
			   eRegTypeTemp, 
			   eRegTypeAddress, 
			   eRegTypeOutputPosition, 
			   eRegTypeOutputColor,
			   eRegTypeOutputFog,
			   eRegTypeOutputTexture,
			   eRegTypeNone 
			};

enum RegComponent {
					eRegCompX,
					eRegCompY,
					eRegCompZ,
					eRegCompW,
					eRegCompAll,
					eRegCompNone
					};

class CNode {
public:
	CNode();
	//note: should be friend of hashtable
	char *m_pSymbol;
	char *m_pType;
	int m_iScope;
	int m_iToken;
	int m_eRegType;
	int m_iRegNum;
	int m_eRegComponent;
	CNode *m_pNextNode;
};

#define NUM_TEMP_REGISTERS 12

class CRegister {
public:
	bool m_bEmpty;
};

class CTempRegisterSet {
public:
	CTempRegisterSet();
	int GetNextAvailableTempReg();
	bool RequestSpecificReg(int iRegNum);
	void MarkAsEmpty(int iRegNum);
	void MarkAsFilled(int iRegNum);

private:
	//from LHS because sometimes we start at RHS depending on algo
	//above comment is not really true any longer, only use r11 for expr
	//int m_iNextEmptyTempRegFromLHS;
	CRegister m_pRegisters[NUM_TEMP_REGISTERS];
};

class CSymbolTable {
public:
	int AddSymbol(char *symbol, char *type, int token, int inRegType, int iRegNum);
	int GetIDFromName(char *name);
	int GetRegTypeFromName(char *name);
	int GetRegNumFromName(char *name);
	int GetNextTempRegNum();
	void IncrementNextTempReg();
	bool DoesSymbolExist(char *symbol);
	void PrintSymbolTable();
	void PrintRow(int iRow);

	CSymbolTable();
	~CSymbolTable();
	void FreeNodes(CNode *pNode);

private:
	CNode NodeArr[256];		
	int m_iNextTempRegNum;	
};

#endif

/*
#ifndef CTEMPREGISTERMANAGER_H
#define CTEMPREGISTERMANAGER_H

#define NUM_TEMP_REGISTERS 12

class CRegister {
public:
	bool m_bEmpty;
};

class CTemporaryRegisterManager {
	public:
	int iLastRegFromEndUsed;

	CTemporaryRegisterManager();
	void GetNextAvailableTempRegisterFromEnd(int *iRegNum);

	private:
	CRegister m_Registers[NUM_TEMP_REGISTERS];
	
};

CTemporaryRegisterManager::CTemporaryRegisterManager()
{
	iLastRegFromEndUsed = 12;
}

void CTemporaryRegisterManager::GetNextAvailableTempRegisterFromEnd(int *iRegNum)
{
	for(int i=NUM_TEMP_REGISTERS;i>0;i--)
	{
		if(m_Registers[i].m_bEmpty == true)
		{
			*iRegNum = i;
			m_Registers[i].m_bEmpty = false;
		}
	}
}

#endif
  */