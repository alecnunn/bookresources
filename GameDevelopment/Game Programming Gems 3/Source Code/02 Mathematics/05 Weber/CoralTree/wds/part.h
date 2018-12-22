/******************************************************************************

Coral Tree wds/part.h
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


#define WDS_PARTITION_FLAGS_NONE		0
#define WDS_PARTITION_FLAGS_LIMIT		1	// sum child limit to partition size
#define WDS_PARTITION_FLAGS_ATTACH_BIT	2	// last child widget attaches
											// to edge during repression
#define WDS_PARTITION_FLAGS_ATTACH		(WDS_PARTITION_FLAGS_LIMIT |		\
											WDS_PARTITION_FLAGS_ATTACH_BIT)

#define WDS_PARTITION_FLAGS_DEFAULT		WDS_PARTITION_FLAGS_LIMIT



/*****************************************************************************
class WDS_Partition : public WDS_Widget

A space divider widget
*****************************************************************************/
class WDS_Partition : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_Partition);

	private:

		long		registered;
		long		direction;
		long		reverse;

		// per-instance pseudo-static
		EW_Widget	*drag;		// child widget being dragged
								// using EW_Widget instead of EW_Widget
								// to allow casting of EW_WidgetList increment
		long		exclusive;	// when on, don't allow children to use events
		long		on_bar;		// dragging using a bar
		long		last_bar;	// on last bar for unlimited partitiion
		long		realmouse;	// store real mouse position
		long		last;		// last mouse position
		long		flags;

		long		DoOperate(EW_Event *event,long post);
		void		PreSizeCheck(EW_RectangleList *rectlist);
		void		PostSizeCheck(EW_RectangleList *rectlist);
		void		GetFixed(long *headmost,long *tailmost);

	public:
				WDS_Partition();

		void	Draw(EW_Redraw *redraw);
		long	Resize(EW_Resize *resize);
		long	Operate(EW_Event *event);
		long	PostOperate(EW_Event *event);

		void	RegisterMouse(long on);

		void	GetSeparatorWidth(long width[3])
					{
					width[0]=InterpretElement(WDS_STATE_0,
										WDS_ELEMENT_PARTITION_SEPARATOR);
					width[1]=InterpretElement(WDS_STATE_1,
										WDS_ELEMENT_PARTITION_SEPARATOR);
					width[2]=InterpretElement(WDS_STATE_ALL,
										WDS_ELEMENT_PARTITION_SEPARATOR);
					};

		long		GetFlags(void)				{ return flags; };
		void		SetFlags(long set)			{ flags = set; };
		void		SetFlagsIncluding(long set)	{ flags |= set; };
		void		SetFlagsExcluding(long set) { flags ^= (flags&set); };

		void	SetReverse(long set)	{ reverse=set; };
		long	GetReverse(void)		{ return reverse; };

		void	SetDirection(long dir)
					{
					direction=dir;
//					SetInversion(direction==WDS_PARTITION_VERTICAL);
					};
		long	GetDirection(void)		{ return direction; };

		EW_Widget *PostIncDecrement(long increment)
					{
					// NOTE: ignores invisible children

					EW_Widget *next;

					do
						{
						if(increment)
							next=GetWidgetList()->PostIncrement();
						else
							next=GetWidgetList()->PostDecrement();
						} while(next && next->GetInvisible());

					return next;
					};

		EW_Widget *SkipNext(EW_Widget *current,long previous)
					{
					// NOTE: ignores invisible children

					do
						{
						if(previous)
							current=(EW_Widget *)current->Prev();
						else
							current=(EW_Widget *)current->Next();
						} while(current && current->GetInvisible());

					return current;
					};
	};
