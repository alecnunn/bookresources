#include "wds.h"

int main(int argc,char **argv,char **envp)
	{
	WDS_NullApplication	application;
	EW_EventContext		event_context;
	EW_Window			window;
	WDS_Button			button;

	window.Open("Hello Window");

	EW_PRINT(EW_APP,EW_ERROR,"Hello Text.");

	button.SetLabel("Hello World!");
	button.SetBounds(EW_BOUNDS_FILL_PARENT);

	event_context.AddWindow(&window);
	window.AddWidget(&button);

	event_context.BeginLoop();

	return 0;
	}
