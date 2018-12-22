
/******************************************************************************

COPYRIGHT_HEADER

******************************************************************************/

#include <gpl/gpl.h>

GPL_Boolean GPL_PosixRegExpFileFilter::SetPattern(const GPL_String & str)
{
	if (!regExp.SetPattern(str))
	{
		return FALSE;
	}
	return TRUE;
}

GPL_Boolean GPL_PosixRegExpFileFilter::Accept(const GPL_String & fileName)
{
	if (!regExp.Matches(fileName))
	{
		return FALSE;
	}
	return TRUE;
}

GPL_Boolean GPL_GlobRegExpFileFilter::SetPattern(const GPL_String & str)
{
	register const unsigned long n = str.GetLength();
	GPL_String string(n, (char) 0);
	for (register unsigned long i = 0; n > i; ++i)
	{
		switch(str[i])
		{
		case '*':
			string.Append("[!-~]*");
			break;

		case '?':
			string.Append('.');
			break;

		case '.':
			string.Append("\\.");
			break;

		default:
			string.Append(str[i]);
			break;
		}
	}
	if (-1 == str.Find('/'))
	{
		string.Prepend("^(.*/)?");
	}
	else
	{
		string.Prepend('^');
	}
	string.Append('$');
	return GPL_PosixRegExpFileFilter::SetPattern(string);
}

GPL_FileSelector::GPL_FileSelector(void)
	: filter((GPL_FileFilter *) GPL_BAD_ADDRESS)
{
}

GPL_FileSelector::~GPL_FileSelector(void)
{
}

const GPL_String & GPL_FileSelector::GetDirectory(void) const
{
	return directory;
}

GPL_FileFilter *GPL_FileSelector::GetFilter(void) const
{
	return filter;
}

const GPL_String & GPL_FileSelector::GetFile(void) const
{
	return file;
}

void GPL_FileSelector::SetDirectory(const GPL_String & d)
{
	directory = d;
}

void GPL_FileSelector::SetFilter(GPL_FileFilter *f)
{
	filter = f;
}

void GPL_FileSelector::SetFile(const GPL_String & f)
{
	file = f;
}

GPL_Boolean GPL_FileSelector::GetFiles(GPL_List<GPL_String> & files)
{
	// Get the file names.
	GPL_Directory dir(directory);
	GPL_Array<GPL_String> names;
	if (!dir.GetNames(names,FALSE))
	{
		return FALSE;
	}

	// Apply the filter to each file name.
	register const unsigned long n = names.GetCount();
	for (register unsigned long i = 0; n > i; ++i)
	{
		OSD_FileAttributes attr;
		GPL_String s;
		s = directory;
		s.Append(OSD_DIRECTORY_SEPARATOR);
		s.Append(names[i]);
		osdFileGetAttributes((char *)s, &attr);
		if(attr.attr & OSD_FILE_ATTR_DIR)
		{
			files.Append(names[i]);
		}
		else if ((GPL_FileFilter *) GPL_BAD_ADDRESS == filter ||
			filter->Accept(names[i]))
		{
			files.Append(names[i]);
		}
	}

	return TRUE;
}



