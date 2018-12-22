#include "setup.h"
#include "messagebox_journal.h"

///////////////////////////////////////////////////////////////////////////////
//
// Journaling services
//
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SWITCH(FastTraceOfRoot,   "Fast Trace",
                                    "General trace information");

IMPLEMENT_SWITCH(SlowTraceOfRoot,   "Slow Trace",
                                    "More precise (and more complete) trace information");

IMPLEMENT_SWITCH(FastAssertOfRoot,  "Fast Assert",
                                    "General assertions");

IMPLEMENT_SWITCH(SlowAssertOfRoot,  "Slow Assert",
                                    "Performance killer assertions");

IMPLEMENT_SWITCH(ReportOfRoot,      "Report",
                                    "General reports");

IMPLEMENT_SWITCHBOX(Debug,          "Debug",
                                    "The debug module");

IMPLEMENT_SWITCH(ReportOfDebug,     "Report",
                                    "General debugging controls reports");

IMPLEMENT_SWITCHBOX(Graphic,        "Graphic",
                                    "The graphical module");

IMPLEMENT_SWITCH(ReportOfGraphic,   "Report",
                                    "General graphical and OpenGL reports");

IMPLEMENT_SWITCHBOX(Animation,      "Animation",
                                    "The graphical animation submodule");

IMPLEMENT_SWITCH(ReportOfAnimation, "Report",
                                    "General animation reports");

IMPLEMENT_SWITCHBOX(Window,         "Window",
                                    "The windowing system");

IMPLEMENT_SWITCH(ReportOfWindow,    "Report",
                                    "General windowing setup and associated events reports");

IMPLEMENT_SWITCHBOX(Events,         "Events",
                                    "The random window event submodule");

IMPLEMENT_SWITCH(ReportOfEvents,    "Report",
                                    "Window events reports");

IMPLEMENT_SWITCH(DummyUsageAssert,  "Dummy Assert",
                                    "Needed for dummy assertions so that the switch is always on");

///////////////////////////////////////////////////////////////////////////////
//
// Setup
//
// This function will setup initial parameters for journaling services. Before
// initialization, all reports are activated and sent to the debugger.
//
///////////////////////////////////////////////////////////////////////////////

void executeSetup() {
    FAST_TRACE;

    // New message box journal
    messageBox = new MessageBoxJournal;
    messageBox->addReference();

    // New settings
    FastTraceOfRoot::getInstance()->setState(false);
    SlowTraceOfRoot::getInstance()->setState(false);
    ReportOfAnimation::getInstance()->setState(false);

    FastAssertOfRoot::getInstance()->setJournal(messageBox);
    SlowAssertOfRoot::getInstance()->setParameters(false, messageBox);

    DummyUsageAssert::getInstance()->setJournal(messageBox);

    // Dump the resulting settings.
    std::ofstream file("tree.txt");
    if(file) {
        Journaling::printOut(file);
    }
}

Journal * messageBox;