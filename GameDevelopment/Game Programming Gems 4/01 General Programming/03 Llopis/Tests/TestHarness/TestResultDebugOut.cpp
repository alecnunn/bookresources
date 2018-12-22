
#include "TestResultDebugOut.h"
#include "Failure.h"

#include <iostream>
#include <sstream>
#include <windows.h>



void TestResultDebugOut::startTests () 
{
    OutputDebugString("\nRunning unit tests...\n");
}

void TestResultDebugOut::addFailure (const Failure & failure) 
{
    TestResult::addFailure(failure);

    std::ostringstream oss;
    oss << failure << std::endl;
    OutputDebugString(oss.str().c_str());
}

void TestResultDebugOut::endTests () 
{
    std::ostringstream oss;
	oss << testCount << " tests run" << std::endl;
	if (failureCount > 0)
        oss << "**** There were " << failureCount << " failures" << std::endl << std::endl;
	else
		oss << "There were no test failures" << std::endl;
    OutputDebugString(oss.str().c_str());
}
