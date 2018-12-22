#include "TestHarness/TestHarness.h"
#include "TimeSourceHRWin32.h"
#include "TimeSourceCustom.h"
#include <windows.h>


TEST (TimeSources, HRWin32)
{
    TimeSourceHRWin32 ts;
    double t1 = ts.GetTime();
    double t2 = ts.GetTime();
    CHECK (t2 > t1);

    t1 = ts.GetTime();
    ::Sleep(200);
    t2 = ts.GetTime();
    double delta = t2 - t1;
    CHECK (delta > 0.150 && delta < 0.250);
}
 

TEST (TimeSources, CustomQueueTime)
{
    TimeSourceCustom ts;
    CHECK_DOUBLES_EQUAL (0, ts.GetTime());
    CHECK_DOUBLES_EQUAL (0, ts.GetTime());

    ts.QueueTime(0.5);
    CHECK_DOUBLES_EQUAL (0.5, ts.GetTime());
    CHECK_DOUBLES_EQUAL (0, ts.GetTime());

    ts.QueueTime(0.5);
    ts.QueueTime(0.8);
    ts.QueueTime(1.3);
    CHECK_DOUBLES_EQUAL (0.5, ts.GetTime());
    CHECK_DOUBLES_EQUAL (0.8, ts.GetTime());
    CHECK_DOUBLES_EQUAL (1.3, ts.GetTime());
    CHECK_DOUBLES_EQUAL (0, ts.GetTime());
}
 

TEST (TimeSources, CustomQueueStep)
{
    TimeSourceCustom ts;
    ts.QueueTime(10);
    ts.QueueStep(0.32);
    ts.QueueStep(0.23);
    ts.QueueStep(0.54);
    ts.QueueStep(0.21);

    CHECK_DOUBLES_EQUAL (10, ts.GetTime());
    CHECK_DOUBLES_EQUAL (10.32, ts.GetTime());
    CHECK_DOUBLES_EQUAL (10.55, ts.GetTime());
    CHECK_DOUBLES_EQUAL (11.09, ts.GetTime());
    CHECK_DOUBLES_EQUAL (11.30, ts.GetTime());
    CHECK_DOUBLES_EQUAL (0, ts.GetTime());

}