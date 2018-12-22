
#include "EventLogger.h"

// For this simple logging test, we'll define two HTML styles for our
// logging output.
#define LOG_TESTCLASS	LOG_COLOR_RED | LOG_BOLD
#define LOG_TESTFN		LOG_COLOR_DK_BLUE | LOG_ITALICS | LOG_UNDERLINE

using namespace logger;

// Nonsense test class
class TestClass
{
public:
	TestClass()
	{
		FN("TestClass()");
		LOG(LOG_TESTCLASS, "Created new TestClass object");
	}
	~TestClass()
	{
		FN("~TestClass()");
		LOG(LOG_TESTCLASS, "Destroyed TestClass object");
	}
	void DoSomething()
	{
		FN("TestClass::DoSomething()");
		LOG(LOG_TESTCLASS, "Function is doing something...");
	}
};

// Nonsense test function designed to intermittently create
// and call member functions of a test class.
void TestFunction(int i)
{
	FN("TestFunction()");
	if(i % 17 == 0)
	{
		LOG(LOG_TESTFN, "Nonsense condition of %d /% 17 == true!", i);
		TestClass obj;	
		if(i % 3 == 0)
			obj.DoSomething();
	}
}

void main()
{
	LOG_INIT("TestLog.html");

	for(int i = 0; i < 1000; i++)
	{
		LOG_UPDATE();
		TestFunction(i);
	}

	LOG_TERM();
}