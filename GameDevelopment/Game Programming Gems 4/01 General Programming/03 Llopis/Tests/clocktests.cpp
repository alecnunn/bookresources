#include "TestHarness/TestHarness.h"
#include "clock.h"
#include "TimeSourceHRWin32.h"
#include "TimeSourceCustom.h"
#include "Observer.h"

#include <windows.h>


TEST (ClockTests, PassageOfTime)
{
    Clock clock(new TimeSourceHRWin32);

    // Should start at 0
    CHECK_DOUBLES_EQUAL (0, clock.GetTime());

    ::Sleep(100);
    clock.FrameStep();
    double t1 = clock.GetTime();
    CHECK (t1 > 0.08 && t1 < 0.12);

    // Any other GetTimes this frame should return the same value
    double t1b = clock.GetTime ();
    CHECK_DOUBLES_EQUAL (t1, t1b);

    clock.FrameStep();
    double t2 = clock.GetTime();
    CHECK (t2 > t1);
}

 

TEST (ClockTests, FrameTime)
{
    Clock clock(new TimeSourceHRWin32);

    // Should start at 0
    CHECK_DOUBLES_EQUAL (0, clock.GetTime());
    CHECK_DOUBLES_EQUAL (0.03, clock.GetFrameDuration ());

    ::Sleep(100);
    clock.FrameStep();
    double t1 = clock.GetTime();
    CHECK_DOUBLES_EQUAL (t1, clock.GetFrameDuration ());

    clock.FrameStep();
    double t2 = clock.GetTime();
    CHECK_DOUBLES_EQUAL (t2-t1, clock.GetFrameDuration ());
}


class UpdateTest : public IObserver
{
public:
    UpdateTest() { m_bUpdated = false; }
    virtual void Notify() { m_bUpdated = true; }

    bool m_bUpdated;
};


TEST (ClockTests, Update)
{
    Clock clock;
    UpdateTest test1;
    UpdateTest test2;
    clock.FrameStep();
    CHECK (!test1.m_bUpdated);
    CHECK (!test2.m_bUpdated);

    clock.AddObserver(&test1);
    clock.AddObserver(&test2);
    CHECK (!test1.m_bUpdated);
    CHECK (!test2.m_bUpdated);
    
    clock.FrameStep();
    CHECK (test1.m_bUpdated);
    CHECK (test2.m_bUpdated);
}


class TimeSourceDeleteTest : public TimeSource
{
public:
    virtual ~TimeSourceDeleteTest() { s_bDeleted = true; }
    virtual double GetTime() const { return 0; }

    static bool s_bDeleted;
};

bool TimeSourceDeleteTest::s_bDeleted = false;


TEST (ClockTests, TimeSourceDeleted)
{
    Clock * pClock = new Clock(new TimeSourceDeleteTest);
    CHECK (TimeSourceDeleteTest::s_bDeleted == false);
    delete pClock;
    CHECK (TimeSourceDeleteTest::s_bDeleted == true);
}



class ClockSignalSetup : public TestSetup
{
public:
    Clock * pClock;

    void setup()
    {
        TimeSourceCustom * pTS = new TimeSourceCustom;
        pTS->QueueStep (0.000);
        pTS->QueueStep (0.020);
        pTS->QueueStep (0.023);
        pTS->QueueStep (0.028);
        pTS->QueueStep (0.029);
        pTS->QueueStep (0.032);
        pTS->QueueStep (0.044);
        pTS->QueueStep (0.024);
        pClock = new Clock(pTS);
    }

    void teardown()
    {
        delete pClock;        
    }
};


TESTFULL (ClockTests, NormalTime, ClockSignalSetup)
{
    CHECK_DOUBLES_EQUAL(0, pClock->GetTime());
    CHECK_DOUBLES_EQUAL(0.03, pClock->GetFrameDuration());
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(0.020, pClock->GetTime());
    CHECK_DOUBLES_EQUAL(0.020, pClock->GetFrameDuration());
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(0.043, pClock->GetTime());
    CHECK_DOUBLES_EQUAL(0.023, pClock->GetFrameDuration());
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(0.071, pClock->GetTime());
    CHECK_DOUBLES_EQUAL(0.028, pClock->GetFrameDuration());
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(0.100, pClock->GetTime());
    CHECK_DOUBLES_EQUAL(0.029, pClock->GetFrameDuration());
}



TESTFULL (ClockTests, NumFrames, ClockSignalSetup)
{
    CHECK_LONGS_EQUAL (0, pClock->GetFrameNumber());
    pClock->FrameStep ();
    CHECK_LONGS_EQUAL (1, pClock->GetFrameNumber());
    pClock->FrameStep ();
    pClock->FrameStep ();
    pClock->FrameStep ();
    CHECK_LONGS_EQUAL (4, pClock->GetFrameNumber());
}



TESTFULL (ClockTests, FrameRate, ClockSignalSetup)
{
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(1/0.020, pClock->GetFrameRate());
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(1/0.023, pClock->GetFrameRate());
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(1/0.028, pClock->GetFrameRate());
}




TESTFULL (ClockTests, FilteredTime, ClockSignalSetup)
{
    pClock->SetFiltering(4, 0.03);
    CHECK_DOUBLES_EQUAL(0.030, pClock->GetFrameDuration());
    CHECK_DOUBLES_EQUAL(0, pClock->GetTime());

    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(0.025, pClock->GetFrameDuration());
    CHECK_DOUBLES_EQUAL(0.025, pClock->GetTime());

    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(0.024333, pClock->GetFrameDuration());
    CHECK_DOUBLES_EQUAL(0.049333, pClock->GetTime());

    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(0.02525, pClock->GetFrameDuration());
    CHECK_DOUBLES_EQUAL(0.074583, pClock->GetTime());

    // This one is not averaged out with the first estimate
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(0.025, pClock->GetFrameDuration());
    CHECK_DOUBLES_EQUAL(0.099583, pClock->GetTime());

    // This one should only averate frames 1-4 (with 0 index)
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(0.028, pClock->GetFrameDuration());
    CHECK_DOUBLES_EQUAL(0.127583, pClock->GetTime());

    // This one should only averate frames 2-5 (with 0 index)
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL(0.03325, pClock->GetFrameDuration());
    CHECK_DOUBLES_EQUAL(0.160833, pClock->GetTime());
}


TEST (ClockTests, StepLimit)
{
    TimeSourceCustom * pTS = new TimeSourceCustom;
    pTS->QueueStep (0.000);
    pTS->QueueStep (0.020);
    pTS->QueueStep (0.500);
    pTS->QueueStep (0.022);
    Clock clock(pTS);

    clock.FrameStep();
    CHECK_DOUBLES_EQUAL (0.02, clock.GetFrameDuration());
    CHECK_DOUBLES_EQUAL (0.02, clock.GetTime());

    // This step is too big, so we use the same duration as the previous frame
    clock.FrameStep();
    CHECK_DOUBLES_EQUAL (0.02, clock.GetFrameDuration());
    CHECK_DOUBLES_EQUAL (0.04, clock.GetTime());

    clock.FrameStep();
    CHECK_DOUBLES_EQUAL (0.022, clock.GetFrameDuration());
    CHECK_DOUBLES_EQUAL (0.062, clock.GetTime());
}
