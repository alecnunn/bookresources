// IDL compiler backend
//
// Copyright 2004 Bae,Hyun-jik.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

#include "StdAfx.h"
#include ".\backend.h"

void WriteHeaderStart(ostream &os)
{
	os<<"#pragma once\n";
}

void WriteFunctionIDs(ostream &os,CSpecification* spec)
{
	for(CSpecification::Functions::iterator func0=spec->m_functions.begin();
		func0!=spec->m_functions.end();
		func0++)
	{
		CFunction& func=*func0;
		os<<"static const RPCHeader RPC_ID_"<<func.m_name<<"="<<func.m_ID<<";\n";
	}
}

void WriteFunctionDecls(ostream &os,CSpecification* spec)
{
	for(CSpecification::Functions::iterator func0=spec->m_functions.begin();
		func0!=spec->m_functions.end();
		func0++)
	{
		CFunction& func=*func0;
		WriteFunctionDecl(os,&func);
		os<<";\n";
	}
}

// Writes an RPC function declaration
void WriteFunctionDecl(ostream &os,CFunction* func)
{
	os<<"RPCResult "<<func->m_name<<"(RPCSendTo sendTo,RPCSendContext sendContext";
	WriteParametersWithComma(os,func);
	os<<")";
}

// Write RPC function parameters starting with commas
void WriteParametersWithComma(ostream &os,CFunction* func)
{
	for(CFunction::Parameters::iterator param0=func->m_params.begin();param0!=func->m_params.end();param0++)
	{
		CParameter& param=*param0;
		os<<",const "<<param.m_type<<" &"<<param.m_name;
	}
}

void WriteFunctionProxies(ostream &os,CSpecification* spec)
{
	for(CSpecification::Functions::iterator func0=spec->m_functions.begin();
		func0!=spec->m_functions.end();
		func0++)
	{
		CFunction& func=*func0;
		WriteFunctionDecl(os,&func);
		os<<"\n{\n";
		os<<"\tunsigned char ____buffer[1024];\n";
		os<<"\tCMessage m(____buffer);\n";
		os<<"\tMessage_Write(m,RPC_ID_"<<func.m_name<<");\n";

		for(CFunction::Parameters::iterator param0=func.m_params.begin();param0!=func.m_params.end();param0++)
		{
			CParameter& param=*param0;
			os<<"\tMessage_Write(m,"<<param.m_name<<");\n";
		}
		os<<"\treturn RPC_Send(sendTo,sendContext,m);\n";
		os<<"}\n\n";
	}
}

void WriteStubDecl(ostream &os,CSpecification* spec)
{
	os<<"RPCResult RPC_DoStub(RPCSendTo recvFrom,RPCSendContext recvContext,CMessage& m);\n";
}

void WriteStubImpl(ostream &os,CSpecification* spec)
{
	os<<"RPCResult RPC_DoStub(RPCSendTo recvFrom,RPCSendContext recvContext,CMessage& m)\n";
	os<<"{\n";
	os<<"\t"<<"RPCHeader msgID;"<<endl;
	os<<"\t"<<"m.SetCursor(0);"<<endl;
	os<<"\t"<<"Message_Read(m,msgID);"<<endl;
	os<<"\t"<<"switch(msgID)"<<endl;
	os<<"\t{"<<endl;

	for(CSpecification::Functions::iterator func0=spec->m_functions.begin();
		func0!=spec->m_functions.end();
		func0++)
	{
		CFunction& func=*func0;
		os<<"\t\tcase RPC_ID_"<<func.m_name<<":"<<endl;
		os<<"\t\t{\n";

		for(CFunction::Parameters::iterator param0=func.m_params.begin();param0!=func.m_params.end();param0++)
		{
			CParameter& param=*param0;
			os<<"\t\t\t"<<param.m_type<<" "<<param.m_name<<";\n";
			os<<"\t\t\tMessage_Read(m,"<<param.m_name<<");\n";
		}

		os<<"\t\t\t"<<func.m_name<<"(recvFrom,recvContext";
		WriteParametersWithCommaAndWithoutTypes(os,&func);
		os<<");\n";
		os<<"\t\t}\n";
		os<<"\t\tbreak;\n";
	}
	os<<"\t}\n";
	os<<"\treturn RPCResult_Ok;\n";
	os<<"}\n";
}

void WriteParametersWithCommaAndWithoutTypes(ostream &os,CFunction* func)
{
	for(CFunction::Parameters::iterator param0=func->m_params.begin();param0!=func->m_params.end();param0++)
	{
		CParameter& param=*param0;
		os<<",";
		os<<param.m_name;
	}
}