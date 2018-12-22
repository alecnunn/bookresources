/******************************************************************************

Coral Tree ew/redraw.h
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
class  EW_Redraw

******************************************************************************/
class EW_Redraw
	{
	private:
		long				mx,my;			// mouse location
		long				force;			// force a full redraw
		long				limited;		// pre-clipped
		EW_Tablet			*tablet;		// Drawing tablet (e.g., PostScript)

	public:
				EW_Redraw()
					{
					mx= -1;
					my= -1;
					force=FALSE;
					limited=FALSE;
					tablet=(EW_Tablet*)NULL;
					};

		void	SetForce(long set)		{ force=set; };
		long	GetForce(void)			{ return force; };

		void	SetLimited(long set)	{ limited=set; };
		long	GetLimited(void)		{ return limited; };

		void	SetMousePosition(long x,long y)		{ mx=x; my=y; };
		void	GetMousePosition(long *x,long *y)	{ *x=mx; *y=my; };

		void		SetTablet(EW_Tablet *t)	{ tablet=t; };
		EW_Tablet	*GetTablet(void)		{ return tablet; };
	};
