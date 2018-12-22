/******************************************************************************

Coral Tree ew/region.h
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



#define	EW_INTERSECT_PENDING	(-1)	// never outside
										// EW_RectangleList::Intersect()
#define	EW_INTERSECT_NONE		0
#define	EW_INTERSECT_PARTIAL	1
#define	EW_INTERSECT_COMPLETE	2



/******************************************************************************
class	EW_Rectangle

******************************************************************************/
class EW_Rectangle
	{
	protected:
		long            position[2];    // lower left corner
		long            dimension[2];   // dimensions

	public:
				EW_Rectangle(void);
virtual			~EW_Rectangle(void) {};

		void    GetGeometry(long *x,long *y,long *w,long *h)
					{
					GetOrigin(x,y);
					GetSize(w,h);
					};
virtual void    SetGeometry(long x, long y, long w, long h)
					{
					// origin last 'cause widgets add some functionality to it
					SetSize(w,h);
					SetOrigin(x,y);
					};

		void    GetOrigin(long *x,long *y)  { *x=position[0]; *y=position[1]; };
virtual void    SetOrigin(long x,long y)
					{
					position[0] = x;
					position[1] = y;
					}

		void    GetSize(long *w,long *h) { *w=dimension[0]; *h=dimension[1]; };
virtual void    SetSize(long w,long h)
					{
					dimension[0] = w;
					dimension[1] = h;
					};

		void	CopyFrom(EW_Rectangle *other)
					{
					other->GetGeometry(&position[0],&position[1],&dimension[0],&dimension[1]);
					};
	};



/******************************************************************************
class	EW_RectangleList : public GPL_DoubleList<EW_Rectangle>

******************************************************************************/
class EW_RectangleList : public GPL_DoubleList<EW_Rectangle>
	{
public:
				EW_RectangleList(void) {};
virtual			~EW_RectangleList(void);

		void	Reset(void)	{ GPL_DoubleList<EW_Rectangle>::Reset(); };
		void	CopyFrom(EW_RectangleList *other);

		void	AddRectangle(long x,long y,long w,long h);
		void	DeleteRectangles(void);
		void	Prune(void);
		void	Draw(EW_Window *window,long colorindex);

		void	GetBounds(long *x,long *y,long *w,long *h);
		long	Intersect(EW_Rectangle *other,EW_RectangleList *intersection);
	};
