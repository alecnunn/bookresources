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
#ifndef CASTNODE_H
#define CASTNODE_H

#include "stdio.h"

//used for register enumerated types
#include "CSymbolTable.h"

#define NUM_CHILDREN 10

//class testerClass { };

class CastNode
{
	public:
	//setters correspond
	//initialize all pointers to NULL;
	CastNode(char *pString);
	CastNode();
	CreateNode(char *string);
	CreateTokenNode(char *string);
	~CastNode();

	void addChildNode(int index, CastNode *pChild);
	//CastNode *getLeftSibling() {return m_pLeftSibling;}
	//void setLeftSibling(CastNode *pNode) {m_pLeftSibling = pNode; }
	//CastNode *getRightSibling() {return m_pRightSibling; }
	//void setRightSibling(CastNode *pNode) {m_pRightSibling = pNode;}
	CastNode *getParent() {return m_pParent;}
	void setParent(CastNode *pNode) {m_pParent = pNode;}
	CastNode *getChild(int i) { return m_pChildren[i]; }
	void setChild(int i, CastNode *pNode) {m_pChildren[i] = pNode;}
	char *getDescription() {return m_pDescription;}
	void setDescription(char *pStr) {m_pDescription = pStr;}

	void setRegType(int eRegType);
	void setRegNum(int iRegNum);
	void setRegComponent(int eRegComp);

	int getRegType();
	int getRegNum();
	int getRegComponent();

	char *getID() {return m_pID;}
	char *getType() { return m_pType; }

	void printNode() 
	{
		printf("Symbol: %s\tType:%c\n", m_pID, m_pType);
	}
	
	//as you reach a node, print it out, go to next node
	//walk nodes until you reach point with NULL children
	static void inOrderTreeWalk(CastNode *pNode);
	static void printTree(CastNode *pNode);
	
	//TODO: make private with accessors
	float fval;
	float fvector[4];
	char *sval;

private: 
	//CastNode *m_pLeftSibling;
	//CastNode *m_pRightSibling;
	CastNode *m_pParent;
	CastNode  *m_pChildren[NUM_CHILDREN];

	//actual data
	char *m_pDescription;
	char *m_pID;
	char *m_pType;
	float m_floatVector[4];

	//register storage information 
	//currently only used for temps
	//otherwise look in the symbol table
	int m_eRegType;
	int m_iRegNum;
	int m_eRegComponent;  //which component we are accessing

}; 


#endif