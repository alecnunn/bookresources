#include "sgx/core/types/types.hpp"
#include "sgx/core/stl/stl.hpp"
#include "sgx/core/helpers/trace.hpp"
#include "sgx/scripting/scripting.hpp"

CScriptingManager  *CScriptingManager::ms_pSingleton;

CScriptingManager::CScriptingManager()
{
}

CScriptingManager::~CScriptingManager()
{
}

CScriptingManager *CScriptingManager::Get()
{
	if (!ms_pSingleton) {
		ms_pSingleton = new CScriptingManager;
	}
	return ms_pSingleton;
}

tBOOL 
CScriptingManager::Register(CScriptingEngine *pEngine, const sgxString &name)
{
    return m_ScriptEngines.insert(ScriptMap::value_type(name, pEngine)).second ? TRUE : FALSE;
}

CScriptState *CScriptingManager::CreateInstance()
{
	if (m_ScriptEngines.begin() == m_ScriptEngines.end()) {
		return NULL;
	}

	return (*m_ScriptEngines.begin()).second->Initialise();
}

CScriptState *CScriptingManager::CreateInstance(const sgxString &name)
{
    ScriptMap::iterator it = m_ScriptEngines.find(name);

    if (it == m_ScriptEngines.end()) {
        return NULL;
    }

	return (*it).second->Initialise();
}
