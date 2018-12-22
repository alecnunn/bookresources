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
#include "CSymbolTable.h"

CTempRegisterSet::CTempRegisterSet()
{
	for(int i=0;i<NUM_TEMP_REGISTERS;i++)
	{ 
		m_pRegisters[i].m_bEmpty = true;
	}
}

int CTempRegisterSet::GetNextAvailableTempReg()
{
	for(int i=0;i<NUM_TEMP_REGISTERS;i++)
	{
		if(m_pRegisters[i].m_bEmpty == true)
		{
			m_pRegisters[i].m_bEmpty = false;
			return i;
		}
	}
	return -1;
}

//requests a specific register for use.  if available, go ahead and return true
//meaning they can use it
bool CTempRegisterSet::RequestSpecificReg(int iRegNum)
{
	if(m_pRegisters[iRegNum].m_bEmpty == true)
	{
		m_pRegisters[iRegNum].m_bEmpty = false;
		return true;
	}
	else 
	{
		return false;
	}
}

void CTempRegisterSet::MarkAsEmpty(int iRegNum)
{
		m_pRegisters[iRegNum].m_bEmpty = true;
}

void CTempRegisterSet::MarkAsFilled(int iRegNum)
{
	m_pRegisters[iRegNum].m_bEmpty = false;
}

CNode::CNode()
{
	m_pSymbol = NULL;
	m_pType = NULL;
	m_iScope = 0;
	m_pNextNode = NULL;

	m_eRegType	= eRegTypeNone;
	m_iRegNum			=	-1;
	m_eRegComponent  = eRegCompNone;


}


CSymbolTable::CSymbolTable()
{
	for(int i=0;i<256;i++)
	{
		char *temp = "start";
		NodeArr[i].m_pSymbol = strdup(temp);
		NodeArr[i].m_pType = strdup("NONE");
		NodeArr[i].m_pNextNode = NULL;
	}

	m_iNextTempRegNum = 0;
}

CSymbolTable::~CSymbolTable()
{
	//free the data
	for(int i=0;i<256;i++)
	{
		CNode *pNode = &NodeArr[i];
		while(pNode != NULL)
		{
			free(pNode->m_pSymbol);
			free(pNode->m_pType);
			pNode = pNode->m_pNextNode;	
		}
	}
	//now free the nodes
/*	for(i=39;i<43;i++)
	{
		printf("working on line %d\n",i);
		
		CNode *pNode = &NodeArr[i];
		
		//first node never gets destroyed, its a static array
		//therefore start at the next node to delete
		while(pNode->m_pNextNode != NULL)
		{
			CNode *pTemp = NULL;
			//next ptr gets stored so its not messed up when deleted
			if(pNode != NULL)
				if(pNode->m_pNextNode != NULL)
					pTemp = pNode->m_pNextNode;
			//delete current pointer
			if(pNode != NULL)
				delete pNode;
			pNode = pTemp;
			if(pNode == NULL)
			{
				//we have deleted all of the extra nodes
				//destroy ourself
				delete pNode;
				break;
			}	
		}
		NodeArr[i].m_pNextNode = NULL;
	}	
	*/
}

bool CSymbolTable::DoesSymbolExist(char *symbol)
{
	//use the first letter of the symbol as the index
	char HashKey = symbol[0];
	int iRow = HashKey;
	CNode *pNode = &NodeArr[iRow];
	
	while(pNode != NULL)
	{
		if(pNode->m_pSymbol == NULL) break;

		if(!strcmp(pNode->m_pSymbol,symbol))
		{	
			//match
			char buf[80];
			sprintf(buf,"'%s' already exists in symbol table\n",symbol);
			OutputDebugString(buf);
			return true;
		
		}
		else
		{
			pNode = pNode->m_pNextNode;
			if(pNode == NULL) break;
		}
	}
	return false;
}

int CSymbolTable::AddSymbol(char* symbol, char *type, int token, int eRegType, int iRegNum)
{
	bool status = DoesSymbolExist(symbol);
	if(status == false)
	{
		//create new node
		CNode *pNewNode = NULL;
		pNewNode = new CNode();
		if(pNewNode->m_pSymbol != NULL)
		{
			free(pNewNode->m_pSymbol);
		}
		if(pNewNode->m_pType != NULL)
		{
			free(pNewNode->m_pType);
		}
		pNewNode->m_pSymbol = strdup(symbol);
		pNewNode->m_pType = strdup(type);
		pNewNode->m_iToken = token;
		pNewNode->m_eRegType = eRegType;
		pNewNode->m_iRegNum = iRegNum;
		pNewNode->m_pNextNode = NULL;
		
		//use the first letter of the symbol as the index
		char HashKey = symbol[0];
		int iRow = HashKey;
		CNode *pNode = &NodeArr[iRow];

		//find the end of the row
		//no need to check pNode for NULL here, first entry is
		//always assumed filled
		while(pNode->m_pNextNode != NULL)
		{
			pNode = pNode->m_pNextNode;
		}
		
		//insert the new node
		pNode->m_pNextNode = pNewNode;

	}

	return 1;
}

int CSymbolTable::GetIDFromName(char *name)
{
	char HashKey = name[0];
	int iRow = HashKey;
	CNode *pNode = &NodeArr[iRow];

	while(pNode != NULL)
	{
		if(!strcmp(pNode->m_pSymbol,name))
		{
			return pNode->m_iToken;
		}
		pNode = pNode->m_pNextNode;
	}
	return -1;
}


int CSymbolTable::GetRegTypeFromName(char *name)
{
	char HashKey = name[0];
	int iRow = HashKey;
	CNode *pNode = &NodeArr[iRow];

	while(pNode != NULL)
	{
		if(!strcmp(pNode->m_pSymbol,name))
		{
			return pNode->m_eRegType;
		}
		pNode = pNode->m_pNextNode;
	}
	return -1;
}

int CSymbolTable::GetRegNumFromName(char *name)
{
	char HashKey = name[0];
	int iRow = HashKey;
	CNode *pNode = &NodeArr[iRow];

	while(pNode != NULL)
	{
		if(!strcmp(pNode->m_pSymbol,name))
		{
			return pNode->m_iRegNum;
		}
		pNode = pNode->m_pNextNode;
	}
	return -1;
}

int CSymbolTable::GetNextTempRegNum()
{
	return m_iNextTempRegNum;
}

void CSymbolTable::IncrementNextTempReg()
{
	m_iNextTempRegNum++;
	
	if(m_iNextTempRegNum > 12)
	{
		printf("Handle Out of Regsiters!!\n");
	}
}

void CSymbolTable::PrintSymbolTable()
{
	for(int i=0;i<256;i++)
	{	
		PrintRow(i);
	}
}

void CSymbolTable::PrintRow(int iRow)
{
	CNode *pNode = NULL;
	pNode = &NodeArr[iRow];

	printf("Row: %d\n",iRow);

	while(pNode != NULL)
	{
		printf("node: %s %s %d\n",pNode->m_pSymbol, pNode->m_pType, pNode->m_iScope);
		pNode = pNode->m_pNextNode;
	}
}
/*

  actually need to loop over each linked list
bool CSymbolTable::lookupSymByTokID(int tokID)
{
	CNode *pNode = NULL;

	for(int i=0;i<26;i++)
	{
		while(pNode != NULL)
		{
			pNode = &NodeArr[i];
			if(pNode->m_iToken == tokID)
			{
				return true;
			}
			pNode = pNode->m_pNextNode;
		}
	}
	return false;
}
*/

