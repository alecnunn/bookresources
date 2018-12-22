#include <stdio.h>

#include "hero.h"
#include "binder.h" 

static int bnd_GetName (lua_State* L)
{
    LuaBinder binder(L);
    Actor* actor = (Hero*)binder.checkusertype(1,"Actor");
    binder.pushstring(actor->GetName());
    return 1;
}

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
    delete hero;
    return 0;
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

static const luaL_reg actorlib[] = {
    {"GetName", bnd_GetName},
    {NULL, NULL}
};

static const luaL_reg herolib[] = {
    {"Create", bnd_Create},
    {"GetEnergy", bnd_GetEnergy},
    {"SetEnergy", bnd_SetEnergy},
    {NULL, NULL}
};

int luaopen_hero (lua_State* L)
{
    LuaBinder binder(L);
    binder.init("Actor",0,actorlib,0);
    binder.init("Hero","Actor",herolib,bnd_Destroy);
    return 1;
}

