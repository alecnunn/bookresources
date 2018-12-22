/******************************************************************************

Coral Tree ew/xdbuf.cc
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

#include "ew.h"



#define EW_XDBUF_DEBUG			FALSE
#define EW_XDBUF_FADE			FALSE	// darken colors in one buffer
#define EW_XDBUF_ALWAYS_FAIL	FALSE



#if EW_GL==EW_X_GFX 

/******************************************************************************
long	EW_Window::Xdbuf_InitColorBuffer(void)

	initialize colormap in preparation for simulated Double Buffering
		under X by using colormap manipulation

	based on article "Graphics Effects by Manipulating X Colormaps"
		in XJournal May-June 1992

	returns FALSE if successful

Keywords:
******************************************************************************/
long EW_Window::Xdbuf_InitColorBuffer(void)
	{
	unsigned long m,n,p;
	unsigned long value;

	long pixels=GetXDBufPixels();
	long planes=GetXDBufPlanes();

#if EW_XDBUF_DEBUG
	printf("pixels=%d planes=%d colors=%d\n",pixels,planes,GetXDBufColors());
#endif

	if(xdbuf_colormapped)
		{
		SetNumberColors(EW_MIN(GetNumberColors(),xdbuf_colormapped_colors));
		}
	else
		{
		XColor colordefs[EW_XDBUF_MAX_PIXELS];
		Status status;

#if EW_XDBUF_DEBUG
		EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Xdbuf_InitColorBuffer()");
#endif

		unsigned long cell;

		while(!xdbuf_allocated)
			{
			pixels=GetXDBufPixels();
			planes=GetXDBufPlanes();

#if EW_XDBUF_DEBUG
			printf("XAllocColorCells(0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x)\n",
									display,colormap,False,xdbuf_planelist,planes,xdbuf_pixellist,pixels);
#endif

#if EW_XDBUF_ALWAYS_FAIL
			status=0;
#else
			status=XAllocColorCells(display,colormap,False,xdbuf_planelist,planes,xdbuf_pixellist,pixels);
#endif

			if(status==0)
				{
				long previous=GetNumberColors();

				SetNumberColors(previous-1);

				EW_PRINT(EW_WINDOW,EW_WARN,
								"EW_Window::Xdbuf_InitColorBuffer() can't allocate color cells -> degrading color resolution");

				if(previous==GetNumberColors())
					{
					EW_PRINT(EW_WINDOW,EW_WARN,"EW_Window::Xdbuf_InitColorBuffer() reached minimum color cells");
					return TRUE;
					}
				}
			else
				xdbuf_allocated=1;
			}

		xdbuf_colormapped=TRUE;
		xdbuf_colormapped_colors=GetNumberColors();

		for(m=0;m<(unsigned long)(EW_MAXCOLORS) && m<(unsigned long)pixels;m++)
			{
			colordefs[m].red  =EwXColor[m][0];
			colordefs[m].green=EwXColor[m][1];
			colordefs[m].blue =EwXColor[m][2];
			}
		for(;m<(unsigned long)pixels;m++)
			{
			colordefs[m].red  =255;
			colordefs[m].green=31;
			colordefs[m].blue =31;
			}

#if EW_XDBUF_DEBUG
		for(m=0;m<pixels;m++)
			printf("xdbuf_pixellist[%d]=0x%x %d\n",m,xdbuf_pixellist[m],xdbuf_pixellist[m]);
		for(m=0;m<planes;m++)
			printf("xdbuf_planelist[%d]=0x%x %d\n",m,xdbuf_planelist[m],xdbuf_planelist[m]);
#endif

		cell=0;
		for(m=0;m<(unsigned long)pixels;m++)
			for(n=0;n<(unsigned long)pixels;n++)
				{
				value=xdbuf_pixellist[m];
				for(p=0;p<(unsigned long)planes;p++)
					if(n&(1<<p))
						value|=xdbuf_planelist[p];

#if EW_XDBUF_DEBUG
				printf("%d,%d,%d value=0x%x (%d) cell=%d color %x,%x,%x\n",
							m,n,p,value,value,cell,
							colordefs[m].red,colordefs[m].green,colordefs[m].blue);
#endif

				Xdbuf_CopyColor(&colordefs[m],xdbuf_mapping[0]+cell,value);
#if EW_XDBUF_FADE
				colordefs[n].red/=2;
				colordefs[n].green/=2;
				colordefs[n].blue/=2;
#endif
				Xdbuf_CopyColor(&colordefs[n],xdbuf_mapping[1]+cell,value);
#if EW_XDBUF_FADE
				colordefs[n].red*=2;
				colordefs[n].green*=2;
				colordefs[n].blue*=2;
#endif
				cell++;
				}

		for(m=0;m<EW_MAXCOLORS;m++)
			{
			value=0L;

			n=m%pixels;

			for(p=0;p<(unsigned long)planes;p++)
				if(n&(1<<p))
					value|=xdbuf_planelist[p];

			index_remap[TRUE][m]=value;
			}
		}

	XGCValues xgcv;

	unsigned long mask;
	unsigned long used=0L;

	for(n=0;n<(unsigned long)planes;n++)
		used |= xdbuf_planelist[n];

//	mask=GCFunction|GCPlaneMask|GCForeground|GCFont|GCLineWidth;
	mask=GCFunction|GCPlaneMask;
	xgcv.function=GXcopy;

	// deal with it later
	xgcv.font=0;

	xgcv.plane_mask= ~used;
	xdbuf_context[0]=XCreateGC(display,win,mask,&xgcv);

	xgcv.plane_mask= used;
	xdbuf_context[1]=XCreateGC(display,win,mask,&xgcv);

	return FALSE;
	}



/******************************************************************************
void	EW_Window::Xdbuf_CopyColor(XColor *def,XColor *color,
														unsigned long pixel)

	copies (XColor *)def to (XColor *)color at pixel cell 'pixel'

Keywords:
******************************************************************************/
void EW_Window::Xdbuf_CopyColor(XColor *def, XColor *color, unsigned long pixel)
	{
#if EW_XDBUF_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Xdbuf_CopyColor()");
#endif

	color->red=def->red;
	color->green=def->green;
	color->blue=def->blue;
	color->flags=DoRed|DoGreen|DoBlue;
	color->pixel=pixel;
	}



/******************************************************************************
void	EW_Window::Xdbuf_SwapBuffers(void)

	simulates a buffer swap by quickly altering the colormap

******************************************************************************/
void EW_Window::Xdbuf_SwapBuffers(void)
	{
//	long bufferindexes=GetXDBufColors();
	long bufferindexes=xdbuf_colormapped_colors*xdbuf_colormapped_colors;

#if EW_XDBUF_DEBUG
	EW_PRINT(EW_WINDOW,EW_LOG,"EW_Window::Xdbuf_SwapBuffers()");
	printf("XStoreColors(0x%x,0x%x,0x%x,%d)\n",display,colormap,xdbuf_mapping[xdbuf_currentbufferindex],bufferindexes);
#endif

//	EW_PRINT(EW_WINDOW,EW_LOG,"%sXdbuf_SwapBuffers() from (%s)%s",ewColorCode(1),GetTitle(),ewColorCode(0));

	XSync(display,FALSE);
	XStoreColors(display,colormap,xdbuf_mapping[xdbuf_currentbufferindex],bufferindexes);
	XSync(display,FALSE);

	xdbuf_currentbufferindex=!xdbuf_currentbufferindex;
	}


#endif
