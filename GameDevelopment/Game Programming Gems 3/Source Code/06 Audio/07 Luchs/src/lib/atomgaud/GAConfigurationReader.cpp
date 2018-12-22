// GAConfigurationReader Implementation
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
#include "GAConfigurationReader.h"

#include <stack>

#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAConfigurationReader() { return( new CGAConfigurationReader); }
const CClass CGAConfigurationReader::classCGAConfigurationReader(CID_GAConfigurationReader, CID_GAObjectReader, "CGAConfigurationReader", CreateCGAConfigurationReader);
const IClass*	CGAConfigurationReader::GetClass() { return((const IClass*)&classCGAConfigurationReader); }


// CTOR
CGAConfigurationReader::CGAConfigurationReader()
{

}

// DTOR
CGAConfigurationReader::~CGAConfigurationReader()
{
	Terminate();
}

// explicit terminate baseclass
void CGAConfigurationReader::Terminate()
{

	CGAObjectReader::Terminate();
}

// build objects from tokens
// status:
// 1 cid read, assuming next token is name
UINT32 CGAConfigurationReader::Process()
{
	UINT32 nToken = m_Tokenizer.GetTokenCount();
	IGAObject* pob = NULL;
	IGAObject* pCurrObject = NULL;
	IDataLocations* pLocations = NULL;
	IDataLocation* pLocation = NULL;
	UINT32 nStatus=0;
	LPTSTR pszParamName = NULL;
	TCHAR szParamValue[256];
	szParamValue[0] = 0;
	IGASignal* pSignal = NULL;
	IEngine* pEngine = GetSystem()->GetEngine();


	std::stack<IGAObject*> obstack;


	for(UINT32 i=0;i < nToken; i++)
	{

		ATOM_CHAR*	psz = m_Tokenizer.GetToken(i); 

		// if this is a classname, create a new object
		pob = (IGAObject*)pEngine->CreateObject(psz);

		if(pob != NULL) { // handle objects
			pCurrObject = pob;
			obstack.push(pob);

			nStatus = 1;

			IClass* pClass = (IClass*)pCurrObject->GetClass();
			if(pClass->IsA(CID_DataLocations)) {
				pLocations = (IDataLocations*) pCurrObject;
				g_GASystem.SetDataLocations(pLocations);
			}
			else if(pClass->IsA(CID_DataLocation)) {
				pLocation = (IDataLocation*) pCurrObject;

				if(pLocations != NULL)
					pLocations->Append(pLocation);

			}
			
			else if(pClass->IsA(CID_GASignal)) {
				
				pSignal = (IGASignal*) pCurrObject;
				g_GASystem.GetSignals()->Append(pSignal);

			}
			else if(pClass->IsA(CID_GASignalFormat)) {
				
				IGASignalFormat* pFormat = (IGASignalFormat*) pCurrObject;
				g_GASystem.SetSignalFormat(pFormat);
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
					pCurrObject->SetParameter(pszParamName, szParamValue);
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
