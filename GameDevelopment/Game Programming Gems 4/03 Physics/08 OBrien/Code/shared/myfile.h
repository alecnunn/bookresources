/*********************************************************************
 * myfile.h
 * Authored by Kris Hauser 2002-2003
 *
 * A convenient platform-independent file interface.
 *
 * Copyright (c) 2002 SquireSoft, used with permission.
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/


#ifndef DUNC_MYFILE
#define DUNC_MYFILE

#include "stdincludes.h"

#if (defined(PS2) && !defined(PS2LINUX))

#include <sifdev.h>
typedef int HANDLE;

#define FILE_BEGIN SCE_SEEK_SET
#define FILE_CURRENT SCE_SEEK_CUR
#define FILE_END SCE_SEEK_END

#else

#include <stdio.h>
typedef FILE* HANDLE;

#define FILE_BEGIN SEEK_SET
#define FILE_CURRENT SEEK_CUR
#define FILE_END SEEK_END

#endif //(defined(PS2) && !defined(PS2LINUX))

#define DATASIZE_UNKNOWN -1
#define FILEREAD 0x1
#define FILEWRITE 0x2

class myFile
{
public:
	myFile();
	~myFile();
	bool Open(const char*, int openmode = FILEREAD | FILEWRITE);
	bool Open(HANDLE, int openmode = FILEREAD | FILEWRITE);
	bool Open(void*, int = DATASIZE_UNKNOWN, int openmode = FILEREAD | FILEWRITE);
	void Close();

	bool Seek(int, int from = FILE_CURRENT);
	int Position() const;
	int Length();

	bool ReadData(void*, int size);
	bool WriteData(const void*, int size);

	bool ReadString(char*, int bufsize);
	bool WriteString(const char*);

private:
	int _mode;
	int _srctype;

	HANDLE _file;
	unsigned char* _datafile;
	unsigned char* _pos;
	unsigned char* _dataend;
	int _datasize;
};



template <class type>
bool ReadFile(myFile& f, type& t)
{
	return f.ReadData(&t, sizeof(t));
}

template <class type>
bool WriteFile(myFile& f, const type& t)
{
	return f.WriteData(&t, sizeof(t));
}



template <class type>
bool ReadArrayFile(myFile& f, type* t, int n)
{
	return f.ReadData(t, sizeof(type)*n);
}

template <class type>
bool WriteArrayFile(myFile& f, const type* t, int n)
{
	return f.WriteData(t, sizeof(type)*n);
}


#endif
