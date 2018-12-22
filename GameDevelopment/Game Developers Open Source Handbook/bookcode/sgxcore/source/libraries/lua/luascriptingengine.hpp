#ifndef SGXLIB_LUA_SCRIPTING_MANAGER_HPP
#define SGXLIB_LUA_SCRIPTING_MANAGER_HPP	1

class CLuaScriptState : public CScriptState {
public:
	CLuaScriptState(CScriptingEngine *pEng);
	~CLuaScriptState();
	virtual	tBOOL	Process(const sgxString &filename);
	virtual	tBOOL	Reset();
	virtual	tBOOL	Run();
	virtual	tBOOL	Step();
	virtual	tBOOL	Evaluate(const sgxString &code);

			tBOOL	DoCall(int status);
			tBOOL	LuaCall();
protected:
	virtual tBOOL	LuaLoad(const sgxString &filename);
	static const char *LuaLoadGetBuffer(lua_State *, void *ud, size_t *size);

private:
	lua_State	*m_LState;
	sgxString	m_Filename;
	tBOOL		m_bLoaded;

	tBOOL LuaReport(int status);
	tBOOL LuaCall(int narg, int clear);

	friend class CLuaScriptingEngine;
};


class CLuaScriptingEngine : public CScriptingEngine {
public:
	static CLuaScriptingEngine	*Create();

	virtual CScriptState	*Initialise();
	virtual	tBOOL			OpenLibrary(CScriptState *pState, const sgxString &libname);
	virtual	tBOOL			OpenStandardLibraries(CScriptState *pState);
	
protected:
    static CLuaScriptingEngine  *ms_pSingleton;
};

#endif /* SGXLIB_LUA_SCRIPTING_MANAGER_HPP */
