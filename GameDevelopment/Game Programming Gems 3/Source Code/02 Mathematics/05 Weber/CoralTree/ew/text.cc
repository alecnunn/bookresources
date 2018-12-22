/******************************************************************************

Coral Tree ew/text.cc
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

/*******************************************************************************

				Font/Text Interface

Load a font:

	long EW_Window::LoadFont(EW_Font *fontdata,EW_FontRequest *request)

	where	fontdata is system-specific font structure
			request is generic specification

	request->GetHeight()	= desired height in pixels
	request->GetStyle()	= OR'ed sum of flags

	flags:
		EW_FONT_FIXED			(default)
		EW_FONT_PROPORTIONAL	(overrides EW_FONT_FIXED)
		EW_FONT_ITALIC
		EW_FONT_BOLD
	

Select a current font:

	long EW_Window::SetFont(EW_Font *fontdata)

 
Toggle underlining mode for window:

	void EW_Window::SetUnderline(long setting)

	where setting is TRUE for on or FALSE for off


Draw string at position (x,y) in current font:

	void EW_Window::Text(long x,long y,char *string)
						OR
	void EW_Window::Text3D(long x,long y,char *string)	(for slight shadow)


Determine pixel width of specific char or string in current font:

	long EW_Window::CharWidth(char c)
	long EW_Window::StringWidth(char *string)


Determine an attribute of current font:

	long EW_Window::FindFontAttribute(long type)
		where type is one of:	EW_FONTATTR_HEIGHT
								EW_FONTATTR_ASCENT
								EW_FONTATTR_DESCENT

******************************************************************************/


#include"ew.h"



#define	EW_DEFAULT_FONT_HEIGHT	12					/* default if font height can't be detected */

#define	EW_TEXT_DEBUG			0					/* print verbosity 0=off, 1=little, 2=lots */
#define EW_SETFONT_DEBUG		FALSE
#define	EW_REPLACE_WGLBITMAP	FALSE				/* TRUE = replace wglUseFontBitmaps() with ewUseFontBitmaps() */
#define	EW_TRUETYPE_ONLY		FALSE				/* TRUE = use TrueType fonts only (under Win32) */
#define	EW_DELETE_FONTS			TRUE				// TRUE = free fonts (otherwise memory leaks, but very little)
													// causes confusion with copied fonts (accessing freed space)

#define	EW_X_MOD_ASCENT			(0)					// was 2
#define	EW_X_MOD_DESCENT		(-1)				// was -1
#define	EW_WIN32_FONT_ADJUST	(0)					// (was -1) increase font scaling in Win32


#if EW_WL==EW_WIN32

	int CALLBACK ewEnumFontFamProc(ENUMLOGFONT FAR* lpelf,NEWTEXTMETRIC FAR* lpntm,int FontType,LPARAM lParam);
	void ewDebugDescribeWin32Font(LOGFONT *logfont);

	#if EW_GL==EW_OPENGL

		/* replacement for faulty wglUseFontBitmaps() */
		BOOL ewUseFontBitmaps(HDC hdc,DWORD first,DWORD count,DWORD listBase);

	#endif

extern COLORREF EwWin32Colortable[EW_MAXCOLORS];

#endif



/******************************************************************************
long	EW_Window::CopyDefaultFontTo(EW_Font *fontdata)

******************************************************************************/
long EW_Window::CopyDefaultFontTo(EW_Font *fontdata)
	{
	if(default_font.GetValid())
		{
		fontdata->CopyFrom(&default_font);
		return FALSE;
		}
	return TRUE;
	}



/******************************************************************************
long	EW_Window::CopyDefaultFontFrom(EW_Font *fontdata)

******************************************************************************/
long EW_Window::CopyDefaultFontFrom(EW_Font *fontdata)
	{
	if(fontdata->GetValid())
		{
		default_font.CopyFrom(fontdata);
		return FALSE;
		}
	EW_PRINT(EW_WINDOW,EW_ERROR,
			"EW_Window::CopyDefaultFontFrom() invalid font");
	return TRUE;
	}



/******************************************************************************
long EW_Window::LoadFont(EW_Font *fontdata,EW_FontRequest *request)

	user-access font loading

	uses request as input
	sets values in fontdata and request

	will degrade request->style as neccessary to try for match
		(can observe result in request->style after function returns)

	returns FALSE if successful

Keywords:
******************************************************************************/
long EW_Window::LoadFont(EW_Font *fontdata,EW_FontRequest *request)
	{
	EW_FontMatch match;

	char message[250];

	short not_matched;

	EW_EventContext *context=GetEventContext();
	if(context==NULL)
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::LoadFont(): window must be attached to an EW_EventContext first");
		return TRUE;
		}

//	fontdata->valid=FALSE;
	fontdata->fonthandle=NULL;

	if(!opened)
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::LoadFont(): font must be loaded through an open window");
		return TRUE;
		}

#if EW_TEXT_DEBUG

	sprintf(message,"EW_Window::LoadFont(): request a font height %.6G style 0x%x",request->GetHeight(),request->GetStyle());
	EW_PRINT(EW_GFX,EW_LOG,message);

#endif

	while(( not_matched=MatchFont(request,&match) && request->GetStyle()))
		{
		if(request->GetStyle() & EW_FONT_ITALIC)
			request->SetStyle( request->GetStyle() ^ EW_FONT_ITALIC );
		else if(request->GetStyle() & EW_FONT_BOLD)
			request->SetStyle( request->GetStyle() ^ EW_FONT_BOLD );
		else
			request->SetStyle(0);
		}

	if(not_matched)
		{
		sprintf(message,"EW_Window::LoadFont(): could not match a font height %.6G style 0x%x",
																			(double)(request->GetHeight()),(int)(request->GetStyle()));
		EW_PRINT(EW_GFX,EW_ERROR,message);
		}
	else
		{
#if EW_TEXT_DEBUG>1 && EW_WL==EW_WIN32

		sprintf(message,"EW_Window::LoadFont() matched with height %d using (%s)",match.logfont.lfHeight,match.name);
		EW_PRINT(EW_GFX,EW_LOG,message);

#endif

		if( !OpenFont(fontdata,&match) )
			return FALSE;
		else
			{
			sprintf(message,"EW_Window::LoadFont(): could not open a font matching height %.6G style 0x%x\n  named \"%s\"",
																			(double)(request->GetHeight()),(int)(request->GetStyle()),match.name);
			EW_PRINT(EW_GFX,EW_ERROR,message);
			}
		}

	return TRUE;
	}



