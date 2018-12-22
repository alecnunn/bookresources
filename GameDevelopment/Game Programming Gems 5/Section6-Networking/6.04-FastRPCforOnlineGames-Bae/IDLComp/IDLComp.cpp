// IDLComp.cpp : Defines the entry point for the console application.
//
//
// Copyright 2004 Bae,Hyun-jik.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

#include "stdafx.h"
#include "IncludeGeneratedFiles.h"
#include "backend.h"

using namespace antlr;

string GetSubstitutedFileName(string originalFileName,const char* postfix,const char* extToSubsitute)
{
	char fullPath[_MAX_PATH];

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_fullpath(fullPath,originalFileName.c_str(),_MAX_PATH);
	_splitpath(fullPath,drive,dir,fname,ext);
	strcat(fname,postfix);
	strcpy(ext,extToSubsitute);

	_makepath(fullPath,drive,dir,fname,ext);

	return string(fullPath);
}

int main(int argc, _TCHAR* argv[])
{

	if(argc==1)
	{
		cout<<"A fast and simple RPC compiler authored by Bae,Hyun-jik(imays@hitel.net)\n";
		cout<<"Command line options: (.IDL file name)\n";
		return -1;
	}

	try 
	{
		RefCount<CSpecification> spec; // parse information
		string inputFileName=argv[1];

		// front-end: read file and parse it
		{
			ifstream fileIn; 

			fileIn.open(inputFileName.c_str());
			IDLLexer lexer(fileIn);
			IDLParser parser(lexer);
			spec=parser.specification(); 
		}

		// back-end: generate proxy and stub code
		{
			ofstream proxyh;
			proxyh.open(GetSubstitutedFileName(inputFileName,"_proxy","h").c_str());
			WriteHeaderStart(proxyh);
			proxyh<<endl;
			WriteFunctionIDs(proxyh,spec);
			proxyh<<endl;
			WriteFunctionDecls(proxyh,spec);
		}

		{
			ofstream proxycpp;
			proxycpp.open(GetSubstitutedFileName(inputFileName,"_proxy","cpp").c_str());
			WriteFunctionProxies(proxycpp,spec);			
		}

		{
			ofstream stubh;
			stubh.open(GetSubstitutedFileName(inputFileName,"_stub","h").c_str());
			WriteHeaderStart(stubh);
			stubh<<endl;
			WriteFunctionIDs(stubh,spec);
			stubh<<endl;
			WriteStubDecl(stubh,spec);
			stubh<<endl;
			WriteFunctionDecls(stubh,spec);
		}

		{
			ofstream stubcpp;
			stubcpp.open(GetSubstitutedFileName(inputFileName,"_stub","cpp").c_str());
			WriteStubImpl(stubcpp,spec);			
		}
	}
	catch( ANTLRException& e )
	{
		cerr << "exception: " << e.getMessage() << endl;
		return -1;
	}
	catch(exception& e)
	{
		cerr << "exception: " << e.what() << endl;
		return -1;
	}
	return 0;
}

