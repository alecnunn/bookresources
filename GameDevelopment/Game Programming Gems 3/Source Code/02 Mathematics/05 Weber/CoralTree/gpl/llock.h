
/******************************************************************************

Coral Tree gpl/llock.h
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

/******************************************************************************
keywords: c++, system, blackbox_system, semaphore

******************************************************************************/

class GPL_LocalLock
{
	private:

		OSD_Mutex 	WriteMutex, ReadMutex, WaitMutex;
		long 		ReaderCount;

	public:

					GPL_LocalLock();
virtual				~GPL_LocalLock();

		long		StartRead();
		long		StopRead();
		long		StartWrite();
		long		StopWrite();
};

