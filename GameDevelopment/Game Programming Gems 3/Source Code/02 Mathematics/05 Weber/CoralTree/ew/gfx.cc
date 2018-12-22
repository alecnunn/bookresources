/******************************************************************************

Coral Tree ew/gfx.cc
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


#include"ew.h"




#define	EW_GFX_DEBUG				FALSE		// TRUE = print verbosity
#define	EW_PALETTE_DEBUG			FALSE		// TRUE = print verbosity
#define EW_COLOR_ALLOC_DEBUG		FALSE

#define EW_XCOLORS_BYNAME			FALSE		// specify X colors by name (vs exact color specification)
#define EW_XCOLORS_BY_CELL			TRUE		// allocate some X colors read-only (from close matches of previous)

#define	EW_COLOR_ACCEPTABLE_ERROR	(0.05)		// percent off-color allowed for preferred color re-use
#define	EW_COLOR_WORST_ERROR		(0.25)		// percent off-color allowed for last-ditch color re-use
#define	EW_COLOR_PERFECT_ERROR		(0.01)		// well, virtually perfect

#define	EW_COLOR_VOID				EW_BLACK	// color of data copied from off-window



/* at start, default font has not been loaded */
short			EW_Window::graphics_initialized=FALSE;
EW_Font			EW_Window::default_font;
unsigned long	EW_Window::index_remap[2][EW_MAXCOLORS];
long			EW_Window::colors_allocated=0;

#if EW_WL==EW_WIN32
HPALETTE		EW_Window::hpal;
#endif


static float EwFloatColor[EW_MAXCOLORS][3];



