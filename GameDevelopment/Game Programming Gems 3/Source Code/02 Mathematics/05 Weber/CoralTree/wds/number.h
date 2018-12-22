/******************************************************************************

Coral Tree wds/number.h
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
WDS_NumberEntry

This widget contains 2 specialized button widgets for incrementing and
decrementing a value.  The value itself is a float, and is registered with
the widget with SetNumber(float *a_float).  This widget also contains
a WDS_GetString widget which is where numbers are typed in.

This widget should be set up with the SetUp function.  SetGeometry and 
Attach should be avoided.

   ----------------------------------
   | label  | entry field | up | dn |
   ----------------------------------

*****************************************************************************/



/******************************************************************************
class	WDS_IncrButton : public WDS_Button

******************************************************************************/
class WDS_IncrButton : public WDS_Button
{
	GPL_OBJECT_INTERFACE(WDS_IncrButton);

	public:

				WDS_IncrButton(){SetDisplayFunction(wdsDfUpArrow);}

virtual	void	ChangeState(long newstate,EW_Event *event);

};



/******************************************************************************
class	WDS_DecrButton : public WDS_Button

******************************************************************************/
class WDS_DecrButton : public WDS_Button
{
	GPL_OBJECT_INTERFACE(WDS_DecrButton);

	public:
				WDS_DecrButton(){SetDisplayFunction(wdsDfDownArrow);}

virtual	void	ChangeState(long newstate,EW_Event *event);
};



/******************************************************************************
class	WDS_NumberEntry : public WDS_Widget

******************************************************************************/
class WDS_NumberEntry : public WDS_Widget
{
	GPL_OBJECT_INTERFACE(WDS_NumberEntry);

	private:
		WDS_GetString	string_entry;
		WDS_IncrButton	incr_button;
		WDS_DecrButton	decr_button;
		WDS_String	label;
		float			stepsize;
		float			*number;
		long			labelwidth;

	public:
		WDS_NumberEntry();
		void Draw(EW_Redraw *redraw);
		void SetUp(long x,long y,long w,long h, long field_w, long button_w);
		void SetLabel(char *a_string);
		void SetStepSize(float a_stepsize){stepsize = a_stepsize;}
		void SetNumber(float *a_float){number = a_float;}
		void SetNumberValue(float a_float){*number = a_float;}
		float *GetNumber(){return number;}
		void Increment();
		void Decrement();
};
