/******************************************************************************

Coral Tree wds/form.h
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

// don't use SetBounds(EW_BOUNDS_FILL_PARENT)
// forms use their own resizing

// fixed sizing based on SetPrefSize(), not SetGeometry()
// modes other than WDS_SIZING_FIXED can change the pref size


// WDS_Form:SetMode(???)
#define	WDS_FORM_MANUAL			0
#define	WDS_FORM_SINGLE			1
#define	WDS_FORM_COLUMN			2
#define	WDS_FORM_SINGLE_COLUMN	(WDS_FORM_SINGLE|WDS_FORM_COLUMN)
#define	WDS_FORM_ROW			4
#define	WDS_FORM_SINGLE_ROW		(WDS_FORM_SINGLE|WDS_FORM_ROW)

// WDS_Form:SetCentering(???)
#define	WDS_FORM_CENTER_NONE	0
#define	WDS_FORM_CENTER_X		1
#define	WDS_FORM_CENTER_Y		2
#define	WDS_FORM_CENTER_XY		(WDS_FORM_CENTER_X|WDS_FORM_CENTER_Y)

// WDS_Form:SetSelectability(???)
#define	WDS_SELECT_NONE			0
#define	WDS_SELECT_SINGLE		1
#define	WDS_SELECT_EXCLUSIVE	2

// WDS_Form:SetSizing(dir,???)		(x,y independant)
#define	WDS_SIZING_FIXED		0
#define	WDS_SIZING_MANUAL		1
#define	WDS_SIZING_FIT_CHILDREN	2

// WDS_Form:SetUniformity(???)
#define	WDS_UNIFORM_NONE	0
#define	WDS_UNIFORM_X		1
#define	WDS_UNIFORM_Y		2
#define	WDS_UNIFORM_BOTH	(WDS_UNIFORM_X|WDS_UNIFORM_Y)


/*****************************************************************************
class WDS_Form : public WDS_Widget

A general purpose widget placement form
*****************************************************************************/
class WDS_Form : public WDS_Widget
	{
	WDS_DECLARE_TYPE(WDS_Form);

	private:

		WDS_Widget	*selection;

		long		centering;
		long		clear;
		long		uniform;
		long		mode;
		long		sizing[2];
		long		selectability;

		void		RegisterExclusivity(long on);

	public:
					WDS_Form();

		void		Draw(EW_Redraw *redraw);
		long		Operate(EW_Event *event);
		long		Resize(EW_Resize *resize);

		void		SetMode(long newmode)			{ mode=newmode; };
		long		GetMode(void)					{ return mode; };

		void		SetCentering(long set)			{ centering=set; };
		long		GetCentering(void)				{ return centering; };

		void		SetClear(long set)				{ clear=set; };
		long		GetClear(void)					{ return clear; };

		void		SetUniformity(long set)			{ uniform=set; };
		long		GetUniformity(void)				{ return uniform; };

		long		GetSelectability(void)			{ return selectability; }
		void		SetSelectability(long set)
						{
						selectability=set;

						// turn on clipping if selectable
						// (but don't automatically turn it off)
						if(selectability && GetClipping()!=EW_CLIPPING_ALL)
							SetClipping(EW_CLIPPING_ALL);
						};

		void		SetSizing(long dir,long val)	{ sizing[dir%2]=val; };
		long		GetSizing(long dir)				{ return sizing[dir%2]; }

		void		SetSelection(WDS_Widget *set);
		WDS_Widget	*GetSelection(void)				{ return selection; }
	};
