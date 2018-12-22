// I used lcc-win32 version 2.4 by Jacob Navia (great!)
// standard MS @ libs required

#include <windows.h>

//because lcc (2.4) doesn't support __declspec(dllimport) I must help myself
//by this "apimacro":
extern __stdcall UINT (*KERNEL32_ORD_0001)(UINT);
//.lib files will be searched for _KERNEL32_ORD_0001
//that's why CONSTANT (obsolete) must be specified in .def EXPORTS

//Visual C++ supports __declspec(dllimport) by the same way as EliASM:  
//.lib files will be searched for __imp__KERNEL32_ORD_0001
//and _KERNEL32_ORD_0001 can be used for other purposes (usually defines code
//label)

void main() {
 char Place[64];
 if ( 
    GetProcAddress( (void*)GetModuleHandle("KERNEL32"), (LPCSTR)1 )
    || 
    (UINT)KERNEL32_ORD_0001 < 0x80000000
    ) 
    ExitProcess(-1);
 wsprintf(Place, "VWIN32 version is: 0x%0.4X.", KERNEL32_ORD_0001(0x002A0000));
 ExitProcess(MessageBox(NULL, Place, "VxDCall Test", MB_OK));
}