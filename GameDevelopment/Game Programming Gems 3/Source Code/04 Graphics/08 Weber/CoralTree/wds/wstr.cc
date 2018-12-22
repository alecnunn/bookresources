/******************************************************************************

Coral Tree wds/wstr.cc
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


#include "wds.h"



#define WDS_STRING_LINE				FALSE	// TRUE = display text as line only
#define WDS_STRING_BOX				FALSE
#define WDS_STRING_MAXPIXELWIDTH	1600	// widest a string line might be


#define WDS_STRING_DEBUG			FALSE	// TRUE = verbosity

#define WDS_TEXT_XOFFSET			0
#define WDS_TEXT_MAX_LINES			256
#define WDS_TEXT_MAX_CHARS			1024



/******************************************************************************
		WDS_String::WDS_String()

Keywords:
******************************************************************************/
WDS_String::WDS_String()
	{
	firstline = 0;
	cursor= -1;
	cursorx=0;
	rangemark= -1;
	cursorbg=EW_PURPLE;
	cursorfg=EW_BLACK;
	confirming=FALSE;
	confirm_flags=WDS_CONFIRM_NULL;
	allow_confirm=TRUE;

	SetEditable(TRUE);
	SetInvalid(FALSE);
	SetHighlight(FALSE);
	SetConfirmCallback(NULL,WDS_CONFIRM_NULL);
	SetStyle(WDS_STRING_STYLE_DEFAULT);
	SetDisplayChar((char)0);
	}



/******************************************************************************
void	WDS_String::CopyFromWDS_String(WDS_String *other)

******************************************************************************/
void WDS_String::CopyFromWDS_String(WDS_String *other)
	{
	// overload, not virtual
	EW_String *estring=other;
	CopyFromString(estring);

	SetEditable(other->GetEditable());
	SetHighlight(other->GetHighlight());
	SetStyle(other->GetStyle());
	SetDisplayChar(other->GetDisplayChar());

	firstline=other->firstline;
	cursor=other->cursor;
	cursorx=other->cursorx;
	rangemark=other->rangemark;
	cursorbg=other->cursorbg;
	cursorfg=other->cursorfg;
	confirming=other->confirming;
	confirm_function=other->confirm_function;
	confirm_flags=other->confirm_flags;
	}



/******************************************************************************
long	WDS_String::GetStringSpace(EW_Window *root,long maxwidth,
													long *width,long *height)

******************************************************************************/
long WDS_String::GetStringSpace(EW_Window *root,long maxwidth,long *width,long *height)
	{
	if(maxwidth<0)
		maxwidth=WDS_STRING_MAXPIXELWIDTH;

	long cly=root->GetFontHeight();

	if(!root->GetMapped())
		{
		*width=ModStringWidth(root,Buffer());
		*height=cly;
		return 0;
		}

	long lines=GetNumberLines(root,maxwidth);

	*width=widest;
	*height= lines*cly + (lines-1);

//	printf("GetStringSpace() lines=%d width=%d height=%d string=(%s)\n",lines,*width,*height,Buffer());

	return lines;
	}



/******************************************************************************
long	WDS_String::GetNumberLines(EW_Window *root,long maxwidth)

******************************************************************************/
long WDS_String::GetNumberLines(EW_Window *root,long maxwidth)
	{
	Operate(root, 0,0, maxwidth,0, 0,0, 0);
	return numberlines;
	}



