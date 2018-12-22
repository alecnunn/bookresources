#ifndef SGX_SCRIPTING_SCRIPTING_MANAGER_HPP
#define SGX_SCRIPTING_SCRIPTING_MANAGER_HPP	1

#include "sgx/core/types/types.hpp"
#include "sgx/core/stl/stl.hpp"

typedef sgxMap<sgxString, CScriptingEngine *> ScriptMap;

class CScriptingManager {
public:
    static  CScriptingManager    *Get();

	tBOOL		Register(CScriptingEngine *pEngine, const sgxString &name);
	CScriptState *CreateInstance();
	CScriptState *CreateInstance(const sgxString &name);


protected:
    static  CScriptingManager    *ms_pSingleton;
    CScriptingManager();

private:
    virtual ~CScriptingManager();
    ScriptMap					  m_ScriptEngines;

};

#endif /* SGX_SCRIPTING_SCRIPTING_MANAGER_HPP */