/******************************************************************************
long	EW_Window::OpenFont(EW_Font *fontdata,EW_FontMatch *match)

	open font of name and scale

	Note: Font Manager translations in /usr/lib/X11/fonts/ps2xlfd_map

	returns FALSE if successful

Keywords: private
******************************************************************************/
long EW_Window::OpenFont(EW_Font *fontdata,EW_FontMatch *match)
	{
	char message[200];

	EW_EventContext *context=GetEventContext();
	if(context==NULL)
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::LoadFont(): window must be attached to an EW_EventContext first");
		return TRUE;
		}

	EW_FontHandle *newhandle=new EW_FontHandle;
	context->GetFontHandleList()->AppendNode(newhandle);
	fontdata->fonthandle=newhandle;

	#if EW_WL==EW_XWIN
		/* X WIndows (Pure or OpenGL) */

		newhandle->FontInfo=XLoadQueryFont(GetDisplay(),match->name);
		if(newhandle->FontInfo==NULL)
			{
			EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::OpenFont(): X font not found");
			newhandle->FontDisplay=NULL;
			return TRUE;
			}
		else
			newhandle->FontDisplay=GetDisplay();

	#elif EW_WL==EW_WIN32

		/* Win32 (OpenGL or GDI) */

#if EW_TEXT_DEBUG>1
		sprintf(message,"EW_Window::OpenFont() height=%d",match->logfont.lfHeight);
		EW_PRINT(EW_GFX,EW_LOG,message);

		ewDebugDescribeWin32Font( &(match->logfont) );
#endif

		newhandle->handle=CreateFontIndirect(&(match->logfont));
		if(newhandle->handle == NULL)
			{
			EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::OpenFont(): could not create font");
			return TRUE;
			}

	#endif


	#if EW_GL==EW_OPENGL

		/* OpenGL */

		#if EW_WL==EW_XWIN

			/* OpenGL on X */

			/* generate display lists for font */
			long lists=newhandle->FontInfo->max_char_or_byte2+1;

		#elif EW_WL==EW_WIN32

			/* OpenGL on Win32 */

			/* uh, is there a way to find these out? */
			newhandle->first=0;
			newhandle->last=255;

			long lists=newhandle->last+1;

		#else
			#error EW_WL not properly defined
		#endif

		newhandle->FontBase=glGenLists(lists);
		if(newhandle->FontBase==0)
			{
			sprintf(message,"EW_Window::OpenFont(): Out of display lists (asking for %d)",lists);
			EW_PRINT(EW_GFX,EW_ERROR,message);
			return TRUE;
			}

	#elif EW_GL==EW_IRISGL

		/* IrisGL on Mex */

		#if FONTMANAGER

			fmfonthandle font1;

			/* open unit-sized 1 point font */
			font1=fmfindfont(match->name);

			if(font1==NULL)
				{
				EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::OpenFont(): could not open fm font");
				return TRUE;
				}

			/* scale to specified point size */
			newhandle->handle=fmscalefont(font1,match->scale);

			if(newhandle->handle==NULL)
				{
				EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::OpenFont(): could not scale fm font");
				return TRUE;
				}

		#endif

	#endif

	newhandle->valid=TRUE;

	return FALSE;
	}



