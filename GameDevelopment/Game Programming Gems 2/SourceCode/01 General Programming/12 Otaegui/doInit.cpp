/* Copyright (C) Javier F. Otaegui, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Javier F. Otaegui, 2001"
 */
HANDLE hMainThread;	// Main Thread handle

static BOOL
doInit( ... )
{
	... // Initialize DirectX and everything else

	DWORD tid;

	hMainThread=CreateThread( 0,
		0,
		&MainThread,
		0,
		0,
		&tid);

	return TRUE;
}

