#include "sgx/core/core.hpp"
#include "sgx/scripting/scripting.hpp"
#include "libraries/lua/luacore.hpp"
#include "libraries/lua/luascriptingengine.hpp"

CLuaScriptingEngine  *CLuaScriptingEngine::ms_pSingleton;

static const luaL_reg lualibs[] = {
  {"base", luaopen_base},
  {"table", luaopen_table},
  {"io", luaopen_io},
  {"string", luaopen_string},
  {"math", luaopen_math},
  {"debug", luaopen_debug},
  {"loadlib", luaopen_loadlib},
  /* add your libraries here */

  /* Null terminator */
  {NULL, NULL}
};


CLuaScriptingEngine	*CLuaScriptingEngine::Create()
{
	sgxAssert(!ms_pSingleton && "The Lua scripting engine has already been created");
	if (!ms_pSingleton) {
		ms_pSingleton = new CLuaScriptingEngine;
	}
	CScriptingManager::Get()->Register(ms_pSingleton, "lua");

	return ms_pSingleton;
}

CScriptState *CLuaScriptingEngine::Initialise()
{
	return new CLuaScriptState(this);
}

tBOOL CLuaScriptingEngine::OpenLibrary(CScriptState *pState, const sgxString &libname)
{
	CLuaScriptState *pLuaState = SGX_STATIC_CAST(CLuaScriptState *, pState);

	const luaL_reg *lib = lualibs;
	for (; lib->func; lib++) {
		if (sgxStrcmp(libname.c_str(), lib->name) == 0) {
			lib->func(pLuaState->m_LState);  /* open library */
			lua_settop(pLuaState->m_LState, 0);  /* discard any results */
			return TRUE;
			}
	}
	return FALSE;
}

tBOOL CLuaScriptingEngine::OpenStandardLibraries(CScriptState *pState)
{
	const luaL_reg *lib = lualibs;
	for (; lib->func; lib++) {
		if (!OpenLibrary(pState, sgxString(lib->name))) {
			return FALSE;
		}
	}

	return TRUE;
}
	