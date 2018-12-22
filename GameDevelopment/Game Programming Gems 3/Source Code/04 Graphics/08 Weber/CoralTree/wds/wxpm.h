/******************************************************************************

Coral Tree wds/wxpm.h
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



#define WDS_TEST_ICONS			100
#define	WDS_TEST_ICON_ROTATE	FALSE
#define	WDS_TEST_ICON_MULT		(WDS_TEST_ICON_ROTATE?5:1)
#define WDS_TEST_ICON_BUTTONS	(WDS_TEST_ICON_MULT*WDS_TEST_ICONS)

#include "xpmdef.h"



/******************************************************************************
class	WDS_IconTestForm

******************************************************************************/
class WDS_IconTestForm : public WDS_Form
	{
	private:
		WDS_Button	button[WDS_TEST_ICON_BUTTONS];
		WDS_Image	icon[WDS_TEST_ICON_BUTTONS];

	public:
					WDS_IconTestForm(void);
virtual				~WDS_IconTestForm(void);
	};
