/******************************************************************************

Coral Tree wds/wxpm.cc
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



#include "wds.h"
#include "xpminc.h"



/******************************************************************************
char	*wdsGetBuiltinXPM(long index)

******************************************************************************/
/*PROTOTYPE*/
char **wdsGetBuiltinXPM(long index)
	{
	switch(index)
		{
#include "xpmcases.h"
		}

	return NULL;
	}



/******************************************************************************
	WDS_IconTestForm::WDS_IconTestForm(void)

******************************************************************************/
WDS_IconTestForm::WDS_IconTestForm(void)
	{
	SetMode(WDS_FORM_ROW);
	SetSizing(WDS_HORIZONTAL,WDS_SIZING_MANUAL);

	long m,n;
	for(m=0;m<WDS_TEST_ICONS;m++)
		for(n=0;n<WDS_TEST_ICON_MULT;n++)
			{
			long y=m*WDS_TEST_ICON_MULT+n;

			icon[y].ReadFromBufferXPM(wdsGetBuiltinXPM(m+1));

			switch(n)
				{
				case 1:
					icon[y].Rotate(TRUE);
					break;
				case 2:
					icon[y].Rotate(FALSE);
					break;
				case 3:
					icon[y].Flip(WDS_HORIZONTAL);
					break;
				case 4:
					icon[y].Flip(WDS_VERTICAL);
					break;
				}

			button[y].SetImage(WDS_STATE_ALL,&icon[y]);
			button[y].SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE);
			AddWidget(&button[y]);
			}
	}



/******************************************************************************
WDS_IconTestForm::~WDS_IconTestForm(void)

******************************************************************************/
WDS_IconTestForm::~WDS_IconTestForm(void)
	{
	long m;
	for(m=0;m<WDS_TEST_ICONS;m++)
		RemoveWidget(&button[m]);
	}
