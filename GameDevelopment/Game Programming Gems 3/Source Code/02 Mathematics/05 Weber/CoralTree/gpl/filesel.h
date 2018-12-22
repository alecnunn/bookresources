
/******************************************************************************

COPYRIGHT_HEADER

******************************************************************************/

class GPL_FileFilter
{
	public:
		virtual GPL_Boolean Accept(const GPL_String & fileName)
		{
			GPL_String f = fileName;f = f;
			return FALSE;
		}
};

class GPL_FilePatternFilter : public GPL_FileFilter
{
	public:
		virtual GPL_Boolean SetPattern(const GPL_String & str)
		{
			GPL_String s = str;s = s;
			return FALSE;
		}
};

class GPL_PosixRegExpFileFilter : public GPL_FilePatternFilter
{
	public:
		GPL_PosixRegExpFileFilter(void) { SetPattern("^.*$"); }
		virtual ~GPL_PosixRegExpFileFilter(void) {}

		virtual GPL_Boolean SetPattern(const GPL_String & str);
		virtual GPL_Boolean Accept(const GPL_String & fileName);

	protected:
		GPL_RegularExpression regExp;
};

class GPL_GlobRegExpFileFilter : public GPL_PosixRegExpFileFilter
{
	public:
		GPL_GlobRegExpFileFilter(void) {}
		virtual ~GPL_GlobRegExpFileFilter(void) {}

		virtual GPL_Boolean SetPattern(const GPL_String & str);
};

class GPL_FileSelector
{
	public:
		GPL_FileSelector(void);
		virtual ~GPL_FileSelector(void);

		virtual const GPL_String & GetDirectory(void) const;
		virtual GPL_FileFilter *GetFilter(void) const;
		virtual const GPL_String & GetFile(void) const;
		virtual GPL_Boolean GetFiles(GPL_List<GPL_String> & files);

		virtual void SetDirectory(const GPL_String & d);
		virtual void SetFilter(GPL_FileFilter *f);
		virtual void SetFile(const GPL_String & f);

	protected:
		GPL_String directory;
		GPL_FileFilter *filter;
		GPL_String file;
};


