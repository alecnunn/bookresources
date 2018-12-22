
/******************************************************************************

Coral Tree wds/getnum.h
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


#define WDS_MAXNUMBERSTRINGLEN 	256




/******************************************************************************
class	WDS_GetNumber : public WDS_GetString

Keywords:
******************************************************************************/
class WDS_GetNumber : public WDS_GetString
{
	GPL_OBJECT_INTERFACE(WDS_GetNumber);
	private:
//		void	(*number_callback)(WDS_Widget *, double);
		double	number;

	public:
				WDS_GetNumber(void);

		long 	Operate(EW_Event *event);

		void	SetNumber(double a_double)
					{
					number = a_double;
					RecognizeNumberChange();
					}
		double	GetNumber(void)				{ return number; }

//		void	DeActivate(void)
//					{
//					RecognizeStringChange();
//					WDS_GetString::DeActivate();
//					};

		void	RecognizeStringChange(void);
		void	RecognizeNumberChange(void);

static	double	InterpretStringAsDouble(char *out_string,long position,
													long value,char *string);

static	void	DoubleToString(char *string,double value)
					{
					sprintf(string,"%.16G",value);
					};
static	double	StringToDouble(char *string)
					{
					return InterpretStringAsDouble(NULL,0,0,string);
					};

};
