/******************************************************************************

Coral Tree wds/gfxtest.h
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


#define WDS_GFXTEST_IMAGESIZE	15


/*****************************************************************************
class	WDS_GfxTest

	graphics tests

*****************************************************************************/
class WDS_GfxTest : public WDS_Widget
	{
	GPL_OBJECT_INTERFACE(WDS_GfxTest);

	private:
		unsigned char	made_image[WDS_GFXTEST_IMAGESIZE*WDS_GFXTEST_IMAGESIZE];
		unsigned char	*loaded_image;

		long	counter[4];
		long	work;
		long	size[2];
		long	delta[2];
		long	loadedsize[2];

	public:
				WDS_GfxTest(void);
virtual			~WDS_GfxTest(void);

		long	Operate(EW_Event *event);
		void	Draw(EW_Redraw *redraw);
		void	Increment(void);

		long	*GetCounterPointer(long m)	{ return &counter[m]; };
	};
