/******************************************************************************

Coral Tree wds/image.h
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


#define	WDS_IMAGE_ROTATE	0
#define	WDS_IMAGE_FLIP		1


/******************************************************************************
class	WDS_Image

******************************************************************************/
class WDS_Image
	{
	private:
		EW_ColorMapping	*remap;
		unsigned char	*readbuffer;
		unsigned char	*drawbuffer;
		long			numbercolors;
		long			flags;
		long			last_bg;
		long			size[2];

		long	Interpret(char **data,char *filename);
		void	Alter(long method,long direction);

	public:

				WDS_Image(void);
virtual			~WDS_Image(void);

		long	GetSize(long *x,long *y);

		void	SetNumberColors(long set)	{ numbercolors=set; };
		long	GetNumberColors(void)		{ return numbercolors; };

		void	SetFlags(long set)			{ flags=set; };
		long	GetFlags(void)				{ return flags; };

		void			SetRemap(EW_ColorMapping *set)	{ remap=set; };
		EW_ColorMapping	*GetRemap(void)					{ return remap; };

		long	ReadFromBufferXPM(char **data);
		long	LoadFromFileXPM(char *filename);

		void	Rotate(long clockwise)	{ Alter(WDS_IMAGE_ROTATE,clockwise); };
		void	Flip(long vertical)		{ Alter(WDS_IMAGE_FLIP,vertical); };

		void	Draw(EW_Window *root,long posx,long posy,long background=255);
	};
