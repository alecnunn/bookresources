/******************************************************************************

Coral Tree ew/resize.h
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



/******************************************************************************
class EW_SizeConstraints

Keywords:
******************************************************************************/
class EW_SizeConstraints
	{
	protected:
		long minx,miny;
		long prefx,prefy;
		long maxx,maxy;

	public:

				EW_SizeConstraints()
					{
					Reset();
					};

		void	Reset(void)
					{
					minx=-1;
					miny=-1;

					prefx=-1;
					prefy=-1;

					maxx=-1;
					maxy=-1;
					};

		void	SetMinSize(long x,long y)		{ minx=x; miny=y; };
virtual	void	GetMinSize(long *x,long *y)		{ *x=minx; *y=miny; };

		void	SetPrefSize(long x,long y)		{ prefx=x; prefy=y; };
		void	GetPrefSize(long *x,long *y)	{ *x=prefx; *y=prefy; };

		void	SetMaxSize(long x,long y)		{ maxx=x; maxy=y; };
		void	GetMaxSize(long *x,long *y)		{ *x=maxx; *y=maxy; };
	};



/******************************************************************************
class EW_Resize

Keywords:
******************************************************************************/
class EW_Resize : public EW_SizeConstraints
	{
	private:

		long	recursive;

	public:

				EW_Resize()
					{
					recursive=TRUE;
					};

		void	SetRecursive(long set)	{ recursive=set; };
		long	GetRecursive(void)		{ return recursive; };

		void	Print(long hexcode);
	};
