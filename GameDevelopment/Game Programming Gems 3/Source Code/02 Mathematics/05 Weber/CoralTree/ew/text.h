/******************************************************************************

Coral Tree ew/text.h
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



/******************************* DEFINE's ************************************/

#define EW_FONT_FIXED			0x00000000
#define EW_FONT_PROPORTIONAL	0x00000001
#define EW_FONT_ITALIC			0x00000002
#define EW_FONT_BOLD			0x00000004

#define EW_FONTATTR_HEIGHT		1
#define EW_FONTATTR_ASCENT		2
#define EW_FONTATTR_DESCENT		3

#define EW_IS_WHITESPACE(c)     (c==' ' || c=='\t')
#define EW_IS_NUMBER(c) 	    (c>='0' && c<='9')
#define EW_IS_UPPERCASE(c) 	    (c>='A' && c<='Z')
#define EW_IS_LOWERCASE(c) 	    (c>='a' && c<='z')
#define EW_IS_ALPHANUM(c) 	    (EW_IS_NUMBER(c) || EW_IS_UPPERCASE(c) ||	\
									EW_IS_LOWERCASE(c))



/******************************* TYPEDEF's ***********************************/

#if EW_WL==EW_XWIN

	typedef struct _EW_FontMatch
		{
		char	name[100];
		} EW_FontMatch;

#elif EW_WL==EW_MEX

	typedef struct _EW_FontMatch
		{
		char	name[100];
		float	scale;
		} EW_FontMatch;

#elif EW_WL==EW_WIN32

	typedef struct _EW_FontMatch
		{
		char	name[100];
		LOGFONT	logfont;
		} EW_FontMatch;

#else
	#error EW_WL not properly defined
#endif



/******************************************************************************
class	EW_FontHandle

Keywords:
******************************************************************************/
class EW_FontHandle : public GPL_DoubleNode
	{
	public:

		short	valid;

	public:
				EW_FontHandle(void);
virtual			~EW_FontHandle(void);

		long	GetValid(void)		{ return valid; };
		void	SetValid(long set)	{ valid=(short)set; };

#if EW_GL==EW_X_GFX

		/* Pure X Windows */

		XFontStruct		*FontInfo;
		Display			*FontDisplay;

#elif EW_GL==EW_OPENGL

	/* OpenGL */

		GLint			FontBase;

	#if EW_WL==EW_XWIN

		/* OpenGL on X */

		XFontStruct		*FontInfo;
		Display			*FontDisplay;

	#elif EW_WL==EW_WIN32

		/* OpenGL on Win32 */

		HFONT			handle;
		long			first,last; // first,last index of defined characters

	#else

		#error EW_WL not properly defined

	#endif

#elif EW_GL==EW_GDI

		HFONT				handle;

#elif EW_GL==EW_IRISGL

	/* IrisGL on Mex */

	#if FONTMANAGER

		fmfonthandle	handle;

	#endif

#else

	#error EW_GL not properly defined

#endif
	};



/******************************************************************************
class	EW_FontHandleList

******************************************************************************/
class EW_FontHandleList : public GPL_IDoubleList<EW_FontHandle>
    {
    public:
                EW_FontHandleList(void)  {};
virtual         ~EW_FontHandleList(void) {};
    };



/******************************************************************************
class	EW_Font

******************************************************************************/
class EW_Font
	{
	public:

		EW_FontHandle	*fonthandle;

	public:
				EW_Font(void);
virtual			~EW_Font(void);

		long	GetValid(void)
					{
					if(fonthandle==NULL)
						return FALSE;

					return fonthandle->GetValid();
					};

		void	CopyFrom(EW_Font *other)
					{ fonthandle=other->fonthandle; };
	};



/******************************************************************************
class	EW_IString : public EW_String, public GPL_DoubleNode

******************************************************************************/
class EW_IString : public EW_String, public GPL_DoubleNode
	{
    public:
				EW_IString(void)  {};
virtual			~EW_IString(void) {};
	};



/******************************************************************************
class	EW_StringList : public GPL_IDoubleList<EW_IString>

******************************************************************************/
class EW_StringList : public GPL_IDoubleList<EW_IString>
	{
    public:
				EW_StringList(void)  {};
virtual			~EW_StringList(void) {};
	};



/******************************************************************************
class	EW_FontRequest

******************************************************************************/
class EW_FontRequest
	{
	private:

		float			height;
		long			style;
		EW_StringList	hintlist;

	public:
						EW_FontRequest(void)
							{
							height=(float)8.0;
							style=0;
							};

		void			SetHeight(float set)	{ height=set; };
		float			GetHeight(void)			{ return height; };

		void			SetStyle(long set)		{ style=set; };
		long			GetStyle(void)			{ return style; };

		void			AddHint(char *fontname);

		EW_StringList	*GetHintList(void)	{ return &hintlist; };
	};
