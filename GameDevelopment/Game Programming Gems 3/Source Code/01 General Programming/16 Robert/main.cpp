#include "setup.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
//
// Main
//
///////////////////////////////////////////////////////////////////////////////

void executeSetup();
void executeSample();

int main(int argc, char * argv[]) {
    FAST_TRACE;

    const char * message = "This sample uses the \"OutputDebugString\" Win32 API function to report events that are occuring in the "
                           "application. To see those reports, you can run the application from a debugger or use a monitoring tool "
                           "like the freeware application \"DebugView\" from Sysinternals.\n"
                           "\n"
                           "See: http://www.sysinternals.com."
                           "\n\n\n"
                           "When running, press 'H' to display the command pannel.";

    MessageBox(0, message, "Journaling Services", MB_ICONINFORMATION | MB_OK);

    GENERAL_REPORT("Setup of journaling services");
    executeSetup();

    GENERAL_REPORT("Executing sample");
    executeSample();

    return 0;
}
