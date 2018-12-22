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
#include <string.h>

#include "CCodeGenerator.h"
//#include "yaccer_tab.h"

void CCodeGenerator::HandleProgram(CastNode *pNode)
{
	if(fp != NULL)
	{
		fprintf(fp,"vs.1.0;\n");
		fprintf(fp,";----------------------------------------------------------------------------------------\n");
		fprintf(fp,";Shader produced by prototype version 0.01 of the Game Programming Gems 3 Programmable Shader Compiler\n");
		fprintf(fp,";-----------------------------------------------------------------------------------------\n\n\n");

		fprintf(fp,";These 3 vectors are used for approximating sin and cos via Taylor Series\n");
		/*
		def c31, 3.141592, 0.5f, 6.2831f, 0.1592f
		def c32,  1.0f, -0.5f, 0.041666666667f, -0.00138888889f
		def c33,  1.0f, -0.1666666667f, 0.00833333f, -0.0001984126984f
		*/
	//	fprintf(fp,"def c32, 3.141592, 0.5f, 6.2831f, 0.1592f\n");
	//	fprintf(fp,"def c33,  1.0f, -0.5f, 0.041666666667f, -0.00138888889f\n");
	//	fprintf(fp,"def c34,  1.0f, -0.1666666667f, 0.00833333f, -0.0001984126984f\n\n");
/*
		For now, so that i don't have to do the special case xform stuff
		------------------------------------------------------------------------------
		; Vertex transformation
		;------------------------------------------------------------------------------

		; Transform to view space (world matrix is identity)
		m4x4 r9, v0, c8

		; Transform to projection space
		m4x4 r10, r9, c12

		; Store output position
		mov oPos, r10

*/
	}
	else 
	{
		printf("FILE pointer in CCodeGenerator is NULL, Quitting!\n");
	}

	if(pNode != NULL)
	{
		CastNode *pChildNode0 = NULL;
		pChildNode0 = pNode->getChild(0);

		if(pChildNode0 != NULL)
		{
			if(!strcmp(pChildNode0->getID(),"Functions"))
			{
				printf("Calling HandleFunction from HandleProgram()\n");
				HandleFunctions(pChildNode0);
			}
			else if(!strcmp(pChildNode0->getID(),"Function"))
			{
				HandleFunction(pChildNode0);
			}

		}
	}

	//this is the final thing that needs done in the shader
	//this is the xform from model to homogenous eye space (clip space)
	//TODO: make part of shader
		fprintf(fp,"; xform to view space\n m4x4 r9,r0,c8\n");
		fprintf(fp,"; xform to proj space\n m4x4 r10, r9, c12\n");
		fprintf(fp,"; store output position\n mov oPos, r10\n\n");

}

void CCodeGenerator::HandleFunctions(CastNode *pNode)
{
	if(pNode != NULL)
	{
		CastNode *pChildNode0 = NULL;

		pChildNode0 = pNode->getChild(0);

		if(pChildNode0 != NULL)
		{
			if(!strcmp(pChildNode0->getID(),"Functions"))
			{
				printf("Calling HandleFunctions from HandleFunctions\n");
				HandleFunctions(pChildNode0);
			}
			if(!strcmp(pChildNode0->getID(),"Function"))
			{
				HandleFunction(pChildNode0);
			}
		}

		CastNode *pChildNode1 = NULL;

		pChildNode1 = pNode->getChild(1);

		if(pChildNode1 != NULL)
		{
			if(!strcmp(pChildNode1->getID(),"Function"))
			{
				HandleFunction(pChildNode1);
			}
		}
	}
}

void CCodeGenerator::HandleFunction(CastNode *pNode)
{
	//HACK:
	//we don't have any way to modify the vertex position legally now,
	//so we will write it into r0 and use the value in r0 instead as the 'official'
	//position value
	//we also need to change the last bit of code that does the xform to read from this register now
	//until we fix this 
	fprintf(fp,";this is our temporary hack for vertex pos mods\n");
	fprintf(fp,"mov r0, v0; move the vertex pos for xform\n\n");
	m_pTempRegSet.MarkAsFilled(0);
	//END HACK



	//ignore function name for now, we will use it later
	//ignore params passed to function for now

	CastNode *pChildNode4 = NULL;

	pChildNode4 = pNode->getChild(4);

	if(pChildNode4 != NULL)
	{
		if(!strcmp(pChildNode4->getID(), "StmtList"))
		{
			HandleStatementList(pChildNode4);
		}
	}
}

