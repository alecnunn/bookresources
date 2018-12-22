/******************************************************************************

Coral Tree ew/xpm.cc
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



#define EW_XPM_DEBUG			FALSE
#define EW_LOADIMAGE_MAXLINE	2048


// error diffusion filters
float ewStucki[3][5]=
	{
	{0.0/42.0,	0.0/42.0,	0.0/42.0,	8.0/42.0,	4.0/42.0},
	{2.0/42.0,	4.0/42.0,	8.0/42.0,	4.0/42.0,	2.0/42.0},
	{1.0/42.0,	2.0/42.0,	4.0/42.0,	2.0/42.0,	1.0/42.0}
	};
float ewBurkes[3][5]=
	{
	{0.0/32.0,	0.0/32.0,	0.0/32.0,	8.0/32.0,	4.0/32.0},
	{2.0/32.0,	4.0/32.0,	8.0/32.0,	4.0/32.0,	2.0/32.0},
	{0.0/32.0,	0.0/32.0,	0.0/32.0,	0.0/32.0,	0.0/32.0}
	};
float ewFloydSteinburg[3][5]=
	{
	{0.0/16.0,	0.0/16.0,	0.0/16.0,	7.0/16.0,	0.0/16.0},
	{0.0/16.0,	3.0/16.0,	5.0/16.0,	1.0/16.0,	0.0/16.0},
	{0.0/16.0,	0.0/16.0,	0.0/16.0,	0.0/16.0,	0.0/16.0}
	};


class EW_ImageMapping	
	{
	public:
		long	code;
		long	transparent;
		long	directmap;
		float	intensity[3];
	};



/******************************************************************************
unsigned char	*ewInterpretImageXPM(char **data,long number_colors,
					long flags,EW_ColorMapping *remap,long *width,long *height)

******************************************************************************/
/*PROTOTYPE*/
unsigned char *ewReadImageXPM(char **data,long number_colors,long flags,EW_ColorMapping *remap,long *width,long *height)
	{
	return ewInterpretXPM(data,NULL,number_colors,flags,remap,width,height);
	}



/******************************************************************************
unsigned char	*ewLoadImageXPM(char *filename,long number_colors,
					long flags,EW_ColorMapping *remap,long *width,long *height)

******************************************************************************/
/*PROTOTYPE*/
unsigned char *ewLoadImageXPM(char *filename,long number_colors,long flags,EW_ColorMapping *remap,long *width,long *height)
	{
	return ewInterpretXPM(NULL,filename,number_colors,flags,remap,width,height);
	}



