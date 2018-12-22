// Light binder
// Uses light userdata without type checking -- use is very limited


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
     luaL_register(L,tname,flist);
 }

 void pushusertype (void* udata, const char* tname) {
     lua_pushlightuserdata(L,udata);
 }

 void* checkusertype (int index, const char* tname) {
     void* udata = lua_touserdata(L,index);
     if (udata==0) luaL_typerror(L,index,tname);    
     return udata;
 } 
private:
 lua_State* L;
};