/******************************************************************************
long	EW_Window::SetFont(EW_Font *fontdata)

	returns FALSE if successful

******************************************************************************/
long EW_Window::SetFont(EW_Font *fontdata)
	{
#if EW_SETFONT_DEBUG
	EW_PRINT(EW_APP,EW_LOG,"EW_Window::SetFont()");
#endif

	if(!fontdata)
		{
		EW_PRINT(EW_APP,EW_ERROR,"EW_Window::SetFont(): NULL EW_Font structure");

		return TRUE;
		}

	if( !fontdata->GetValid() )
		{
		if(default_font.GetValid())
			{
			EW_PRINT(EW_APP,EW_WARN,"EW_Window::SetFont(): font not valid, using default");
			current_font.CopyFrom(&default_font);
			}
		else
			{
			if(graphics_initialized)
				EW_PRINT(EW_APP,EW_WARN,"EW_Window::SetFont(): font and default are both invalid");
/*
			else
				EW_PRINT(EW_APP,EW_WARN,"EW_Window::SetFont(): graphics not initialized; font and default are both invalid");
*/

			return TRUE;
			}
		}

#if EW_TEXT_DEBUG>1
	static char message[200];
#endif

	EW_FontHandle *newhandle=fontdata->fonthandle;

	WinSet(FALSE);

	if(GetOpened())
		{
		#if EW_GL==EW_X_GFX

			/* Pure X Windows */
			if(GetDoubleBuffer())
				{
				XSetFont(display,xdbuf_context[0],newhandle->FontInfo->fid);
				XSetFont(display,xdbuf_context[1],newhandle->FontInfo->fid);
				}
			else
				XSetFont(display,GetGC(),newhandle->FontInfo->fid);

		#endif


		#if EW_WL==EW_WIN32

			/* Win32 (OpenGL or GDI) */

			if(!current_font.GetValid() || newhandle->handle!=current_font.fonthandle->handle || !font_set)
				{
				font_set=TRUE;
#if EW_TEXT_DEBUG
				if(current_font.GetValid())
					sprintf(message,"EW_Window::SetFont(): replacing HFONT 0x%x with 0x%x",
											current_font.fonthandle->handle,newhandle->handle);
				else
					sprintf(message,"EW_Window::SetFont(): replacing HFONT <undefined> with 0x%x",
											newhandle->handle);

				EW_PRINT(EW_GFX,EW_LOG,message);
#endif

				if( (SelectObject(hdc,newhandle->handle)) == NULL)
					{
					EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::SetFont(): could not select font object");

					/* try to resort back to previous font */
					if(current_font.fonthandle!=newhandle && SetFont(&current_font))
						EW_PRINT(EW_GFX,EW_FATAL,"EW_Window::SetFont(): could not restore previous font");

					return TRUE;
					}

				#if EW_GL==EW_GDI
					if( SetBkMode(hdc,TRANSPARENT) == 0)
						EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::SetFont(): could not make text background transparent");

					if( SetTextAlign(hdc,TA_BASELINE | TA_LEFT | TA_NOUPDATECP) == GDI_ERROR)
						EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::SetFont(): could not set text alignment");
				#endif
				}
		#endif


		#if EW_GL==EW_OPENGL

			/* OpenGL */

			if(newhandle->FontBase==lastfontbase)
				{
#if EW_TEXT_DEBUG
				EW_PRINT(EW_APP,EW_LOG,"EW_Window::SetFont(): font already set to 0x%x",lastfontbase);
#endif
				}
			else
				{
#if EW_TEXT_DEBUG
				EW_PRINT(EW_APP,EW_LOG,"EW_Window::SetFont(): base from 0x%x to 0x%x",lastfontbase,newhandle->FontBase);
#endif

				int first,last;

#if EW_TEXT_DEBUG
				EW_PRINT(EW_GFX,EW_LOG,"EW_Window::SetFont(): placing font at %d",newhandle->FontBase);
#endif

				#if EW_WL==EW_XWIN

					/* OpenGL on X */

					Font fontid;

					fontid=newhandle->FontInfo->fid;
					first=newhandle->FontInfo->min_char_or_byte2;
					last=newhandle->FontInfo->max_char_or_byte2;

					glXUseXFont(fontid,first,last-first+1,newhandle->FontBase+first);

				#elif EW_WL==EW_WIN32

					long ok;

					/* OpenGL on Win32 */

					first=newhandle->first;
					last=newhandle->last;

#if EW_REPLACE_WGLBITMAP
					ok=ewUseFontBitmaps(hdc,first,last-first+1,newhandle->FontBase+first);
#else
					ok=wglUseFontBitmaps(hdc,first,last-first+1,newhandle->FontBase+first);
#endif

					if(!ok)
						{
						char message[256];
						sprintf(message,"EW_Window::SetFont(): %sUseFontBitmaps(hdc,%d,%d,%d) failed",
											EW_REPLACE_WGLBITMAP? "ew": "wgl",first,last-first+1,newhandle->FontBase+first);
						EW_PRINT(EW_GFX,EW_ERROR,message);
						}

				#else
					#error EW_WL not properly defined
				#endif

				lastfontbase=newhandle->FontBase;
				}

		#elif EW_GL==EW_IRISGL

			/* IrisGL on Mex */

			#if FONTMANAGER

				fmsetfont(newhandle->handle);

			#endif

		#endif
		}

	CopyCurrentFontFrom(fontdata);

	return FALSE;
	}



/******************************************************************************
void	EW_Window::CopyCurrentFontFrom(EW_Font *fontdata)

******************************************************************************/
void EW_Window::CopyCurrentFontFrom(EW_Font *fontdata)
	{
	if( !fontdata->GetValid() )
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::SetCurrentFont() font not valid");
		return;
		}

	current_font.CopyFrom(fontdata);

	font_height=FindFontAttribute(EW_FONTATTR_HEIGHT);

	font_adjustx=0;
	font_adjusty=FindFontAttribute(EW_FONTATTR_DESCENT);
	}



/******************************************************************************
void	EW_Window::SetUnderline(long setting)

Keywords:
******************************************************************************/
void EW_Window::SetUnderline(long setting)
	{
	underline=setting;
	}



/******************************************************************************
void	EW_Window::DrawTextWithShadow(long x,long y,char *string,
													long len,long colorindex)

	draws text with a pseudo-3D shadow

	Note: glGetFloatv(GL_CURRENT_INDEX,&color) is way too slow,
			so must hold color in global

******************************************************************************/
void EW_Window::DrawTextWithShadow(long x,long y,char *string,long len,long colorindex)
	{
	DrawText(x+1,y,  string,len,index_shadow);
	DrawText(x,  y+1,string,len,colorindex);
	}



/******************************************************************************
void	EW_Window::DrawText(long x,long y,char *string,long len,
															long colorindex)

	print string as graphics text

	if len<0, assumes NULL terminated

******************************************************************************/
void EW_Window::DrawText(long x,long y,char *string,long len,long colorindex)
	{
#if FALSE && EW_TEXT_DEBUG>1
	char message[200];
	sprintf(message,"EW_Window::Text(%d,%d,\"%s\")",x,y,string);
	EW_PRINT(EW_GFX,EW_LOG,message);
#endif

	long duplicate=FALSE;

	if(len<(long)(strlen(string)-1))
		len=strlen(string)-1;
	else if(len>=0)
		{
		string=osdStrdup(string);
		string[len]=0;
		duplicate=TRUE;
		}

	long sx,sy;

	GetSize(&sx,&sy);

	if(underline)
		{
		long width=StringWidth(string);

		Line(x,y,x+width,y);
		}

	x+=font_adjustx;
	y+=font_adjusty;

//	RectFill(x,y,x+15,y+7,EW_RED);

	ActivateColor(colorindex);

	#if EW_GL==EW_X_GFX

		/* Pure X Windows */
		XDrawString(display,win,GetGC(),x,EW_REMAP_Y(y,sy),string,strlen(string));
//		Line(x,y,x+10,y);

	#elif EW_GL==EW_OPENGL

		/* GDI side-by-side test */
/*
		SetTextColor(hdc,EwWin32Colortable[EW_RED]);
		TextOut(hdc,x,EW_REMAP_Y(y,sy),string,strlen(string));
		ActivateColor(index_pen);
*/

		/* OpenGL */
		glRasterPos2i(x,y);

		glPushAttrib(GL_LIST_BIT);

		glListBase(current_font.fonthandle->FontBase);
		glCallLists(strlen(string),GL_UNSIGNED_BYTE,(unsigned char *)string);

		glPopAttrib();

	#elif EW_GL==EW_IRISGL

		/* IrisGL on Mex */
		cmov2i(x,y);

		#if FONTMANAGER
			fmprstr(string);
		#else
			charstr((String)string);
		#endif

	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		if( SetTextColor(hdc,EwWin32Colortable[colorindex]) == CLR_INVALID)
			EW_PRINT(EW_GFX,EW_WARN,"EW_Window::Text(): SetTextColor() reports invalid color");

		if( TextOut(hdc,x,EW_REMAP_Y(y,sy),string,strlen(string)) == FALSE)
			EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::Text(): TextOut() failed");

	#else
		#error EW_GL not properly defined
	#endif

	if(duplicate)
		osdFree(string);
	}



