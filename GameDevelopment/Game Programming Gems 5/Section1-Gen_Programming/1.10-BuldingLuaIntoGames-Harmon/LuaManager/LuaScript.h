/*----------------------------------------------------------------------------
    LUA SCRIPT
    Class to manage a single Lua thread/state/script.

    contact: Matthew Harmon matt@matthewharmon.com
*/

/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/

#ifndef _LUASCRIPT_H
#define _LUASCRIPT_H


/*--------------------------------------------------------------------------*/
/*                              Include Files                               */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                            Defined Constants                             */
/*--------------------------------------------------------------------------*/

typedef enum luaScriptStateTag
    {
    LSS_WAITFRAME,
    LSS_WAITTIME,
    LSS_RUNNING,
    LSS_NOTLOADED,
    LSS_DONE
    } LUASCRIPTSTATE;


/*--------------------------------------------------------------------------*/
/*                              Data Structures                             */
/*--------------------------------------------------------------------------*/

class LUAMANAGER;
typedef struct lua_State lua_State;

class LUASCRIPT
    {
    public:                         // ---------------------------------------

    LUAMANAGER*     manager;            // LUAMANAGER object that created this script
    LUASCRIPT*      next;
    LUASCRIPTSTATE  state;              // are we waiting, not loaded yet, etc.
    float           waitTimestamp;      // time to wake up
    int             waitFrame;          // number of frames to wait
    float           time;               // current time

                    LUASCRIPT          (LUAMANAGER* mgr);
                    ~LUASCRIPT         (void);
    void            CreateThread       (void);
    LUASCRIPT*      Update             (float       elapsedSec);
    void            RunFile            (char*       fileName);
    int             RunString          (char*       commandString);
    void            CallFn             (char*       fnName,
                                        int         iParam);
    void            AbortWait          (void);

    private:                        // ---------------------------------------

    lua_State*      threadState;
    char            lastErrorString[256];
    void            ResumeScript       (float   param);
    void            FormatError        (void);
    void            OutputError        (char*   strType);
    };


/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/

#endif // _LUASCRIPT_H

