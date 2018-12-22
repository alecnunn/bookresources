#include "sgx/scripting/scriptingengine.hpp"

CScriptState *CScriptingEngine::Initialise()
{
	return 0;
}

tBOOL CScriptingEngine::OpenLibrary(CScriptState *pState, const sgxString &libname)
{
	return FALSE;
}

tBOOL CScriptingEngine::OpenStandardLibraries(CScriptState *pState)
{
	return FALSE;
}