/******************************************************************************
long	WDS_String::Operate(EW_Window *root, long x, long y, long w, long h,
												long mx, long my, long flags)

Keywords:
******************************************************************************/
long WDS_String::Operate(EW_Window *root, long x, long y, long w, long h, long mx, long my, long flags)
{
	static char part[WDS_TEXT_MAX_LINES][WDS_TEXT_MAX_CHARS];
	static char piece[WDS_TEXT_MAX_CHARS];
	static char buf[WDS_TEXT_MAX_CHARS];
	static long partindex[WDS_TEXT_MAX_LINES];

	const char *text=Buffer();

	long cly=root->GetFontHeight();	// absolute character size
	long cly1=cly+1;				// character size with pixel of space
	long maxlines= (h+1)/cly1;

//	printf("cly=%d cly1=%d h=%d maxlines=%d\n",cly,cly1,h,maxlines);

	long showcount = 0,count=0,partcount;
	long length=Length();
	long sincespace=0;
	long done=FALSE;
	long cursorline= -1,cursorwidth=0;
	long rangemarkline= -1,rangemarkx=0,rangemarkwidth=0;
	long c=0,i=0,l=0;

	if( maxlines==0 || (maxlines && (flags & WDS_STRING_SINGLELINE)) )
		maxlines = 1;

	if(maxlines<0 || maxlines>WDS_TEXT_MAX_LINES)
		maxlines=WDS_TEXT_MAX_LINES;

	w -= WDS_TEXT_XOFFSET * 2;

	long lines=0;
	widest=0;
//	while(!done && lines<maxlines)
	while(!done && lines<WDS_TEXT_MAX_LINES)
		{
		osdMemset(part[lines],0,WDS_TEXT_MAX_CHARS);
		partcount=0;
		sincespace=0;
		long newline=FALSE;
		long endline=FALSE;
		long partwidth=0;
		long charwidth;

		done= (text[count]==0);

		while( !newline && partcount<WDS_TEXT_MAX_CHARS &&
									(text[count]=='\n' || (partwidth+(charwidth=ModCharWidth(root,text[count]))<=w)) )
			{
			if(EW_IS_WHITESPACE(text[count]))
				sincespace=0;
			else
				sincespace++;

			if(count==GetCursorPosition() || count==GetRangeMarkPosition())
				{
				if(count==GetCursorPosition())
					{
#if WDS_STRING_DEBUG
					printf("cursor on line %d\n",lines);
#endif

					cursorline=lines;
					cursorx=partwidth;
					cursorwidth=charwidth;
					}

				if(count==GetRangeMarkPosition())
					{
					rangemarkline=lines;
					rangemarkx=partwidth;
					rangemarkwidth=charwidth;
					}

				if(flags&WDS_STRING_CURSOR_STARTLINE)
					{
					cursor=lines? partindex[lines-1]: 0;
					}

				if(flags&WDS_STRING_CURSOR_ENDLINE)
					endline=TRUE;
				}

			partwidth+=charwidth;

			if(text[count]==0 || text[count]=='\n')
				{
				newline=TRUE;
				}
			else
				{
				if(GetDisplayChar())
					part[lines][partcount++]=GetDisplayChar();
				else
					part[lines][partcount++]=text[count];
				}

			count++;
			}

		// make sure it's set right
		partwidth=ModStringWidth(root,part[lines]);

		if(text[count-1]==0)
			done=TRUE;
	
		if(!done && text[count-1]!='\n')
			{
			if(sincespace==partcount)
				sincespace= (partwidth>w);

			part[lines][partcount-sincespace]=0;
			count-=sincespace;
			}
		else if(cursorline<0)
			{
#if WDS_STRING_DEBUG
			printf("cursor defaults to %d\n",lines);
#endif

			cursorline=lines;
			cursorx=partwidth;
			cursorwidth=WDS_TEXT_XOFFSET;
			}

		long stringwidth=partwidth;
		if(widest<stringwidth)
			widest=stringwidth;

/*
		if(text[count]=='\n')
			count++;
*/

#if FALSE
		/* remove trailing white space */
		long partlength=strlen(part[lines]);
		while( partlength>0 && EW_IS_WHITESPACE(part[lines][partlength-1]) )
			part[lines][--partlength]=0;
#endif

		partindex[lines] = count;

		if(endline)
			cursor=EW_MIN(count-1,length);

		lines++;
		}

#if WDS_STRING_DEBUG
		printf("pos=%d,%d cursor=%d line %d rangemark=%d line %d lines=%d/%d\n",GetCursorPosition(),GetRangeMarkPosition(),
					cursorx,cursorline,rangemarkx,rangemarkline,lines,maxlines);
		long m;
		for(m=0;m<lines;m++)
			 printf("part[%d]=\"%s\"\n",m,part[m]);
#endif


	numberlines=lines;

	long initial_cline = cursorline;
	if(GetCursorPosition() < 0)
		{
		firstline = 0;
		}
	else
		{
		if(flags & WDS_STRING_CURSOR_UP)
			{
			if(cursorline > 0)
				cursorline--;
			}
		else if(flags & WDS_STRING_CURSOR_DOWN)
			{
			if(cursorline < lines-1)
				cursorline++;
			}

		if(cursorline < firstline)
			{
			firstline = cursorline;
			}
		else if(cursorline >= firstline + maxlines)
			{
			firstline = cursorline - maxlines + 1;
			}

		}

	long tlines;

	if(lines > maxlines)
		tlines = maxlines;
	else
		tlines = lines;

	long lastline = firstline + tlines - 1;

	if(lastline >= lines)
	{
		lastline = lines - 1;
		firstline = lastline - tlines + 1;
	}

/*
if(flags & WDS_STRING_DRAW)
printf("firstline=%d lastline=%d lines=%d tlines=%d maxlines=%d (%s)\n",firstline,lastline,lines,tlines,maxlines,part[0]);
*/

	long offy=(h-tlines*cly1)/2+(tlines-1)*cly1;
	long line;
	long hit=FALSE;
	long in_range=FALSE;
	if(maxlines)
	{
		long underline=root->GetUnderline();
		root->SetUnderline(GetStyle()&WDS_STRING_STYLE_UNDERLINE);

		for(line=firstline;line<=lastline;line++)
		{
			long mx_adj;
			long ltx;
			long tx;
			long ty=y+offy;
			long partwidth=ModStringWidth(root,part[line]);

			if(flags & WDS_STRING_CENTER)
			{
				tx = x + WDS_TEXT_XOFFSET + (w - partwidth)/2;
			}
			else if(flags & WDS_STRING_RIGHT)
			{
				tx = x + WDS_TEXT_XOFFSET + w - partwidth;
			}
			else
			{
				tx = x + WDS_TEXT_XOFFSET;
				ltx = tx;
			}
			mx_adj = mx - tx;

			/* MOUSE POINTER LOCATING */
			if(my >= ty && my < ty+cly1)
			{
				if(line == 0) i = 0;
				else i = partindex[line-1];

				l = 0;
				c = strlen(part[line]) - 1;
				l = partwidth;
				if(flags & WDS_STRING_HALF_SHIFT)
				{
					l -= ModStringWidth(root,&(part[line][c]))/2;
				}
				if(mx_adj < l)
				{
					strcpy(buf,part[line]);
					while(c >= 0)
					{
						buf[c] = 0;
						l = ModStringWidth(root,buf);
						if(flags & WDS_STRING_HALF_SHIFT)
						{
							l -= ModStringWidth(root,&buf[c-1]) / 2;
						}
						if(mx_adj >= l)
						{
							hit = TRUE;
							break;
						}
						c--;
					}
				}
				else
				{
					hit = TRUE;
					c++;
				}
				if(!hit)
				{
					hit = -TRUE;
				}
			}

			/* draw cursor */
			if(	(	(line==cursorline)									||
					(line==rangemarkline && GetRangeMarkPosition()>=0)	||
					(in_range)	)											&&
				(GetCursorPosition()>=0))
			{
				long curx = tx+cursorx;
				long ramx = tx+rangemarkx;
				if( cursorline!=initial_cline && (flags&WDS_STRING_CURSOR_OP) )
				{
					if(cursorline==line)
					{
						if(flags & WDS_STRING_RIGHT)
							{
							if(flags & WDS_STRING_CURSOR_UP)
								{
								ltx = x + WDS_TEXT_XOFFSET + w - ModStringWidth(root,part[line + 1]);
								}
							else if(flags & WDS_STRING_CURSOR_DOWN)
								{
								ltx = x + WDS_TEXT_XOFFSET + w - ModStringWidth(root,part[line - 1]);
								}
							else
								ltx = x + WDS_TEXT_XOFFSET + w - ModStringWidth(root,part[line]);
							}
						else if(flags & WDS_STRING_CENTER)
							{
							if(flags & WDS_STRING_CURSOR_UP)
								{
								ltx = x + WDS_TEXT_XOFFSET + (w - ModStringWidth(root,part[line + 1]))/2;
								}
							else if(flags & WDS_STRING_CURSOR_DOWN)
								{
								ltx = x + WDS_TEXT_XOFFSET + (w - ModStringWidth(root,part[line - 1]))/2;
								}
							else
								ltx = x + WDS_TEXT_XOFFSET + (w - ModStringWidth(root,part[line]))/2;
							}
						else
							{
							ltx = tx;
							}

						if(line == 0)
							i=0;
						else
							i=partindex[line-1];

						long cu;
						cu=strlen(part[line]) - 1;
						strncpy(buf,part[line],cu+1);
						while(cu >= 0)
							{
							buf[cu]=0;

							if((tx + ModStringWidth(root,buf)) < (ltx + cursorx+cursorwidth/2 ))
								{
								cursor=i+cu;
								break;
								}

							cu--;
							}

						if(cu<0)
							cursor=i;
					}
				}
				else if(flags & WDS_STRING_DRAW)
				{
					if(GetRangeMarkPosition()<0 || GetCursorPosition()==GetRangeMarkPosition()) /* JUST THE CURSOR */
					{
						root->RectFill(curx-1,ty,curx,ty+cly-1,cursorbg);
					}
					else if(cursorline==rangemarkline) /* SINGLE LINE SELECTION */
					{
						if(cursorx < rangemarkx)
//							root->RectFill(curx-1,ty,ramx+rangemarkwidth-1,ty+cly-1,cursorbg);
							root->RectFill(curx-1,ty,ramx-1,ty+cly-1,cursorbg);
						else
//							root->RectFill(ramx-1,ty,curx+cursorwidth-1,ty+cly-1,cursorbg);
							root->RectFill(ramx-1,ty,curx-1,ty+cly-1,cursorbg);
					}
					else if(cursorline==line)
					{
						if(in_range)
						{
//							root->RectFill(tx,ty,curx+cursorwidth-1,ty+cly-1,cursorbg);
							root->RectFill(tx,ty,curx-1,ty+cly-1,cursorbg);
							in_range=FALSE;
						}
						else
						{
							root->RectFill(curx-1,ty,tx+ModStringWidth(root,part[line]),ty+cly-1,cursorbg);
							in_range=TRUE;
						}
					}
					else if(rangemarkline==line)
					{
						if(in_range)
						{
//							root->RectFill(tx,ty,ramx+rangemarkwidth-1,ty+cly-1,cursorbg);
							root->RectFill(tx,ty,ramx-1,ty+cly-1,cursorbg);
							in_range=FALSE;
						}
						else
						{
//							fprintf(stderr,"RO\n");

							root->RectFill(ramx-1,ty,tx+ModStringWidth(root,part[line]),ty+cly-1,cursorbg);
							in_range=TRUE;
						}
					}
					else /* FULLY MARKED LINE */
					{
//						fprintf(stderr,"FL\n");

						root->RectFill(tx,ty,tx+ModStringWidth(root,part[line]),ty+cly-1,cursorbg);
					}
				}
			}

			if(flags & WDS_STRING_DRAW)
				{
				long start=EW_MIN(cursor,rangemark);
				long end=  EW_MAX(cursor,rangemark);
				long offset= line? partindex[line-1]: 0;
				long partlen=strlen(part[line]);
				long subpos=0;
				long sublen;
				long dx=0;

				if(rangemark<0)
					{
					start=cursor;
					end= -1;
					}

#if WDS_STRING_LINE
				root->Line(tx,ty+root->GetFontHeight(),tx+ModStringWidth(root,part[line]),ty,EW_GREEN);
#else

				// highlight: before, on, after
				long m;
				for(m=0;m<3;m++)
					{
//	printf("line (%s)\n",part[line]);

					if(m==0)
						sublen=EW_MIN(partlen,start-offset);
					else if(m==1)
//						sublen=EW_MIN(partlen,end+1-offset-subpos);
						sublen=EW_MIN(partlen,end-offset-subpos);
					else
						sublen=partlen;

#if WDS_STRING_DEBUG
					printf("m=%d tx,ty=%d,%d firstline,lastline=%d,%d subpos=%d sublen=%d\n",
							m,tx,ty,firstline,lastline,subpos,sublen);
#endif
						
					if(sublen>0)
						{
						strncpy(piece,&part[line][subpos],sublen);
						piece[sublen]=0;
//	printf("%d piece (%s)\n",m,piece);

#if WDS_STRING_DEBUG
					printf("part[%d]=\"%s\" piece=\"%s\"\n",line,part[line],piece);
#endif

						long bold;
						for(bold=0;bold<((GetStyle()&WDS_STRING_STYLE_BOLD)? 2: 1);bold++)
							{
							if(m==1)
								root->Text(tx+dx+bold,ty,piece,-1,(GetStyle() & WDS_STRING_STYLE_3D),cursorfg);
							else
								root->Text(tx+dx+bold,ty,piece,-1,(GetStyle() & WDS_STRING_STYLE_3D));
							}
						subpos+=sublen;
						partlen-=sublen;
						dx+=ModStringWidth(root,piece);
						}
					}
#endif

#if WDS_STRING_BOX
				root->RectWire(x,y,x+w-1,y+h-1,EW_RED);
				root->Line(x+1,y,x+1,y+cly,EW_OLIVE);
#endif
				}

			offy-=cly1;
		}

		root->SetUnderline(underline);
	}

	//omega = showcount;

	if(hit==TRUE)
		return i+c;
	else if(hit==-TRUE)
	{
		return WDS_STRING_MISS_TO_LEFT;
	}
	else
	{
		return WDS_STRING_MISS;
	}
}



/******************************************************************************
long	WDS_String::ModCharWidth(EW_Window *root,char data)

******************************************************************************/
long WDS_String::ModCharWidth(EW_Window *root,char data)
	{
	if(GetDisplayChar())
		return root->CharWidth(GetDisplayChar());
	else
		return root->CharWidth(data);
	}



/******************************************************************************
long	WDS_String::ModStringWidth(EW_Window *root,char *data)

******************************************************************************/
long WDS_String::ModStringWidth(EW_Window *root,char *data)
	{
	if(GetDisplayChar())
		return strlen(data)*root->CharWidth(GetDisplayChar());
	else
		return root->StringWidth(data);
	}