/******************************************************************************
long EW_Window::CharWidth(char c)

	return pixel width of single character in current font

Keywords:
******************************************************************************/
long EW_Window::CharWidth(char c)
	{
	char string[2];

	string[0]=c;
	string[1]=0;

	return StringWidth(string);
	}



/******************************************************************************
long	EW_Window::StringWidth(const char *string)

	return pixel width of given string in current font

Keywords:
******************************************************************************/
long EW_Window::StringWidth(const char *string)
	{
	EW_Font *usefont=&current_font;

	if(!usefont->GetValid())
		usefont=&default_font;

	if(!usefont->GetValid())
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::StringWidth() invalid font (%s)",GetTitle());
		return 1;
		}

	#if EW_WL==EW_XWIN

		/* X Windows (Pure or OpenGL) */
		return XTextWidth(usefont->fonthandle->FontInfo,string,strlen(string));

	#elif EW_WL==EW_WIN32

		HDC temp_hdc=hdc;

		// if not opened, can't access window's font, but can use phantom window
		if(!GetOpened())
			{
			if(!GetEventContext())
				return 0;

			EW_Window *phantom=GetEventContext()->GetPhantom();

//			EW_PRINT(EW_GFX,EW_WARN,"EW_Window::StringWidth(): (%s) accessing phantom (%s)",GetTitle(),phantom->GetTitle());

			phantom->SetFont(usefont);
			temp_hdc=phantom->GetHDC();
			}

		/* Win32 (OpenGL or GDI) */
		SIZE size;

		if( GetTextExtentPoint32(temp_hdc,string,strlen(string),&size) == FALSE)
			{
			EW_PRINT(EW_GFX,EW_WARN,"EW_Window::StringWidth(): getting extent failed");
			return 0;
			}
		else
			return size.cx;

	#elif EW_GL==EW_IRISGL

		/* IrisGL on Mex */

		#if FONTMANAGER
			return fmgetstrwidth(font->handle,string);
		#else
			return strwidth((String)string);
		#endif

	#else
		#error EW_GL and EW_WL not properly defined
	#endif
	}



/******************************************************************************
long	EW_Window::GetFontHeight(void)

	returns value previously found by FindFontAttribute()

Keywords:
******************************************************************************/
long EW_Window::GetFontHeight(void)
	{
	if(font_height>0)
		return font_height;
	else
		{
		if(GetOpened())
			EW_PRINT(EW_GFX,EW_WARN,"EW_Window::GetFontHeight(): font_height<=0, using default height");

		return EW_DEFAULT_FONT_HEIGHT;
		}
	}



/******************************************************************************
long	EW_Window::FindFontAttribute(long type)

	determines font attribute from OS-specific calls

	attribute type:
		EW_FONTATTR_ASCENT
		EW_FONTATTR_HEIGHT
		EW_FONTATTR_DESCENT

	Note the tweakings on the ascent and descent values.

Keywords: private
******************************************************************************/
long EW_Window::FindFontAttribute(long type)
	{
	if(!current_font.GetValid())
		{
		EW_PRINT(EW_GFX,EW_WARN,"EW_Window::FindFontAttribute(): font not valid");
		return 1;
		}

	#if EW_WL==EW_XWIN

		/* X Windows (Pure or OpenGL) */

		int direction;			/* direction hint */
		int ascent,descent;		/* pixels above and below baseline */

		XCharStruct overall;	/* overall char structure */

		/* quick test of sample string "Qy," (should get high and low extents) */
		XTextExtents(current_font.fonthandle->FontInfo,"Qy,",3,&direction,&ascent,&descent,&overall);

//		printf("ascent=%d descent=%d\n",ascent,descent);

		switch(type)
			{
			case EW_FONTATTR_HEIGHT:
				return ascent+EW_X_MOD_ASCENT + descent+EW_X_MOD_DESCENT;

			case EW_FONTATTR_ASCENT:
				return ascent+EW_X_MOD_ASCENT;

			case EW_FONTATTR_DESCENT:
				return descent+EW_X_MOD_DESCENT;
			}

	#elif EW_WL==EW_WIN32

		HDC temp_hdc=hdc;

		// if not opened, can't access window's font, but can use phantom window
		if(!GetOpened())
			{
			if(!GetEventContext())
				return 0;

			EW_Window *phantom=GetEventContext()->GetPhantom();

			phantom->SetFont(&current_font);
			temp_hdc=phantom->GetHDC();
			}

		switch(type)
			{
			case EW_FONTATTR_HEIGHT:

				/* Win32 (OpenGL or GDI) */
				SIZE size;

				// numbers can be taller than letters in Win32 fonts
				if( GetTextExtentPoint32(temp_hdc,"1y",2,&size) == FALSE)
					{
					EW_PRINT(EW_GFX,EW_WARN,"EW_Window::FindFontAttribute(): getting extent failed, using default height");
					return EW_DEFAULT_FONT_HEIGHT;
					}
				else
					return size.cy;
//					return size.cy-EW_WIN32_FONT_ADJUST;

			case EW_FONTATTR_ASCENT:
			case EW_FONTATTR_DESCENT:

				TEXTMETRIC metric;

				if( GetTextMetrics(temp_hdc,&metric) == FALSE )
					{
					EW_PRINT(EW_GFX,EW_WARN,"EW_Window::FindFontAttribute(): GetTextMetrics() failed");
					return 0;
					}
				else if(type==EW_FONTATTR_ASCENT)
					return metric.tmAscent;
				else
					return metric.tmDescent;
//					return metric.tmDescent-EW_WIN32_FONT_ADJUST;
			}

	#elif EW_GL==EW_IRISGL

		/* IrisGL on Mex */

		#if FONTMANAGER
			fmfontinfo info;

			if(fmgetfontinfo(font->handle,&info))
				{
				if(type==EW_FONTATTR_DESCENT)
					{
					EW_PRINT(EW_GFX,EW_WARN,"EW_Window::FindFontAttribute(): fontmanager font problem, using default");
					return 0;
					}
				else
					{
					EW_PRINT(EW_GFX,EW_WARN,"EW_Window::FindFontAttribute(): fontmanager font problem, using default");
					return EW_DEFAULT_FONT_HEIGHT;
					}
				}
			else
				{
				switch(type)
					{
					case EW_FONTATTR_HEIGHT:
						return info.height;

					case EW_FONTATTR_ASCENT:
						return info.height-info.yorig;

					case EW_FONTATTR_DESCENT:
						return info.yorig;
					}
				}

		#else
			switch(type)
				{
				case EW_FONTATTR_HEIGHT:
					return getheight();

				case EW_FONTATTR_ASCENT:
					return getheight()-getdescender();

				case EW_FONTATTR_DESCENT:
					return getdescender();
				}
		#endif


				/* text drawing already relative to bottom */
				return 0;

	#else
		#error EW_GL and EW_WL not properly defined
	#endif

	EW_PRINT(EW_GFX,EW_WARN,"EW_Window::FindFontAttribute(): illegal type");
	return 0;
	}



