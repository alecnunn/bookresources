

#ifndef TESTRESULTSTDERR_H
#define TESTRESULTSTDERR_H


#include "TestResult.h"

class Failure;

class TestResultStdErr : public TestResult
{
public:
	virtual void addFailure (const Failure & failure);
	virtual void endTests ();
};

#endif
