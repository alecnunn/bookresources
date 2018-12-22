/******************************************************************************

Coral Tree gpl/dir.cc
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
#include <gpl/gpl.h>

GPL_Boolean GPL_Directory::GetNames (GPL_Array<GPL_String> & names,
										const GPL_Boolean fullpath)
{
	if (0 == name.GetLength())
	{
		return FALSE;
	}
	char **entries;
	register const long n = osdDirectoryScan (name, &entries, 0, 0);
	if (-1 != n)
	{
		names.Resize (n);
		for (register unsigned long i = 0; n > (long) i; ++i)
		{
			if (fullpath)
			{
				names[i] = name;
				if (0 < name.GetLength() &&
					OSD_DIRECTORY_SEPARATOR != name[name.GetLength() - 1])
				{
					names[i].Append (OSD_DIRECTORY_SEPARATOR);
				}
				names[i].Append (entries[i]);
			}
			else
			{
				names[i] = entries[i];
			}
		}
		osdDirectoryListFree(&entries);
		return TRUE;
	}
	return FALSE;
}

GPL_Boolean GPL_Directory::GetFiles (GPL_Array<GPL_File> & files)
{
	GPL_Array<GPL_String> names;
	if (GetNames (names))
	{
		register const unsigned long n = names.GetCount ();
		files.Resize (n);
		for (register unsigned long i = 0; n > i; ++i)
		{
			files[i].Open (names[i], OSD_FILE_READ);
		}
		return TRUE;
	}
	return FALSE;
}
