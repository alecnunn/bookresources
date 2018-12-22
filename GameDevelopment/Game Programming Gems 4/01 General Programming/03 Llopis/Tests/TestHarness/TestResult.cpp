
#include "TestResult.h"
#include "Failure.h"

#include <iostream>
#include <sstream>
#include <windows.h>


TestResult::~TestResult()
{
}

void TestResult::testWasRun()
{
	testCount++;
}

void TestResult::startTests () 
{
}

void TestResult::addFailure (const Failure & ) 
{
	failureCount++;
}

void TestResult::endTests () 
{
}
