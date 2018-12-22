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
#include "CastNode.h"

#include <stdio.h>
#include <string.h>

CastNode::CastNode(char *pString)
{
	m_pParent = NULL;
//	m_pLeftSibling = NULL;
//	m_pRightSibling = NULL;
	m_pID = strdup(pString);
	m_pType = NULL;
	fval = 0.0;
	sval = NULL;
	m_pDescription = NULL;

	for(int i=0;i<NUM_CHILDREN;i++)
	{
		m_pChildren[i] = NULL;
	}

	m_iRegNum = -1;
	m_eRegType = eRegTypeNone;
	m_eRegComponent = eRegCompNone;
}

void CastNode::setRegType(int eRegType)
{
	m_eRegType = eRegType;
}

void CastNode::setRegNum(int iRegNum)
{
	m_iRegNum = iRegNum;
}

void CastNode::setRegComponent(int eRegComp)
{
	m_eRegComponent = eRegComp;
}

int CastNode::getRegType()
{
	return m_eRegType;
}

int CastNode::getRegNum()
{
	return m_iRegNum;
}

int CastNode::getRegComponent()
{
	return m_eRegComponent;
}

CastNode::CreateTokenNode(char *pString)
{
	m_pParent = NULL;
//	m_pLeftSibling = NULL;
//	m_pRightSibling = NULL;
	m_pID = strdup(pString);
	m_pType = "token";

	for(int i=0;i<NUM_CHILDREN;i++)
	{
		m_pChildren[i] = NULL;
	}
}

CastNode::CastNode()
{
	m_pParent = NULL;
//	m_pLeftSibling = NULL;
//	m_pRightSibling = NULL;
	m_pID = NULL;
	m_pType = NULL;

	for(int i=0;i<NUM_CHILDREN;i++)
	{
		m_pChildren[i] = NULL;
	}
}

void CastNode::inOrderTreeWalk(CastNode *pNode)
{
	//as you reach a node, print it out, go to next node
	//walk nodes until you reach point with NULL children
	if(pNode == NULL)
	{
		printf("pNode is NULL, returning up tree\n");
		return;
	}

	if(pNode->m_pID == NULL)
	{
		printf("pNode id is NULL\n");
	}
	else 
	{
		printf("%s ",pNode->m_pID);
		if(pNode->m_pType != NULL)
		{
			printf("%s is Type",pNode->m_pType);
		}
		printf("\n");
		if(pNode->m_pDescription != NULL)
		{
		//	printf(" %s is description\n", pNode->m_pDescription);
		}
	}
	
	for(int i=0;i<NUM_CHILDREN;i++)
	{
		if(pNode->m_pChildren[i] != NULL)
		{
			printf("Child[%d] Descending down tree\n",i);
			pNode->inOrderTreeWalk(pNode->m_pChildren[i]);
			printf("Ascending back to child[%d]\n",i);
		}
	}
}

//i want a function that takes a parent, an index, and an astNode and puts it into the ast
void CastNode::addChildNode(int index, CastNode *pChild)
{
	//m_pParent = this;
	pChild->setParent(this);
	m_pChildren[index] = pChild;
}