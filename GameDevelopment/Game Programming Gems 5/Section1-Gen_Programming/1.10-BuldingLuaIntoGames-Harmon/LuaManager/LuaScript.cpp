/*----------------------------------------------------------------------------
    LUA SCRIPT.CPP
    Object to manage a single Lua thread/coroutine script.

    contact: Matthew Harmon matt@matthewharmon.com
*/

/*--------------------------------------------------------------------------*/
/*                              Include Files                               */
/*--------------------------------------------------------------------------*/

#include    <stdio.h>
#include    <string.h>
#include    <assert.h>
#include    "LuaScript.h"
#include    "LuaManager.h"
extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
    }


/*----------------------------------------------------------------------------
    CONSTRUCTOR
*/

LUASCRIPT::LUASCRIPT(
    LUAMANAGER*     mgr)
    {
    manager             = mgr;
    state               = LSS_NOTLOADED;
    time             = 0;
    strcpy(lastErrorString, "No error.\n");

    // create a thread/state for this object
    threadState = lua_newthread(manager->masterState);
    
    // save a pointer to the thread manager object in the global table
    // using the new thread's vm pointer as a key
    lua_pushlightuserdata(manager->masterState, threadState);
    lua_pushlightuserdata(manager->masterState, this );
    lua_settable(manager->masterState, LUA_GLOBALSINDEX );
    }


/*----------------------------------------------------------------------------
    DESTRUCTOR
*/

LUASCRIPT::~LUASCRIPT()
    {
    // Unhook from the script manager
    manager->UnlinkScript(this);
    }


/*----------------------------------------------------------------------------
    RUN FILE
*/

void LUASCRIPT::RunFile(
    char*       fileName)
    {
    int         status;

    assert(manager->masterState);
    assert(threadState);

    status = luaL_loadfile(threadState, fileName);
    if (status == 0)
        {
        ResumeScript(0.0f);
        }
    else 
        {
        FormatError();
        OutputError("Syntax Error");
        }
    }


/*----------------------------------------------------------------------------
    RUN STRING
*/

int LUASCRIPT::RunString(
    char*       commandString)
    {
    int         status;

    assert(manager->masterState);
    assert(threadState);

    status = luaL_loadbuffer(threadState, commandString, strlen(commandString), "Console");
    if (status == 0)
        status = lua_pcall(threadState, lua_gettop(threadState)-1, 0, 0);

    if (status)
        {
        FormatError();
        return(-1);
        }

    return(0);
    }


/*----------------------------------------------------------------------------
    UPDATE
    The purpose of update is basically to check if any suspended scripts need
    to be turned back on again!

    Returns the next script in the list.  This assists in linked list traversal
    where this script may delete it's next sibling.

*/

LUASCRIPT* LUASCRIPT::Update(
    float       elapsedSec)
    {
    time += elapsedSec;

    switch(state)
        {
        case LSS_WAITTIME:
            if (time >= waitTimestamp)
                ResumeScript(0.0f);
            break;
        case LSS_WAITFRAME:
            waitFrame--;
            if (waitFrame <= 0)
                ResumeScript(0.0f);
            break;
        case LSS_NOTLOADED:
            break;
//        case LSS_DONE:
        default :
            // if we aren't waiting and we get here, the script must be done
//            if (autoDelete)
//                {
//                LUASCRIPT*  sNext;
//
//                sNext = next;
//                delete(this);
//                return(sNext);
//                }
            break;
        }

    return(next);
    }


/*----------------------------------------------------------------------------
    ABORT WAIT
    Resumes esecution of the script, pushing 1 as a return value from the
    call that suspended the script.
*/

void LUASCRIPT::AbortWait()
    {
    ResumeScript(1.0f);
    }


/*----------------------------------------------------------------------------
    RESUME SCRIPT
    Runs or resumes a LUA script.  Executes until a yeild is encountered or
    the script terminates.

    The parameter to this function is pushed on the stack as the return value
    of the previous yield.
*/

void LUASCRIPT::ResumeScript(
    float       param)
    {
    int         status;

    // we're about to run/resume the thread, so set the global
    state  = LSS_RUNNING;

    // param is treated as a return value from the function that yielded
    lua_pushnumber(threadState, param);

    status = lua_resume(threadState, 1);

    if (status)
        {
        FormatError();
        OutputError("Runtime Error:");
        }
    }


/*----------------------------------------------------------------------------
    CALL FN
    Calls a Lua function.  Shows how to pass a single parameter, but not how
    to retrieve return values.
*/

void LUASCRIPT::CallFn(
    char*       fnName,
    int         iParam)
    {
    int         status;

    // find the lua function and push it on the stack
    lua_getglobal (threadState, fnName);

    // push our single argument
    lua_pushnumber(threadState, iParam);

    // now, call into Lua
    status = lua_pcall(threadState, 1, 0, 0);

    if (status)
        {
        FormatError();
        OutputError("Runtime Error:");
        }
    }


/*----------------------------------------------------------------------------
    FORMAT ERROR
    Grabs the last error message and stuffs it in lastErrorString.
*/

void LUASCRIPT::FormatError()
    {
    const char* msg;

    msg = lua_tostring(threadState, -1);
    if (msg == NULL)
        msg = "(error with no message)";
    lua_pop(threadState, 1);

    strcpy(lastErrorString, msg);
    }


/*----------------------------------------------------------------------------
    OUTPUT ERROR
*/

void LUASCRIPT::OutputError(
    char*   strType)
    {
    printf("%s %s \n", strType, lastErrorString);
    }