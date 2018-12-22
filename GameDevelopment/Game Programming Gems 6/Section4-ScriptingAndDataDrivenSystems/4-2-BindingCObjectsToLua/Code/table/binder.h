// Table binder
// Uses table to bind object (natural extensibility)


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
        lua_setfield(L,-2,"_base");   // mt._base = base mt
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
    lua_rawget(L,LUA_ENVIRONINDEX);     // get object in env table
    if (lua_isnil(L,-1)) {
       lua_newtable(L);                 // create table to be the object
       lua_pushlightuserdata(L,udata);  // push address
       lua_setfield(L,-2,"_pointer");   // table._pointer = address
       luaL_getmetatable(L,tname);      // get metatable 
       lua_setmetatable(L,-2);          // set metatable for table
       lua_pushlightuserdata(L,udata);  // push address
       lua_pushvalue(L,-2);             // push table
       lua_rawset(L,LUA_ENVIRONINDEX);  // envtable[address] = table
    }
 }

  void *checkusertype (int index, const char *tname) {
     lua_getfield(L, LUA_REGISTRYINDEX, tname);   /* get metatable of given type */
     lua_getmetatable(L,index);                   /* get associated metatable */
     while (lua_istable(L,-1)) {
         if (lua_rawequal(L,-1,-2)) {
             lua_pop(L,2);                      // pop string and mt
             lua_getfield(L,index,"_pointer");  // get address
             void* udata = lua_touserdata(L,-1);
             return udata;
         }
         lua_getfield(L,-1,"_base");          // get mt._base
         lua_replace(L,-2);                   // replace: mt = mt._base
     }
     luaL_typerror(L,index,tname);
     return NULL;
 }

void* checkusertype (int index, char* tname) {
     lua_getmetatable(L, index);                // get metatable
     while (lua_istable(L,-1)) {                // while mt is valid 
         lua_pushvalue(L,-1);                   // duplicate mt
         lua_rawget(L, LUA_REGISTRYINDEX);      // get stored type name 
         const char *tn = lua_tostring(L, -1);
         if (tn && (strcmp(tn, tname) == 0)) {    // compare types
             lua_getfield(L,index,"_pointer");  // get address
             void* udata = lua_touserdata(L,-1);
             lua_pop(L,2);                      // pop string and mt
             return udata;
         }
         lua_pop(L,1);                          // pop string 
         lua_getfield(L,-1,"_base");            // get mt._base
         lua_replace(L,-2);                     // replace: mt = mt._base
     }
     lua_pop(L, 1);                             // pop mt
     luaL_typerror(L,index,tname);
     return NULL;
 }

private:
 lua_State* L;

};

