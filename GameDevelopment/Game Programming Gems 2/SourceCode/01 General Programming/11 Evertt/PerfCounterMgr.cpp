/* Copyright (C) Jeff Evertt, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jeff Evertt, 2001"
 */
void CWin32PerfCounterMgr::GetPerfCounter(
LARGE_INTEGER &iCounter) {
	DWORD dwLow,dwHigh;
	__asm {
		rdtsc
		mov	dwLow, eax
		mov	dwHigh, edx
	}
	iCounter.QuadPart = ((unsigned __int64)dwHigh << 32)
                         | (unsigned __int64)dwLow; }
