

#ifndef TESTRESULT_H
#define TESTRESULT_H


class Failure;

class TestResult
{
public:
    TestResult () : failureCount (0), testCount(0) {}
    virtual ~TestResult();
	virtual void testWasRun ();
	virtual void startTests ();
	virtual void addFailure (const Failure & failure);
	virtual void endTests ();

    int  getNumFailed() const { return failureCount; }

protected:
	int failureCount;
	int testCount;
};

#endif
