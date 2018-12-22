#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "sgx/scripting/scripting.hpp"
#include "libraries/lua/luacore.hpp"
#include "libraries/lua/luascriptingengine.hpp"

CLuaScriptState::CLuaScriptState(CScriptingEngine *pEng) : CScriptState(pEng)
{
	CLuaScriptingEngine *pLSE = SGX_STATIC_CAST(CLuaScriptingEngine *, pEng);

	m_LState = lua_open();  /* create state */
	m_bLoaded = FALSE;
}

CLuaScriptState::~CLuaScriptState()
{
	lua_close(m_LState);
}

static int StateMain(lua_State *l)
{
CLuaScriptState *pLState = (CLuaScriptState *)lua_touserdata(l, 1);

	return pLState->LuaCall();
}

#define SGX_LUA_BUFFERSIZE		1024
typedef struct LoadF {
  CSGXFile *pFile;
  char buff[SGX_LUA_BUFFERSIZE];
} LoadF;

const char *CLuaScriptState::LuaLoadGetBuffer(lua_State *, void *ud, size_t *size) 
{
  LoadF *lf = (LoadF *)ud;
  *size = lf->pFile->Read((tBYTE *)lf->buff, SGX_LUA_BUFFERSIZE);
  return (*size > 0) ? lf->buff : NULL;
}

tBOOL CLuaScriptState::LuaLoad(const sgxString &filename) 
{
CSGXFile file(filename);
LoadF lf;
int fnameindex = lua_gettop(m_LState) + 1;  /* index of filename on the stack */
int status;

	if (!file.IsValid()) {
		return FALSE;
	}

	lua_pushfstring(m_LState, "@%s", filename.c_str());
	lf.pFile = &file;
	status = lua_load(m_LState, LuaLoadGetBuffer, &lf, lua_tostring(m_LState, -1));
	file.Close();

	if (status) {
		lua_settop(m_LState, fnameindex);
		return FALSE;
	}

	return TRUE;
}


tBOOL CLuaScriptState::LuaCall()
{
	if (!m_bLoaded) {
		LuaLoad(m_Filename);
		m_bLoaded = TRUE;
	}

	return DoCall(0);
}

tBOOL CLuaScriptState::Run()
{
	int status;
	status = lua_cpcall(m_LState, &StateMain, this);
	return status==0 ? TRUE : FALSE;
}

tBOOL CLuaScriptState::Process(const sgxString &filename)
{
	m_Filename = filename;
	m_bLoaded = FALSE;
	return TRUE;
}

tBOOL CLuaScriptState::Evaluate(const sgxString &code)
{
	return lua_dostring(m_LState, code.c_str()) == 0 ? TRUE : FALSE;
}

tBOOL CLuaScriptState::Step()
{
	return FALSE;
}

tBOOL CLuaScriptState::Reset()
{
	return FALSE;
}

tBOOL CLuaScriptState::LuaReport(int status)
{
	if (status) {
		sgxTrace(lua_tostring(m_LState, -1));
		lua_pop(m_LState, 1);
	}
	return TRUE;
}

tBOOL CLuaScriptState::DoCall(int status)
{
	if (status == 0) {
		status = LuaCall(0, 1);
	}
	return LuaReport(status);
}

tBOOL CLuaScriptState::LuaCall(int narg, int clear)
{
  int status;
  int base = lua_gettop(m_LState) - narg;  /* function index */

  lua_pushliteral(m_LState, "_TRACEBACK");
  lua_rawget(m_LState, LUA_GLOBALSINDEX);  /* get traceback function */
  lua_insert(m_LState, base);  /* put it under chunk and args */
//  signal(SIGINT, laction);
  status = lua_pcall(m_LState, narg, (clear ? 0 : LUA_MULTRET), base);
//  signal(SIGINT, SIG_DFL);
  lua_remove(m_LState, base);  /* remove traceback function */
  return status;
}

