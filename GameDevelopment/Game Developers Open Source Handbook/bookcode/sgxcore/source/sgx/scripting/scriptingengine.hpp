#ifndef SGX_SCRIPTING_SCRIPTING_ENGINE_HPP
#define SGX_SCRIPTING_SCRIPTING_ENGINE_HPP	1

#include "sgx/core/types/types.hpp"
#include "sgx/core/helpers/trace.hpp"
#include "sgx/scripting/scriptingengine.hpp"

class CScriptingEngine;

class CScriptState {
public:
	CScriptState(CScriptingEngine *);
	virtual ~CScriptState();

	virtual	tBOOL	Process(const sgxString &filename);

	virtual	tBOOL	Reset();
	virtual	tBOOL	Run();
	virtual	tBOOL	Step();
	virtual	tBOOL	Evaluate(const sgxString &code);

	virtual	tBOOL	OpenLibrary(const sgxString &libname);
	virtual	tBOOL	OpenStandardLibraries();

protected:
	CScriptingEngine	*m_pEngine;
private:
	CScriptState() { sgxError("Ctor must be called with the engine pointer"); }
};


class CScriptingEngine {
public:
	virtual	CScriptState *Initialise();
	virtual	tBOOL		OpenLibrary(CScriptState *pState, const sgxString &libname);
	virtual	tBOOL		OpenStandardLibraries(CScriptState *pState);
};

#endif /* SGX_SCRIPTING_SCRIPTING_ENGINE_HPP */
