#include "wds.h"



class TEST_Application: public EW_Application
	{
	private:
		EW_EventContext		event_context;
		EW_Window			window;
		WDS_PointerEntry	pentry;
		long				counter;


	public:

				TEST_Application(): EW_Application(this)	{};

virtual	long	Initialize(void)
					{
					SetEventContext(&event_context);

					window.Open("Counter");

					pentry.SetBounds(EW_BOUNDS_FILL_PARENT);
					pentry.SetPointerType(WDS_LONG);
					pentry.SetPointer(&counter);

					event_context.AddWindow(&window);
					window.AddWidget(&pentry);

					window.SetDoWork(TRUE);

					counter=0;

					return TRUE;
					}
virtual	void	Finalize(void)	{};

virtual	const	char	*GetName(void)				const	{ return "WDS App Test"; };
virtual	const	char	*GetParameterUsage(void)	const	{ return "ignored"; };


virtual	void	 Process(void)
					{
					counter++;
					osdMilliSleep(100);
					};
	};



int main(int argc,char **argv,char **envp)
	{
	TEST_Application	application;

	return application.Main();
	}
