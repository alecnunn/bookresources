/*----------------------------------------------------------------------------
    LUAMANAGER.H
    Lua Script Manager.
    contact: Matthew Harmon matt@matthewharmon.com
*/

/*--------------------------------------------------------------------------*/
/*                              Include Files                               */
/*--------------------------------------------------------------------------*/

#include    <stdio.h>
extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
    }
#include    "LuaScript.h"
#include    "LuaManager.h"
#include    "LuaScriptLib.h"


/*----------------------------------------------------------------------------
    CONSTRUCTOR
*/

LUAMANAGER::LUAMANAGER()
    {
    masterState = lua_open();

    if (masterState)
        {
        // open any auxiliary libaraies we may need
        lua_baselibopen (masterState); 
      //lua_iolibopen   (masterState); 
      //lua_strlibopen  (masterState); 
        lua_mathlibopen (masterState); 
        // including our own!
        LuaOpenScriptLib(masterState);
        }

    head            = NULL;
    }


/*----------------------------------------------------------------------------
    DESTRUCTOR
    Destroy all children scripts we are managing, and then close the Lua state.
*/

LUAMANAGER::~LUAMANAGER()
    {
    LUASCRIPT*  s;
    LUASCRIPT*  next;

    // destroy all children scripts
    s = head;
    while (s)
        {
        // save the next pointer in case the script is deleted
        next = s->next;
        delete s;
        s = next;
        }

    lua_close(masterState);
    }


/*----------------------------------------------------------------------------
    CREATE SCRIPT
    Creates a script object.
*/

LUASCRIPT* LUAMANAGER::CreateScript()  
    {
    LUASCRIPT*  s;

    // create a script object
    s = new LUASCRIPT(this);

    // tell the script object who its manager is
    s->manager = this;

    // link the new script into the list
    s->next = head;
    head = s;

    return(s);
    }


/*----------------------------------------------------------------------------
    UNLINK SCRIPT
    Unhooks a script from the linked list of scripts.
*/

void LUAMANAGER::UnlinkScript(
    LUASCRIPT*  s)
    {
    LUASCRIPT*  prev;

    // if s is at the head, simply unlink it
    if (head == s)
        {
        head = s->next;
        return;
        }

    // find previous link
    prev = head;
    while (prev)
        {
        if (prev->next == s)
            {
            prev->next = s->next;
            return;
            }
        prev = prev->next;
        }
    }


/*----------------------------------------------------------------------------
    UPDATE
    Call every script's update routine.
    For convenience, scripts return their child when Update is called.  This
    helps when a script destroys itself during Update.
*/

void LUAMANAGER::Update(
    float       elapsedSec)
    {
    LUASCRIPT*  s;

    s = head;
    while (s)
        {
        s = s->Update(elapsedSec);
        }
    }