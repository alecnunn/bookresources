// Typed light binder
// Uses light userdata with type checking


extern "C" {
#include "lua.h" 
#include "lauxlib.h" 
}

class LuaBinder
{
public:
 LuaBinder (lua_State* L)
 : L(L)
 {
 }
 void pushnumber (double n)
 {
     lua_pushnumber(L,n);
 }
 double checknumber (int index)
 {
     return luaL_checknumber(L,index);
 }
 void pushstring (const char* s)
 {
     lua_pushstring(L,s); 
 }
 const char* checkstring (int index)
 {
  return luaL_checkstring(L,index);
 }
 void init (const char* tname, const luaL_reg *flist) {
     lua_newtable(L);                  // create table for type checking
     lua_replace(L,LUA_ENVIRONINDEX);  // set table as env table
     luaL_register(L,tname,flist);     // create libtable
 }

 void pushusertype (void* udata, const char* tname) {
     lua_pushlightuserdata(L,udata);   // push address
     lua_pushvalue(L,-1);              // duplicate address
     lua_pushstring(L,tname);          // push type name
     lua_rawset(L,LUA_ENVIRONINDEX);   // envtable[address] = type name
 }

 void* checkusertype (int index, const char* tname) {
     void* udata = lua_touserdata(L,index);
     if (udata==0 || !checktype(udata,tname))
         luaL_typerror(L,index,tname);
     return udata;
 } 

 void releaseusertype (void* udata) {
     lua_pushlightuserdata(L,udata);
     lua_pushnil(L);
     lua_settable(L,LUA_ENVIRONINDEX);
     printf("done %p\n",udata);
 }

private:
 lua_State* L;

 int checktype (void* udata, const char* tname) {
     lua_pushlightuserdata(L,udata);         // push address
     lua_rawget(L,LUA_ENVIRONINDEX);         // get envtable[address]
     const char* stored_tname = lua_tostring(L,-1);
     int result = stored_tname && strcmp(stored_tname,tname)==0;
     lua_pop(L,1);
     return result;
 }
 
};