/******************************************************************************
unsigned char	*ewInterpretXPM(char **data,char *filename,long number_colors,
					long flags,EW_ColorMapping *remap,long *width,long *height)

	if data!=NULL, contains XPM data in memory

	remap is a 0x7fffffff-terminated list of color-indexes to remap pixels to
	if remap[n]<0, indicates actual colorindex
	if remap==NULL, no remap is done

	returns NULL if not completely successful

******************************************************************************/
/*PROTOTYPE*/
unsigned char *ewInterpretXPM(char **data,char *filename,long number_colors,long flags,EW_ColorMapping *remap,long *width,long *height)
	{
	char message[1024];
	char charcode[32],arg[128];
	char line[EW_LOADIMAGE_MAXLINE];
	unsigned char *buffer;
	char *iteration;
	long code;
	long intensity[3];
	long m,n;
	long x,y,yx;

	EW_ImageMapping *mapping;
	EW_ImageMapping *temp;

	GPL_File file;
	GPL_FileIterator iterator;

	long mappings=0;
	if(remap)
		mappings=remap->GetSize();

	if(data)
		iteration=data[0];
	else
		{
		if( !file.Open(filename,OSD_FILE_READ) )
			{
			sprintf(message,"ewLoadImageXPM() cannot open file (%s) for read",filename);
			gplPrint(GPL_SOFTWARE,GPL_WARN,message);
			return NULL;
			}

		// confirm format
		long bytes=file.ReadLine(line,EW_LOADIMAGE_MAXLINE);
		if(bytes<1 || strcmp(line,"/* XPM */"))
			{
			sprintf(message,"ewLoadImageXPM() file (%s) is not XPM",filename);
			EW_PRINT(EW_GFX,EW_WARN,message);
			return NULL;
			}

		iterator.Reset(&file);
		iterator.SetDelimiter('"');
		
		// read: static char * ...
		iteration=iterator.Next();

		// read: width height numcolors chars-per-pixel [hotx hoty] [XPMEXT]
		iteration=iterator.Next();
		}

#if EW_XPM_DEBUG
	printf("header: (%s)\n",iteration);
#endif

	long offset=0;
	long value[6];
	int inc;
	float fvalue;

	for(m=0;m<6;m++)
		{
		fvalue=0;
		sscanf(&iteration[offset],"%f%n",&fvalue,&inc);
		value[m]=(long)fvalue;
/*
		arg[0]=0;
		sscanf(&iteration[offset],"%*[ ]%[^ ]%n",arg,&inc);
		value[m]=atoi(arg);
*/
		offset+=inc;
		}

	if(width)
		*width=value[0];
	if(height)
		*height=value[1];
	long numcolors=value[2];
	long charsperpixel=value[3];

	if(file.EndOfFile())
		{
		sprintf(message,"ewLoadImageXPM() file (%s) has premature EOF (in header)",filename);
		EW_PRINT(EW_GFX,EW_WARN,message);
		return NULL;
		}

#if EW_XPM_DEBUG
	printf("numcolors=%d width=%d height=%d charsperpixel=%d sizeof(mapping[0])=%d\n",
															numcolors,value[0],value[1],charsperpixel,sizeof(mapping[0]));
#endif

	mapping=(EW_ImageMapping *)osdMalloc(numcolors*sizeof(EW_ImageMapping));
	temp=(EW_ImageMapping *)osdMalloc(value[0]*value[1]*sizeof(EW_ImageMapping));
	buffer=(unsigned char *)osdMalloc(value[0]*value[1]);

	if(!mapping || !temp || !buffer)
		{
		EW_PRINT(EW_GFX,EW_ERROR,"ewLoadImageXPM() could not allocate buffers");

		if(mapping)
			osdFree(mapping);
		if(temp)
			osdFree(temp);
		if(buffer)
			osdFree(buffer);

		return NULL;
		}

	char colorprefix;
	char scancode[32];

	for(m=0;m<numcolors;m++)
		{
		if(data)
			iteration=data[1+m];
		else
			{
			// read: " , \n "
			iteration=iterator.Next();

			// read: ?? c #rrggbb
			iteration=iterator.Next();
			}

#if EW_XPM_DEBUG
		printf("mapping: (%s) charsperpixel=%d\n",iteration,charsperpixel);
#endif

		charcode[0]=0;
		colorprefix=0;
		arg[0]=0;
		sprintf(scancode,"%%%dc %%*s %%c%%2x%%2x%%2x",(int)charsperpixel);
		sscanf(iteration,scancode,charcode,&colorprefix,&intensity[0],&intensity[1],&intensity[2]);

		code=0;
		for(y=0;y<charsperpixel;y++)
			code= (code<<8) + charcode[y];

		charcode[charsperpixel]=0;

#if EW_XPM_DEBUG
		printf("  (%s) code=%d (%s) colorprefix=(%c) intensity=%d,%d,%d\n",scancode,code,charcode,colorprefix,
																			intensity[0],intensity[1],intensity[2]);
#endif

		long remapped=FALSE;
		mapping[m].directmap= -1;

		if(m<mappings)
			{
			if(remap->IsIndex(m))
				{
				mapping[m].directmap=remap->GetMapIndex(m);
				}
			else
				{
				long value=remap->GetMapColor(m);

				intensity[2]=value&0xff;
				value=value>>8;

				intensity[1]=value&0xff;
				value=value>>8;

				intensity[0]=value&0xff;
				remapped=TRUE;
				}
			}

		mapping[m].code=code;

		for(y=0;y<3;y++)
			{
			if(colorprefix=='#' || remapped)
				{
				mapping[m].intensity[y]=intensity[y];
				mapping[m].transparent=FALSE;
				}
			else
				{
				mapping[m].intensity[y]=0xff;
				mapping[m].transparent=TRUE;
				}
			}
		}

	for(y=0;y<value[1];y++)
		{
		yx=y*value[0];

		if(data)
			iteration=data[1+numcolors+y];
		else
			{
			// read: " , \n "
			iteration=iterator.Next();

			// read: aabbccddeeff ...
			iteration=iterator.Next();
			}

#if EW_XPM_DEBUG
		printf("line: (%s)\n",iteration);
#endif

		if((long)(strlen(iteration))<value[0]*charsperpixel)
			{
			EW_PRINT(EW_GFX,EW_ERROR,"ewLoadImageXPM() data line too short");
			}
		else
			for(x=0;x<value[0];x++)
				{
				// calc code
				code=0;
				for(m=0;m<charsperpixel;m++)
					code= (code<<8) + iteration[x*charsperpixel+m];

				// find code
				m=numcolors-1;
				while(m>0 && mapping[m].code!=code)
					m--;

				for(n=0;n<3;n++)
					temp[yx+x].intensity[n]=mapping[m].intensity[n];

				temp[yx+x].transparent=mapping[m].transparent;
				temp[yx+x].directmap=mapping[m].directmap;
				}

#if EW_XPM_DEBUG
		for(x=0;x<value[0];x++)
			printf("%.2G ",temp[yx+x].intensity[0]);
		printf("\n");
#endif
		}

	long j,k,j1,j2,k2;
	long error[3];

	long dither= !(flags&EW_XPM_FLAG_NO_DITHER);

	for(y=0;y<value[1];y++)
		{
		yx=y*value[0];

		for(x=0;x<value[0];x++)
			{
			if(temp[yx+x].directmap>=0)
				buffer[yx+x]=temp[yx+x].directmap;
			else if(temp[yx+x].transparent)
				buffer[yx+x]=255;
			else
				{
				buffer[yx+x]=ewFindNearestColor(number_colors,temp[yx+x].intensity);

				// determine error
				ewGetColorMapping(buffer[yx+x],error);

				for(n=0;n<3;n++)
					error[n]-=(long)(temp[yx+x].intensity[n]);

				j1=EW_MAX(0,2-x);
				j2=EW_MIN(5,2+value[0]-x);
				k2=EW_MIN(3,value[1]-y);

				// distribute error
				if(dither)
					for(k=0;k<k2;k++)
						for(j=j1;j<j2;j++)
							for(n=0;n<3;n++)
								temp[(y+k)*value[0]+x+j-2].intensity[n]-=error[n]*ewFloydSteinburg[k][j];
				}
			}
		}

	osdFree(mapping);
	osdFree(temp);

	return buffer;
	}



