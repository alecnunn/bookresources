#ifndef GPL_FILE_H
#define GPL_FILE_H

/******************************************************************************

Coral Tree gpl/file.h
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

#include <osd/osd.h>

/******************************************************************************
class GPL_FileRep

File representation.
******************************************************************************/
class GPL_FileRep : public GPL_ReferenceRep
{
	public:
		OSD_File		file;
		char			*name;
		unsigned long	flags;
		GPL_Boolean		endOfFile;

		GPL_FileRep(const OSD_File f, const char *n, const unsigned long fl)
			: file(f), flags(fl), endOfFile(FALSE)
		{
			name = osdStrdup(n);
		}
		virtual ~GPL_FileRep(void)
		{
			osdFree(name);
		}

		GPL_Boolean operator==(const GPL_FileRep & rep)
		{
			return file.FileHandle == rep.file.FileHandle && flags == rep.flags;
		}
};

/******************************************************************************
class GPL_File

File.
******************************************************************************/
class GPL_File : public GPL_Reference
{
	public:
					GPL_File(void) {}
					GPL_File(const GPL_File & file);
					GPL_File(const char *name, const unsigned long flags);
virtual				~GPL_File(void);

		GPL_File	& operator =(const GPL_File & file);
		GPL_Boolean	operator ==(const GPL_File & file) { return rep == file.rep; }
		GPL_Boolean	operator !=(const GPL_File & file) { return !(operator==(file)); }

		GPL_Boolean	Open(const char *name, const unsigned long flags);
		GPL_Boolean	Duplicate(const GPL_File & file);
		GPL_Boolean	Close(void);

		GPL_Boolean	Copy(const char *destination) const;
		GPL_Boolean	Delete(void);

		char		*GetName(void) const;
		unsigned long	GetSize(void) const;
		char		*GetDirectory(void) const { return GetDirectory(GetName()); }
		char		*GetBase(void) const { return GetBase(GetName()); }
		char		*GetExtension(void) const { return GetExtension(GetName()); }
		char		*GetFullPathName(void) const { return GetFullPathName(GetName()); }
		unsigned long	GetFlags(void) const;
		GPL_Boolean	EndOfFile(void) const;
		GPL_Boolean	IsOpen(void) const { return IsValid(); }
		GPL_Boolean	GetAttributes(OSD_FileAttributes & attr) const { return GetAttributes(GetName(), attr); }
		GPL_Boolean	GetModificationTime(OSD_Time & time) { return GetModificationTime(GetName(), time); }
		long		GetPointer(void);
		GPL_Boolean	SetPointer(const long offset, const long start);
		long		Read(char *buffer, const unsigned long len);
		char		*Read(unsigned long *length, unsigned long grain_size=0);
		long		Read(GPL_CharPtr &cptr);
		long		ReadLine(char *buffer,const unsigned long maxlen);
		long		Write(char *buffer, const unsigned long len);
		long		Write(const char *buffer, const unsigned long len);
		long		Printf(char *format, ...);
		GPL_Boolean	Lock(long offset, long len, GPL_Boolean ex, GPL_Boolean wait);
		GPL_Boolean	Unlock(long offset, long len);

static	GPL_Boolean	Exists(const char *name);
static	GPL_Boolean	Copy(const char *name, const char *copy);
static	GPL_Boolean	Delete(const char *name);
static	GPL_Boolean	GetAttributes(const char *name, OSD_FileAttributes & attr);
static	GPL_Boolean	GetModificationTime(const char *name, OSD_Time & time);
static	char		*GetDirectory(const char *name);
static	char		*GetBase(const char *name);
static	char		*GetExtension(const char *name);
static	char		*GetFullPathName(const char *name);

protected:
virtual	void		Privatize(void) {}
};



/******************************************************************************
class	GPL_FileIterator

******************************************************************************/
class GPL_FileIterator
{
	public:
		GPL_FileIterator(const char d = '\n');
		virtual ~GPL_FileIterator(void);

		char GetDelimiter(void) const { return delimiter; }

		void SetDelimiter(const char d) { delimiter = d; }
		void Reset(GPL_File *f);
		char *Next(void);

	protected:
#define GPL_FILEITERATOR_CHUNK_SIZE	16
		char delimiter;
		GPL_File *file;
		char *buffer;
		unsigned long size;
		unsigned long front;
		unsigned long back;
};

#endif /* GPL_FILE_H */