/******************************************************************************
long	EW_Window::ProbeFontHeight(char *name)

******************************************************************************/
long EW_Window::ProbeFontHeight(char *name)
	{
#if EW_WL==EW_XWIN
	XFontStruct *fontinfo=XLoadQueryFont(GetDisplay(),name);
	if(fontinfo==NULL)
		{
		EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::ProbeFontHeight(%s): X font not found",name);
		return -1;
		}

	int direction;			/* direction hint */
	int ascent,descent;		/* pixels above and below baseline */
	XCharStruct overall;	/* overall char structure */

	/* quick test of sample string "Qy," (should get high and low extents) */
	XTextExtents(fontinfo,"Qy,",3,&direction,&ascent,&descent,&overall);

	long height=ascent+EW_X_MOD_ASCENT + descent+EW_X_MOD_DESCENT;

	XFreeFont(GetDisplay(),fontinfo);

	return height;
#else

	return -1;
#endif
	}



/******************************************************************************
long	EW_Window::MatchFont(EW_FontRequest *request,EW_FontMatch *match)

	sets font match for given request

	returns TRUE if there was a problem

Keywords: private
******************************************************************************/
long EW_Window::MatchFont(EW_FontRequest *request,EW_FontMatch *match)
	{
#if EW_TEXT_DEBUG>1
	char message[200];
#endif

	EW_StringList *hintlist=request->GetHintList();
	long hint=0;
	long hints=hintlist->GetNumberNodes();

	#if EW_WL==EW_XWIN
		const long fixed_offset=2;	/* index of first fixed font */
		const long fontnames=5;		/* number of font names */
		static const char fontname[fontnames][4][50]=
			{
			/* font name				light		dark		italic (x=can't) */

			{"itc avant garde gothic",	"book",		"demi",		"o"},
			{"helvetica",				"medium",	"bold",		"o"},
//			{"lucida",					"medium",	"bold",		"i"},
			{"clean",					"medium",	"bold",		"i"},
			{"fixed",					"medium",	"bold",		"x"},
			{"screen",					"medium",	"bold",		"i"}
			};

		char pattern[200],string[200];
		char **fontlist;

		short fontnumber= (request->GetStyle() & EW_FONT_PROPORTIONAL)? 0: fixed_offset;
		short matched=FALSE;

		int matches;

		float score,best_score=50.0;	/* best score wins */

#if EW_TEXT_DEBUG>1
		sprintf(message,"EW_Window::MatchFont() matching %.6G style %x",request->GetHeight(),request->GetStyle());
		EW_PRINT(EW_GFX,EW_LOG,message);
#endif

		long hintcount=0;

		while(hint<hints*2 || fontnumber<fontnames)
			{
			long is_hint=FALSE;

			if(hint<hints*2)
				{
				if( !(hint%hints) )
					hintcount=0;

				is_hint=TRUE;

				if(hint<hints)
					{
					// try assuming only family-weight-slant is specified
					hintlist->ToIndex(hint)->CopyToBuffer(string);
					sprintf(pattern,"-*-%s-*-*-*-*-*-*-*-*-*-*",string);
					}
				else
					{
					// try assuming complete specification
					hintlist->ToIndex(hint-hints)->CopyToBuffer(pattern);
					}

				hint++;
				}
			else
				{
				sprintf(pattern,"-*-%s-%s-%s-*-*-*-*-*-*-*-*-*-*",fontname[fontnumber][0],
										((request->GetStyle())&EW_FONT_BOLD)?	fontname[fontnumber][2]: fontname[fontnumber][1],
										((request->GetStyle())&EW_FONT_ITALIC)?	fontname[fontnumber][3]: "r");
				}

#if EW_TEXT_DEBUG>1
		sprintf(message,"EW_Window::MatchFont() using \"%s\"",pattern);
		EW_PRINT(EW_GFX,EW_LOG,message);
#endif

			fontlist=XListFonts(display,pattern,100,&matches);

			if(matches)
				{
				long m;

				short matchindex= -1;
				float namedsize,actual;

				for(m=0;m<matches;m++)
					{
					ewExtractArgument(7,'-',fontlist[m],string);

					namedsize=atof(string);

					// tweakable scoring equation - part 1 (best possible)
					score= (is_hint? hintcount: 5) + 0.5*fontnumber + ( (fontnumber>=fixed_offset)? 5: 0 );

					// if not even close, don't waste the time loading the font
					if( fabs(request->GetHeight()-namedsize)<3 && score<best_score)
						actual=ProbeFontHeight(fontlist[m]);
					else
						{
						actual=namedsize;
						namedsize=99.0;		// so we know it was used
						}

					if(actual<0.0)
						continue;

					// tweakable scoring equation - part 2 (adjustment for size discrepency)
					score+= fabs(request->GetHeight() - actual);

#if EW_TEXT_DEBUG>1
					sprintf(message,"EW_Window::MatchFont()  height=%.2f/%.2f reported=%.2f -> score %.6G on \"%s\"",
																	actual,request->GetHeight(),namedsize,score,fontlist[m]);
					EW_PRINT(EW_GFX,EW_LOG,message);
#endif

					if( best_score>score && actual>0.0 )
						{
#if EW_TEXT_DEBUG>1
				sprintf(message,"EW_Window::MatchFont()    LOWEST SCORE SO FAR (%.6G vs %.6G)",score,best_score);
						EW_PRINT(EW_GFX,EW_LOG,message);
#endif

						best_score=score;
						matchindex=m;
						matched=TRUE;
						}
					}

				hintcount++;

				if(matchindex>=0)
					strcpy(match->name,fontlist[matchindex]);

				XFreeFontNames(fontlist);
				}

			if(!is_hint)
				fontnumber++;
			}

		if(!matched)
			{
			match->name[0]=0;
			return TRUE;
			}
		else
			{
#if EW_TEXT_DEBUG>1
			sprintf(message,"EW_Window::MatchFont() match: \"%s\"",match->name);
			EW_PRINT(EW_GFX,EW_LOG,message);
#endif
			return FALSE;
			}

	#elif EW_WL==EW_MEX
		#if FONTMANAGER

			const long fixed_offset=3;	/* index of first fixed font */
			const long fontnames=6;		/* number of font names */
			static const char fontname[6][4][50]=
				{
				"AvantGarde-Book",	"AvantGarde-BookOblique",	"AvantGarde-Demi",	"AvantGarde-DemiOblique",
				"Helvetica",		"Helvetica-Oblique",		"Helvetica-Bold",	"Helvetica-BoldOblique",
				"LucidaSans-Roman",	"LucidaSans-Italic",		"LucidaSans-Bold",	"LucidaSans-BoldItalic",
				"Clean",			"Clean-Italic",				"Clean-Bold",		"",
				"Fixed",			"",							"Fixed-Bold",		"",
				"Screen",			"",							"Screen-Bold",		"",
				};

			fmfonthandle handle=NULL;

			short fontnumber= (request->GetStyle() & EW_FONT_PROPORTIONAL)? 0: fixed_offset;

			short x= ( (request->GetStyle() & EW_FONT_BOLD)? 2: 0) + ( (request->GetStyle() & EW_FONT_ITALIC)? 1: 0);

			while( fontnumber<fontnames && !(handle=fmfindfont(fontname[fontnumber][x])) )
				fontnumber++;

		if(handle)
			{
			fmfreefont(handle);

			strcpy(match->name,fontname[fontnumber][x]);

			/* tweaked scaling correction ( why sqrt(2.0)? ) */
			match->scale=request->GetHeight()/sqrt(2.0);

#if EW_TEXT_DEBUG>1
			sprintf(message,"EW_Window::MatchFont() match: \"%s\" %.6G",match->name,match->scale);
			EW_PRINT(EW_GFX,EW_LOG,message);
#endif

			return FALSE;
			}
		else
			{
			match->name[0]=0;
			return TRUE;
			}

		#else

			/* IrisGL without Font Manager */
			strcpy(match->name,"default");
			return FALSE;

		#endif

	#elif EW_WL==EW_WIN32

#if EW_TRUETYPE_ONLY
		const long fixed_offset=2;	/* index of first fixed font */
		const long fontnames=5;		/* number of font names */
		static const char fontname[fontnames][LF_FACESIZE]=
			{
			"Arial Narrow",
			"Arial",
			"Lucida Console",
			"Times New Roman",
			"Courier New",
			};
#else
		const long fixed_offset=3;	/* index of first fixed font */
		const long fontnames=4;		/* number of font names */
		static const char fontname[fontnames][LF_FACESIZE]=
			{
			"MS Sans Serif",
			"Small Fonts",
			"System",
			"Terminal",
			};
#endif
		char pattern[200];

		short fontnumber= (request->GetStyle() & EW_FONT_PROPORTIONAL)? 0: fixed_offset;

		long matches;
		long matched=FALSE;

		float score,best_score=50.0;	/* best score wins */
		LOGFONT fontlist[100];

		float pointsize=request->GetHeight()+EW_WIN32_FONT_ADJUST;
//		float mapped_height= MulDiv(pointsize,GetDeviceCaps(hdc,LOGPIXELSY),72);

#if EW_TEXT_DEBUG>1
		sprintf(message,"EW_Window::MatchFont() matching %.6G style %x",pointsize,request->GetStyle());
		EW_PRINT(EW_GFX,EW_LOG,message);
#endif

		// NOTE: using TrueType scaling, size is not an issue -> keep first
#if EW_TEXT_DEBUG>1
		sprintf(message,"hint=%d/%d fontnumber=%d/%d match=%d EW_TRUETYPE_ONLY=%d",
											hint,hints,fontnumber,fontnames,match,EW_TRUETYPE_ONLY);
		EW_PRINT(EW_GFX,EW_LOG,message);
#endif
		while( (hint<hints || fontnumber<fontnames) && (!matched || !EW_TRUETYPE_ONLY) )
			{
			long is_hint=FALSE;

			if(hint<hints)
				{
				is_hint=TRUE;

				hintlist->ToIndex(hint)->CopyToBuffer(pattern);

				hint++;
				}
			else
				{
				strcpy(pattern,fontname[fontnumber]);
				}

#if EW_TEXT_DEBUG>1
			sprintf(message,"EW_Window::MatchFont()  scanning \"%s\"",pattern);
			EW_PRINT(EW_GFX,EW_LOG,message);
#endif

			fontlist[0].lfHeight=0;

			/* fill fontlist[] using custom callback ewEnumFontFamProc() */
			EnumFontFamilies(hdc,pattern,

/* old Alpha compiler required this cast */
#if TRUE
					(int(__stdcall *)(const struct tagLOGFONTA*,const struct tagTEXTMETRICA*,unsigned long,long))&ewEnumFontFamProc,

#else
					(int(__stdcall *)(void))&ewEnumFontFamProc,
#endif
					(long)fontlist);

			matches=fontlist[0].lfHeight;

			if(matches>0)
				{
				long m;

				short matchindex= -1;
				float actual;

				/* look through matches; since TrueType fonts are scalable, take the first one found */
				for(m=1;m<matches+1 && !(matched && EW_TRUETYPE_ONLY);m++)
					{
					actual=fontlist[m].lfHeight;

/*
					long converted=(actual*0.625+0.9)/2;
					converted*=2;

					EW_PRINT(EW_GFX,EW_LOG,"pointsize=%.6G -> %.6G %d\n",
										actual,72*actual/GetDeviceCaps(hdc,LOGPIXELSY),GetDeviceCaps(hdc,LOGPIXELSY));
					EW_PRINT(EW_GFX,EW_LOG,"pointsize=%.6G -> %d\n",actual,converted);
*/

					/* tweakable scoring equation */
					score= fabs(pointsize - actual) + fontnumber;

					long is_truetype= (fontlist[m].lfOutPrecision&OUT_TT_ONLY_PRECIS) != 0;

#if EW_TEXT_DEBUG>1
					sprintf(message,"EW_Window::MatchFont()   score %.6G on \"%s\":%.6G  truetype=%d",
																							score,pattern,actual,is_truetype);
					EW_PRINT(EW_GFX,EW_LOG,message);
#endif

// is_truetype=TRUE;

					if(best_score>score && (is_truetype || !EW_TRUETYPE_ONLY) )
						{
#if EW_TEXT_DEBUG>1
						sprintf(message,"EW_Window::MatchFont()    LOWEST SCORE SO FAR (%.6G vs %.6G)",score, best_score);
						EW_PRINT(EW_GFX,EW_LOG,message);
#endif

						best_score=score;
						matchindex=m;
						matched=TRUE;
						}
					}

				if(matchindex>=0)
					{
					osdMemcpy(&(match->logfont),&fontlist[matchindex],sizeof(LOGFONT));

					/* for consistency with other architectures */
					strcpy(match->name,pattern);
					}
				}

			fontnumber++;
			}

		if(!matched)
			{
#if EW_TEXT_DEBUG>1
			EW_PRINT(EW_GFX,EW_LOG,"EW_Window::MatchFont()  NO MATCH");
#endif
			match->name[0]=0;
			return TRUE;
			}
		else
			{
#if EW_TEXT_DEBUG>1
			sprintf(message,"EW_Window::MatchFont() match: \"%s\" point %.6G",match->name,pointsize);
			EW_PRINT(EW_GFX,EW_LOG,message);
#endif

			(match->logfont).lfWeight=			(request->GetStyle() & EW_FONT_BOLD)? 700: 400;
			(match->logfont).lfItalic=			(request->GetStyle() & EW_FONT_ITALIC)? TRUE: FALSE;

#if EW_TRUETYPE_ONLY
												// negative means (absolute) value in pixels instead of Device units
			(match->logfont).lfHeight=			-pointsize;

			(match->logfont).lfWidth=			0;	/* 0 = automatically retain aspect ratio */
#endif

/*
			osdMemset(&(match->logfont),0,sizeof((match->logfont)));

			(match->logfont).lfCharSet=			ANSI_CHARSET;
			(match->logfont).lfOutPrecision=	OUT_DEFAULT_PRECIS;
			(match->logfont).lfClipPrecision=	CLIP_DEFAULT_PRECIS;
			(match->logfont).lfQuality=			PROOF_QUALITY;
			(match->logfont).lfPitchAndFamily= 	FF_DONTCARE | (request->GetStyle() & EW_FONT_PROPORTIONAL)? VARIABLE_PITCH: FIXED_PITCH;
			strcpy((match->logfont).lfFaceName,	match->name);
*/

			return FALSE;
			}
	#else
		#error EW_WL not properly defined
	#endif

	}