/******************************************************************************
void	EW_Window::InitGraphics(void)

	initialize Graphics Abstraction

******************************************************************************/
void EW_Window::InitGraphics(void)
	{
	char message[100];

	if(!graphics_initialized || EW_WL==EW_XWIN)
		{
		// default unless otherwise set
		colors_allocated=EW_MAXCOLORS;

		long m,n;
		for(m=0;m<EW_MAXCOLORS;m++)
			for(n=0;n<3;n++)
				EwFloatColor[m][n]=EwXColor[m][n]/(float)0xFFFF;

		#if EW_WL==EW_XWIN

			/* X Windows */

			if( (EW_GL==EW_OPENGL) || !double_buffer)
				{
				while(!colormap_allocated)
					{
					long entries=GetNumberColors();

//					Colormap colormap=DefaultColormap(display,screen);

#if EW_XCOLORS_BYNAME

					XColor screen_def,exact_def;

					for(m=0;m<entries;m++)
						if( !XAllocNamedColor(display,colormap,EwXColorName[m],&screen_def,&exact_def) )
							{
							sprintf(message,"EW_Window::InitGraphics(): can't alloc X color \"%s\" indirect index %d",
																										EwXColorName[m],m);
							EW_PRINT(EW_GFX,EW_ERROR,message);
							}
						else
							index_remap[FALSE][m]=exact_def.pixel;

					colormap_allocated=TRUE;
					colors_allocated=entries;

#elif EW_XCOLORS_BY_CELL

					// get colormap values
					long depth=visual_information.depth;
					long cells=(long)(pow(2.0,(double)depth));

#if TRUE
					cells=EW_MIN(1024,cells);
#endif

					XColor *xcolors=(XColor *)osdMalloc(sizeof(XColor)*cells);
					long *xcolorvalid=(long *)osdMalloc(sizeof(long)*cells);

					for(m=0;m<cells;m++)
						{
						xcolorvalid[m]=TRUE;
						xcolors[m].pixel=m;
						xcolors[m].flags=DoRed|DoGreen|DoBlue;
						}

					XQueryColors(display,colormap,xcolors,cells);

#if EW_COLOR_ALLOC_DEBUG
					for(m=0;m<cells;m++)
						printf("cell %3d  0x%x 0x%x 0x%x  0x%x\n",m,
									xcolors[m].red,
									xcolors[m].green,
									xcolors[m].blue,
									xcolors[m].flags);
#endif

					XColor screen_in_out;
					unsigned long single_pixel[1];
					long mapping[EW_MAXCOLORS];

					long matched[4];	// perfect, good, poor, modula'd
					for(m=0;m<4;m++)
						matched[m]=0;

					for(m=0;m<EW_MAXCOLORS;m++)
						mapping[m]= -1;

					// pass 0: find close matches
					// pass 1: try to allocate cells for the rest
					// pass 2: use poor matches for remainder
					long pass;
					for(pass=0;pass<3;pass++)
						{
						for(m=0;m<entries;m++)
							if(mapping[m]<0)
								{
								screen_in_out.red=EwXColor[m][0];
								screen_in_out.green=EwXColor[m][1];
								screen_in_out.blue=EwXColor[m][2];

								if(pass!=1)
									{
									long done=FALSE;
									long cell=0;
									while(!done)
										{
										float dr,dg,db;
										float error;
										float besterror=1.0;
										long bestcell= -1;

										// find nearest valid color
										while(cell<cells)
											{
											if(xcolorvalid[cell])
												{
												// determine distance
												dr=EwXColor[m][0]-xcolors[cell].red;
												dg=EwXColor[m][1]-xcolors[cell].green;
												db=EwXColor[m][2]-xcolors[cell].blue;
												error=sqrt(dr*dr+dg*dg+db*db)/(float)0xFFFF;

												if(error<besterror)
													{
#if EW_COLOR_ALLOC_DEBUG
//													printf("index %d best %d error %.6G diff <%x,%x,%x>\n",m,bestcell,besterror,dr,dg,db);
#endif
													besterror=error;
													bestcell=cell;

													if(error<0.01)
														break;
													}
												}
											cell++;
											}

										if(bestcell== -1)
											{
#if EW_COLOR_ALLOC_DEBUG
											printf("index %d cannot be matched\n",m);
#endif
											break;
											}
										if( (pass==0 && besterror>EW_COLOR_ACCEPTABLE_ERROR) ||
											(pass==2 && besterror>EW_COLOR_WORST_ERROR) )
											{
#if EW_COLOR_ALLOC_DEBUG
											printf("index %d (0x%x,0x%x,0x%x) best %d error %.6G > %.6G not acceptable\n",m,
													EwXColor[m][0],
													EwXColor[m][1],
													EwXColor[m][2],
													bestcell,besterror,pass? EW_COLOR_WORST_ERROR: EW_COLOR_ACCEPTABLE_ERROR);
#endif

											// if doesn't even reach worst error, use an index we've already got
											if(pass==2)
												{
#if EW_COLOR_ALLOC_DEBUG
												printf("remapping index %d\n",m);
#endif
												if(mapping[m%8]>=0)
													mapping[m]=mapping[m%8];
												else if(mapping[m%4]>=0)
													mapping[m]=mapping[m%4];

												matched[3]++;
												}

											continue;
											}

#if EW_COLOR_ALLOC_DEBUG
										printf("index %d (0x%x,0x%x,0x%x) attempting to use %d (0x%x,0x%x,0x%x) error %.6G\n",
													m,
													EwXColor[m][0],
													EwXColor[m][1],
													EwXColor[m][2],
													bestcell,
													xcolors[bestcell].red,
													xcolors[bestcell].green,
													xcolors[bestcell].blue,
													besterror);
#endif
										screen_in_out.red=xcolors[bestcell].red;
										screen_in_out.green=xcolors[bestcell].green;
										screen_in_out.blue=xcolors[bestcell].blue;

										// attempt to use
										if( !XAllocColor(display,colormap,&screen_in_out) )
											{
#if EW_COLOR_ALLOC_DEBUG
											printf("failed: not read only\n");
#endif
											xcolorvalid[bestcell]=FALSE;
											}
										else
											{
											done=TRUE;

											if(((long)(screen_in_out.pixel))!=bestcell)
												{
#if EW_COLOR_ALLOC_DEBUG
												printf("failed: not same index (%d)\n",screen_in_out.pixel);
#endif
												xcolorvalid[bestcell]=FALSE;

												// free
												single_pixel[0]=screen_in_out.pixel;
												XFreeColors(display,colormap,single_pixel,1,0);
												done=FALSE;
												}

											// if cell was changed for this color, don't use
											if( screen_in_out.red!=xcolors[screen_in_out.pixel].red ||
												screen_in_out.green!=xcolors[screen_in_out.pixel].green ||
												screen_in_out.blue!=xcolors[screen_in_out.pixel].blue)
												{
#if EW_COLOR_ALLOC_DEBUG
												printf("failed: allocation occurred to cell %d\n",screen_in_out.pixel);
#endif
												xcolorvalid[screen_in_out.pixel]=FALSE;

												if(done)
													{
													// free
													single_pixel[0]=screen_in_out.pixel;
													XFreeColors(display,colormap,single_pixel,1,0);
													done=FALSE;
													}
												}
											}

										if(done)
											{
											mapping[m]=bestcell;

											if(besterror<0.01)
												matched[0]++;
											else if(pass==0)
												matched[1]++;
											else
												matched[2]++;
											}
										}
									}
								else
									{
									// read-only cell
									if( !XAllocColor(display,colormap,&screen_in_out) )
										{
#if TRUE
										sprintf(message,
												"EW_Window::InitGraphics(): colormap full, will approximate");
										EW_PRINT(EW_GFX,EW_WARN,message);
#else
										sprintf(message,
												"EW_Window::InitGraphics(): can't alloc X color %x,%x,%x indirect index %d",
																				EwXColor[m][0],EwXColor[m][1],EwXColor[m][2],m);
										EW_PRINT(EW_GFX,EW_ERROR,message);
#endif

										// go on to next pass
										break;
										}
									else
										{
										mapping[m]=screen_in_out.pixel;

										matched[0]++;

										long dr=EwXColor[m][0]-screen_in_out.red;
										long dg=EwXColor[m][1]-screen_in_out.green;
										long db=EwXColor[m][2]-screen_in_out.blue;
										float error=sqrt(dr*dr+dg*dg+db*db)/(float)0xFFFF;


#if EW_COLOR_ALLOC_DEBUG
										printf("index %d to pixel %d (0x%x,0x%x,0x%x)->(0x%x,0x%x,0x%x) %.6G\n",m,mapping[m],
												EwXColor[m][0], EwXColor[m][1], EwXColor[m][2],
												screen_in_out.red, screen_in_out.green, screen_in_out.blue,
												error);
#endif
										if(pass==2 && error>EW_COLOR_ACCEPTABLE_ERROR)
											{
#if EW_COLOR_ALLOC_DEBUG
											printf("index %d percent-error %.6G rejected (limited to %.6G)\n",
																						m,error,EW_COLOR_ACCEPTABLE_ERROR);
#endif
											single_pixel[0]=screen_in_out.pixel;
											XFreeColors(display,colormap,single_pixel,1,0);
											}
										}
									}
								}
						}

					for(m=0;m<EW_MAXCOLORS;m++)
						if(mapping[m]<0)
							{
							index_remap[FALSE][m]=BlackPixel(display,screen);
							matched[3]++;
							}
						else
							index_remap[FALSE][m]=mapping[m];

					osdFree(xcolors);
					osdFree(xcolorvalid);

					colormap_allocated=TRUE;
					colors_allocated=entries;

					// remark if any non-perfect mappings
					if(matched[1]||matched[2]||matched[3])
						{
						EW_PRINT(EW_GFX,EW_LOG,
										"EW_Window::InitGraphics() color-mapping: %d perfect, %d good, %d poor, %d modula'd",
										matched[0],matched[1],matched[2],matched[3]);
						}

#else

					XColor colordef;
					colordef.flags=DoRed|DoGreen|DoBlue;

					long status=XAllocColorCells(display,colormap,False,NULL,0,index_remap[FALSE],entries);
					if(status==0)
						{
						long previous=GetNumberColors();

						SetNumberColors(previous-1);

						EW_PRINT(EW_WINDOW,EW_WARN,
											"EW_Window::InitGraphics() can't allocate color cells -> degrading color resolution");

						if(previous==GetNumberColors())
							EW_PRINT(EW_WINDOW,EW_FATAL,"EW_Window::InitGraphics() reached minimum color cells");
						}
					else
						{
						colormap_allocated=TRUE;
						colors_allocated=entries;

						for(m=0;m<entries;m++)
							{
							colordef.red=EwXColor[m][0];
							colordef.green=EwXColor[m][1];
							colordef.blue=EwXColor[m][2];
							colordef.pixel=index_remap[FALSE][m];

							XStoreColor(display,colormap,&colordef);
							}
						}
#endif

#if EW_COLOR_ALLOC_DEBUG
					printf("color entries=%d\n",entries);
					for(m=0;m<EW_MAXCOLORS;m++)
						printf("index[0][%d]=%d\n",m,index_remap[0][m]);
#endif
					}

				XGCValues xgcv;
				unsigned long mask=GCFunction|GCPlaneMask;

				xgcv.function=GXcopy;
				xgcv.plane_mask= ~(0L);

//	printf("Create (%s) graphics_context=0x%x DefaultGC()=0x%x\n",GetTitle(),graphics_context,DefaultGC(display,screen));

			    if(graphics_context && graphics_context!=DefaultGC(display,screen))
					{
//	printf("Free Old (%s) graphics_context=0x%x\n",GetTitle(),graphics_context);

        			XFreeGC(display,graphics_context);
					graphics_context=NULL;
					}

				graphics_context=XCreateGC(display,win,mask,&xgcv);
				if(graphics_context==NULL)
					EW_PRINT(EW_WINDOW,EW_ERROR,"EW_Window::InitGraphics() XCreateGC() failed");

//	printf("Result (%s) graphics_context=0x%x DefaultGC()=0x%x\n",GetTitle(),graphics_context,DefaultGC(display,screen));
				}

		#elif EW_WL==EW_WIN32

			const long palette_size=256;

#if EW_PALETTE_DEBUG
			PALETTEENTRY *entry,entries[1024];
			long number=GetSystemPaletteEntries(hdc,0,0,NULL);
number=32;
			long read=GetSystemPaletteEntries(hdc,0,number,entries);

			sprintf(message,"system palette %d/%d",read,number);
			EW_PRINT(EW_GFX,EW_LOG,message);

			for(m=0;m<number;m++)
				{
				sprintf(message,"syscolor %d 0x%x",m,entries[m]);
				EW_PRINT(EW_GFX,EW_LOG,message);
				}
#endif

			long numcolors=GetDeviceCaps(hdc,NUMCOLORS);
			long bitspixel=GetDeviceCaps(hdc,BITSPIXEL);
			long planes=GetDeviceCaps(hdc,PLANES);
			long sizepalette=GetDeviceCaps(hdc,SIZEPALETTE);
			long colorres=GetDeviceCaps(hdc,COLORRES);
			long clipcaps=GetDeviceCaps(hdc,CLIPCAPS);
			long rcpalette=GetDeviceCaps(hdc,RASTERCAPS)&RC_PALETTE;

			// for wintable of 1, use Win32 standard garbage colors
			// wintable=(rcpalette!=0);

#if EW_PALETTE_DEBUG
			sprintf(message,
					"NUMCOLORS=%d BITSPIXEL=%d PLANES=%d SIZEPALETTE=%d COLORRES=%d CLIPCAPS=%d RASTERCAPS&RC_PALETTE=%d",
					numcolors,bitspixel,planes,sizepalette,colorres,clipcaps,rcpalette);
			EW_PRINT(EW_GFX,EW_LOG,message);
#endif

//			SetSystemPaletteUse(hdc,SYSPAL_NOSTATIC);

//			logpalette = (NPLOGPALETTE)LocalAlloc( LMEM_FIXED, sizeof(LOGPALETTE) + EW_MAXCOLORS*sizeof(PALETTEENTRY) );
			logpalette = (NPLOGPALETTE)LocalAlloc( LMEM_FIXED, sizeof(LOGPALETTE) + palette_size*sizeof(PALETTEENTRY) );

			if(!logpalette)
				EW_PRINT(EW_GFX,EW_ERROR,"could not allocate memory for palette");

			logpalette->palVersion = 0x300;
//			logpalette->palNumEntries = palette_size;
//			logpalette->palNumEntries = numcolors;
			logpalette->palNumEntries = EW_MAXCOLORS;

			for(m=0;m<EW_MAXCOLORS;m++)
//			for(m=0;m<numcolors;m++)
				{
//				*((DWORD *)(&logpalette->palPalEntry[m])) = (DWORD)32+(PC_NOCOLLAPSE<<24);
//				*((DWORD *)(&logpalette->palPalEntry[m])) = (DWORD)EwWin32Colortable[wintable][m]+(PC_NOCOLLAPSE<<24);
				*((DWORD *)(&logpalette->palPalEntry[m])) = (DWORD)EwWin32Colortable[m];

#if EW_PALETTE_DEBUG
				sprintf(message,"set index %d to 0x%x",m,(DWORD)EwWin32Colortable[m]);
				EW_PRINT(EW_GFX,EW_LOG,message);
#endif
				}

			hpal = CreatePalette((LPLOGPALETTE)logpalette);

			if( !ResizePalette(hpal,32) )
				EW_PRINT(EW_GFX,EW_WARN,"couldn't expand palette");

			if( !SelectPalette(hdc,hpal,FALSE) )
				EW_PRINT(EW_GFX,EW_ERROR,"couldn't select palette");

			long palette_mapped=RealizePalette(hdc);

			if(palette_mapped==GDI_ERROR)
				EW_PRINT(EW_GFX,EW_ERROR,"couldn't realize palette");

#if EW_PALETTE_DEBUG
			read=GetSystemPaletteEntries(hdc,0,number,entries);

			sprintf(message,"%d logical colors mapped to system palette %d/%d",palette_mapped,read,number);
			EW_PRINT(EW_GFX,EW_LOG,message);

			for(m=0;m<number;m++)
				{
				sprintf(message,"syscolor %d 0x%x",m,entries[m]);
				EW_PRINT(EW_GFX,EW_LOG,message);
				}
#endif

#if TRUE
			// reset the table to closest 
			for(m=0;m<EW_MAXCOLORS;m++)
				{
#if EW_PALETTE_DEBUG
				sprintf(message,"color %d from 0x%x",m,EwWin32Colortable[m]);
				EW_PRINT(EW_GFX,EW_LOG,message);
#endif

				EwWin32Colortable[m]=GetNearestColor(hdc,EwWin32Colortable[m]);

#if EW_PALETTE_DEBUG
				sprintf(message," to 0x%x",EwWin32Colortable[m]);
				EW_PRINT(EW_GFX,EW_LOG,message);
#endif
				}
#endif

#if FALSE
			long value;
			for(m=0;m<EW_MAXCOLORS;m++)
				{
				index_remap[FALSE][m]=GetNearestPaletteIndex(hpal,EwWin32Colortable[m]);
				}

			for(m=0;m<EW_MAXCOLORS;m++)
				{
				entry= &entries[index_remap[FALSE][m]];
				value= ((((entry->peBlue)<<8) + entry->peGreen)<<8) + entry->peRed;
				EwWin32Colortable[m]=(COLORREF)value;

				sprintf(message,"color %d uses index %d 0x%x",m,index_remap[FALSE][m],value);
				EW_PRINT(EW_GFX,EW_LOG,message);
				}
#else
			for(m=0;m<EW_MAXCOLORS;m++)
				index_remap[FALSE][m]=m;
#endif

		#elif EW_GL==EW_IRISGL

			/* IrisGL on Mex */

			#if FONTMANAGER
				fminit();

/* 				OpenFont(font,"Helvetica-Narrow",9.0); */
/* 				OpenFont(font,"AvantGarde-Book",9.0); */

			#endif

		#else
			#error EW_WL and EW_GL not properly defined
		#endif

		graphics_initialized=TRUE;
		}

#if EW_WL==EW_WIN32

	if( !SelectPalette(hdc,hpal,FALSE) )
		EW_PRINT(EW_GFX,EW_ERROR,"couldn't select palette");

	long palette_mapped=RealizePalette(hdc);

#endif

/* made index_remap static in EW_Window */
/*
	else
		{
		for(m=0;m<EW_MAXCOLORS;m++)
			index_remap[FALSE][m]=exact_def.pixel;
		}
*/

	// set current font to default (if not already set)
	if( !current_font.GetValid() && default_font.GetValid())
		current_font.CopyFrom(&default_font);
	}



