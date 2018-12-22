#ifndef SGXLIB_LUA_CORE_HPP
#define SGXLIB_LUA_CORE_HPP		1


#ifndef LUA_NUMBER
	#define LUA_NUMBER	float
#endif

extern "C" { 
	#include "libraries/lua/include/lua.h"
	#include "libraries/lua/include/lauxlib.h"
	#include "libraries/lua/include/lualib.h"
}

#endif /* SGXLIB_LUA_CORE_HPP */
