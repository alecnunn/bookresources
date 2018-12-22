// disable MS warning about long names/debug builds
#pragma warning ( disable : 4786 )

#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <map>
#include <list>

#include <cLua.h>
#include <dxcontext.h>
#include <GUIManager.h>

#define SAFE_RELEASE(x) if (x) {x->Release(); x=NULL;}
#define SAFE_DELETE(x) if (x) {delete x; x=NULL;} 

