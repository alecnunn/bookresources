#ifndef SETUP__HEADER
#define SETUP__HEADER

#include "journaling.h"

///////////////////////////////////////////////////////////////////////////////
//
// Journaling services
//
///////////////////////////////////////////////////////////////////////////////

DECLARE_SWITCH(FastTraceOfRoot, Root);
DECLARE_SWITCH(SlowTraceOfRoot, Root);
DECLARE_SWITCH(FastAssertOfRoot, Root);
DECLARE_SWITCH(SlowAssertOfRoot, Root);
DECLARE_SWITCH(ReportOfRoot, Root);

DECLARE_SWITCHBOX(Debug, Root);
    DECLARE_SWITCH(ReportOfDebug, Debug);

DECLARE_SWITCHBOX(Graphic, Root);
    DECLARE_SWITCH(ReportOfGraphic, Graphic);

    DECLARE_SWITCHBOX(Animation, Graphic);
        DECLARE_SWITCH(ReportOfAnimation, Animation);

DECLARE_SWITCHBOX(Window, Root);
    DECLARE_SWITCH(ReportOfWindow, Window);

    DECLARE_SWITCHBOX(Events, Window);
        DECLARE_SWITCH(ReportOfEvents, Window);

DECLARE_SWITCH(DummyUsageAssert, Root);

///////////////////////////////////////////////////////////////////////////////
//
// Helpers
//
///////////////////////////////////////////////////////////////////////////////

#define FAST_TRACE TRACE(FastTraceOfRoot)
#define SLOW_TRACE TRACE(SlowTraceOfRoot)
#define FAST_ASSERT(Expression) ASSERT(FastAssertOfRoot, Expression)
#define SLOW_ASSERT(Expression) ASSERT(SlowAssertOfRoot, Expression)

#define DUMMY_ASSERT(Expression) ASSERT(DummyUsageAssert, Expression)

#define GENERAL_REPORT(Expression) REPORT(ReportOfRoot, "[General] - " << Expression)
#define DEBUG_REPORT(Expression) REPORT(ReportOfDebug, "[Debug] - " << Expression)
#define GRAPHIC_REPORT(Expression) REPORT(ReportOfGraphic, "[Graphic] - " << Expression)
#define ANIMATION_REPORT(Expression) REPORT(ReportOfAnimation, "[Animation] - " << Expression)
#define WINDOW_REPORT(Expression) REPORT(ReportOfWindow, "[Window] - " << Expression)
#define EVENTS_REPORT(Expression) REPORT(ReportOfEvents, "[Events] - " << Expression)

///////////////////////////////////////////////////////////////////////////////
//
// Journals
//
///////////////////////////////////////////////////////////////////////////////

extern Journal * messageBox;

#endif
