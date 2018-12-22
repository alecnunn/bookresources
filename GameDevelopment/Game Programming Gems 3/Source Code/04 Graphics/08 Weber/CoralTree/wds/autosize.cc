#include "wds.h"


#define	BUTTONS	16


/******************************************************************************
int		main(int argc,char **argv,char **envp)

******************************************************************************/
int main(int argc,char **argv,char **envp)
	{
	EW_EventContext	event_context;
	EW_Window		window;
	EW_Font			fontdata;
	EW_FontRequest	request;
	WDS_Scope		scope;
	WDS_Form		form;
	WDS_Button		button[BUTTONS];
	char			message[256];

	window.Open("Pick A Number");

	request.SetHeight(64);

	if( !window.LoadFont(&fontdata,&request) )
		window.SetFont(&fontdata);

	window.SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);
	scope.SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);

	form.SetMode(WDS_FORM_ROW);
	form.SetMaxSize(200,-1);

	long m;
	for(m=0;m<BUTTONS;m++)
		{
		sprintf(message,"%d",m);

		button[m].SetLabel(message);
		button[m].SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE);

		form.AddWidget(&button[m]);
		}

	event_context.AddWindow(&window);
	window.AddWidget(&scope);
	scope.AddWidget(&form);

	event_context.BeginLoop();

	return 0;
	}
