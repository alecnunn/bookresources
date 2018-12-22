/*----------------------------------------------------------------------------
    LUA MANAGER.H
    Manages the single, master Lua state.

    contact: Matthew Harmon matt@matthewharmon.com
*/

/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/

// signal header has been included
#ifndef _LUAMANAGER_H
#define _LUAMANAGER_H


/*--------------------------------------------------------------------------*/
/*                              Data Structures                             */
/*--------------------------------------------------------------------------*/

class LUASCRIPT;
typedef struct lua_State lua_State;

class LUAMANAGER
    {
    public:                         // ---------------------------------------
    lua_State*  masterState;

                LUAMANAGER         (void);
                ~LUAMANAGER        (void);

    LUASCRIPT*  CreateScript       (void);
    void        UnlinkScript       (LUASCRIPT*  s);
    void        Update             (float       elapsedSec);
    int         NumScripts         (void);

    private:                        // ---------------------------------------

    LUASCRIPT*  head;
    };


/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/

#endif // _LUAMANAGER_H
