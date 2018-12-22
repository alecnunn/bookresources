/* SGX Core - A cross-platform set of library abstractions for game development.
   Copyright (C) 1992-2006
   Steven Goodwin

  Version 9.3

The SGX Core is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by the Free
Software Foundation.

The SGX Core is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#include "drivers/wintel32/wintel32core.hpp"

#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "sgx/scripting/scripting.hpp"
#include "sgx/system/system.hpp"

#include "drivers/wintel32/filesystem/filesystem.hpp"
#include "drivers/wintel32/core/helpers/wintrace.hpp"
#include "drivers/wintel32/core/threading/threading.hpp"

#include "libraries/lua/lua.hpp"
#include "libraries/expat/expat.hpp"


int main(int argc, char **argv)
{
    SGX_SCOPE("main()");

	CWin32Output::Create();

    CGlobalTrace::Get()->SetTraceLevel(SGX_ERR_REPORT_ALL);
    sgxTrace(SID_INFO("--------------------- The SGX Core -------------------"));
    sgxTrace(SID_INFO("--------  Copyright Steven Goodwin 1994-2006  --------"));
     
    sgxSafetyCheck();

    CHardwarePlatform::Get()->DumpStats();

	// An example of using the abstracted filesystem
	// Taken from the original SGX engine, and explained fully
	// in the book, Cross-Platform Game Programming
#if 0

    CSGXDeviceWin32Disc drive("c:\\temp");
    CSGXFileSystem::Get()->Mount(drive, "/win");

    CSGXDeviceWin32Disc localdir(".\\assets");
    CSGXFileSystem::Get()->Mount(localdir, "/assets");
    CSGXDeviceWin32Disc skydir(".\\assets\\sky1");
    CSGXFileSystem::Get()->Mount(skydir, "/assets/sky1");
#endif

	// How to create the derived class for XML parsing, using expat .
	// Example of standard read and write operations that build a
	// DOM tree.
#if 0
	sgxXMLExpatParser::Create();

	sgxXMLTree tree;
	sgxXMLReader rd(tree);

	rd.Read("/some_input.xml");

	sgxXMLWriter wr(tree);

	wr.Write("/output.xml");
#endif

	// How to initialize the scripting engine for Lua.
	// Plus an example of reading input from a file,
	// and using the immediate interpreter.
#if 0
	CLuaScriptingEngine::Create();


	CScriptState *ptr = CScriptingManager::Get()->CreateInstance(sgxString("lua"));

    ptr->OpenStandardLibraries();

	ptr->Process(sgxString("testfile.lua"));
	ptr->Run();
	ptr->Evaluate(sgxString("print \"hi\""));
#endif

	// Using a simple regular expression using PCRE.
#if 0
	sgxRegex r("(a*)(b*)(a+)");

	sgxVector<sgxString> results;

	tUINT32 n = r.Match("abbaa", results);
	vector<sgxString>::iterator it = results.begin();
	for(;it!=results.end();++it) {
		sgxTrace("%s",(*it).c_str());
	}
#endif


	
    return 0;
}
