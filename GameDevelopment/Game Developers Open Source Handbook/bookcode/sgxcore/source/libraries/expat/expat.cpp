#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"

#include "libraries/expat/xmlparse/xmlparse.h"
#include "libraries/expat/expat.hpp"

struct expat_state {
	sgxXMLNode				*pCurrentRoot;
	sgxVector<sgxXMLNode *>	NodeStack;
};


sgxXMLParser *sgxXMLExpatParser::Create()
{
	if (!ms_pSingleton) {
		ms_pSingleton = new sgxXMLExpatParser;
	}
	return ms_pSingleton;
}

sgxXMLExpatParser::sgxXMLExpatParser()
{
}

sgxXMLExpatParser::~sgxXMLExpatParser()
{
}

// ---------------

void startElement(void *userData, const char *name, const char **atts)
{
	expat_state *pState = (expat_state *)userData;

	sgxXMLNode	*pTag = new sgxXMLNode;

	pTag->SetElement(sgxXMLElement(name));

	const char **p = atts;
	while(*p) {
		pTag->AddAttribute(sgxString(*p), sgxString(*(p+1)));
		p+=2;
	}

	pState->pCurrentRoot = pState->pCurrentRoot->AddNode(pTag);
	pState->NodeStack.push_back(pState->pCurrentRoot);
}

void endElement(void *userData, const char *name)
{
	expat_state *pState = (expat_state *)userData;

	pState->NodeStack.pop_back();
	pState->pCurrentRoot = pState->NodeStack.back();
}

void dataHandler(void *userData, const XML_Char *s, int len)
{
	expat_state *pState = (expat_state *)userData;

	pState->pCurrentRoot->AppendData(sgxString(s, len));
}


tBOOL sgxXMLExpatParser::Parse(sgxXMLTree &tree, CSGXFile &file)
{
char buf[1024];	// this is arbitrary
int done;
int depth = 0;
expat_state state;

	state.pCurrentRoot = tree.GetRootNode();
	state.NodeStack.push_back(state.pCurrentRoot);

	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, &state);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, dataHandler);

	do {
		int len = file.Read((tBYTE *)buf, sizeof(buf));

		done = len < sizeof(buf);

		if (!XML_Parse(parser, buf, len, done)) {
			sgxTrace(
			"%s at line %d\n",
			XML_ErrorString(XML_GetErrorCode(parser)),
			XML_GetCurrentLineNumber(parser));
			return FALSE;
		}
	} while (!done);

	XML_ParserFree(parser);
	return TRUE;
}
