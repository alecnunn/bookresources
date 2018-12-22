/* Copyright (C) Yossarian King, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Yossarian King, 2001"
 */
#include <windows.h>

extern int  main(int argc, char *argv[]);

int	__stdcall	WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    return main(__argc, __argv);
}

void    message(char *sz)
{
    MessageBox(NULL, sz, "float", MB_OK);
}
