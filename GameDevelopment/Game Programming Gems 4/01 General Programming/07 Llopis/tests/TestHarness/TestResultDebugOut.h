

#ifndef TESTRESULTDEBUGOUT_H
#define TESTRESULTDEBUGOUT_H


#include "TestResult.h"


class TestResultDebugOut : public TestResult
{
public:
	virtual void startTests ();
	virtual void addFailure (const Failure & failure);
	virtual void endTests ();
};

#endif
