#include "sgx/scripting/scriptingengine.hpp"

CScriptState::CScriptState(CScriptingEngine *pEng)
{
	m_pEngine = pEng;
}

CScriptState::~CScriptState()
{
}

tBOOL CScriptState::Process(const sgxString &filename)
{
	return FALSE;
}

tBOOL CScriptState::Reset()
{
	return FALSE;
}

tBOOL CScriptState::Run()
{
	return FALSE;
}

tBOOL CScriptState::Step()
{
	return FALSE;
}

tBOOL CScriptState::Evaluate(const sgxString &code)
{
	return FALSE;
}

tBOOL	CScriptState::OpenLibrary(const sgxString &libname)
{
	return m_pEngine->OpenLibrary(this, libname);
}

tBOOL	CScriptState::OpenStandardLibraries()
{
	return m_pEngine->OpenStandardLibraries(this);
}