#if EW_WL==EW_WIN32

/******************************************************************************
int CALLBACK ewEnumFontFamProc(ENUMLOGFONT FAR* lpelf,
						NEWTEXTMETRIC FAR* lpntm,int FontType,LPARAM lParam)

Keywords:
******************************************************************************/
int CALLBACK ewEnumFontFamProc(ENUMLOGFONT FAR* lpelf,NEWTEXTMETRIC FAR* lpntm,int FontType,LPARAM lParam)
	{
#if EW_TEXT_DEBUG>1

	EW_PRINT(EW_GFX,EW_LOG,"ewEnumFontFamProc() callback: Font Description");
	ewDebugDescribeWin32Font( &(lpelf->elfLogFont) );

	char message[200];
	sprintf(message,"ewEnumFontFamProc() type=0x%x (0x%x,0x%x,0x%x)",FontType,DEVICE_FONTTYPE,RASTER_FONTTYPE,TRUETYPE_FONTTYPE);
	EW_PRINT(EW_GFX,EW_LOG,message);

#endif

	LOGFONT *fontlist=(LOGFONT *)lParam;

	long entries=fontlist[0].lfHeight;

	entries++;

	osdMemcpy(&fontlist[entries],&(lpelf->elfLogFont),sizeof(LOGFONT));

	fontlist[0].lfHeight=(float)entries;

	return TRUE;
	}



