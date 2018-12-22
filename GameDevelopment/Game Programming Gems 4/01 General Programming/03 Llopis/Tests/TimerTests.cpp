#include "TestHarness/TestHarness.h"
#include "Timer.h"
#include "Clock.h"
#include "TimeSourceCustom.h"


class TimerSetup : public TestSetup
{
public:
    Clock * pClock;
    Timer * pTimer;

    void setup()
    {
        TimeSourceCustom * pTS = new TimeSourceCustom;
        pTS->QueueStep (0.2);
        pTS->QueueStep (0.2);
        pTS->QueueStep (0.2);
        pTS->QueueStep (0.2);
        pClock = new Clock(pTS);
        pTimer = new Timer(*pClock);
    }

    void teardown()
    {
        delete pTimer;
        delete pClock;        
    }
};



TESTFULL (TimerTests, PassageOfTime, TimerSetup)
{
    CHECK_DOUBLES_EQUAL (0, pClock->GetTime());
    CHECK_DOUBLES_EQUAL (0, pTimer->GetTime());
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL (0.2, pClock->GetTime());
    CHECK_DOUBLES_EQUAL (0.2, pTimer->GetTime());
    pClock->FrameStep();
    CHECK_DOUBLES_EQUAL (0.4, pClock->GetTime());
    CHECK_DOUBLES_EQUAL (0.4, pTimer->GetTime());
}


TESTFULL (TimerTests, FrameTime, TimerSetup)
{
    CHECK_DOUBLES_EQUAL (0, pTimer->GetFrameDuration());
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL (0.2, pTimer->GetFrameDuration());
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL (0.2, pTimer->GetFrameDuration());
}


TESTFULL (TimerTests, Scale, TimerSetup)
{
    CHECK_DOUBLES_EQUAL (0, pClock->GetTime());
    CHECK_DOUBLES_EQUAL (0, pTimer->GetTime());
    CHECK_DOUBLES_EQUAL (1, pTimer->GetScale());

    pTimer->SetScale(1.5f);
    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL (0.2, pClock->GetTime());
    CHECK_DOUBLES_EQUAL (0.3, pTimer->GetTime());
    CHECK_DOUBLES_EQUAL (1.5, pTimer->GetScale());

    pTimer->SetScale(0.2f);
    pClock->FrameStep();
    CHECK_DOUBLES_EQUAL (0.4, pClock->GetTime());
    CHECK_DOUBLES_EQUAL (0.34, pTimer->GetTime());
    CHECK_DOUBLES_EQUAL (0.2, pTimer->GetScale());
}


TESTFULL (TimerTests, Pause, TimerSetup)
{
    CHECK_DOUBLES_EQUAL (0, pClock->GetTime());
    CHECK_DOUBLES_EQUAL (0, pTimer->GetTime());
    CHECK (pTimer->IsPaused () == false);

    pClock->FrameStep ();
    CHECK_DOUBLES_EQUAL (0.2, pClock->GetTime());
    CHECK_DOUBLES_EQUAL (0.2, pTimer->GetTime());

    pTimer->Pause(true);
    pClock->FrameStep();
    CHECK_DOUBLES_EQUAL (0.4, pClock->GetTime());
    CHECK_DOUBLES_EQUAL (0.2, pTimer->GetTime());
    CHECK (pTimer->IsPaused () == true);

    pTimer->Pause(false);
    pClock->FrameStep();
    CHECK_DOUBLES_EQUAL (0.6, pClock->GetTime());
    CHECK_DOUBLES_EQUAL (0.4, pTimer->GetTime());
    CHECK (pTimer->IsPaused () == false);
}
