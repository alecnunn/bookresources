
/******************************************************************************

Coral Tree wds/hfile.h
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


// WDS_HierarchyFile::filetype
#define WDS_HIER_FILE_NULL		0
#define WDS_HIER_FILE_DIR		1
#define WDS_HIER_FILE_LINK		2


// WDS_HierarchyFile WDS_Widget::SetElementColor(EW_COLOR_PEN,?) for text
#if TRUE
	// on EW_WHITE
	#define WDS_HIER_FILE_COLOR_DRWX	EW_BLUEGREEN
	#define WDS_HIER_FILE_COLOR_DR_X	EW_PURPLE
	#define WDS_HIER_FILE_COLOR_D___	EW_BLACK
	#define WDS_HIER_FILE_COLOR_D_nn	EW_PINK

	#define WDS_HIER_FILE_COLOR__RWX	EW_BLACK
	#define WDS_HIER_FILE_COLOR__RW_	EW_BLUE
	#define WDS_HIER_FILE_COLOR__R_X	EW_DARKGREY
	#define WDS_HIER_FILE_COLOR__R__	EW_RED
	#define WDS_HIER_FILE_COLOR_____	EW_MEDGREY

	#define WDS_HIER_FILE_COLOR___nn	EW_CYAN
#elif FALSE
	// on EW_LIGHTGREY
	#define WDS_HIER_FILE_COLOR_DRWX	EW_BLUEGREEN
	#define WDS_HIER_FILE_COLOR_DR_X	EW_PURPLE
	#define WDS_HIER_FILE_COLOR_D___	EW_WHITE
	#define WDS_HIER_FILE_COLOR_D_nn	EW_PINK

	#define WDS_HIER_FILE_COLOR__RWX	EW_BLACK
	#define WDS_HIER_FILE_COLOR__RW_	EW_BLUE
	#define WDS_HIER_FILE_COLOR__R_X	EW_DARKGREY
	#define WDS_HIER_FILE_COLOR__R__	EW_RED
	#define WDS_HIER_FILE_COLOR_____	EW_MEDGREY

	#define WDS_HIER_FILE_COLOR___nn	EW_CYAN
#else
	// on EW_DARKGREY
	#define WDS_HIER_FILE_COLOR_DRWX	EW_OLIVE
	#define WDS_HIER_FILE_COLOR_DR_X	EW_PINK
	#define WDS_HIER_FILE_COLOR_D___	EW_MEDGREY
	#define WDS_HIER_FILE_COLOR_D_nn	EW_YELLOW

	#define WDS_HIER_FILE_COLOR__RWX	EW_GREEN
	#define WDS_HIER_FILE_COLOR__RW_	EW_CYAN
	#define WDS_HIER_FILE_COLOR__R_X	EW_PURPLE
	#define WDS_HIER_FILE_COLOR__R__	EW_RED
	#define WDS_HIER_FILE_COLOR_____	EW_LIGHTGREY

	#define WDS_HIER_FILE_COLOR___nn	EW_BLACK
#endif


/******************************************************************************
class	WDS_HierarchyFile

Keywords:
******************************************************************************/
class WDS_HierarchyFile:public WDS_HierarchyNode
	{
	GPL_OBJECT_INTERFACE(WDS_HierarchyFile);

	private:

		WDS_String	fullname;
		long		filesize;
		long		attributes;
		long		filetype;

		long		(*file_handler)(char *serverhost,char *request,char **response);

	public:

					WDS_HierarchyFile();

		void		SetFullPathName(char *buffer)	{ fullname.NewBuffer(buffer); };
		void		GetFullPathName(char *buffer)	{ fullname.CopyToBuffer(buffer); };

		void		SetFileHandler(long (*fp)(char *,char *,char **))
						{
						file_handler=fp;
						};

virtual	void		ShowEntry(void);

static	long		DefaultFileHandler(char *serverhost,char *request,char **response);
static	char		*ReadArgument(char *string,char *argument);
	};
