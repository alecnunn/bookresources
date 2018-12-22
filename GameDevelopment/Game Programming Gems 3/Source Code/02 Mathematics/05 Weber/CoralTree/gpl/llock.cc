
/******************************************************************************

Coral Tree gpl/llock.cc
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#include "gpl.h"

/******************************************************************************
GPL_LocalLock::GPL_LocalLock()
******************************************************************************/
GPL_LocalLock::GPL_LocalLock()
{
	ReaderCount = 0;
	osdMutexCreate(&WriteMutex);
	osdMutexCreate(&ReadMutex);
	osdMutexCreate(&WaitMutex);
}

/******************************************************************************
GPL_LocalLock::~GPL_LocalLock()
******************************************************************************/
GPL_LocalLock::~GPL_LocalLock()
{
	osdMutexDestroy(&WriteMutex);
	osdMutexDestroy(&ReadMutex);
	osdMutexDestroy(&WaitMutex);
}

/******************************************************************************
long GPL_LocalLock::StartRead()
******************************************************************************/
long GPL_LocalLock::StartRead()
{
	osdMutexLock(&WriteMutex);
		osdMutexLock(&ReadMutex);
			if(ReaderCount == 0)
			{
				osdMutexLock(&WaitMutex);
			}
			ReaderCount++;
		osdMutexUnlock(&ReadMutex);
	osdMutexUnlock(&WriteMutex);
	osdThreadHardYield();

	return TRUE;
}

/******************************************************************************
long GPL_LocalLock::StopRead()
******************************************************************************/
long GPL_LocalLock::StopRead()
{
	osdMutexLock(&ReadMutex);
		ReaderCount--;
		if(ReaderCount == 0)
		{
			osdMutexUnlock(&WaitMutex);
		}
		else if(ReaderCount < 0)
		{
			gplLog("GPL ERROR", "StopRead on lock was not preceded by StartRead");
		}
	osdMutexUnlock(&ReadMutex);
	osdThreadSoftYield();

	return TRUE;
}

/******************************************************************************
long GPL_LocalLock::StartWrite()
******************************************************************************/
long GPL_LocalLock::StartWrite()
{
	osdMutexLock(&WriteMutex);
		osdMutexLock(&WaitMutex);
		osdMutexUnlock(&WaitMutex);

	return TRUE;
}

/******************************************************************************
long GPL_LocalLock::StopWrite()
******************************************************************************/
long GPL_LocalLock::StopWrite()
{
	osdMutexUnlock(&WriteMutex);
	osdThreadHardYield();

	return TRUE;
}


