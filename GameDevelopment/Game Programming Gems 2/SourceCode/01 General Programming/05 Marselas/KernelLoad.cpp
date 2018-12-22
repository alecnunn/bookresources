/* Copyright (C) Herb Marselas, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Herb Marselas, 2001"
 */
typedef BOOL (WINAPI *CopyFileEx_PROC) (LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, LPBOOL pbCancel, DWORD dwCopyFlags);

HMODULE hKernel32 = LoadLibrary("kernel32.dll");
if (!hKernel32)
{
   // handle error - kernel32.dll not found. Wow! That's really bad
}

CopyFileEx_PROC pfnCopyFileEx;
pfnCopyFileEx = (CopyFileEx_PROC) GetProcAddress(hKernel32, "CopyFileExA");

BOOL bReturn;
if (pfnCopyFileEx)
{
   // use CopyFileEx to copy the file
   bReturn = (pfnCopyFileEx)(pExistingFile, pDestinationFile, ...);
}
else
{
   // use the regular CopyFile function
   bReturn = CopyFile(pExistingFile, pDestinationFile, FALSE);
}