void CCodeGenerator::HandleStatementList(CastNode *pNode)
{
	CastNode *pChildNode0 = NULL;

	pChildNode0 = pNode->getChild(0);

	if(pChildNode0 != NULL)
	{
		if(!strcmp(pChildNode0->getID(),"StmtList"))
		{
			HandleStatementList(pChildNode0);
		}
		else if(!strcmp(pChildNode0->getID(),"Stmt"))
		{
			HandleStatement(pChildNode0);
		}
	}

	CastNode *pChildNode1 = NULL;

	pChildNode1 = pNode->getChild(1);

	if(pChildNode1 != NULL)
	{
		if(!strcmp(pChildNode1->getID(),"Stmt"))
		{
			HandleStatement(pChildNode1);
		}
	}
}

void CCodeGenerator::HandleStatement(CastNode *pNode)
{
	printf("Handling Statement\n");

	CastNode *pChildNode1 = NULL;
	pChildNode1 = pNode->getChild(1);

	if(pChildNode1 != NULL)
	{
		//expressions with assignment are handled here
		if(!strcmp(pChildNode1->getID(),"Equals"))
		{	
			int iMaxDepth = 0;
			printf("***here\n");
			HandleAssignmentStmt(pNode, &iMaxDepth);
			printf("***here2\n");
			//have to clean up the registers
			//from iMaxDepth to MaxNumRegisters, set m_bEmpty to true so they 
			//can be used by the next expression
			//for(int i=13-iMaxDepth;i<13;i++)
			//{
			//	m_pTempRegSet.MarkAsEmpty(i);
			//}
			//instead, just mark r11 as empty
			m_pTempRegSet.MarkAsEmpty(11);
		}	
	
	}
	//type declarations are handled here
	//TODO::there will be some kind of 'if' wrapper here
	HandleTypeDeclarations(pNode);
}

//here we get the whole string
//Expr T_EQUALS Expr T_SEMICOLON

void CCodeGenerator::HandleAssignmentStmt(CastNode *pNode, int *iMaxDepth)
{
	bool bFinished = false;
	//First, we're going to take care of the obvious statement a = b
	CastNode *pTempNode = NULL;
	pTempNode = pNode->getChild(2);
	if(pTempNode != NULL)
	{
		pTempNode = pTempNode->getChild(0);
		if(pTempNode == NULL)
		{
			printf("hit it\n");
			int iLHSRegNum = -1;
			int eLHSRegType = eRegTypeNone;
			int iRHSRegNum = -1;
			int eRHSRegType = eRegTypeNone;
			GetRegAndRegType(((pNode->getChild(0))->sval), &iLHSRegNum, &eLHSRegType);
			GetRegAndRegType(((pNode->getChild(2))->sval), &iRHSRegNum, &eRHSRegType);
			char regStringRHS[5];
			char regStringLHS[5];
			BuildRegisterString(regStringRHS,iRHSRegNum,eRHSRegType,(pNode->getChild(2))->getRegComponent());
			BuildRegisterString(regStringLHS,iLHSRegNum,eLHSRegType,(pNode->getChild(0))->getRegComponent());
			
			char buf[80];
			if(eLHSRegType == eRegTypeConst || eLHSRegType == eRegTypeVertex)
			{
				sprintf(buf,"illegal LHS in next instruction!\n");
				fprintf(fp,buf);
			}
			sprintf(buf,"mov\t%s, %s;\n\n", regStringLHS, regStringRHS);
			fprintf(fp,buf);	
			bFinished = true;
		}
		
	}
	if(bFinished == false)
	{
		//If its not this then we use our RHS assignment handler that does more complicated expressions
		HandleRHSOfAssign(pNode->getChild(2),0, iMaxDepth);
		//HandleLHSOfAssign
		//we know the LHS is in the symbol table
		//TODO: guarantee LHS is only a string, DO NOT allow true expressions on LHS
		//to simplify things.
		//grab the value in the temp variable on the end of the temp array (r11)
		//lookup the register name and type of the LHS
		//store it in the register of the LHS
		//done
		int iLHSRegNum = -1;
		int eLHSRegType = eRegTypeNone;

		GetRegAndRegType(((pNode->getChild(0))->sval), &iLHSRegNum, &eLHSRegType);
		char regStringRHSResult[5];
		char regStringLHS[5];
		BuildRegisterString(regStringRHSResult,11,eRegTypeTemp,eRegCompAll);
		BuildRegisterString(regStringLHS,iLHSRegNum,eLHSRegType,(pNode->getChild(0))->getRegComponent());
		
		char buf[80];
		if(eLHSRegType == eRegTypeConst || eLHSRegType == eRegTypeVertex)
		{
			sprintf(buf,"illegal LHS in next instruction!\n");
			fprintf(fp,buf);
		}
		sprintf(buf,"mov\t%s, %s;\n\n", regStringLHS, regStringRHSResult);
		fprintf(fp,buf);
	}


}	

