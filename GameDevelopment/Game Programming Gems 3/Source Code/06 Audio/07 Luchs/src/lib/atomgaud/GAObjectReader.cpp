// GAObjectReader Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GAObjectReader.h"

#include <stack>

#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAObjectReader() { return( new CGAObjectReader); }
const CClass CGAObjectReader::classCGAObjectReader(CID_GAObjectReader, CID_GAFile, "CGAObjectReader", CreateCGAObjectReader);
const IClass*	CGAObjectReader::GetClass() { return((const IClass*)&classCGAObjectReader); }


// CTOR
CGAObjectReader::CGAObjectReader()
{
	m_pszText = NULL;
	m_szPathName[0] = 0;
}

// DTOR
CGAObjectReader::~CGAObjectReader()
{
	Terminate();
}

// explicit terminate baseclass
void CGAObjectReader::Terminate()
{
	ATOM_FREE(m_pszText);

	CGAFile::Terminate();
}

// return the current used full qualified path+filename
ATOM_CHAR* CGAObjectReader::GetPathName()
{
	return(m_szPathName);
}


// read textfile completly
UINT32 CGAObjectReader::Read(const ATOM_CHAR* pszPathName)
{
	if(pszPathName == NULL)
		return(0);
	// make media path, if available and pszPathName is not fully qualified
	m_szPathName[0]=0;

	if(pszPathName[1] != ':' ) {

		// try specified data locations
		IDataLocations* pLocations = g_GASystem.GetDataLocations();
		if(pLocations != NULL) {
			IDataLocation* pLocation = pLocations->FindByType("Patch");

			if(pLocation != NULL) {
				ATOM_CHAR* pszPath = pLocation->GetLocation();
				_tcscpy(m_szPathName,pszPath); // todo: make sure last char is '\'
			}
		}
		else { // try application root
			IEngine* pEngine = GetSystem()->GetEngine();
			IApplication* pApp = pEngine->GetApplication();
			if(pApp != NULL)
				pApp->GetRoot(m_szPathName);

		}
	}

	_tcscat(m_szPathName, pszPathName);

	UINT32 nBytes = 0;
	BOOL b = Open(m_szPathName, IGAFile::ReadBin);

	if(b == TRUE) {
		nBytes = GetSize();

		// if open was successful, add this path to the locations:
		//IDataLocations* pLocations = g_GASystem.GetDataLocations();
		//if(pLocations != NULL) {
		//	pLocations->Append("Audio", pszPathName);

		ATOM_FREE(m_pszText);
		m_pszText = (ATOM_CHAR*)calloc(nBytes, 1);
		CGAFile::Read(m_pszText, nBytes);
		Close();
		m_Tokenizer.Process(m_pszText);
	//	m_Tokenizer.Dump();

		Process();	
	}

	return(nBytes);
}

// build objects from tokens
// status:
// 1 cid read, assuming next token is name
UINT32 CGAObjectReader::Process()
{
	UINT32 nToken = m_Tokenizer.GetTokenCount();
	IGAObject* pob = NULL;
	IGAObject* pCurrObject = NULL;
	IGAPatch* pPatch = NULL;
	IGAProcessor* pProcessor = NULL;
	UINT32 nStatus=0;
	LPTSTR pszParamName = NULL;
	TCHAR szParamValue[256];
	szParamValue[0] = 0;
	IGASignal* pSignal = NULL;

//	GetSystem()->GetEngine()->Output("CGAObjectReader - begin");
	std::stack<IGAObject*> obstack;


	for(UINT32 i=0;i < nToken; i++)
	{

		ATOM_CHAR*	psz = m_Tokenizer.GetToken(i); 
	//	GetSystem()->GetEngine()->Output(psz);

		// if this is a classname, create a new object
		pob = (IGAObject*)g_GASystem.CreateObject(psz);

		if(pob != NULL) { // handle objects
			pCurrObject = pob;
			obstack.push(pob);

			nStatus = 1;

			IClass* pClass = (IClass*)pCurrObject->GetClass();
			if(pClass->IsA(CID_GAPatch)) {
				pPatch = (IGAPatch*) pCurrObject;
				g_GASystem.GetPatches()->Append(pPatch);
			}
			else if(pClass->IsA(CID_GAProcessor)) {
				pProcessor = (IGAProcessor*) pCurrObject;

				if(pPatch != NULL)
					pPatch->GetProcessors()->Append(pProcessor);

			}
			else if(pClass->IsA(CID_GASignal)) {
				
				pSignal = (IGASignal*) pCurrObject;
				g_GASystem.GetSignals()->Append(pSignal);

			}
		}
		else { // handle parameters for current object

		
			if(*psz == '{')  {// object begin
				nStatus = 2;
				continue;
			}
			else if(*psz == '}') { // object end

				obstack.pop();
				nStatus = 3;
				continue;

			}
			else if(*psz == '=') {// param begin
				nStatus = 4;
				continue;
			}
			else if(*psz == ';') { // param end
				nStatus = 5;

				// set parameter
				if(pszParamName != NULL && szParamValue[0] != 0) {

					pCurrObject = obstack.top();

				//	{
				//	GetSystem()->GetEngine()->Output(szBuffer);
				//	}

					if(pCurrObject != NULL) {
				//		char szBuf[256];
				//		sprintf(szBuf, "Param %s=%s\r\n",pszParamName,szParamValue );
				//		::OutputDebugString(szBuf);
						pCurrObject->SetParameter(pszParamName, szParamValue);

					}

					pszParamName=NULL;
					szParamValue[0] = 0;

				}
				continue;
			}
			else if(*psz == ',') { // param separator
				nStatus = 6;
				continue;
			}
		}



		if(nStatus == 1 ) {// assuming name of created object following immediately after classname
			pCurrObject->SetName(psz);
			continue;
		}

		else if(nStatus == 2 || nStatus == 3 || nStatus == 5) {
			pszParamName = psz;
		}
		else if(nStatus == 4) { // first param value
			_tcscpy(szParamValue,psz);
		}

		// append param value
		if(nStatus == 6  && szParamValue[0] != 0) {

			_tcscat(szParamValue, _T(","));
			_tcscat(szParamValue, psz);

		}

	}



	return(0);
}






#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
