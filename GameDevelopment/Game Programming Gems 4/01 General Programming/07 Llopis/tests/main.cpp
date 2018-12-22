
#include "TestHarness/TestHarness.h"
#include "TestHarness/TestResultStdErr.h"

int main()
{
    TestResultStdErr tr;
	TestRegistry::runAllTests(tr);
	return tr.getNumFailed();

}