/******************************************************************************
void	ewDebugDescribeWin32Font(LOGFONT *logfont)

Keywords:
******************************************************************************/
void ewDebugDescribeWin32Font(LOGFONT *logfont)
	{
	char piece[100],message[500];

	sprintf(message,"ewDebugDescribeWin32Font(): \"%s\"\n",logfont->lfFaceName);
	sprintf(piece,	"  lfHeight=%d ",logfont->lfHeight);
	strcat(message,piece);
	sprintf(piece,	"lfWidth=%d ",logfont->lfWidth);
	strcat(message,piece);
	sprintf(piece,	"lfWeight=%d ",logfont->lfWeight);
	strcat(message,piece);
	sprintf(piece,	"lfItalic=%d\n",logfont->lfItalic);
	strcat(message,piece);
	sprintf(piece,	"  lfPitchAndFamily=0x%x (0x%x,0x%x,0x%x)",
												logfont->lfPitchAndFamily,FF_DONTCARE,VARIABLE_PITCH,FIXED_PITCH);
	strcat(message,piece);

	EW_PRINT(EW_GFX,EW_LOG,message);
	}



#if EW_GL==EW_OPENGL // && EW_WL==EW_WIN32

/******************************************************************************
BOOL	ewUseFontBitmaps(HDC hdc,DWORD first,DWORD count,DWORD listBase)

	NOT FOR USE

	intended replacement for wglUseFontBitmaps()

	However, although wglUseFontBitmaps() has problems with non-TrueType
	fonts, ewUseFontBitmaps() can't do them at all

	It never really worked for TrueType fonts either.

Keywords: Archival
******************************************************************************/
BOOL ewUseFontBitmaps(HDC hdc,DWORD first,DWORD count,DWORD listBase)
	{
	GLYPHMETRICS glyphmetrics;
	MAT2 matrix;

	char message[200];
	unsigned char *buffer;

	long m;
	long buffersize;

	/* identity transformation */
	matrix.eM11.value=1;
	matrix.eM11.fract=0.0;
	matrix.eM12.value=0;
	matrix.eM12.fract=0.0;
	matrix.eM21.value=0;
	matrix.eM21.fract=0.0;
	matrix.eM22.value=1;
	matrix.eM22.fract=0.0;

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	/* don't start before 32 */
	if(first<32)
		{
		count-= 32-first;
		first=32;
		}

	for(m=0;m<count;m++)
		{
		buffersize=GetGlyphOutline(hdc,'x',GGO_BITMAP,&glyphmetrics,0,NULL,&matrix);

		sprintf(message,"ewUseFontBitmaps(): %d char=%d '%c', buffersize=%d (%d)",m,first+m,first+m,buffersize,GDI_ERROR);
		EW_PRINT(EW_GFX,EW_LOG,message);

		if(buffersize==GDI_ERROR)
			{
			sprintf(message,"GetGlyphOutline(%d,%d,GGO_BITMAP,&glyphmetrics,0,NULL,&matrix) failed (getting buffersize)\n",
																									hdc,first+m);
			EW_PRINT(EW_GFX,EW_ERROR,message);
			return FALSE;
			}

		buffer=new unsigned char[buffersize];

		sprintf(message,"GetGlyphOutline(%d,%d,GGO_BITMAP,&glyphmetrics,%d,%d,&matrix) failed (getting bitmap)\n",
																								hdc,first+m,buffersize,buffer);
		EW_PRINT(EW_GFX,EW_LOG,message);

		if( GetGlyphOutline(hdc,first+m,GGO_BITMAP,&glyphmetrics,buffersize,buffer,&matrix) == FALSE)
			{
			sprintf(message,"GetGlyphOutline(%d,%d,%d,&glyphmetrics,%d,%d,&matrix) failed (getting bitmap)\n",
																						hdc,first+m,GGO_BITMAP,buffersize,buffer);
			EW_PRINT(EW_GFX,EW_ERROR,message);
			return FALSE;
			}

		sprintf(message,"ewUseFontBitmaps(): list %d <- glBitmap(%d,%d,%d,%d,%d,%d,buffer)",
				listBase+first+m,
				glyphmetrics.gmBlackBoxX,
				glyphmetrics.gmBlackBoxY,
				glyphmetrics.gmptGlyphOrigin.x,
				glyphmetrics.gmptGlyphOrigin.y,
				glyphmetrics.gmCellIncX,
				glyphmetrics.gmCellIncY
				);
		EW_PRINT(EW_GFX,EW_LOG,message);

		glNewList(listBase+first+m,GL_COMPILE);
		glBitmap(
				glyphmetrics.gmBlackBoxX,
				glyphmetrics.gmBlackBoxY,
				glyphmetrics.gmptGlyphOrigin.x,
				glyphmetrics.gmptGlyphOrigin.y,
				glyphmetrics.gmCellIncX,
				glyphmetrics.gmCellIncY,
				buffer
				);
		glEndList();

		sprintf(message,"ewUseFontBitmaps(): %d delete buffer",m);
		EW_PRINT(EW_GFX,EW_LOG,message);

		delete buffer;
		}

	return TRUE;
	}

