// Full binder
// Uses full userdata with inheritance


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
 void init (const char* tname, const char* bname, 
	       const luaL_reg *flist, int (*destroy) (lua_State*)) { 
     lua_newtable(L);                 // create table for uniqueness
     lua_pushstring(L,"v");
     lua_setfield(L,-2,"__mode");     // set as weak-value table
     lua_pushvalue(L,-1);             // duplicate table
     lua_setmetatable(L,-2);          // set itself as metatable
     lua_replace(L,LUA_ENVIRONINDEX); // set table as env table
     luaL_register(L,tname,flist);    // create libtable 
     luaL_newmetatable(L,tname);      // create metatable for objects
     lua_pushvalue(L,-2);
     lua_setfield(L,-2,"__index");    // mt.__index = libtable
     lua_pushcfunction(L,destroy);
     lua_setfield(L,-2,"__gc");       // mt.__gc = destroy
     if (bname) {
         luaL_getmetatable(L,bname);
         lua_setfield(L,-2,"_base");  // mt._base = base mt
     }
     lua_pop(L,1);                    // pop metatable
     if (bname) {
         lua_getfield(L,LUA_GLOBALSINDEX,bname);
	 lua_setfield(L,-2,"__index"); // libtable.__index = base libtable
         lua_pushvalue(L,-1);          // duplicate libtable
         lua_setmetatable(L,-2);       // set itself as metatable
     }
 } 

 void pushusertype (void* udata, char* tname) {
     lua_pushlightuserdata(L,udata);
     lua_rawget(L,LUA_ENVIRONINDEX);     // get udata in env table
     if (lua_isnil(L,-1)) {
         void** ubox = (void**)lua_newuserdata(L,sizeof(void*));
         *ubox = udata;                    // store address in udata
         luaL_getmetatable(L,tname);     // get metatable 
         lua_setmetatable(L,-2);         // set metatable for udata
         lua_pushlightuserdata(L,udata); // push address
         lua_pushvalue(L,-2);            // push udata
         lua_rawset(L,LUA_ENVIRONINDEX); // envtable[address] = udata
     }
 }

 void *checkusertype (int index, const char *tname) {
     lua_getfield(L, LUA_REGISTRYINDEX, tname);   /* get metatable of given type */
     lua_getmetatable(L,index);                   /* get associated metatable */
     while (lua_istable(L,-1)) {
         if (lua_rawequal(L,-1,-2)) {
             lua_pop(L,2);
             return *((void**)lua_touserdata(L,index));
         }
         lua_getfield(L,-1,"_base");          // get mt._base
         lua_replace(L,-2);                   // replace: mt = mt._base
     }
     luaL_typerror(L,index,tname);
     return NULL;
 }

private:
 lua_State* L;
};

