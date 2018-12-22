
#include <windows.h>
#include <mmsystem.h>

// STL includes
// this disables the warnings caused by the STL namespace
#pragma warning ( disable : 4786 )

#include <list>
#include <string>

#include <DXBase.h>

#define SAFE_RELEASE(x) if (x) {x->Release(); x=NULL;}
#define SAFE_DELETE(x) if (x) {delete x; x=NULL;} 
#define SAFE_DELETE_ARRAY(x) if (x) {delete[] x; x=NULL;} 

