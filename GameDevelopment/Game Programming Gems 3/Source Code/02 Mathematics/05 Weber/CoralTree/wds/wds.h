/******************************************************************************

Coral Tree wds/wds.h
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


/* Widget Set for Event Window system */

#ifndef __WDS_H__
#define __WDS_H__


// generic direction flag (see partition, scroll region/bar)
#define WDS_HORIZONTAL		EW_HORIZONTAL
#define WDS_VERTICAL		EW_VERTICAL


// turn on multi-colored test backgrounds
#define WDS_COLOR_FILL		FALSE
#define WDS_COLOR_WIRE		FALSE

// little coloring for clarity (don't get carried away)
#define WDS_COLOR_ENHANCE	TRUE

#define WDS_SMART_REDRAW	TRUE

// internal timers (only a request to clarify debugging)
#define	WDS_TIMER_ID_TIP	7
#define	WDS_TIMER_ID_POPUP	8


class WDS_Widget;
class WDS_TipWindow;
class WDS_TieList;
class WDS_TieValues;
class WDS_Scope;
class WDS_String;
class WDS_StringList;

typedef long WDS_Typecode;

#include <ew/ew.h>

#include <wds/wds_proto.h>

#include <wds/cond.h>
#include <wds/help.h>
#include <wds/bevel.h>
#include <wds/wstr.h>
#include <wds/look.h>
#include <wds/image.h>
#include <wds/wwidget.h>
#include <wds/tie.h>
#include <wds/form.h>
#include <wds/scope.h>
#include <wds/button.h>
#include <wds/portal.h>
#include <wds/getstr.h>
#include <wds/pentry.h>
#include <wds/number.h>
#include <wds/part.h>
#include <wds/scroll.h>
#include <wds/popup.h>
#include <wds/menu.h>
#include <wds/getnum.h>
#include <wds/gfxtest.h>
#include <wds/hier.h>
#include <wds/htest.h>
#include <wds/hfile.h>
#include <wds/textlist.h>
#include <wds/mfd.h>
#include <wds/divider.h>
#include <wds/pick.h>
#include <wds/table.h>
#include <wds/document.h>
#include <wds/relay.h>
#include <wds/tip.h>
#include <wds/wxpm.h>
#include <wds/hardlook.h>
#include <wds/wba_wds.h>


#endif /* __WDS_H__ */