/******************************************************************************
void	EW_Window::InitDefaultFont(void)

******************************************************************************/
void EW_Window::InitDefaultFont(void)
	{
	if(default_font.GetValid())
		{
		EW_PRINT(EW_GFX,EW_LOG,"EW_Window::InitDefaultFont(): default font is already valid");
		return;
		}

	WinSet(FALSE);

	EW_FontRequest request;

	request.SetHeight(12);
	request.SetStyle(EW_FONT_PROPORTIONAL);

	if( LoadFont(&default_font,&request) )
		{
		EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::InitDefaultFont(): could not load a font height %.6G style 0x%x",
																				request.GetHeight(),request.GetStyle());
		}
	}



/******************************************************************************
void	EW_Window::SetColor(long type,long index)

	set current color for type:
		EW_COLOR_PEN, EW_COLOR_WIDGET, EW_COLOR_BACKGROUND,
		EW_COLOR_LIT, or EW_COLOR_SHADOW

	colors passed as longs since some systems may not support
	float indexes (auto-dithered)

******************************************************************************/
void EW_Window::SetColor(long type,long index)
	{
	switch(type)
		{
		case EW_COLOR_UNUSED:
			break;

		case EW_COLOR_PEN:
			index_pen=index;
			break;

		case EW_COLOR_WIDGET:
			index_widget=index;
			break;

		case EW_COLOR_BACKGROUND:
			index_back=index;
			break;

		case EW_COLOR_LIT:
			index_lit=index;
			break;

		case EW_COLOR_HALF_LIT:
			index_halflit=index;
			break;

		case EW_COLOR_SHADOW:
			index_shadow=index;
			break;

		case EW_COLOR_HALF_SHADOW:
			index_halfshadow=index;
			break;

		default:
			{
			char message[256];
			sprintf(message,"EW_Window::SetColor(%d,%d): bad index type",(int)type,(int)index);
			EW_PRINT(EW_GFX,EW_WARN,message);

			break;
			}
		}
	}



