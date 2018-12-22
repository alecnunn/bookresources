// disable MS warning about long names/debug builds
#pragma warning ( disable : 4786 )

#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <map>
#include <list>

#ifndef DWORD_PTR
#define DWORD_PTR long *
#endif

#include <cLua.h>
#include <DxBase.h>
#include <GUIManager.h>

#define SAFE_RELEASE(x) if (x) {x->Release(); x=NULL;}
#define SAFE_DELETE(x) if (x) {delete x; x=NULL;} 