void CCodeGenerator::HandleRHSOfAssign(CastNode *pNode, int iCurHeight, int *iMaxDepth)
{
/* move up to parent
	pParent = pNode->getParent();
	get parents reg
	get op
	get child[2] reg
	generate instruction
	put result in a temp register and make sure the LHS symbol in the symbol
	table is updated to hold the register/reg type
	Show the resultant string generated
	*/
	CastNode *pChildNode0 = NULL;
	pChildNode0 = pNode->getChild(0);

	//increment counter for the registers
	iCurHeight++;
	*iMaxDepth = iCurHeight;
	if(pChildNode0 != NULL )
	{
		//make sure its not a vector type
		if(strcmp(pChildNode0->getID(),"Vector"))
		{
			printf("Descending\n");
			HandleRHSOfAssign(pChildNode0, iCurHeight, iMaxDepth);
		}
	}

	//termination condition
	//if the parents child[1] node is an equals sign we are done with this part and we need to finish the assignment
	//in the HandleAssignment method
	if(pNode)
	{
		CastNode *tempNode = NULL;
		tempNode = pNode->getParent();
		if(tempNode)
		{
			tempNode = tempNode->getChild(1);
			if(!strcmp(tempNode->getID(),"Equals"))
			{
				printf("Found the top of the expression, we are done\n");
				return;
			}
		}
	}

	//Here we just initialize and set values for the data elements we make use of later,
	//they may be NULL and that is OKAY!!
	CastNode *pParent = NULL;
	pParent = pNode->getParent();

	CastNode *pChild0 = NULL;
	pChild0 = pParent->getChild(0);

	CastNode *pChild1 = NULL;
	pChild1 = pParent->getChild(1);

	CastNode *pChild2 = NULL;	
	pChild2 = pParent->getChild(2);

	/***************************************************
	Handle Argument 1
	*********************************************************/
	//if its type is an expression and the sval is NULL, it must be in the temps
	int eRegTypeArg1 = eRegTypeNone;
	int iRegNumArg1 = -1;
	int eRegCompArg1 = eRegCompNone;

	//first, check to see if its in the astNode information
	//if so then we use the value stored
	//if the register number is -1 it must be some other type of value--look it up in
	//the symbol table,

	
	//the reason we have to check pNode like this is because there may have been an expression
	//that has been built already up to this node that contains the value, so we just check to see
	//if we should use that register
	if(pNode->getRegNum() != -1)
	{
		iRegNumArg1 = pNode->getRegNum();
		eRegTypeArg1 = pNode->getRegType();
		eRegCompArg1 = pNode->getRegComponent();
	}
	else
	{
		//otherwise, it is not stored in any register yet and we need to get the value into the register
		//there are 2 cases here.
		//first, maybe its a vector and we will process it accordingly
		CastNode *pNodeChild0 = NULL;
		pNodeChild0 = pNode->getChild(0);
		if(pNodeChild0 != NULL)
		{
			//TODO: this will all get wrapped in a handle vector node function
			if(!strcmp(pNodeChild0->getID(), "Vector"))
			{
				//load the values into the register then use this register as the regnum
				//TODO: the register could also be a variable, but we are not going to worry about
				//that now
				char buf[80];
				float v1 = (pNodeChild0->getChild(0))->fval;
				float v2 = (pNodeChild0->getChild(1))->fval;
				float v3 = (pNodeChild0->getChild(2))->fval;
				float v4 = (pNodeChild0->getChild(3))->fval;
				sprintf(buf,"def c45, %ff, %ff, %ff, %ff;\n",v1,v2,v3,v4);
				fprintf(fp,buf);
				iRegNumArg1 = 45;
				eRegTypeArg1 = eRegTypeConst;
				eRegCompArg1 = eRegCompAll;
			}
			
		}
		else if(!strcmp(pNode->getID(),"T_NORMALIZE"))
		{
			printf("here\n");
			//if we see normalize then we have to get the argument that we want normalized.  it can be an expression
			//which is complicated....
			//TODO: handle this complicated expression, for now assume its a single variable
			//we will use our standard 'temp' working reg and write the result out
			
			if(pParent != NULL)
			{
				CastNode *pNormalizee = NULL;
				pNormalizee = pParent->getChild(1);
				
				//see if the normalizee has been stored in a register yet
				//-1 means it is not in a register yet
				if(pNormalizee->getRegNum() != -1)
				{
					iRegNumArg1 = pNormalizee->getRegNum();
					eRegTypeArg1 = pNormalizee->getRegType();
					eRegCompArg1 = pNormalizee->getRegComponent();
				}
				else
				{
					GetRegAndRegType(pNormalizee->sval, &iRegNumArg1, &eRegTypeArg1);
					eRegCompArg1 = pNormalizee->getRegComponent();
				}
				/*
					normalize ( r1 ):

					dp3 r2, r1, r1;
					rsq r2.x, r2.x;
					rsq r2.y, r2.y;
					rsq r2.z, r2.z;
					mul r2.x, r2.x, r1.x;
					mul r2.y, r2.y, r1.y;
					mul r2.z, r2.z, r1.z;
				*/
				char buf[256];
				char StringArg1[5];
				char StringResult[5];
				int iResultReg = 11;
				BuildRegisterString(StringArg1, iRegNumArg1, eRegTypeArg1,eRegCompArg1);
				//put result of normalize into all of the vector components, that is cool, this is what you do anyway
				BuildRegisterString(StringResult, iResultReg, eRegTypeTemp,eRegCompAll);
				sprintf(buf,";normalize function\n");
				fprintf(fp,buf);
				sprintf(buf, "dp3\t%s, %s, %s;\n",StringResult, StringArg1, StringArg1);
				fprintf(fp,buf);
				sprintf(buf, "rsq\tr10, %s;\n",StringResult);
				fprintf(fp,buf);
				//sprintf(buf, "rsq\tr10.y, %s.x;\n",StringResult);
				//fprintf(fp,buf);
				//sprintf(buf, "rsq\tr10.z, %s.x;\n",StringResult);
				//fprintf(fp,buf);
				sprintf(buf, "mul\t%s, %s, r10;\n", StringResult, StringArg1);
				fprintf(fp,buf);
			
				//sprintf(buf, "mul\t%s.y, %s.y, r10.y;\n", StringResult, StringArg1);
				//fprintf(fp,buf);
				//sprintf(buf, "mul\t%s.z, %s.z, r10.z;\n", StringResult, StringArg1);
				//fprintf(fp, buf);
				//sprintf(buf,";end normalize\n");
				//fprintf(fp,buf);

				//is this right?
				//store, for child node 0, where its result is stored for later recursions to 
				//reference via the astNodes
				pParent->setRegType(eRegTypeTemp);
				pParent->setRegNum(iResultReg);
				pParent->setRegComponent(eRegCompAll);
				return;
			}
		}
		//TODO: Insert other single ops here :)
		else if(!strcmp(pNode->getID(),"T_SQRT"))
		{
			printf("TODO");
			fprintf(fp, "NEED TO put in code for SQRT\n");
			return;
		}
		else if(!strcmp(pNode->getID(),"T_NEGATE"))
		{
			printf("TODO");
			fprintf(fp, "NEED TO put in code for NEGATE\n");
			return;
		}
		else if(!strcmp(pNode->getID(),"T_MAXWITH0"))
		{
			printf("in maxwith0\n");
			if(pParent != NULL)
			{
				CastNode *pArg = NULL;
				pArg = pParent->getChild(1);

				if(pArg->getRegNum() != -1)
				{
					iRegNumArg1 = pArg->getRegNum();
					eRegTypeArg1 = pArg->getRegType();
				}
				else
				{
					GetRegAndRegType(pArg->sval, &iRegNumArg1, &eRegTypeArg1);
				}

				eRegCompArg1 = pArg->getRegComponent();

				char buf[256];
				char StringArg1[5];
				char StringResult[5];
				int iResultReg = 11;
				BuildRegisterString(StringArg1, iRegNumArg1, eRegTypeArg1,eRegCompArg1);
				//basically, we put this back in the same component
				BuildRegisterString(StringResult, iResultReg, eRegTypeTemp,eRegCompArg1);
			
				sprintf(buf, "max\t%s, %s, c0.xxxx;\n", StringResult, StringArg1);
				fprintf(fp,buf);
				
				pParent->setRegType(eRegTypeTemp);
				pParent->setRegNum(iResultReg);
				pParent->setRegComponent(eRegCompArg1);

				return;
			}	
		}
		else if(!strcmp(pNode->getID(),"T_CLAMPTO1"))
		{
			printf("in clampto1\n");
			if(pParent != NULL)
			{
				CastNode *pArg = NULL;
				pArg = pParent->getChild(1);

				if(pArg->getRegNum() != -1)
				{
					iRegNumArg1 = pArg->getRegNum();
					eRegTypeArg1 = pArg->getRegType();
				}
				else
				{
					GetRegAndRegType(pArg->sval, &iRegNumArg1, &eRegTypeArg1);
				}

				eRegCompArg1 = pArg->getRegComponent();

				char buf[256];
				char StringArg1[5];
				char StringResult[5];
				int iResultReg = 11;
				BuildRegisterString(StringArg1, iRegNumArg1, eRegTypeArg1,eRegCompArg1);
				BuildRegisterString(StringResult, iResultReg, eRegTypeTemp,eRegCompArg1);
			
				sprintf(buf, "min\t%s, %s, c1.xxxx;\n", StringResult, StringArg1);
				fprintf(fp,buf);
				
				pParent->setRegType(eRegTypeTemp);
				pParent->setRegNum(iResultReg);
				pParent->setRegComponent(eRegCompArg1);
				return;
			}	
		}
		else if((!strcmp(pNode->getID(),"T_SIN")) || (!strcmp(pNode->getID(), "T_COS")))
		{
			printf("found sin\n");

			if(pParent != NULL)
			{
				CastNode *pArg = NULL;
				pArg = pParent->getChild(1);

				if(pArg->getRegNum() != -1)
				{
					iRegNumArg1 = pArg->getRegNum();
					eRegTypeArg1 = pArg->getRegType();
				}
				else
				{
					GetRegAndRegType(pArg->sval, &iRegNumArg1, &eRegTypeArg1);
				}

				eRegCompArg1 = pArg->getRegComponent();

				char buf[256];
				char StringArg1[5];
				char StringResult[5];
				int iResultReg = 11;
				BuildRegisterString(StringArg1, iRegNumArg1, eRegTypeArg1,eRegCompArg1);
				BuildRegisterString(StringResult, iResultReg, eRegTypeTemp,eRegCompArg1);
		
			//start new stuff here
				sprintf(buf,";compute sin() or cos()\n");
				fprintf(fp,buf);
				//first thing we have to do is get the (possibly) constant into a temp register
				//so nVidia Dx8 limitations can be overcome
				//grab value out of constant register and put in next  reg
				int iNextTempReg = -1;
				iNextTempReg = m_pTempRegSet.GetNextAvailableTempReg();
				char tempRegString[5];
				BuildRegisterString(tempRegString, iNextTempReg, eRegTypeTemp,eRegCompArg1);
				//sprintf(buf, "mov %s, %s\n", tempRegString, StringArg1);
				sprintf(buf, "mov r4.x, %s\n", StringArg1);
				fprintf(fp,buf);
				sprintf(buf, "sge r5.x, r4.x, r4.x\n");
				fprintf(fp, buf);
				sprintf(buf, "mul r5.y, r4.x, r4.x\n");
				fprintf(fp, buf);
				sprintf(buf, "mul r4.y, r4.x, r5.y\n");
				fprintf(fp, buf);
				sprintf(buf, "mul r5.z, r5.y, r5.y\n");
				fprintf(fp, buf);
				sprintf(buf, "mul r4.z, r4.x, r5.z\n");
				fprintf(fp, buf);
				sprintf(buf, "mul r5.w, r5.y, r5.z\n");
				fprintf(fp, buf);
				sprintf(buf, "mul r4.w, r4.x, r5.w\n");
				fprintf(fp, buf);
				//now we decide whether its a sin or cos result
				if(!strcmp(pNode->getID(),"T_SIN")) 
				{	
					sprintf(buf, "mul r4, r4, c34\n");  //c34 has exponents for taylor series
					fprintf(fp, buf);
					sprintf(buf, "dp4 r4.x, r4, c1.xxxx\n");
					fprintf(fp,buf);
					//move the result to the result reg
					sprintf(buf, "mov %s, r4.x\n", StringResult);
					fprintf(fp,buf);
				}
				else if(!strcmp(pNode->getID(),"T_COS"))
				{
					sprintf(buf, "mul r5, r5, c33\n");  //c34 has exponents for taylor series
					fprintf(fp, buf);
					sprintf(buf, "dp4 r5.x, r5, c1.xxxx\n");
					fprintf(fp,buf);
					sprintf(buf, "mov %s, r5.x\n", StringResult);
					fprintf(fp,buf);
				}
			

				//sprintf(buf,"mov %s, %s.xxxx\n",StringResult,StringResult);
				//fprintf(fp,buf);
				
				pParent->setRegType(eRegTypeTemp);
				pParent->setRegNum(iResultReg);
				pParent->setRegComponent(eRegCompArg1); //use result reg same as argument
				return;
			}
		}
		else if(!strcmp(pNode->getID(),"T_FLOOR"))
		{
			printf("in floor\n");
			if(pParent != NULL)
			{
				CastNode *pArg = NULL;
				pArg = pParent->getChild(1);

				if(pArg->getRegNum() != -1)
				{
					iRegNumArg1 = pArg->getRegNum();
					eRegTypeArg1 = pArg->getRegType();
				}
				else
				{
					GetRegAndRegType(pArg->sval, &iRegNumArg1, &eRegTypeArg1);
				}

				eRegCompArg1 = pArg->getRegComponent();

				char buf[256];
				char StringArg1[5];
				char StringResult[5];
				int iResultReg = 11;
				BuildRegisterString(StringArg1, iRegNumArg1, eRegTypeArg1,eRegCompArg1);
				BuildRegisterString(StringResult, iResultReg, eRegTypeTemp,eRegCompArg1);
			
				sprintf(buf, "expp %s.y, %s.x\n", StringResult, StringArg1);
				fprintf(fp,buf);
				sprintf(buf, "add %s.y, %s.x, -%s.y\n", StringResult, StringArg1, StringResult);
				fprintf(fp,buf);
				sprintf(buf, "mov %s.x, %s.y\n",StringResult, StringResult);
				fprintf(fp,buf);
				
				pParent->setRegType(eRegTypeTemp);
				pParent->setRegNum(iResultReg);
				pParent->setRegComponent(eRegCompArg1);

				return;
			}	
		}
		else if(!strcmp(pNode->getID(),"T_CEILING"))
		{
			printf("in ceiling\n");
			if(pParent != NULL)
			{
				CastNode *pArg = NULL;
				pArg = pParent->getChild(1);

				if(pArg->getRegNum() != -1)
				{
					iRegNumArg1 = pArg->getRegNum();
					eRegTypeArg1 = pArg->getRegType();
				}
				else
				{
					GetRegAndRegType(pArg->sval, &iRegNumArg1, &eRegTypeArg1);
				}

				eRegCompArg1 = pArg->getRegComponent();

				char buf[256];
				char StringArg1[5];
				char StringResult[5];
				int iResultReg = 11;
				BuildRegisterString(StringArg1, iRegNumArg1, eRegTypeArg1,eRegCompArg1);
				BuildRegisterString(StringResult, iResultReg, eRegTypeTemp,eRegCompArg1);
			
				sprintf(buf, "expp %s.y, -%s.x\n", StringResult, StringArg1);
				fprintf(fp,buf);
				sprintf(buf, "add %s.y, %s.x, %s.y\n", StringResult, StringArg1, StringResult);
				fprintf(fp,buf);
				sprintf(buf, "mov %s.x, %s.y\n",StringResult, StringResult);
				fprintf(fp,buf);
				
				pParent->setRegType(eRegTypeTemp);
				pParent->setRegNum(iResultReg);
				pParent->setRegComponent(eRegCompArg1);

				return;
			}	
		}

		else {
			//second, if its not, its a new value and we will get its register and type that way
			GetRegAndRegType(pChild0->sval,&iRegNumArg1, &eRegTypeArg1);
			eRegCompArg1 = pChild0->getRegComponent();
		}
	}

/***************************************************
Handle Argument 2
*********************************************************/

	int eRegTypeArg2 = eRegTypeNone;
	int iRegNumArg2 = -1;
	int eRegCompArg2 = eRegCompNone;

	//here, we check to see if arg2 is a register or not.
	//
	
	CastNode *pParentNodeChild2 = NULL;
	pParentNodeChild2 = pParent->getChild(2);
	CastNode *pParentNodeChild2Child = NULL;
	//i think that this is not pParentNodeChild2Child!! but i had to change it. :(
	if(pParentNodeChild2 != NULL)
	{
		pParentNodeChild2Child = pParentNodeChild2->getChild(0);
		if(pParentNodeChild2Child != NULL)
		{
			//TODO: this will all get wrapped in a handle vector node function
			if(!strcmp(pParentNodeChild2Child->getID(), "Vector"))
			{
				//load the values into the register then use this register as the regnum
				//TODO: the register could also be a variable, but we are not going to worry about
				//that now
				char buf[80];
				float v1 = (pParentNodeChild2Child->getChild(0))->fval;
				float v2 = (pParentNodeChild2Child->getChild(1))->fval;
				float v3 = (pParentNodeChild2Child->getChild(2))->fval;
				float v4 = (pParentNodeChild2Child->getChild(3))->fval;
				sprintf(buf,"def c46, %ff, %ff, %ff, %ff;\n",v1,v2,v3,v4);
				fprintf(fp,buf);
				iRegNumArg2 = 46;
				eRegTypeArg2 = eRegTypeConst;
				eRegCompArg2 = eRegCompAll;
			}
		}
		else {
			GetRegAndRegType(pChild2->sval,&iRegNumArg2, &eRegTypeArg2);
			eRegCompArg2 = pChild2->getRegComponent();
		}
	}

/********************************************
Generate Instruction for instructions with 2 args
**********************************************/

	char regStringArg1[5];
	char regStringArg2[5];

	BuildRegisterString(regStringArg1,iRegNumArg1,eRegTypeArg1,eRegCompArg1);
	BuildRegisterString(regStringArg2,iRegNumArg2,eRegTypeArg2,eRegCompArg2);

	//Have Arg1 and Arg2, build an instruction and emit it
	//Store the result in a temp register
	//NOPE Update the symbol table with the temporary register 
	//Based on the height in the tree we know the temporary register you are using
	//can we maybe use temp registers 12 and down then variables can use 0-12 up?

	char buf[80];

	//need to get a temp register to load the result
	//mark it as unavailable
	int iTempRegNum = -1;
	int iResultReg = 11;


//	m_pTempRegMgr.GetNextAvailableTempRegisterFromEnd(&iTempRegNum);
	//TODO: MaxNumRegs method needed here for 12
	//if(m_pTempRegSet.RequestSpecificReg(iResultReg) == true)
	//{
	char regStringTemp[5];
	BuildRegisterString(regStringTemp,iResultReg,eRegTypeTemp,eRegCompAll);

	char *temp = pChild1->getID();

	if(!strcmp(pChild1->getID(),"T_PLUS"))
	{
		sprintf(buf,"add\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
		fprintf(fp,buf);
		printf("add\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
	}
	else if(!strcmp(pChild1->getID(),"T_MINUS"))
	{
		sprintf(buf,"sub\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
		fprintf(fp,buf);
		printf("sub\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
	}
	else if(!strcmp(pChild1->getID(),"T_MULTIPLY"))
	{
		sprintf(buf,"mul\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
		fprintf(fp,buf);
		printf("mul\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
	}
	else if(!strcmp(pChild1->getID(),"T_DIVIDE"))
	{
		sprintf(buf,"rcp\t%s, %s;\n", regStringArg2, regStringArg2);
		fprintf(fp,buf);
		sprintf(buf,"mul\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
		fprintf(fp,buf);
		printf("rcp\t%s, %s;\n", regStringArg2, regStringArg2);
		printf("mul\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
	}
	else if(!strcmp(pChild1->getID(),"T_DOT4"))
	{
		sprintf(buf,"dp4\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
		fprintf(fp,buf);
		printf("dp4\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
	}
	else if(!strcmp(pChild1->getID(),"T_DOT3"))
	{
		sprintf(buf,"dp3\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
		fprintf(fp,buf);
		printf("dp3\t%s, %s, %s;\n", regStringTemp, regStringArg1, regStringArg2);
	}
	else if(!strcmp(pChild1->getID(),"T_POWER"))
	{
		//load reg10 with the right parts (r10.xxyy), via a mov instruction
		//compute result for r11
		sprintf(buf,"; raise to a power\n", regStringArg1);
		fprintf(fp,buf);
		sprintf(buf,"mov\tr10.x, c1.x;\n");
		fprintf(fp,buf);
		sprintf(buf,"mov\tr10.y, %s.x;\n", regStringArg1);
		fprintf(fp,buf);
		sprintf(buf,"mov\tr10.z, %s.y;\n", regStringArg2);
		fprintf(fp,buf);			
		//i think i want no .x so that all entries are filled with the result
		sprintf(buf,"lit\t%s.z, r10.xyzz;\n", regStringTemp);
		fprintf(fp,buf);
		//propagate result throughout the register
		sprintf(buf,"mov\t%s, %s.z\n",regStringTemp,regStringTemp);
		fprintf(fp,buf);
	}

	//store, for child node 0, where its result is stored for later recursions to 
	//reference via the astNodes
	pParent->setRegType(eRegTypeTemp);
	pParent->setRegNum(iResultReg);
	//right now, the result of any binary operation goes into the entire LHS.
	//then, when the assignment takes place, it will only be written to one of the registers.
	pParent->setRegComponent(eRegCompAll);
	//}
//	else
//	{
//		printf("Error in register request\n");
//		exit(-1);
//	}

}

void CCodeGenerator::HandleTypeDeclarations(CastNode *pNode)
{
}
/*

void CCodeGenerator::BuildRegisterString(char *pString, int iRegNum, int eRegType)
{
	if(eRegType == eRegTypeConst)
	{
		sprintf(pString,"%c%d\0",'c',iRegNum);
	}
	else if(eRegType == eRegTypeVertex)
	{
		sprintf(pString,"%c%d\0",'v',iRegNum);
	}
	else if(eRegType == eRegTypeTemp)
	{
		sprintf(pString,"%c%d\0",'r',iRegNum);
	}
	else if(eRegType == eRegTypeOutputPosition)
	{
		sprintf(pString,"oPos\0");
	}
	else if(eRegType == eRegTypeOutputColor)
	{
		sprintf(pString,"oD0\0");
	}
	else if(eRegType == eRegTypeOutputFog)
	{
		sprintf(pString,"oFog\0");
	}
	else if(eRegType == eRegTypeOutputTexture)
	{
		sprintf(pString,"oT0\0");
	}
	else 
	{
		sprintf(pString,"ERROR\n");
	}
}
*/
void CCodeGenerator::BuildRegisterString(char *pString, int iRegNum, int eRegType, int eRegComponent)
{
	char *pEOS = NULL; //EOS = EndOfString
	pEOS = pString;
	if(eRegType == eRegTypeConst)
	{
		sprintf(pString,"%c%d\0",'c',iRegNum);
	}
	else if(eRegType == eRegTypeVertex)
	{
		sprintf(pString,"%c%d\0",'v',iRegNum);
	}
	else if(eRegType == eRegTypeTemp)
	{
		sprintf(pString,"%c%d\0",'r',iRegNum);
	}
	else if(eRegType == eRegTypeOutputPosition)
	{
		sprintf(pString,"oPos\0");
	}
	else if(eRegType == eRegTypeOutputColor)
	{
		sprintf(pString,"oD0\0");
	}
	else if(eRegType == eRegTypeOutputFog)
	{
		sprintf(pString,"oFog\0");
	}
	else if(eRegType == eRegTypeOutputTexture)
	{
		sprintf(pString,"oT0\0");
	}
	else 
	{
		sprintf(pString,"ERROR\n");
	}

	//now, attach the eRegComponent if necessary
	if(eRegComponent != eRegCompAll)
	{
		while(*pEOS != '\0') 
		{
			pEOS++;
		}
		if(eRegComponent == eRegCompX)
		{
			sprintf(pEOS,".x\0");
		}
		else if(eRegComponent == eRegCompY)
		{
			sprintf(pEOS,".y\0");
		}
		else if(eRegComponent == eRegCompZ)
		{
			sprintf(pEOS,".z\0");
		}
		else if(eRegComponent == eRegCompW)
		{
			sprintf(pEOS,".w\0");
		}
	}

}

void CCodeGenerator::GetRegAndRegType(char *sval, int *iRegNum, int *eRegType)
{
	printf("TODO: WHAT IF ITS A FLOAT OR WHATEVE!!\n");
	printf("TODO: haven't determined what the register type is yet\n");
			
	//cannot be a float, therefore it must be a symbol
	//StripComponent -- basically, we want to strip off the .x,y,z,w from the string before finding it in the symbol table



	bool status = symTab.DoesSymbolExist(sval);

	if(status == false)
	{
		//add it to a temp register
		//start at temp register 40, increment up
		int iNextTempReg = -1;
	
		//iNextTempReg = symTab.GetNextTempRegNum();
		//symTab.IncrementNextTempReg();

		iNextTempReg = m_pTempRegSet.GetNextAvailableTempReg();

		symTab.AddSymbol(sval,"variable",-1,eRegTypeTemp,iNextTempReg);
		*eRegType = eRegTypeTemp;
		*iRegNum = iNextTempReg;
	}
	else 
	{
		//question: do i care what type it is?
		//yes, if its a temp that is different than if it is a constant
		//TODO: get these in one pass so we don't have multiple lookups
		*eRegType = symTab.GetRegTypeFromName(sval);
		//these are the output registers, we will do something special with them in ...
		//since we have sufficient info with the output value just use that
		if( *eRegType == eRegTypeOutputPosition  || *eRegType == eRegTypeOutputColor ||
			*eRegType == eRegTypeOutputFog		|| *eRegType == eRegTypeOutputTexture) 
		{
			*iRegNum = -1;
		}
		else 
		{
			*iRegNum = symTab.GetRegNumFromName(sval);
		}
	}
}

void CCodeGenerator::HandleExpression(CastNode *node)
{
	printf("Handling Expression\n");
	//generate code here
	
}

void CCodeGenerator::ParseFloatVectors(char *pString, float *v1, float *v2, float *v3, float *v4)
{
	//may be more than one vector, but for now we will grab the first one and pass it on down
//	char buf[80];
/*	buf = strtok(pString, '(');
	printf("buf is %s\n",buf);
	buf = strtok(pString, ')');
	printf("buf is now %s, buf);
*/
}

void CCodeGenerator::ParseFloatVector(char *pString, float *f1, float *f2, float *f3, float *f4)
{
}


