
#include "TestResultStdErr.h"
#include "Failure.h"

#include <iostream>
#include <sstream>
#include <windows.h>


void TestResultStdErr::addFailure (const Failure & failure) 
{
    TestResult::addFailure(failure);
    std::cerr << failure << std::endl;
}

void TestResultStdErr::endTests () 
{
	std::cerr << testCount << " tests run" << std::endl;
	if (failureCount > 0)
		std::cerr << "******* There were " << failureCount << " failures" << std::endl;
	else
		std::cerr << "There were no test failures" << std::endl;
}