#endif // EW_GL==EW_OPENGL ( && EW_WL==EW_WIN32 )

#endif // EW_WL==EW_WIN32



/******************************************************************************
long	ewExtractArgument(long number,char separator,char *input,char *output)

	extract string between (number)th and (number+1)th instance of separator
		in input and copy into output (0th arg is before first separator)

	example:
		ewExtractArgument(2,"-","abc-de-fghi-jk",output)
			sets output to "fghi"

	returns 0 if successful

Keywords:
******************************************************************************/
/*PROTOTYPE*/
long ewExtractArgument(long number,char separator,char *input,char *output)
	{
	char format[10];

	long length=strlen(input);
	long instance=0;
	long m=0;
	long success;


	while(m<length && instance<number)
		{
		if(input[m]==separator)
			instance++;
		m++;
		}

	if(( success=(m<length) ))
		{
		sprintf(format,"%%[^%c]",separator);
		sscanf(&input[m],format,output);
		}

/* 	printf("Argument %d in \"%s\" is \"%s\" (%s)\n",number,input,output,format); */

	return success;
	}



/******************************************************************************
		EW_FontHandle::EW_FontHandle(void)

******************************************************************************/
EW_FontHandle::EW_FontHandle(void)
	{
	valid=FALSE;

#if EW_WL==EW_XWIN

	FontInfo=NULL;
	FontDisplay=NULL;

#endif
	}



/******************************************************************************
		EW_FontHandle::~EW_FontHandle(void)

******************************************************************************/
EW_FontHandle::~EW_FontHandle(void)
	{
#if EW_WL==EW_XWIN

#if EW_DELETE_FONTS
	if(valid && FontDisplay && FontInfo)
		{
		XFreeFont(FontDisplay,FontInfo);
		}
#endif

#endif
	}



/******************************************************************************
		EW_Font::EW_Font(void)

******************************************************************************/
EW_Font::EW_Font(void)
	{
	fonthandle=NULL;
	}



/******************************************************************************
		EW_Font::~EW_Font(void)

******************************************************************************/
EW_Font::~EW_Font(void)
	{
	}



/******************************************************************************
void	EW_FontRequest::AddHint(char *fontname)

******************************************************************************/
void EW_FontRequest::AddHint(char *fontname)
	{
	EW_IString *newstring=new EW_IString();

	newstring->NewBuffer(fontname);

	hintlist.AppendNode(newstring);
	}