/******************************************************************************
void	ewGetColor(long index,long intensity[3])

******************************************************************************/
/*PROTOTYPE*/
void ewGetColorMapping(long index,long intensity[3])
	{
	long n;

	for(n=0;n<3;n++)
		intensity[n]=EwXColor[index][n]>>8;
	}



/******************************************************************************
long	ewFindNearestColor(long number_colors,float intensity[3])

	returns color index of color closest to given intensity

******************************************************************************/
/*PROTOTYPE*/
long ewFindNearestColor(long number_colors,float intensity[3])
	{
	float diff,sum;
	float leastdiff=100.0;
	long least=0;

	long m,n;

//	printf("rgb=%.2f,%.2f,%.2f\n",intensity[0]/(float)0xFF,intensity[1]/(float)0xFF,intensity[2]/(float)0xFF);

	for(m=0;m<number_colors;m++)
		{
		sum=0.0;

		for(n=0;n<3;n++)
			{
			diff=EwXColor[m][n]/(float)0xFFFF-intensity[n]/(float)0xFF;
			sum+=diff*diff;
			}

		diff=sqrt(sum);

/*
		printf("  vs %.2f,%.2f,%.2f diff=%.2f/%.2f (%d)\n",
		EwXColor[m][0]/(float)0xFFFF,EwXColor[m][1]/(float)0xFFFF,EwXColor[m][2]/(float)0xFFFF,
		diff,leastdiff,m);
*/

		if(leastdiff>diff)
			{
			leastdiff=diff;
			least=m;
			}
		}

	return least;
	}
