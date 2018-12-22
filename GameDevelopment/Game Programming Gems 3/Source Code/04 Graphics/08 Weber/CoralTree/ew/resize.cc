/******************************************************************************

Coral Tree ew/resize.cc
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

#include "ew.h"



/******************************************************************************
void EW_Resize::Print(long hexcode)

Keywords:
******************************************************************************/
void EW_Resize::Print(long hexcode)
    {
    char message[200];

    sprintf(message,"RESIZE:  min=%d,%d max=%d,%d pref=%d,%d this=0x%x code=0x%x",
                            (int)minx,(int)miny,(int)maxx,(int)maxy,(int)prefx,(int)prefy,(int)this,(int)hexcode);
    EW_PRINT(EW_WIDGET,EW_LOG,message);
    }