/******************************************************************************
long	EW_Window::GetColor(long type)

	get current color for type:
		EW_FG_COLOR, EW_BG_COLOR, EW_COLOR_LIT, or EW_COLOR_SHADOW

	Note that undex X, this is not the same as the true pixel color index.

******************************************************************************/
long EW_Window::GetColor(long type)
	{
	switch(type)
		{
		case EW_COLOR_UNUSED:
			return 0;

		case EW_COLOR_PEN:
			return (long)index_pen;

		case EW_COLOR_WIDGET:
			return (long)index_widget;

		case EW_COLOR_BACKGROUND:
			return (long)index_back;

		case EW_COLOR_LIT:
			return (long)index_lit;

		case EW_COLOR_HALF_LIT:
			return (long)index_halflit;

		case EW_COLOR_SHADOW:
			return (long)index_shadow;

		case EW_COLOR_HALF_SHADOW:
			return (long)index_halfshadow;

		default:
			{
			char message[256];
			sprintf(message,"EW_Window::GetColor(%d): bad index type",(int)type);
			EW_PRINT(EW_GFX,EW_WARN,message);

			return 0L;
			}
		}
	}



/******************************************************************************
void	EW_Window::ActivateColor(EW_COLORTYPE index)

******************************************************************************/
void EW_Window::ActivateColor(EW_COLORTYPE index)
	{
	// limit to number of available colors
	index%=GetNumberColors();
	index%=colors_allocated;	// ERROR div by zero

	#if EW_GL==EW_X_GFX

		if(double_buffer && xdbuf_currentbufferindex==0)
			XSetForeground(display,GetGC(),xdbuf_pixellist[index]);
		else
			XSetForeground(display,GetGC(),index_remap[double_buffer][index]);

	#elif EW_GL==EW_OPENGL

		if(GetRGBMode())
			glColor3fv(EwFloatColor[index]);
		else
			glIndexi(index_remap[FALSE][index]);

	#elif EW_GL==EW_IRISGL

		color((Colorindex)index);

	#elif EW_GL==EW_GDI

		if(!SelectObject(hdc,hpen[index]))
			EW_PRINT(EW_GFX,EW_ERROR,"could not activate color (pen)");
		if(!SelectObject(hdc,hbrush[index]))
			EW_PRINT(EW_GFX,EW_ERROR,"could not activate color (brush)");

	#else
		#error EW_GL not properly defined
	#endif
	}



