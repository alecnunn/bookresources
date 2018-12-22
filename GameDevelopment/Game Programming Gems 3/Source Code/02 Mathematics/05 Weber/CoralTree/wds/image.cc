/******************************************************************************

Coral Tree wds/image.cc
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



/******************************************************************************
		WDS_Image::WDS_Image(void)

******************************************************************************/
WDS_Image::WDS_Image(void)
	{
	last_bg= -1;
	size[0]=0;
	size[1]=0;
	numbercolors=16;
	flags=EW_XPM_FLAG_NONE;
	remap=NULL;

	readbuffer=NULL;
	drawbuffer=NULL;
	}



/******************************************************************************
		WDS_Image::~WDS_Image(void)

******************************************************************************/
WDS_Image::~WDS_Image(void)
	{
	if(readbuffer)
		osdFree(readbuffer);
	if(drawbuffer)
		osdFree(drawbuffer);
	}



/******************************************************************************
long	WDS_Image::Interpret(char **data,char *filename)

	private

******************************************************************************/
long WDS_Image::Interpret(char **data,char *filename)
	{
	long number=numbercolors;

	if(number<2)
		number=2;

	if(readbuffer)
		{
		osdFree(readbuffer);
		readbuffer=NULL;
		}

	if(data==NULL && filename==NULL)
		return FALSE;

	readbuffer=ewInterpretXPM(data,filename,number,flags,remap,&size[0],&size[1]);

	last_bg= -1;
	drawbuffer=(unsigned char *)osdRealloc(drawbuffer,size[0]*size[1]);

	return (readbuffer!=NULL);
	}



/******************************************************************************
long	WDS_Image::GetSize(long *x,long *y)

******************************************************************************/
long WDS_Image::GetSize(long *x,long *y)
	{
	if(readbuffer)
		{
		*x=size[0];
		*y=size[1];
		return TRUE;
		}
	else
		{
		*x=0;
		*y=0;
		return FALSE;
		}
	}



/******************************************************************************
long	WDS_Image::ReadFromBufferXPM(char **data)

******************************************************************************/
long WDS_Image::ReadFromBufferXPM(char **data)
	{
	long result=Interpret(data,NULL);

	return result;
	}



/******************************************************************************
long	WDS_Image::LoadFromFileXPM(char *filename)

******************************************************************************/
long WDS_Image::LoadFromFileXPM(char *filename)
	{
	long result=Interpret(NULL,filename);

	return result;
	}



/******************************************************************************
void	WDS_Image::Draw(EW_Window *root,long posx,long posy,long background)

******************************************************************************/
void WDS_Image::Draw(EW_Window *root,long posx,long posy,long background)
	{
	if(!readbuffer)
		return;

	if(background==255)
		{
		root->ImageDraw(posx,posy,size[0],size[1],readbuffer,TRUE);
		}
	else
		{
		if(!drawbuffer || background!=last_bg)
			{
			long m,length=size[0]*size[1];

			for(m=0;m<length;m++)
				{
				if(readbuffer[m]==255)
					drawbuffer[m]=background;
				else
					drawbuffer[m]=readbuffer[m];
				}
			}

		if(drawbuffer)
			root->ImageDraw(posx,posy,size[0],size[1],drawbuffer,FALSE);
		}
	}



/******************************************************************************
void	WDS_Image::Alter(long method,long direction)

******************************************************************************/
void WDS_Image::Alter(long method,long direction)
	{
	if(!readbuffer)
		return;

	last_bg= -1;

	long length=size[0]*size[1];
	unsigned char *temp=(unsigned char *)osdMalloc(length);

	osdMemcpy(temp,readbuffer,length);

	long x,y,yx1,yx2;

	if(method==WDS_IMAGE_FLIP)
		{
		if(direction==WDS_VERTICAL)
			{
			for(y=0;y<size[1];y++)
				{
				yx1=y*size[0];
				yx2=(size[1]-y-1)*size[0];
				for(x=0;x<size[0];x++)
					readbuffer[yx1+x]=temp[yx2+x];
				}
			}
		else
			{
			for(y=0;y<size[1];y++)
				{
				yx1=y*size[0];
				for(x=0;x<size[0];x++)
					readbuffer[yx1+x]=temp[yx1+(size[0]-x-1)];
				}
			}
		}
	else
		{
		if(direction)
			{
			for(y=0;y<size[1];y++)
				{
				yx1=y*size[0];
				for(x=0;x<size[0];x++)
					{
					yx2=(size[0]-x-1)*size[1];
					readbuffer[yx1+x]=temp[yx2+y];
					}
				}
			}
		else
			{
			for(y=0;y<size[1];y++)
				{
				yx1=(size[1]-y-1)*size[0];
				for(x=0;x<size[0];x++)
					{
					yx2=x*size[1];
					readbuffer[yx1+x]=temp[yx2+y];
					}
				}
			}
		}

	osdFree(temp);
	}
