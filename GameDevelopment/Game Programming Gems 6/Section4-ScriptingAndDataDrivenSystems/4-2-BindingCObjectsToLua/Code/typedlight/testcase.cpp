#include <stdio.h>

#include "hero.h"
#include "binder.h" 

static int bnd_Create (lua_State* L)
{
    LuaBinder binder(L);
    Hero* h = new Hero(lua_tostring(L,1));
    binder.pushusertype(h,"Hero");
    return 1;
}

static int bnd_Destroy (lua_State* L)
{
    LuaBinder binder(L);
    Hero* hero = (Hero*) binder.checkusertype(1,"Hero");
    binder.releaseusertype(hero);
    delete hero;
    return 0;
}

static int bnd_GetName (lua_State* L)
{
    LuaBinder binder(L);
    Hero* hero = (Hero*)binder.checkusertype(1,"Hero");
    binder.pushstring(hero->GetName());
    return 1;
}

static int bnd_GetEnergy (lua_State* L)
{
    LuaBinder binder(L);
    Hero* hero = (Hero*)binder.checkusertype(1,"Hero");
    binder.pushnumber(hero->GetEnergy());
    return 1;
}

static int bnd_SetEnergy (lua_State* L)
{
    LuaBinder binder(L);
    Hero* hero = (Hero*)binder.checkusertype(1,"Hero");
    double n = binder.checknumber(2);
    hero->SetEnergy(n);
    return 0;
}

static const luaL_reg herolib[] = {
    {"Create", bnd_Create},
    {"Destroy", bnd_Destroy},
    {"GetName", bnd_GetName},
    {"GetEnergy", bnd_GetEnergy},
    {"SetEnergy", bnd_SetEnergy},
    {NULL, NULL}
};

int luaopen_hero (lua_State* L)
{
    LuaBinder binder(L);
    binder.init("Hero",herolib);
    return 1;
}

