/******************************************************************************

Coral Tree wds/button.h
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



/*****************************************************************************
class	WDS_Portal : public WDS_Widget

*****************************************************************************/
class WDS_Portal : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_Portal);

	public:
					WDS_Portal();
virtual				~WDS_Portal()	{};

virtual	long		Operate(EW_Event *event);
virtual	void		Draw(EW_Redraw *redraw);

		void		DrawFigure(void);

	private:
		float		angle;
	};
