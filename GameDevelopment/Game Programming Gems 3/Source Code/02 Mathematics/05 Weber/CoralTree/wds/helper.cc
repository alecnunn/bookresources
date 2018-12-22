#include "wds.h"



void help_callback(WDS_Activation *activation);



/******************************************************************************
class	TEST_Application: public WDS_NullApplication

******************************************************************************/
class TEST_Application: public WDS_NullApplication
	{
	private:
		EW_EventContext		event_context;
		EW_Window			window;
		WDS_Scope			scope;
		WDS_Form			form;
		WDS_Button			button;
		WDS_PointerEntry	getfile,getcommand,getdata;
		EW_Font				fontdata;
		EW_FontRequest		request;

	public:

		WDS_String			filename;
		long				command,data;

					TEST_Application()		{};
virtual	void		Process(void)			{ event_context.BeginLoop(); };
virtual const char	*GetName(void)	const	{ return "WDS Help Test"; };

virtual long		Initialize(void)
						{
						event_context.AddWindow(&window);

						window.Open("Helper");

						request.SetHeight(16);
						request.SetStyle(EW_FONT_PROPORTIONAL);

						if( !window.LoadFont(&fontdata,&request) )
							window.SetFont(&fontdata);

						window.AddWidget(&scope);
						scope.AddWidget(&form);
						form.AddWidget(&getfile);
						form.AddWidget(&getcommand);
						form.AddWidget(&getdata);
						form.AddWidget(&button);

						window.SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);
						scope.SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);

						form.SetMode(WDS_FORM_COLUMN);

						WDS_Activation activation;
						activation.SetObject(this);
						button.SetStateChangeCallback(WDS_STATE_0,&help_callback,&activation);
						button.SetLabel("Help Me!");
						button.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE);

						getfile.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_LEFT);
						getfile.SetAnnotation("Help File");
						getfile.SetLocalAnnotationSize(100,0);
						getfile.SetSize(300,0);
						getfile.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);
						getfile.SetPointerType(WDS_STRING);
						getfile.SetPointer(&filename);
						filename.NewBuffer("hoh.hlp");

						getcommand.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_LEFT);
						getcommand.SetAnnotation("wcommand");
						getcommand.SetLocalAnnotationSize(100,0);
						getcommand.SetSize(200,0);
						getcommand.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);
						getcommand.SetPointerType(WDS_LONG);
						getcommand.SetPointer(&command);
						getcommand.GetString()->SetConfirmCallback(WDS_String::ConfirmLong,WDS_CONFIRM_NULL);
						command=1;

						getdata.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_LEFT);
						getdata.SetAnnotation("dwdata");
						getdata.SetLocalAnnotationSize(100,0);
						getdata.SetSize(200,0);
						getdata.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);
						getdata.SetPointerType(WDS_LONG);
						getdata.SetPointer(&data);
						getdata.GetString()->SetConfirmCallback(WDS_String::ConfirmLong,WDS_CONFIRM_NULL);
						data=0;

						return TRUE;
						}
	};



/******************************************************************************
void	help_callback(WDS_Activation *activation)

******************************************************************************/
void help_callback(WDS_Activation *activation)
    {
	TEST_Application	*application=(TEST_Application *)activation->GetObject();
    WDS_Button			*button=(WDS_Button *)activation->GetThisWidget();

    EW_Event event;

	// finish current edit
    event.SetSIS(EW_EVENT_KEYBOARD,EW_KEY_ALL,EW_STATE_ANY);
    button->RegisterEventBypass(TRUE,&event,WDS_BYPASS_FLAG_TAKE,FALSE);
    button->RegisterEventBypass(FALSE,&event,WDS_BYPASS_FLAG_TAKE,FALSE);

	char *filename=application->filename.Buffer();
	long command=application->command;
	long data=application->data;

	button->GetHelp()->SetFCD(filename,command,data);

	EW_PRINT(EW_APP,EW_LOG,"SpawnHelp() \"%s\" %d %d",filename,command,data);

	long result=button->SpawnHelp();

	EW_PRINT(EW_APP,EW_LOG," result=%d",result);
	}



/******************************************************************************
int		main(const int argc,const char **argv,const char **envp)

******************************************************************************/
int main(const int argc,const char **argv,const char **envp)
	{
	TEST_Application    application;

	application.MakeAware(argc,argv,envp);

	return application.Main();
	}