/******************************************************************************
void	EW_Window::Clear(void)

	clear (fill) a window with a specific color

	only function that doesn't use ActivateColor() since it may use inherent
		background colors

******************************************************************************/
void EW_Window::Clear(void)
	{
	#if EW_GL==EW_X_GFX

		/* Pure X Windows */

#if TRUE
		long sx,sy;

		GetSize(&sx,&sy);
		RectFill(0,0,sx-1,sy-1,index_back);
#else
		long set;
		if(double_buffer && xdbuf_currentbuffer==&xdbuf_colorbuffer[0])
//			set=xdbuf_pixellist[index_back];
			set=xdbuf_pixellist[EW_RED];
		else
//			set=index_remap[FALSE][index_back];
			set=index_remap[FALSE][EW_BLUE];

		XSetBackground(display,GetGC(),set);
		XSetWindowBackground(display,win,set);

		XClearWindow(display,win);
#endif

	#elif EW_GL==EW_OPENGL

		/* OpenGL */
		if(FALSE&& GetRGBMode())
			{
			// NOTE: glClear() ignores stencils but not scissors

			glClearDepth(1.0f);
			glClearColor(	EwFloatColor[index_back][0],
							EwFloatColor[index_back][1],
							EwFloatColor[index_back][2],
							1.0f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			}
		else
			{
			long sx,sy;

			GetSize(&sx,&sy);
			RectFill(0,0,sx-1,sy-1,index_back);
			}
	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		RECT	a_rect;

		if(!GetClientRect(hwnd,&a_rect))
			EW_PRINT(EW_GFX,EW_ERROR,"could not get client rectangle");

		if(!FillRect(hdc,&a_rect,hbrush[index_back]))
			EW_PRINT(EW_GFX,EW_ERROR,"could not fill rectangle");


	#elif EW_GL==EW_IRISGL

		/* IrisGL */
		color(index_back);
		clear();

	#else
		#error EW_GL not properly defined
	#endif
	}



/******************************************************************************
void	EW_Window::Flush(long block)

	flush the graphics pipeline

	if block, waits until pipeline is cleared
	(system-dependent, as available)

******************************************************************************/
void EW_Window::Flush(long block)
	{
	#if EW_GL==EW_X_GFX

		/* Pure X Windows */
		if(block)
			XSync(display,FALSE);
		else
			XFlush(display);

	#elif EW_GL==EW_OPENGL

		/* OpenGL */
		if(block)
			glFinish();
		else
			glFlush();

	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		//TODO: figure out why GdiFlush occasionally returns FALSE
		GdiFlush();
		if(!GdiFlush())	
			EW_PRINT(EW_GFX,EW_ERROR,"could not flush");

	#elif EW_GL==EW_IRISGL

		/* IrisGL */
		if(block)
			finish();
		else
			gflush();

	#else
		#error EW_GL not properly defined
	#endif
	}


/******************************************************************************
void	EW_Window::BufferSwap(void)

	swap image buffers, use in double buffer mode only

******************************************************************************/
void EW_Window::BufferSwap(void)
	{
	if(!double_buffer)
		return;

/*
	static long flip=0;
	flip=!flip;
	EW_PRINT(EW_EVENT,EW_LOG,"%sswap buffers %d %s",ewColorCode(1),flip,ewColorCode(0));
*/
		
	#if EW_GL==EW_X_GFX

		/* Pure X Windows */
		Xdbuf_SwapBuffers();

	#elif EW_GL==EW_OPENGL

		/* OpenGL */
		#if EW_WL==EW_XWIN
			glXSwapBuffers(display,win);
		#elif EW_WL==EW_WIN32
			if(!SwapBuffers(hdc))
				EW_PRINT(EW_GFX,EW_ERROR,"could not swap buffers");
		#endif

	#elif EW_GL==EW_IRISGL

		/* IrisGL */
		swapbuffers();

	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		EW_PRINT(EW_APP,EW_WARN,"Windows GDI does not support double buffering");

	#else
		#error EW_GL not properly defined
	#endif
	}



/******************************************************************************
void	EW_Window::RectWire(long left,long bottom,long right,long top,
															long colorindex)

	draw a hollow rectangle including ALL edges (not just up to)

******************************************************************************/
void EW_Window::RectWire(long left,long bottom,long right,long top,long colorindex)
	{
	long sx,sy;

	GetSize(&sx,&sy);

	if(left>right || left>=sx || right<0 || bottom>top || bottom>=sy || top<0 )
		return;

	ActivateColor(colorindex);

	#if EW_GL==EW_X_GFX

		/* Pure X Windows */
		XDrawRectangle(display,win,GetGC(),left,EW_REMAP_Y(top,sy),right-left,top-bottom);

	#elif EW_GL==EW_OPENGL

		/* OpenGL */

		/* temporarily switch to wireframe polygon mode */
		glPolygonMode(GL_FRONT,GL_LINE);

		// unlike RectFill(), not right+1,top+1
		glRecti(left,bottom,right,top);
		glPolygonMode(GL_FRONT,GL_FILL);

	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		static RECT rect;
		SetRect(&rect,left,EW_REMAP_Y(top,sy),right+1,EW_REMAP_Y(bottom,sy)+1);
		if(!FrameRect(hdc,&rect,hbrush[colorindex]))
			EW_PRINT(EW_GFX,EW_ERROR,"could not draw rectangle");

	#elif EW_GL==EW_IRISGL

		/* IrisGL */
		recti(left,bottom,right,top);

	#else
		#error EW_GL not properly defined
	#endif
	}



/******************************************************************************
void	EW_Window::RectFill(long left,long bottom,long right,long top,
															long colorindex)

	draw a filled rectangle including ALL edges (not just up to)

******************************************************************************/
void EW_Window::RectFill(long left,long bottom,long right,long top,long colorindex)
	{
	long sx,sy;

	GetSize(&sx,&sy);

	if(left>right || left>=sx || right<0 || bottom>top || bottom>=sy || top<0 )
		return;

	ActivateColor(colorindex);

	#if EW_GL==EW_X_GFX

		/* Pure X Windows (why must Fill be ..+1 and Wire not?) */
		XFillRectangle(display,win,GetGC(),left,EW_REMAP_Y(top,sy),right-left+1,top-bottom+1);

	#elif EW_GL==EW_OPENGL

		/* OpenGL */
		glRecti(left,bottom,right+1,top+1);

	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		static RECT rect;
		SetRect(&rect,left,EW_REMAP_Y(top,sy),right+1,EW_REMAP_Y(bottom,sy)+1);

		if(!FillRect(hdc,&rect,hbrush[colorindex]))
			EW_PRINT(EW_GFX,EW_ERROR,"could not draw rectangle");

	#elif EW_GL==EW_IRISGL

		/* IrisGL */
		rectfi(left,bottom,right,top);

	#else
		#error EW_GL not properly defined
	#endif
	}



/******************************************************************************
void EW_Window::ArcWireFast(long cx,long cy,long rx,long ry,
										float start,float end,long colorindex)

	done for X only

	could add methods for other graphics

******************************************************************************/
void EW_Window::ArcWireFast(long cx,long cy,long rx,long ry,float start,float end,long colorindex)
	{
	#if EW_GL==EW_X_GFX

	ActivateColor(colorindex);

		long sx,sy;

		GetSize(&sx,&sy);

		// Pure X Windows
		if(start==end)

			{
			start=0.0;
			end=360.0;
			}

		XDrawArc(display,win,GetGC(),cx-rx,EW_REMAP_Y(cy+ry,sy),rx*2,ry*2,int(start*64),int(end*64));

	#else

		DrawArc(FALSE,cx,cy,rx,ry,start,end,colorindex);

	#endif
	}


/******************************************************************************
void	EW_Window::DrawArc(long fill,long cx,long cy,long rx,long ry,
                                        float start,float end,long colorindex)

Keywords:
******************************************************************************/
void EW_Window::DrawArc(long fill,long cx,long cy,long rx,long ry,float start,float end,long colorindex)
	{
	long sx,sy;
	GetSize(&sx,&sy);

	ActivateColor(colorindex);

	if(TRUE)
		{
		SymmetricArc(fill,cx,cy,rx,ry,start,end,colorindex);
		return;
		}

	// IGNORED

	if(end<=start)
		end=start+360.0;

	#if EW_GL==EW_X_GFX

		int a1=(int)(start*64);
		int a2=(int)(end*64-a1);

		if(fill)
			XFillArc(display,win,GetGC(),cx-rx,EW_REMAP_Y(cy+ry,sy),rx*2,ry*2,a1,a2);
		else
			XDrawArc(display,win,GetGC(),cx-rx,EW_REMAP_Y(cy+ry,sy),rx*2,ry*2,a1,a2);

	#elif EW_GL==EW_GDI

		int stx=cx+100*sin(start);
		int sty=cy+100*cos(start);
		int endx=cx+100*sin(end);
		int endy=cy+100*cos(end);

		if(fill)
			Pie(hdc,cx-rx,EW_REMAP_Y(cy+ry,sy),cx+rx,EW_REMAP_Y(cy-ry,sy),stx,sty,endx,endy);
		else
			Arc(hdc,cx-rx,EW_REMAP_Y(cy+ry,sy),cx+rx,EW_REMAP_Y(cy-ry,sy),stx,sty,endx,endy);

//	#elif EW_GL==EW_OPENGL
//	#elif EW_GL==EW_IRISGL
	#else

		PolygonalArc(fill,cx,cy,rx,ry,start,end,colorindex);

	#endif
	}



#define EW_GFX_SCALE_ANGLES	TRUE	// scale angles for non-circular arc (45 degrees may not be perfect diagonal)



/******************************************************************************
void	EW_Window::SymmetricArc(long fill,long cx,long cy,long rx,long ry,
                                        float start,float end,long colorindex)

	represented pixelization of circle guaranteed symmetric in both axis

	drawn completely with horizontal lines

	WARNING: unfilled mode not fully functional

Keywords:
******************************************************************************/
void EW_Window::SymmetricArc(long fill,long cx,long cy,long rx,long ry,float start,float end,long colorindex)
	{
	float scale=1.0;
#if EW_GFX_SCALE_ANGLES
	scale=rx/(float)ry;
#endif

	long m;

	long section;
	for(section=0;section<2;section++)
		{
		long split=FALSE;
		float start2=start;
		float end2=end;

		while(start2<0.0) start2+=360.0;
		while(start2>=360.0) start2-=360.0;
		while(end2<0.0) end2+=360.0;
		while(end2>360.0) end2-=360.0;

		if(section)
			{
			if(start2>end2)
				{
				start2=0.0;
				split=TRUE;
				}
			else
				break;
			}
		else
			{
			if(start2>end2)
				{
				end2=360.0;
				split=TRUE;
				}
			}

		long lstart=(long)start2%360;
		long lend=(long)end2%360;

		long full_ellipse=(start2==end2 || end2-start2>=360.0);

		if(end2==360.0)
			lend=360;

//			printf("%d %d to %d\n",section,lstart,lend);

		long start_on_top=(lstart<180);
		long end_on_top=(lend<180);

		long draw_top=(full_ellipse || start_on_top || end_on_top);
		long draw_bot=(full_ellipse || !start_on_top || !end_on_top);

		float start_tan=tan(start2*M_PI/180.0);
		float end_tan=tan(end2*M_PI/180.0);

		if(!start_on_top)
			start_tan= -start_tan;
		if(!end_on_top)
			end_tan= -end_tan;

		float fx,fy,startx,endx;

		long left,right;
		long lastleft[2],lastright[2];
		long started[2];

		started[0]=FALSE;
		started[1]=FALSE;

		for(m=ry-1;m>=0;m--)
			{
			fy=m+0.5;
			fx=rx*sqrt(1.0-fy*fy/(float)(ry*ry));

//			if(rx==6 && ry==6) printf("%d,%d %d %.2f,%.2f\n",rx,ry,m,fx,fy);

			fx+=0.5;

			if(start_tan==0.0)
				startx= (lstart==180)? -rx: rx;
			else if(start2==90.0 || start2==180.0)
				startx=0.0;
			else
				startx=scale*fy/start_tan;

			if(end_tan==0.0)
				endx= (lend==180)? -rx: rx;
			else if(end2==90.0 || end2==180.0)
				endx=0.0;
			else
				endx=scale*fy/end_tan;

			right=(long)((full_ellipse || !start_on_top)? fx: EW_MIN(fx,startx));
			left=(long)((full_ellipse || !end_on_top)? -fx: EW_MAX(-fx,endx));

			if(!m && !fill && !full_ellipse)
				{
				if(start_on_top && !split)
					right=0;
				if(end_on_top)
					left=0;
				}

/*
printf("top fx,fy=%.2f,%.2f start,end=%.2f,%.2f startx,endx=%.2f,%.2f l,r=%d,%d top=%d,%d\n",
										fx,fy,start,end,startx,endx,left,right,start_on_top,end_on_top);
*/

			if( draw_top && cx+left<=cx+right-1 )
				{
				if(fill || !started[0])
					Line(cx+left,cy+m,cx+right-1,cy+m,colorindex);
				else
					{
					Line(cx+left,cy+m,lastleft[0],cy+m,colorindex);
					Line(lastright[0],cy+m,cx+right-1,cy+m,colorindex);
					}

				started[0]=TRUE;
				}

			lastleft[0]=cx+left;
			lastright[0]=cx+right-1;

			left=(long)((full_ellipse || start_on_top)? -fx: EW_MAX(-fx,startx));
			right=(long)((full_ellipse || end_on_top)? fx: EW_MIN(fx,endx));

			if(!m && !fill && !full_ellipse)
				{
				if(!start_on_top)
					left=0;
				if(!end_on_top && !split)
					right=0;
				}

/*
printf("bot l,r=%d,%d draw=%d,%d\n",left,right,draw_top,draw_bot);
*/

			if( draw_bot && cx+left<=cx+right-1 )
				{
				if(fill || !started[1])
					Line(cx+left,	cy-m-1,	cx+right-1,	cy-m-1,	colorindex);
				else
					{
					Line(cx+left,cy-m-1,lastleft[1],cy-m-1,colorindex);
					Line(lastright[1],cy-m-1,cx+right-1,cy-m-1,colorindex);
					}

				started[1]=TRUE;
				}

			lastleft[1]=cx+left;
			lastright[1]=cx+right-1;
			}
		}
	}



/******************************************************************************
void	EW_Window::PolygonalArc(long fill,long cx,long cy,long rx,long ry,
                                        float start,float end,long colorindex)

	NOTE: not neccessarily the same effect as DrawArc()
		for example, X results in slightly different coverage with arcs

Keywords:
******************************************************************************/
void EW_Window::PolygonalArc(long fill,long cx,long cy,long rx,long ry,float start,float end,long colorindex)
	{
	long (*vert)[2];
	long m,n;
	long langle;

	float fx,fy;
	float angle,t;

	long radial=0;
	long vertices=EW_MAX(rx,ry);

//	long full_ellipse=(end<=start || end-start>=360.0);
	long full_ellipse=(end==start);

	if(full_ellipse)
		{
		// round up to factor of 4

//		radial=(vertices+3)/4 * (end-start)/360.0;
		radial=EW_MAX(rx,ry);

		if(radial<1)
			radial=1;
		vertices=radial*4;
		}

//		printf("fill=%d r=%d,%d vertices=%d\n",fill,rx,ry,vertices);

	vert=(long (*)[2])osdMalloc(vertices*sizeof(vert[0]));

	if(!full_ellipse)
		{
		// angular
		for(m=0;m<vertices-1;m++)
			{
			angle=start+(end-start)*m/(float)(vertices-2);
			langle=(long)angle%360;

			t=tan(angle*M_PI/180.0);

#if EW_GFX_SCALE_ANGLES
			fx=sqrt( 1.0 / ( 1.0/(float)(rx*rx) + t*t/(float)(rx*rx) ) );
#else
			fx=sqrt( 1.0 / ( 1.0/(float)(rx*rx) + t*t/(float)(ry*ry) ) );
#endif

			if( langle >90  && langle<270 )
				fx= -fx;

			fy= ry*sqrt(1.0-fx*fx/(float)(rx*rx));

			if( langle >= 180 )
				fy= -fy;

//			printf("%d/%d %.2f %.2f,%.2f\n",m,vertices,angle,fx,fy);

			vert[m][0]=(long)(cx+fx);
			vert[m][1]=(long)(cy+fy);
			}
		
		if(!fill || end<=start || end-start>=360.0)
			{
			vert[vertices-1][0]=vert[vertices-2][0];
			vert[vertices-1][1]=vert[vertices-2][1];
			}
		else
			{
			vert[vertices-1][0]=cx;
			vert[vertices-1][1]=cy;
			}
		}
	else
		{
		// four corners

		// some redundancy
		for(m=0;m<radial+1;m++)
			{
			fx= rx * sqrt(m/(float)radial);
			fy= sqrt(ry*ry*(1.0-fx*fx/(float)(rx*rx)));

			// for round-up
			fx+=0.5;
			fy+=0.5;

			vert[m][0]=(long)(cx+fx);
			vert[m][1]=(long)(cy-fy);

			n=2*radial-m;
			vert[n][0]=(long)(cx+fx);
			vert[n][1]=(long)(cy+fy);

			n=2*radial+m;
			vert[n][0]=(long)(cx-fx);
			vert[n][1]=(long)(cy+fy);

			n=(vertices-m)%vertices;
			vert[n][0]=(long)(cx-fx);
			vert[n][1]=(long)(cy-fy);
			}
		}

	if(fill)
		PolyFill(vert,vertices,colorindex);
	else
		PolyWire(FALSE,vert,vertices,colorindex);

	osdFree(vert);
	}



/******************************************************************************
void	EW_Window::PolyWire(long closed,long vert[][2],long vertices,long colorindex)

	this can be optimised

Keywords:
******************************************************************************/
void EW_Window::PolyWire(long closed,long vert[][2],long vertices,long colorindex)
	{
	long m;

	for(m=1;m<vertices;m++)
		Line(vert[m-1][0],vert[m-1][1],vert[m][0],vert[m][1],colorindex);

	if(closed)
		Line(vert[vertices-1][0],vert[vertices-1][1],vert[0][0],vert[0][1],colorindex);
	}



/******************************************************************************
void	EW_Window::PolyFill(long vert[][2],long vertices,long colorindex)

Keywords:
******************************************************************************/
void EW_Window::PolyFill(long vert[][2],long vertices,long colorindex)
	{
	long sx,sy;
	long m;

	GetSize(&sx,&sy);

	ActivateColor(colorindex);

	#if EW_GL==EW_X_GFX

		/* Pure X Windows */
		XPoint *vertex=(XPoint *)osdMalloc(vertices*sizeof(XPoint));

		for(m=0;m<vertices;m++)
			{
			vertex[m].x=vert[m][0];
			vertex[m].y=EW_REMAP_Y(vert[m][1],sy);
			}

		XFillPolygon(display,win,GetGC(),vertex,vertices,Convex,CoordModeOrigin);

		osdFree(vertex);

	#elif EW_GL==EW_OPENGL

		/* OpenGL */

		glBegin(GL_POLYGON);
		for(m=0;m<vertices;m++)
			glVertex2i(vert[m][0],vert[m][1]);
		glEnd();

	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		POINT *pnt=(POINT *)osdMalloc(vertices*sizeof(POINT));

		for(m=0;m<vertices;m++)
			{
			pnt[m].x = vert[m][0];
			pnt[m].y = EW_REMAP_Y(vert[m][1],sy);
			}

		if(!Polygon(hdc,pnt,vertices))
			EW_PRINT(EW_GFX,EW_ERROR,"could not draw polygon");

		osdFree(pnt);

	#elif EW_GL==EW_IRISGL

		/* IrisGL */
		bgnpolygon();
		for(m=0;m<vertices;m++)
			v2i(vert[m]);
		endpolygon();

	#else
		#error EW_GL not properly defined
	#endif
	}



/******************************************************************************
void	EW_Window::TriangleWire(long a[2],long b[2],long c[2],long colorindex)

	draw arbitrary 2D filled triangle in current color

	for potential future convention, all vertices should be in right-hand order
	to imply a surface-normal

	could be optimised for some systems

******************************************************************************/
void EW_Window::TriangleWire(long a[2],long b[2],long c[2],long colorindex)
	{
	Line(a[0],a[1],b[0],b[1],colorindex);
	Line(b[0],b[1],c[0],c[1],colorindex);
	Line(c[0],c[1],a[0],a[1],colorindex);
	}



/******************************************************************************
void	EW_Window::TriangleFill(long a[2],long b[2],long c[2],long colorindex)

	draw arbitrary 2D filled triangle in current color

	for future convention, all vertices should be in right-hand order
	to imply a surface-normal

******************************************************************************/
void EW_Window::TriangleFill(long a[2],long b[2],long c[2],long colorindex)
	{
	long sx,sy;

	GetSize(&sx,&sy);

	ActivateColor(colorindex);

	#if EW_GL==EW_X_GFX

		/* Pure X Windows */
		XPoint vertex[3];

		vertex[0].x=a[0];
		vertex[0].y=EW_REMAP_Y(a[1],sy);

		vertex[1].x=b[0];
		vertex[1].y=EW_REMAP_Y(b[1],sy);

		vertex[2].x=c[0];
		vertex[2].y=EW_REMAP_Y(c[1],sy);

		XFillPolygon(display,win,GetGC(),vertex,3,Convex,CoordModeOrigin);

		// fill in the space that X's algorithm leaves unfilled
		TriangleWire(a,b,c,colorindex);

	#elif EW_GL==EW_OPENGL

		/* OpenGL */

		// fill doesn't fill completely -> so do outline, too

		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		glBegin(GL_TRIANGLES);
		glVertex2i(a[0],a[1]);
		glVertex2i(b[0],b[1]);
		glVertex2i(c[0],c[1]);
		glEnd();

		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

		glBegin(GL_TRIANGLES);
		glVertex2i(a[0],a[1]);
		glVertex2i(b[0],b[1]);
		glVertex2i(c[0],c[1]);
		glEnd();

	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		static POINT pnt[3];
		pnt[0].x = a[0];
		pnt[0].y = EW_REMAP_Y(a[1],sy);
		pnt[1].x = b[0];
		pnt[1].y = EW_REMAP_Y(b[1],sy);
		pnt[2].x = c[0];
		pnt[2].y = EW_REMAP_Y(c[1],sy);

		if(!Polygon(hdc,pnt,3))
			EW_PRINT(EW_GFX,EW_ERROR,"could not draw polygon");

	#elif EW_GL==EW_IRISGL

		/* IrisGL */
		bgnpolygon();
		v2i(a);
		v2i(b);
		v2i(c);
		endpolygon();

	#else
		#error EW_GL not properly defined
	#endif
	}



/******************************************************************************
void	EW_Window::Line(long x1,long y1,long x2,long y2,long colorindex)

	draw arbitrary 2D line in given color index

	this line includes drawing BOTH endpoints

******************************************************************************/
void EW_Window::Line(long x1,long y1,long x2,long y2,long colorindex)
	{
	long sx,sy;

	GetSize(&sx,&sy);

#if EW_GFX_DEBUG
		printf("EW_Window::Line(%d,%d,%d,%d,%d)\n",x1,y1,x2,y2,colorindex);
#endif

	ActivateColor(colorindex);

	#if EW_GL==EW_X_GFX

		/* Pure X Windows */
		XDrawLine(display,win,GetGC(),x1,EW_REMAP_Y(y1,sy),x2,EW_REMAP_Y(y2,sy));

	#elif EW_GL==EW_OPENGL

		/* OpenGL */
		glBegin(GL_LINES);
		glVertex2i(x1,y1);
		glVertex2i(x2,y2);
		glEnd();

	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		if(!MoveToEx(hdc,x1,EW_REMAP_Y(y1,sy),NULL))
			EW_PRINT(EW_GFX,EW_ERROR,"could not move to");

		if(!LineTo(hdc,x2,EW_REMAP_Y(y2,sy)))
			EW_PRINT(EW_GFX,EW_ERROR,"could not draw line");

	#elif EW_GL==EW_IRISGL

		/* IrisGL */
		long vert[2][2];

		vert[0][0]=x1;
		vert[0][1]=y1;

		vert[1][0]=x2;
		vert[1][1]=y2;

		bgnline();
		v2i(vert[0]);
		v2i(vert[1]);
		endline();

	#else
		#error EW_GL not properly defined
	#endif


	#if EW_GL==EW_OPENGL || EW_GL==EW_GDI

		/* OpenGL and GDI draw up to but not including, so we add the extra point for consistancy */
		Point(x2,y2,colorindex);

	#endif
	}



/******************************************************************************
void	EW_Window::Point(long x,long y,long colorindex)

	draw arbitrary 2D point of given colorindex

Keywords:
******************************************************************************/
void EW_Window::Point(long x,long y,long colorindex)
	{
	#if EW_GFX_DEBUG
		printf("EW_Window::Point(%d,%d,%d)\n",x,y,colorindex);
	#endif

	long sx,sy;

	GetSize(&sx,&sy);

	#if EW_GL==EW_X_GFX

		ActivateColor(colorindex);

		/* Pure X Windows */
		XDrawPoint(display,win,GetGC(),x,EW_REMAP_Y(y,sy));

	#elif EW_GL==EW_OPENGL

		ActivateColor(colorindex);

		/* OpenGL */
		glBegin(GL_POINTS);
		glVertex2i(x,y);
		glEnd();

	#elif EW_GL==EW_GDI

		/* Win32 GDI */
		static long remapped_y;
		remapped_y = EW_REMAP_Y(y,sy);

#if FALSE
		if(SetPixel(hdc,x,remapped_y,EwWin32Colortable[colorindex])== -1)
			EW_PRINT(EW_GFX,EW_ERROR,"could not set point");
#elif FALSE
		RectFill(x,y,x,y,colorindex);
#else
		ActivateColor(colorindex);

		if(!MoveToEx(hdc,x,remapped_y,NULL))
			EW_PRINT(EW_GFX,EW_ERROR,"could not move to point");

		/* since GDI uses an "up to" paradigm, we have to draw to an adjacent pixel to make a point */
		if(!LineTo(hdc,x+1,remapped_y))
			EW_PRINT(EW_GFX,EW_ERROR,"could not draw point");
#endif

	#elif EW_GL==EW_IRISGL

		ActivateColor(colorindex);

		/* IrisGL */
		long vert[2][2];

		vert[0][0]=x;
		vert[0][1]=y;

		bgnpoint();
		v2i(vert[0]);
		endpoint();

	#else
		#error EW_GL not properly defined
	#endif
	}



/******************************************************************************
void	EW_Window::ImageDraw(long posx,long posy,long lenx,long leny,
										unsigned char *buffer,long transparent)

	ok for icon-sized images, but not really efficient.

	True X and GDI versions should be implemented, if possible.

	NOTE: double buffering could make true X images a mess

******************************************************************************/
void EW_Window::ImageDraw(long posx,long posy,long lenx,long leny,unsigned char *buffer,long transparent)
	{
	if(transparent)
		{
		// for transparent, has to be brute force

		long x,y,yx,value;

		for(y=0;y<leny;y++)
			{
			yx=lenx*y;

			for(x=0;x<lenx;x++)
				{
				value=buffer[yx+x];

				if(value!=255)
					Point(posx+x,posy+leny-1-y,value);
				}
			}

		return;
		}

	#if FALSE	// EW_GL==EW_X_GFX
		printf("GetVisualInfo()=0x%x XCreateImage(0x%x,0x%x,0x%x,0x%x,0,0x%x,%d,%d,8,0)\n",
							GetVisualInfo(),display,GetVisualInfo()->visual,GetVisualInfo()->depth,XYBitmap,buffer,lenx,leny);

// try XYPixmap instead of XYBitmap

		// fails every time
		XImage *ximage=XCreateImage(display,GetVisualInfo()->visual,GetVisualInfo()->depth,XYBitmap,0,buffer,lenx,leny,8,0);

		if(!ximage)
			{
			EW_PRINT(EW_GFX,EW_ERROR,"XCreateImage() failed");
			return;
			}

//		XInitImage(ximage);

		XPutImage(display,win,GetGC(),ximage,0,0,posx,posy,lenx,leny);

		ximage->data=NULL;
		XDestroyImage(ximage);

	#elif EW_GL==EW_OPENGL

		// invert while drawing
		long y;
		for(y=0;y<leny;y++)
			{
			glRasterPos2i(posx,posy+leny-1-y);
			glDrawPixels(lenx,1,GL_COLOR_INDEX,GL_BYTE,&buffer[y*lenx]);
			}

	#elif FALSE // EW_GL==EW_GDI

		// untested

		HDC hdc_compatible=CreateCompatibleDC(hdc);
		if(!hdc)
			EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::ImageDraw() CreateCompatibleDC() failed");
		else
			{
			HBITMAP hbitmap=CreateCompatibleBitmap(hdc,lenx,leny);
			if(!hbitmap)
				EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::ImageDraw() CreateCompatibleBitmap() failed");
			else
				{
				if(!SetBitmapBits(hbitmap,lenx*leny,buffer))
					EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::ImageDraw() SetBitmapBits() failed");

				if(!SelectObject(hdc_compatible,hbitmap))
					EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::ImageDraw() SelectObject() failed");
				else
					{
					if(!BitBlt(hdc,posx,posy,lenx,leny,hdc_compatible,0,0,SRCCOPY))
						EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::ImageDraw() BitBlt() failed");
					}

				DeleteObject(hbitmap);
				}

			DeleteDC(hdc_compatible);
			}

	#else
		// brute force (hey, it works)
		long x,y,yx;

		for(y=0;y<leny;y++)
			{
			yx=lenx*y;

			for(x=0;x<lenx;x++)
				Point(posx+x,posy+leny-1-y,buffer[yx+x]);
			}

	#endif
	}



/******************************************************************************
void	EW_Window::ImageCopy(long srcx,long srcy,long destx,long desty,
															long lenx,long leny)

	copy region of size sx,sy from srcx,srcy to destx,desty

******************************************************************************/
void EW_Window::ImageCopy(long srcx,long srcy,long destx,long desty,long lenx,long leny)
	{
	long sx,sy;

	GetSize(&sx,&sy);

#if TRUE
	// limit bounds

	long osrc[2],odest[2],olen[2];

	osrc[0]=srcx;
	osrc[1]=srcy;
	odest[0]=destx;
	odest[1]=desty;
	olen[0]=lenx;
	olen[1]=leny;

	if(srcx<0)
		{
		destx-=srcx;
		lenx+=srcx;
		srcx=0;
		}
	if(destx<0)
		{
		srcx-=destx;
		lenx+=destx;
		destx=0;
		}

	lenx=EW_MIN(lenx,sx-srcx);
	lenx=EW_MIN(lenx,sx-destx);

	if(srcy<0)
		{
		desty-=srcy;
		leny+=srcy;
		srcy=0;
		}
	if(desty<0)
		{
		srcy-=desty;
		leny+=desty;
		desty=0;
		}

	leny=EW_MIN(leny,sy-srcy-1);
	leny=EW_MIN(leny,sy-desty-1);

	// fill in areas copied from outside of window bounds

	if(odest[0]>=0 && odest[0]<destx)
		RectFill(odest[0],odest[1],destx,odest[1]+olen[1]-1,EW_COLOR_VOID);
	if(destx+lenx<odest[0]+olen[0])
		RectFill(destx+lenx,odest[1],odest[0]+olen[0]-1,odest[1]+olen[1]-1,EW_COLOR_VOID);

	if(odest[1]>=0 && odest[1]<desty)
		RectFill(odest[0],odest[1],odest[0]+olen[0]-1,desty,EW_COLOR_VOID);
	if(desty+leny<odest[1]+olen[1])
		RectFill(odest[0],desty+leny,odest[0]+olen[0]-1,odest[1]+olen[1]-1,EW_COLOR_VOID);

#endif

	if(srcx>=sx-1 || srcy>=sy-1 || destx>=sx-1 || desty>=sy-1 || lenx<1 || leny<1)
		return;
	
	#if EW_GL==EW_X_GFX

		#if FALSE

			// on SGI, this technique is faster than XCopyArea()
			// on Sun, it is much, much slower (around 2 seconds)

			XImage *ximage=XGetImage(display,win,srcx,EW_REMAP_Y(srcy+leny-1,sy),lenx,leny,AllPlanes,XYPixmap);

			if(!ximage)
				{
				EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::ImageCopy() XGetImage() failed");
				return;
				}

			XPutImage(display,win,GetGC(),ximage,0,0,destx,EW_REMAP_Y(desty+leny-1,sy),lenx,leny);

			XFree(ximage);

		#else

			// about the same speed on Sun and SGI

			XCopyArea(display,win,win,GetGC(),srcx,EW_REMAP_Y(srcy+leny-1,sy),lenx,leny,destx,EW_REMAP_Y(desty+leny-1,sy));

		#endif

	#elif EW_GL==EW_OPENGL

		glRasterPos2i(destx,desty);
		glCopyPixels(srcx,srcy,lenx,leny,GL_COLOR);

	#elif EW_GL==EW_GDI

		if( !BitBlt(hdc,destx,EW_REMAP_Y(desty+leny-1,sy),lenx,leny,hdc,srcx,EW_REMAP_Y(srcy+leny-1,sy),SRCCOPY) )
			EW_PRINT(EW_GFX,EW_ERROR,"EW_Window::ImageCopy() BitBlt() failed");

	#else

	#endif
	}



/******************************************************************************
void	EW_Window::GreyOut(long x,long y,long sx,long sy,long colorindex)

	haze over area, but must let some of underlying area show through

******************************************************************************/
void EW_Window::GreyOut(long x,long y,long sx,long sy,long colorindex)
	{
#if TRUE
	// with inc of 2, looks like evenly spaced points at 50% fill
	LineOut(x,y,sx,sy,0,2,colorindex);
#else
	// draw LOTS of points
	// could be more efficient by using XDrawPoints(), etc.
	long m,n;

	for(n=0;n<sy;n+=2)
		for(m=0;m<sx;m+=1)
			Point(x+m,y+n+m%2,colorindex);
#endif
	}



/******************************************************************************
void	EW_Window::LineOut(long x,long y,long sx,long sy,long offset,long inc,
															long colorindex)

	diagonal hatching, in lines with bands starting from lower left pixel
		and incrementing NE with NW-SE lines

	 C D
	B C D
	 B C
	A B C

******************************************************************************/
void EW_Window::LineOut(long x,long y,long sx,long sy,long offset,long inc,long colorindex)
	{
	// diagonal lines
	long x1,y1,x2,y2;
	long ix=inc-sy+offset-1;
	long iy=inc-sx+offset-1;

	while(ix<sx)
		{
		if(ix<0)
			{
			x1=x;
			y1=y+sy-1+ix;
			}
		else
			{
			x1=x+ix;
			y1=y+sy-1;
			}

		if(iy<0)
			{
			x2=x+sx-1+iy;
			y2=y;
			}
		else
			{
			x2=x+sx-1;
			y2=y+iy;
			}

		Line(x1,y1,x2,y2,colorindex);

		ix+=inc;
		iy+=inc;
		}
	}
