// Simple Lua console with limited vocabulary
// Using cLua object and Lua library

// include the standard system headers.
#include <stdio.h>
#include <string.h>
#include <cLua.h>

LuaGlue _Version(lua_State *L)
{
	puts("This is Version 2.0 of the Console Program");
	return 0;
}

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
	puts("Lua Console (c) 2004 Charles River Media");
	puts("Enter Lua commands at the prompt, [QUIT] to exit\n\n");

	cLua *pLua = new cLua;

	pLua->AddFunction("Version", _Version);

	// process commands
	const char *pCommand = GetCommand();
	while(stricmp(pCommand, "QUIT") != 0)
	{
		// pass the string to cLua
		if(!pLua->RunString(pCommand))
		{
			printf("ERROR:%s\n", pLua->GetErrorString());
		}

		// get next command
		pCommand = GetCommand();
	}
	
	delete pLua;
}
