// Simple Lua console with limited vocabulary

// include the standard system headers.
#include <stdio.h>
#include <string.h>

/*
** Include the Lua headers. 
**	Note that they are "C" language headers.
**	Since we are C++ program, we need to let the C++ compiler 
**	know that the referenced prototypes and data will not have C++
**	namespace processing.
*/
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

/*
** This is an example of a LuaGlue function
** The function will be called from the Lua Environment
** Again, since Lua is written in ANSI C, we need to 
** turn off the C++ namespace function (extern "C")
*/
extern "C" int _Version(lua_State *L)
{
	puts("This is Version 1.0 of the Console Program");
	puts(LUA_VERSION);
	puts(LUA_COPYRIGHT);
	puts(LUA_AUTHORS);
	return 0;
}

/*
** This is where we hook up out "Glue" functions. 
** The program will register each of the defined functions 
** and allow them to be called from Lua.
*/
static luaL_reg ConsoleGlue[] = 
{
	{"Version",		_Version},
	{NULL, NULL}
};

char gpCommandBuffer[254];

const char *GetCommand(void)
{
	printf("Ready> ");
	return gets(gpCommandBuffer);
	puts("\n");
}

void main(void)
{
	// print the banner.
	puts("Lua Console (basic) (c) 2004 Charles River Media");
	puts("Enter Lua commands at the prompt, [QUIT] to exit\n\n");

	// Init the Lua environment
	lua_State *pLuaState = lua_open();
	luaopen_base(pLuaState);
	luaopen_io(pLuaState);
	luaopen_string(pLuaState);
	luaopen_math(pLuaState);
	luaopen_debug(pLuaState);
	luaopen_table(pLuaState);

	// this loop registers all of our LuaGlue functions with
	// the Lua Environment using the names we provide.
	for(int i=0; ConsoleGlue[i].name; i++)
	{
		lua_register(pLuaState, ConsoleGlue[i].name, ConsoleGlue[i].func);
	}


	// process commands
	const char *pCommand = GetCommand();
	while(stricmp(pCommand, "QUIT") != 0)
	{
		// send command to the lua environment
		if (luaL_loadbuffer(pLuaState, pCommand, strlen(pCommand), NULL) == 0)
		{
			if(lua_pcall(pLuaState, 0, LUA_MULTRET, 0) != 0)
			{
				// error on running the command
				printf("ERROR:%s\n", luaL_checkstring(pLuaState, -1));
			}
		}
		else
		{
			//error loading the command
			printf("ERROR:%s\n", luaL_checkstring(pLuaState, -1));
		}

		// get next command
		pCommand = GetCommand();
	}

	lua_close(pLuaState);
}
