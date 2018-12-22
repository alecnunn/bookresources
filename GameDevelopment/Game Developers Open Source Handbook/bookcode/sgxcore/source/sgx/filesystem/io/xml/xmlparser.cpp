#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"

sgxXMLParser *sgxXMLParser::ms_pSingleton;

sgxXMLParser::sgxXMLParser()
{
}

sgxXMLParser::~sgxXMLParser()
{
}

sgxXMLParser *sgxXMLParser::Create()
{
	if (!ms_pSingleton) {
		ms_pSingleton = new sgxXMLParser;
	}
	return ms_pSingleton;
}

sgxXMLParser *sgxXMLParser::Get()
{
	if (!ms_pSingleton) {
		ms_pSingleton = Create();
	}
	return ms_pSingleton;
}

tBOOL sgxXMLParser::Parse(sgxXMLTree &tree, CSGXFile &file)
{
	return FALSE;
}
