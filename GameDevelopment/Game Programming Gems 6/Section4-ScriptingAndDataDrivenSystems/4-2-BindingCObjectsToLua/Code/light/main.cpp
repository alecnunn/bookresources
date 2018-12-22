/*
** Binding C/C++ objects to Lua
** W. Celes, Luiz H. de Figueiredo, R. Ierusalismchy
**
** This code initializes the Lua library and the binding code.
*/


extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


int luaopen_hero (lua_State* L);

static int pmain (lua_State* L)
{
    luaL_openlibs(L);  /* open standard libraries */
    luaopen_hero(L);
    return 0;
}

int main (int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr,"usage: testcase filename.lua\n");
        return 1;
    }

    /* Open libraries */
    lua_State *L = lua_open();  /* create state */
    if (L == NULL) {
        fprintf(stderr,"cannot create Lua state\n");
        return 1;
    }
    lua_pushcfunction(L,pmain);
    if (lua_pcall(L,0,0,0)) {
        fprintf(stderr,"%s\n",lua_tostring(L,-1));
        return 1;
    }

    /* Execute Lua code */
    if (luaL_loadfile(L,argv[1])) {
        fprintf(stderr,"cannot load file %s\n", argv[1]);
        return 1;
    }
    if (lua_pcall(L,0,0,0)) {
        fprintf(stderr,"%s\n",lua_tostring(L,-1));
        return 1;
    }

    return 0;
}

