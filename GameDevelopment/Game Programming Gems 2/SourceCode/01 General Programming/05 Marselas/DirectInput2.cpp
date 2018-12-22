/* Copyright (C) Herb Marselas, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Herb Marselas, 2001"
 */
typedef HRESULT (WINAPI* DirectInput8Create_PROC)
                  (HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut,
                   LPUNKNOWN punkOuter);

HMODULE hDInputLib = LoadLibrary("dinput8.dll");
if (!hDInputLib)
{
   // handle error - DInput 8 not found. Is it installed incorrectly or at all?
}

DirectInput8Create_PROC diCreate;
diCreate = (DirectInput8Create_PROC) GetProcAddress(hDInputLib, "DirectInput8Create");

if (!diCreate)
{
   // handle error - DInput 8 exists, but the function can't be found.
}

HRESULT hr = (diCreate)(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
                        (LPVOID*) &mDirectInput, NULL);
if (FAILED(hr))
{
   // handle error - initialization error
}
