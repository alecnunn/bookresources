/******************************************************************************

Coral Tree wds/xwds.cc
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

/******************************************************************************

NOTICE: this is a test program that intentially does some odd things.

While it may be good example of how to do specific things, it is not
neccessarily a good example of how to organize code behind an interface.
In fact, its handling of objects memory-wise is pretty poor.

******************************************************************************/

#include "wds.h"


#define BUTTONS					32

#define ROOTSCOPE				TRUE
#define LEFTSCOPE				TRUE
#define ALT_LOOKS				TRUE
#define HIER_FILE				TRUE	// WDS_HierarchyFile vs. WDS_HierarchyTest
#define TYPE_CHECK				FALSE
#define LOAD_FONT				FALSE	// load alternative size font
#define PART_LONECHILD_TEST		FALSE
#define INVALID_BYPASS_TEST		FALSE
#define NUDE_TEST				FALSE
#define MAP_TEST				TRUE
#define TIP_TEST				FALSE
#define FIXED_SIZE				FALSE	// fixed-size main window
#define INVALID_STRINGS			FALSE	// don't fix bad strings, just highlight them
#define ICON_FORM				FALSE	// show all the built-in icons
#define MENU_ON_PARTITION		FALSE	// put menu on partition instead
#define FORM_REMAINDER_TEST		TRUE

#define	USE_MFD					FALSE
#define	MFD_NODES1				7
#define	MFD_NODES2				9

#define MAIN_WINDOW				TRUE
#define PART_WINDOW				TRUE
#define POPUP_WINDOW			FALSE
#define TABLE_WINDOW			FALSE
#define MAKE_MENU				TRUE

#define POPUP_DOUBLE_BUFFER		FALSE
#define TABLE_DOUBLE_BUFFER		FALSE

#define FONT_STYLE				EW_FONT_PROPORTIONAL	// EW_FONT_PROPORTIONAL | EW_FONT_BOLD | EW_FONT_ITALIC

#define PICKLIST_ENTRIES1		7
#define PICKLIST_ENTRIES2		57

#define RADIO_CHOICES			7
#define RADIO_ANNOTATE			TRUE

#define TEST_TABLE_EDIT_PARTIAL	FALSE
#define TEST_TABLE_ROWS			77
#define TEST_TABLE_COLUMNS		7
#define TEST_TABLE_HEADERS		1
#define TEST_TABLE_ROWLABELS	2

#if TRUE

#define TEST_TABLE_EDITABLE		WDS_TABLE_EDITABLE_ALL
#define TEST_TABLE_ALLSELECT	WDS_TABLE_ALLSELECT_ROW

#else

#define TEST_TABLE_EDITABLE		WDS_TABLE_EDITABLE_NONE
#define TEST_TABLE_ALLSELECT	WDS_TABLE_ALLSELECT_ROW

#endif



/******************************************************************************
class	WDS_TestApp: public WBA_Application

******************************************************************************/
class WDS_TestApp: public WBA_Application
	{
	public:
				long	timers;
				long	rgbmode;
				long	double_buffer;
				long	number_colors;
				float	font_size;

	public:
						WDS_TestApp(): WBA_Application(this)
							{
							timers=0;
							rgbmode=0;
							double_buffer=0;
							number_colors=0;
							font_size=0.0;
							};

virtual					~WDS_TestApp(void)						{ };
virtual			long	Initialize(void)						{ return TRUE; }
virtual			void	Finalize(void)
							{
							EW_PRINT(EW_APP,EW_LOG,"WDS_TestApp::Finalize() wds_test terminating gracefully");
							};

virtual	const	char	*GetName(void)				const	{ return "WDS Test"; };
virtual	const	char	*GetParameterUsage(void)	const	{ return "ignored"; };

				void	Setup(const int argc,const char **argv,const char **envp)
							{
							GPL_Aware gpl;
							WBA_Aware wba;
							EW_Aware ew;
							WDS_Aware wds;

							//				env variable				default	description				aliases (#,names,...)
							WBA_ARGS.Set(	"WDS_TEST_PRINT_HELP",		"0",	"print help",				1,	"h");
							WBA_ARGS.Set(	"WDS_TEST_TIMERS",			"1",	"activate timers",			1,	"t");
							WBA_ARGS.Set(	"WDS_TEST_DOUBLE_BUFFER",	"0",	"activate double buffering",1,	"b");
							WBA_ARGS.Set(	"WDS_TEST_RGB_MODE",		"0",	"activate RGB buffering",	1,	"r");
							WBA_ARGS.Set(	"WDS_TEST_COLORS",			"16",	"maximum colors",			1,	"c");
							WBA_ARGS.Set(	"WDS_TEST_FONT_SIZE",		"12.0",	"font size in pixels",		1,	"f");

							WBA_ENV.LoadEnvVariables(envp,FALSE);
							WBA_ENV.LoadCommandLine(argc,argv,TRUE);
							};
	protected:

virtual			void	Process(void);
	};



/******************************************************************************
int		main(const int argc,const char **argv,const char **envp)

******************************************************************************/
int main(const int argc,const char **argv,const char **envp)
	{
	WDS_TestApp	*application;
	application=new WDS_TestApp();

	application->Setup(argc,argv,envp);

	long value;

	if( (value=WBA_ARGS.Find("WDS_TEST_PRINT_HELP")->GetLong()) !=0 )
		{
		if(value>2)
			application->Printf("%s",application->DumpEnvironment());
		else if(value>1)
			application->Printf("%s",application->GetUsage(TRUE));
		else
			application->Printf("%s",application->GetUsage(FALSE));

		application->Exit();
		}

	application->timers=WBA_ARGS.Find("WDS_TEST_TIMERS")->GetLong();
	application->rgbmode=WBA_ARGS.Find("WDS_TEST_RGB_MODE")->GetLong();
	application->double_buffer=WBA_ARGS.Find("WDS_TEST_DOUBLE_BUFFER")->GetLong();
	application->number_colors=WBA_ARGS.Find("WDS_TEST_COLORS")->GetLong();
	application->font_size=WBA_ARGS.Find("WDS_TEST_FONT_SIZE")->GetDouble();

	EW_Window::SetDefaultRGBMode(application->rgbmode);

	return application->Main();
	}



/******************************************************************************
void	look_change(WDS_Activation *activation)

	for testing, the first three alter the default in WDS_Widget
	the last can override the default with a different instance

	note that it would be redundant to pass default_looks when
	initializing the actual default looks

******************************************************************************/
void look_change(WDS_Activation *activation)
	{
	static WDS_Looks slooks;
	static char name[5][24]={ "athena","hybrid","motif","simple","win32"};

	WDS_Button *button=(WDS_Button *)activation->GetThisWidget();
	WDS_Looks *default_looks=button->GetDefaultLooks();
	EW_EventContext *context=button->GetRootWindow()->GetEventContext();
//	EW_Event *event=activation->GetEvent();

	char title[128];
	strcpy(title,"WDS Test using ");

	long code=activation->GetCode();

	switch(code)
		{
		case 0:
#if ALT_LOOKS
			slooks.Initialize(WDS_LOOK_WIN32,default_looks);
			button->SetLooks(&slooks);
#else
			button->SetLooks(NULL);
			button->GetLooks()->Initialize(WDS_LOOK_WIN32,NULL);
#endif
			strcat(title,"hard-wired win32");
			break;
		default:
			button->SetLooks(NULL);
			if(code>5)
				{
				button->GetLooks()->LoadFromFile(name[(code-1)%5]);
				strcat(title,"loaded ");
				}
			else
				{
				button->GetLooks()->ReadFromBuffer(wdsGetBuiltinLAF(code));
				strcat(title,"imbedded ");
				}

			strcat(title,name[(code-1)%5]);
			break;
		}

	button->GetRootWindow()->SetTitle(title);

#if TIP_TEST
	long wx,wy,mx,my;
	button->GetRootWindow()->GetOrigin(&wx,&wy);
	activation->GetEvent()->GetMousePosition(&mx,&my);

	char tiptext[128];
	sprintf(tiptext,"Tip:\n%s",title);

	button->SetTipOrigin(wx+mx,wy+my);
	button->SetTipText(tiptext);
	button->ShowTip(TRUE);
#endif

	EW_WindowList *windowlist=context->GetWindowList();

	EW_Window *node;

	windowlist->ToHead();
	while( (node=windowlist->PostIncrement()) != NULL)
		{
		// propagate relevant changes to all windows
		button->GetLooks()->ResetWindow((EW_Window *)node);
		node->SetDirty(EW_DIRTY_COMPLETE);
		}

	button->GetRootWindow()->RingBell();
	}



/******************************************************************************
void	end_callback(WDS_Activation *activation)

******************************************************************************/
void end_callback(WDS_Activation *activation)
	{
	EW_Window *endwindow=(EW_Window *)activation->GetObject();
	EW_EventContext *context=endwindow->GetEventContext();

	if(endwindow)
		endwindow->DelayedDelete();

	if(context)
		context->EndLoop();
	}



/******************************************************************************
void	part_callback(WDS_Activation *activation)

Keywords:
******************************************************************************/
void part_callback(WDS_Activation *activation)
	{
	EW_Window *window=(EW_Window *)activation->GetObject();
	long code=activation->GetCode();
	EW_EventContext *context=window->GetEventContext();

	if(code==0)
		{
#if MAP_TEST
		window->UnMap();
#else
		window->Close();
#endif
		}

	if(code==1)
		{
		long index= (context->GetCursorIndex()+1)%EW_CURSORS;
		context->SetCursorIndex(index);

		EW_PRINT(EW_APP,EW_LOG,"Cursor index %d",index);
		}

	if(code==2)
		{
		EW_Window *endwindow=new EW_Window;
		WDS_Button *endbutton=new WDS_Button;
		WDS_Activation activation;

		activation.SetObject(endwindow);
		endbutton->SetStateChangeCallback(WDS_STATE_0,&end_callback,&activation);
		endbutton->SetLabel("End Loop");
		endbutton->SetSize(-1,50);
		endbutton->SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_X);
//		endwindow->SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);
		endwindow->AddWidget(endbutton);
		context->AddWindow(endwindow);

//		context->AppendWindowToModalStack(endwindow);
		endwindow->SetFlags(EW_WINDOW_MODAL_EXCLUSIVE);

		endwindow->SetTitle("End");
		endwindow->Open(EW_POSITION_OVER,window);
		}
	}



/******************************************************************************
void	table_change(WDS_Activation *activation)

	re-arrange the table

Keywords:
******************************************************************************/
void table_change(WDS_Activation *activation)
	{
	WDS_Table *table=(WDS_Table *)activation->GetThisWidget();

	long code=activation->GetCode();
	long newstate=activation->GetState();

	long row,column;

	table->GetLastEditIndices(&row,&column);

//	printf("wds_test Table Callback state=%d code=%d on row %d column %d\n",newstate,code,row,column);

#if TRUE
	if(newstate==WDS_STATE_0)
		{
		WDS_TableEntry *entry[3];
		WDS_TableEntryList entrylist;
		long m;

		WDS_TableEntryList *rowlist=table->GetTableEntryList();

		// write-protect to 30th entry/third column
		table->GetString(30,3)->SetEditable(FALSE);

		if(TRUE)
			return;

		// move 2 rows at last edited to before 30
		table->MoveRows(row,2,30);

		// delete/copy 3 rows at last edited to before 10
		rowlist->ToIndex(row);
		for(m=0;m<3;m++)
			entry[m]=rowlist->PostIncrement();

		table->DeleteRows(row,3);

		for(m=0;m<3;m++)
			{
//			printf("Appending (%s)\n",entry[m]->GetStringList()->ToHead()->Buffer());

			entrylist.AppendNode(entry[m]);
			}

		table->InsertRowsBefore(10,&entrylist);
		}
#endif
	}



/******************************************************************************
void	button_change(WDS_Activation *activation)

Keywords:
******************************************************************************/
void button_change(WDS_Activation *activation)
	{
	WDS_Button *button=(WDS_Button *)activation->GetThisWidget();

	long code=activation->GetCode();

	button->SetLabel((char *)(code? "Ouch": "Don't Push Me"));

	EW_EventContext *context=button->GetRootWindow()->GetEventContext();

//	activation->Dump((long)button);

	if(code)
		{
#if PART_WINDOW
		EW_WindowList *windowlist=context->GetWindowList();

		EW_Window *node;

		windowlist->ToHead();
		while( (node=windowlist->GetCurrent()) != NULL)
			{
			if( !strcmp(node->GetTitle(),"Partition Test") )
				{
				if(node->GetMapped())
					{
					EW_PRINT(EW_APP,EW_LOG,"Close Partition Window");
#if MAP_TEST
					node->UnMap();
#else
					node->Close();
#endif
//					node->DelayedDelete();
					}
				else
					{
					EW_PRINT(EW_APP,EW_LOG,"Open Partition Window");

					long mx=0,my=0;
					EW_Event *event=activation->GetEvent();
					if(event)
						event->GetMousePosition(&mx,&my);

					node->SetOrigin(mx+20,my-20);
#if MAP_TEST
					node->Map(EW_POSITION_RELATIVE_TO,button->GetRootWindow());

//					node->GetEventContext()->AppendWindowToModalStack(node);
#else
					node->Open(EW_POSITION_RELATIVE_TO,button->GetRootWindow());
#endif
					}
				}

			windowlist->SetCurrent(node);
			windowlist->PostIncrement();
			}
#endif
		}

#if INVALID_BYPASS_TEST
	if(code)
		{
		WDS_Button doomed;

		EW_PRINT(EW_APP,EW_LOG,"Setting up button with bypass");

		EW_Event event;

		event.SetSIS(1,2,3);

		button->AddWidget(&doomed);
		doomed.RegisterEventBypass(TRUE,&event,0,FALSE);
		button->RemoveWidget(&doomed);

		EW_PRINT(EW_APP,EW_LOG,"Destructing button leaving bypass");

		// doomed will destruct and leave the bypass in place
		}
#endif

#if NUDE_TEST
	if(code)
		{
		WDS_Button	clothed,*nude=new WDS_Button();

		EW_PRINT(EW_APP,EW_LOG,"attempting delete on nude 0x%x (acceptable)",nude);
		delete nude;

		EW_PRINT(EW_APP,EW_LOG,"attempting delete on clothed 0x%x (otherwise undefined behavior)",&clothed);
		delete &clothed;
		}
#endif
	}



/******************************************************************************
void	dump_tree(WDS_Activation *activation)

Keywords:
******************************************************************************/
void dump_tree(WDS_Activation *activation)
	{
	WDS_Button *button=(WDS_Button *)activation->GetThisWidget();

	EW_EventContext *context=button->GetRootWindow()->GetEventContext();

	context->SetCursorIndex(EW_CURSOR_WAIT);

	button->GetLooks()->Dump((long)button);
	button->GetRootWindow()->GetEventContext()->Dump((long)button,FALSE);

	context->SetCursorIndex(EW_CURSOR_NORMAL);
	}



/******************************************************************************
int		main(int argc,char **argv,char **envp)

******************************************************************************/
void WDS_TestApp::Process(void)
	{
	EW_EventContext		event_context;					// context for event loop
	EW_Font				fontdata;						// font information
	EW_FontRequest		request;

	EW_Window			mainwindow;						// primary test window
	EW_Window			blesswindow;					// popup test window
	EW_Window			tablewindow;					// table test window
	EW_Window			*partwindow=new EW_Window;

	// widgets in first window
	WDS_Relay_F1Help		relay;
	WDS_DumpWidgetsRelay	dump_relay;

	WDS_MFD				mfd;
	WDS_Button			*abutton,dump_button,button1,button[BUTTONS];
	WDS_RadioButton		radio[3];
	WDS_RadioScope		radioscope;
	WDS_ToggleButton	toggle;
	WDS_Scope			rootscope,leftscope,rightscope,bscope,tablescope;
	WDS_GetString		leftstring,upperrightstring,datestring,timestring,phonestring,longstring,floatstring,ssnstring;
	WDS_PointerEntry	scroll_number;
	WDS_Partition		h_partition,v_partition,left_part,test_vpart,bpartition,menu_partition;
	WDS_Form			scrollform,rowform[2],autoform,columnform,outerform,tieform,bform,confirmform;
	WDS_ScrollRegion	scrollregion,tieregion,hierscroll,scrollconfirm;
	WDS_ScrollBar		freescrollbar[3];
	WDS_TextList		textlist;
	WDS_TieList			scrolltie;
	WDS_Table			table;
	WDS_Divider			divider;
	WDS_Pick			shortpick,longpick,partpick;
	WDS_Document		document;
	WDS_GfxTest			gfx_test;
	WDS_IconTestForm	iconform;
#if HIER_FILE
	WDS_HierarchyFile	hierarchy;
#else
	WDS_HierarchyTest	hierarchy;
#endif

	GPL_BitGroup		numbers;

	// widgets in partwindow
	WDS_Scope			partscope;
	WDS_Partition		fixpartition;
	WDS_Form			partform,controlform,partentryform;
	WDS_RadioScope		partradioscope;
	WDS_Button			partbutton[3];
	WDS_GetString		partentry[3];
	WDS_Portal			portal;

	// widgets in blesswindow
	WDS_Popup			popup, popup2, popup3;
	WDS_Popup			*popwin;
	WDS_PopupBase		*popupbase;
	WDS_MenuBar			menubar,amenubar;
	WDS_Popup			menubutton1, menubutton2, menubutton3;
	WDS_Button			menupush;

	// used widgets for type check test
	WDS_Widget			*check_widget;
	WDS_Button			check_button;
	WDS_ScrollBarButton	check_scrollbarbutton;
	WDS_HierarchyForm	check_hierarchyform;

	WDS_Activation		activation;
	WDS_Image			dump_image[2];

	long j,k,m,n;

	char message[200];

	// configure output
	ewGlobalOutput.SetFilename("wds_test.out");
//	ewGlobalOutput.SetFlags(OSD_OUT_STDERR | OSD_OUT_FILE | OSD_OUT_MESSAGEBOX);
	ewGlobalOutput.SetFlags(OSD_OUT_STDERR);
//	ewGlobalOutput.PrintLine("WDS TEST","Output Log");

	const char window_text[4][16]={ "NONE", "EW_XWIN", "EW_WIN32", "EW_MEX" };
	const char graphics_text[5][16]={ "NONE", "EW_X_GFX", "EW_OPENGL", "EW_IRISGL", "EW_GDI" };

	sprintf(message,"EW_WL=%s EW_GL=%s",window_text[EW_WL%4],graphics_text[EW_GL%5]);
	ewGlobalOutput.PrintLine("Output Log",message);

#if FALSE
	WDS_String	teststring;
	teststring.NewBuffer("ABCDEF");
	teststring.InsertBuffer(4,"GHI");
	printf("testbuffer=(%s)\n",teststring.Buffer());
#endif

	// associate windows with event context
#if MAIN_WINDOW
	event_context.AddWindow(&mainwindow);
#endif
	event_context.AddWindow(partwindow);
	event_context.AddWindow(&blesswindow);

	// window sizes
	blesswindow.SetGeometry(10,10,200,200);
	mainwindow.SetGeometry(20,20,600,450);

	mainwindow.SetRGBMode(rgbmode);
	mainwindow.SetDoubleBuffer(double_buffer);
	partwindow->SetDoubleBuffer(double_buffer);
	blesswindow.SetDoubleBuffer(POPUP_DOUBLE_BUFFER);

	mainwindow.SetNumberColors(number_colors);
	partwindow->SetNumberColors(number_colors);
	blesswindow.SetNumberColors(number_colors);
	tablewindow.SetNumberColors(number_colors);

	mainwindow.SetFlags(EW_WINDOW_NOMAP);
	mainwindow.Open("WDS Test");
	mainwindow.Map();

	// open windows first so that we can load a font
/*
#if MAIN_WINDOW

#if FIXED_SIZE
	mainwindow.SetFlags(EW_WINDOW_FIXED_SIZE);
#endif

	mainwindow.Open("WDS Test");
#endif
*/

#if PART_WINDOW
	partwindow->SetTitle("Partition Test");

#if MAP_TEST
	partwindow->SetFlags(EW_WINDOW_NOMAP|EW_WINDOW_MODAL_EXCLUSIVE);
	partwindow->Open(EW_POSITION_RELATIVE_TO,NULL);
#endif

#endif

#if POPUP_WINDOW
	blesswindow.Open("pop test");
#endif

	// form a font request
	request.SetHeight(font_size);
	request.SetStyle(FONT_STYLE);
//	request.AddHint("cave-medium-r");
//	request.AddHint("Comic Sans MS");

#if LOAD_FONT
	// load font and set as current font
	if( !mainwindow.LoadFont(&fontdata,&request) )
		{
		mainwindow.SetFont(&fontdata);
		mainwindow.CopyDefaultFontFrom(&fontdata);
		}
#endif

	long fontheight=(long)request.GetHeight();

	// fill optional widget names (used for debugging only)
	rootscope.SetName("rootscope");
	bscope.SetName("bscope");
	tablescope.SetName("tablescope");
	menu_partition.SetName("menu_partition");
	h_partition.SetName("h_partition");
	v_partition.SetName("v_partition");
	bpartition.SetName("bpartition");
	test_vpart.SetName("test_vpart");
	scrollform.SetName("scrollform");
	rowform[0].SetName("rowform 0");
	rowform[1].SetName("rowform 1");
	autoform.SetName("autoform");
	radioscope.SetName("radioscope");
	tieform.SetName("tieform");
	confirmform.SetName("confirmform");
	scrollconfirm.SetName("scrollconfirm");
	columnform.SetName("columnform");
	leftscope.SetName("leftscope");
	portal.SetName("portal");
	rightscope.SetName("rightscope");
	scrollregion.SetName("scrollregion");
	textlist.SetName("textlist");
	outerform.SetName("outerform");
	tieregion.SetName("tieregion");
	leftstring.SetName("leftstring");
	upperrightstring.SetName("upperrightstring");
	floatstring.SetName("floatstring");
	longstring.SetName("longstring");
	phonestring.SetName("phonestring");
	ssnstring.SetName("ssnstring");
	datestring.SetName("datestring");
	timestring.SetName("timestring");
	scroll_number.SetName("scroll_number");
	shortpick.SetName("shortpick");
	partpick.SetName("partpick");
	longpick.SetName("longpick");
	document.SetName("document");

	partscope.SetName("partscope");
	fixpartition.SetName("fixpartition");
	partform.SetName("partform");
	partradioscope.SetName("partradioscope");
	partentryform.SetName("partentryform");
	controlform.SetName("controlform");

	rowform[0].SetLabel("Row Form 0");
	rowform[1].SetLabel("Row Form 1");
	autoform.SetLabel("Fixed Form");
	radioscope.SetLabel("Radio Scope");

	popup.SetName("popup");
	popup2.SetName("popup 2");
	popup3.SetName("popup 3");
	menupush.SetName("menupush");
	menubar.SetName("menubar");
	amenubar.SetName("amenubar");
	menubutton1.SetName("MB 1");
	menubutton2.SetName("MB 2");
	menubutton3.SetName("MB 3");

	toggle.SetName("toggle");

	leftstring.GetString()->SetEditable(FALSE);
	leftstring.SetString("Once\nupon a time, there were three little pigs.");
	leftstring.SetAcceptKeys('\\');
	leftstring.SetAbortKeys('`');

	long invalid= INVALID_STRINGS? WDS_CONFIRM_MARK_INVALID|WDS_CONFIRM_AUTO_HIGHLIGHT: 0;

	upperrightstring.SetString("Hello World!");
	upperrightstring.GetString()->SetDisplayChar('*');
	timestring.GetString()->SetConfirmCallback(WDS_String::ConfirmTime,
						WDS_CONFIRM_LEAD_ZERO|WDS_CONFIRM_ELAPSED|WDS_CONFIRM_SECONDS|WDS_CONFIRM_ALLOW_EMPTY|invalid);
	timestring.SetString("7:38p");
	datestring.GetString()->SetConfirmCallback(WDS_String::ConfirmDateTime,
		invalid|WDS_CONFIRM_STROKE|WDS_CONFIRM_USA|WDS_CONFIRM_AMPM|WDS_CONFIRM_SECONDS|WDS_CONFIRM_LEAD_ZERO);
	datestring.SetString("97 4/16 515p");
	longstring.GetString()->SetConfirmCallback(WDS_String::ConfirmLong,WDS_CONFIRM_NULL);
	longstring.SetString("AB 1.23 kde");
	phonestring.GetString()->SetConfirmCallback(WDS_String::ConfirmPhone,invalid|WDS_CONFIRM_NANP_PRETTY|WDS_CONFIRM_MAP_CHAR);
	phonestring.SetString("(908) 765-4321");
	ssnstring.GetString()->SetConfirmCallback(WDS_String::ConfirmSSN,invalid|WDS_CONFIRM_NULL);
	ssnstring.SetString("012-34-5678");
	floatstring.GetString()->SetConfirmCallback(WDS_String::ConfirmFloat,WDS_CONFIRM_NULL);
	floatstring.SetString("AB 56.7 5de");

	numbers.Resize(256);
	numbers.SetRange('0','9');
	numbers.SetBit('-');
	ssnstring.SetKeySet(&numbers);

	longstring.GetString()->SetEditable(FALSE);

	confirmform.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,
																		WDS_WIDGET_ANNOTATION_TOP|WDS_WIDGET_ANNOTATION_IMBED);
	confirmform.SetAnnotation("Confirm");
	confirmform.SetLocalAnnotationSize(40,0);

	timestring.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_LEFT);
	timestring.SetLocalAnnotationSize(40,0);
	timestring.SetAnnotation("Time:");

	datestring.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_TOP);
//	datestring.SetLocalAnnotationSize(30,fontheight+2);
	datestring.SetLocalAnnotationSize(40,20);
	datestring.SetAnnotation("Date:");
	longstring.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_TOP);
	longstring.SetLocalAnnotationSize(0,0);
	longstring.SetAnnotation("Long:");
	phonestring.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_LEFT);
	phonestring.SetLocalAnnotationSize(40,0);
	phonestring.SetAnnotation("Phone:");
	ssnstring.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_LEFT);
	ssnstring.SetLocalAnnotationSize(0,0);
	ssnstring.SetAnnotation("Social\nSecurity\nNumber:");
	floatstring.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_LEFT);
	floatstring.SetLocalAnnotationSize(0,0);
	floatstring.SetAnnotation("Float:");

	// button callbacks
	activation.SetCode(TRUE);
	dump_button.SetStateChangeCallback(WDS_STATE_0,&dump_tree,&activation);
	button1.SetStateChangeCallback(WDS_STATE_1,&button_change,&activation);
	activation.SetCode(FALSE);
	button1.SetStateChangeCallback(WDS_STATE_0,&button_change,&activation);
	table.SetStateChangeCallback(WDS_STATE_ALL,&table_change,&activation);

	// menus/pop-ups
	menupush.SetPrefSize(40,20);
	menubutton1.SetPrefSize(50,16);
	menubutton2.SetPrefSize(50,16);
	menubutton2.SetPopItem(EW_ITEM_LEFT|EW_ITEM_RIGHT);
	menubutton3.SetPrefSize(50,16);
	amenubar.SetPrefSize(10,25);

	//popup.SetGeometry(10,10,100,100);
//	popup.SetBounds(EW_BOUNDS_FILL_PARENT);
	popup.SetLocation(WDS_POPUP_LOC_MOUSE);
	popup.SetFlags(WDS_POPUP_GRAB_POINTER|WDS_POPUP_SIBLING_HANDOFF|WDS_POPUP_AUTOPOP|WDS_POPUP_NONSTICK);
	popup.GetBaseWindow()->SetGeometry(0,0,150,70);

	popup2.SetGeometry(10,10,50,50);
	popup2.SetLocation(WDS_POPUP_LOC_RIGHT);

	popup3.SetGeometry(10,10,50,50);
	popup3.SetLocation(WDS_POPUP_LOC_RIGHT);

	menubutton3.GetBaseWidget()->AddWidget(&popup3);
	menubutton1.GetBaseWidget()->AddWidget(&menupush);

	amenubar.AddWidget(&menubutton1);
	amenubar.AddWidget(&menubutton2);
	amenubar.AddWidget(&menubutton3);

	popupbase = popup.GetBaseWidget();
	popupbase->AddWidget(&popup2);

	popup.SetGeometry(0,0,100,100);

	popwin = new WDS_Popup("pop window");
	popwin->SetGeometry(0,0,40,50);
	popwin->SetFlags(WDS_POPUP_PERSISTANT|WDS_POPUP_GRAB_POINTER|WDS_POPUP_NONSTICK);
	popwin->SetLocation(WDS_POPUP_LOC_MOUSE);

	bscope.SetBounds(EW_BOUNDS_FILL_PARENT);

	rootscope.SetBounds(EW_BOUNDS_FILL_PARENT);

	// partition modes
	menu_partition.SetBounds(EW_BOUNDS_FILL_PARENT);
	menu_partition.SetDirection(WDS_VERTICAL);
	menu_partition.SetFlags(WDS_PARTITION_FLAGS_ATTACH);
	h_partition.SetDirection(WDS_HORIZONTAL);
	h_partition.SetFlags(WDS_PARTITION_FLAGS_ATTACH);
	left_part.SetDirection(WDS_VERTICAL);
	left_part.SetFlags(WDS_PARTITION_FLAGS_ATTACH);
	v_partition.SetDirection(WDS_VERTICAL);
	v_partition.SetFlags(WDS_PARTITION_FLAGS_ATTACH);
	test_vpart.SetDirection(WDS_VERTICAL);
	test_vpart.SetFlags(WDS_PARTITION_FLAGS_ATTACH);

	bpartition.SetBounds(EW_BOUNDS_FILL_PARENT);
	bpartition.SetDirection(WDS_VERTICAL);
//	bpartition.SetFlags(WDS_PARTITION_FLAGS_ATTACH);

	// turn off vertical scrolling on hierarchy test
	hierscroll.GetLocalLook()->SetElement(WDS_STATE_1,WDS_ELEMENT_SCROLLREGION_SCROLLABLE,WDS_SCROLLABLE_OFF);

#if HIER_FILE
	hierarchy.SetLabel("Root Filesystem /");
	hierarchy.SetFullPathName("/");
#else
	hierarchy.SetLabel("Hierarchy");
#endif

	// geometries
	rightscope.SetSize(0,130);
	partpick.SetGeometry(5,110,95,0);
	shortpick.SetGeometry(5,110,95,0);
	longpick.SetGeometry(15,80,77,0);
	upperrightstring.SetGeometry(15,5,60,70);

	textlist.SetGeometry(115,85,50,30);

	shortpick.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_LEFT);
	shortpick.SetLocalAnnotationSize(30,0);
	shortpick.SetAnnotation("Pick:");

	document.SetGeometry(110,5,150,140);

	leftstring.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);

	longstring.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);
	phonestring.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);
	ssnstring.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);
	datestring.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE);
	timestring.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);

#if TRUE
	floatstring.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);
#else
	floatstring.SetBounds(EW_VERTICAL,EW_BOUNDS_NONE);
#endif

	floatstring.SetGeometry(10,55,80,20);
	longstring.SetGeometry(5,30,75,20);
	phonestring.SetGeometry(15,40,135,30);
	ssnstring.SetGeometry(25,44,130,30);
//	datestring.SetGeometry(15,5,90,20);
	datestring.SetGeometry(15,5,170,0);
	timestring.SetGeometry(0,75,120,0);

	left_part.SetSize(120,0);
	leftscope.SetSize(0,190);
	v_partition.SetSize(90,0);
	rowform[0].SetSize(150,0);
	rowform[1].SetSize(150,0);
	outerform.SetSize(0,100);
	tieregion.SetSize(0,55);
	hierscroll.SetSize(0,50);
	divider.SetSize(0,150);
	table.SetSize(0,150);

	outerform.SetSizing(WDS_HORIZONTAL,WDS_SIZING_MANUAL);
	outerform.SetSizing(WDS_VERTICAL,WDS_SIZING_MANUAL);
	outerform.SetMode(WDS_FORM_COLUMN);
//	scrollregion.SetBounds(EW_BOUNDS_FILL_PARENT);

	outerform.SetLocalElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_TOP|WDS_WIDGET_ANNOTATION_IMBED);
	outerform.SetAnnotation("Scroll Area");
	outerform.SetLocalAnnotationSize(0,0);

/*
	toggle.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_BORDER,WDS_BEVELBOX_IN|WDS_BEVELBOX_SMOOTH|WDS_BEVELBOX_ELLIPTICAL);
	toggle.GetLocalLook()->SetElement(WDS_STATE_3,WDS_ELEMENT_BORDER,WDS_BEVELBOX_BORDERLESS);
	toggle.GetLocalLook()->SetElement(WDS_STATE_ALL,EW_COLOR_HALF_SHADOW,
									WDS_ELEMENT_MAP|EW_COLOR_SHADOW);
	toggle.GetLocalLook()->SetElement(WDS_STATE_ALL,EW_COLOR_SHADOW,
									WDS_ELEMENT_MAP|EW_COLOR_HALF_SHADOW);
	toggle.GetLocalLook()->SetElement(WDS_STATE_0,EW_COLOR_WIDGET,EW_WHITE);
	toggle.GetLocalLook()->SetElement(WDS_STATE_1,EW_COLOR_WIDGET,EW_BLACK);
*/

	toggle.SetAnnotation("Switch");
	toggle.SetGeometry(0,100,60,16);

	for(m=0;m<3;m++)
		{
		radio[m].SetGeometry(0,125+25*m,60,16);

		sprintf(message,"selection %d",m);
		radio[m].SetAnnotation(message);
		}

	dump_button.SetLabel("Dump");
	button1.SetLabel("Press me if you think you can!");
//	button1.SetLabel("Howdy\n\nPartner");
//	button1.SetLabel("Xy XyXy XyyX XXyy Xy XyXy XyyX XXyy");

	scrollregion.AddScrollField(0,0.2,0.1);
	scrollregion.AddScrollField(0,0.6,0.0);

	leftstring.SetGeometry(10,120,80,70);
	button1.SetGeometry(15,70,100,40);

	dump_button.SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE);
	dump_button.SetOrigin(5,5);
	dump_button.SetImage(WDS_STATE_0,&dump_image[0]);
	dump_button.SetImage(WDS_STATE_1,&dump_image[1]);

	EW_ColorMapping remap;

	remap.SetMapToIndex(0,EW_RED);
	remap.SetMapToColor(1,0x000040);
	remap.SetMapToIndex(2,255);
	remap.SetMapToColor(3,0x404000);

	dump_image[0].SetNumberColors(mainwindow.GetNumberColors());
	dump_image[1].SetNumberColors(mainwindow.GetNumberColors());

#if FALSE
	dump_image[0].ReadFromBufferXPM(wdsGetBuiltinXPM(0));
#else
	dump_image[0].LoadFromFileXPM("dump0.xpm");
#endif
	dump_image[1].SetRemap(&remap);
//	dump_image[1].SetFlags(EW_XPM_FLAG_NO_DITHER);
	dump_image[1].LoadFromFileXPM("dump1.xpm");

//	printf("colors=%d\n",mainwindow.GetNumberColors());

	portal.SetGeometry(130,140,80,60);
	portal.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_BORDER,WDS_BEVELBOX_RIDGED|WDS_BEVELBOX_SMOOTH);
	portal.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_DEPTH,6);

	for(m=0;m<3;m++)
		{
		// note "height" is scrollbar width, which comes from looks
		freescrollbar[m].SetGeometry(120,5+30*m,110,25);
		freescrollbar[m].SetFraction(0.4-0.1*m);
		freescrollbar[m].GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_SCROLLBAR_CHILDREN,
																						WDS_SCROLLBAR_CHILDREN_SLAVES|(m<<1));
		freescrollbar[m].GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_SCROLLBAR_WIDTH,12+m*8);
		}

	scroll_number.SetPointerType(WDS_LONG);
	scroll_number.SetPointer(gfx_test.GetCounterPointer(1));
	scroll_number.SetGeometry(120,100,50,20);

	// lay out widget hierarchy
#if ROOTSCOPE
	mainwindow.AddWidget(&rootscope);
	rootscope.AddWidget(&menu_partition);
#else
	mainwindow.AddWidget(&menu_partition);
#endif

#if MENU_ON_PARTITION
	fixpartition.AddWidget(&menubar);
#else
	menu_partition.AddWidget(&menubar);
#endif

#if ICON_FORM
	menu_partition.AddWidget(&iconform);
#endif

#if LEFTSCOPE
	left_part.AddWidget(&leftscope);
	leftscope.AddWidget(&button1);
	leftscope.AddWidget(&dump_button);
	leftscope.AddWidget(&leftstring);
	for(m=0;m<3;m++)
		leftscope.AddWidget(&freescrollbar[m]);
	leftscope.AddWidget(&scroll_number);
	leftscope.AddWidget(&portal);
#else
	left_part.AddWidget(&leftstring);
#endif

	left_part.AddWidget(&gfx_test);

#if USE_MFD
	WDS_Form *mfdform;

	menu_partition.AddWidget(&mfd);
	mfd.Register(TRUE);

	mfd.AddWidget(&h_partition);
	mfd.SetDefaultWidget(&h_partition);

	mfd.CreateMFDNode("File",NULL);
	mfd.CreateMFDNode("File->New",NULL);
	mfd.CreateMFDNode("File->Open",NULL);
	mfd.CreateMFDNode("File->Save",NULL);
	mfd.CreateMFDNode("File->Save As",NULL);
	mfd.CreateMFDNode("File->Close",NULL);
	mfd.CreateMFDNode("File->Print",NULL);
	mfd.CreateMFDNode("File->Exit",NULL);
	mfd.CreateMFDNode("Edit",NULL);
	mfd.CreateMFDNode("Edit->Undo",NULL);
	mfd.CreateMFDNode("Edit->Cut",NULL);
	mfd.CreateMFDNode("Edit->Copy",NULL);
	mfd.CreateMFDNode("Edit->Paste",NULL);

	mfd.CreateMFDNode("Test",NULL);

	mfd.CreateMFDNode("Help",NULL);
	mfd.CreateMFDNode("Help->About",NULL);

	for(n=0;n<MFD_NODES1;n++)
		{
		sprintf(message,"Test->%d MFD",n);
		mfd.CreateMFDNode(message,NULL);
		}

	for(m=0;m<MFD_NODES2;m++)
		{
		sprintf(message,"Form %d",m);

		mfdform=new WDS_Form();
		mfdform->SetAnnotation(message);
		mfdform->SetLocalAnnotationSize(0,0);

		mfdform->SetSizing(WDS_HORIZONTAL,WDS_SIZING_MANUAL);
		mfdform->SetSizing(WDS_VERTICAL,WDS_SIZING_MANUAL);

		mfdform->SetLocalElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,
												WDS_WIDGET_ANNOTATION_TOP|WDS_WIDGET_ANNOTATION_IMBED);

		for(n=0;n<MFD_NODES1;n++)
			{
			sprintf(message,"Test->%d MFD->%d,%d Sub",n,m,n);
			mfd.CreateMFDNode(message,mfdform);
			}
		}
#else
	menu_partition.AddWidget(&h_partition);
#endif

	h_partition.AddWidget(&left_part);
	h_partition.AddWidget(&v_partition);
	h_partition.AddWidget(&test_vpart);
	h_partition.AddWidget(&columnform);

	v_partition.AddWidget(&outerform);
	outerform.AddWidget(&scrollregion);
	v_partition.AddWidget(&tieregion);
	v_partition.AddWidget(&rightscope);
	v_partition.AddWidget(&scrollconfirm);
	scrollconfirm.AddWidget(&confirmform);

	rightscope.AddWidget(&upperrightstring);
	rightscope.AddWidget(&shortpick);
	rightscope.AddWidget(&longpick);
	rightscope.AddWidget(&document);
	rightscope.AddWidget(&textlist);

	// Enable PostScript dumping
	rightscope.AddWidget(&dump_relay);
	dump_relay.Register(TRUE);

	rightscope.AddWidget(&relay);
	relay.Register(TRUE);

	scrollregion.AddWidget(&scrollform);
	tieregion.AddWidget(&tieform);

	test_vpart.AddWidget(&hierscroll);
	test_vpart.AddWidget(&divider);

#if TABLE_WINDOW
	event_context.AddWindow(&tablewindow);
	tablewindow.SetDoubleBuffer(TABLE_DOUBLE_BUFFER);
	tablewindow.Open("Test Table");
	tablewindow.AddWidget(&tablescope);
	tablescope.AddWidget(&table);
	tablescope.SetBounds(EW_BOUNDS_FILL_PARENT);
	table.SetBounds(EW_BOUNDS_FILL_PARENT);
#else
	test_vpart.AddWidget(&table);
#endif

	gfx_test.SetBounds(EW_HORIZONTAL,EW_BOUNDS_FILL_PARENT);

	hierscroll.AddWidget(&hierarchy);

	confirmform.AddWidget(&timestring);
	confirmform.AddWidget(&datestring);
	confirmform.AddWidget(&longstring);
	confirmform.AddWidget(&phonestring);
	confirmform.AddWidget(&ssnstring);
	confirmform.AddWidget(&floatstring);
	confirmform.AddWidget(&toggle);
	for(m=0;m<3;m++)
		confirmform.AddWidget(&radio[m]);

	divider.AddWidget(&rowform[0]);
	divider.AddWidget(&rowform[1]);
	divider.AddWidget(&autoform);
	divider.AddWidget(&radioscope);

	// ties two horizontal scroll bars together
	scrollregion.GetScrollBar(EW_HORIZONTAL)->SetTieList(&scrolltie);
	tieregion.GetScrollBar(EW_HORIZONTAL)->SetTieList(&scrolltie);
	tieregion.GetLocalLook()->SetElement(WDS_STATE_0,WDS_ELEMENT_SCROLLREGION_SCROLLABLE,WDS_SCROLLABLE_ON);


	// forms

	radioscope.SetBounds(EW_BOUNDS_USE_MAX);

	autoform.SetBounds(EW_BOUNDS_USE_MAX);
	autoform.SetMode(WDS_FORM_COLUMN);
	autoform.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_FORM_SEPARATOR,2);
//	autoform.SetSizing(EW_VERTICAL,WDS_SIZING_FIXED);

//	autoform.SetSizing(EW_HORIZONTAL,WDS_SIZING_FIXED);
//	autoform.SetSizing(EW_VERTICAL,WDS_SIZING_FIXED);
//	autoform.SetPrefSize(50,75);

	scrollform.SetSizing(EW_VERTICAL,WDS_SIZING_FIXED);
	scrollform.SetMode(WDS_FORM_COLUMN);
	scrollform.SetPrefSize(-1,200);
	scrollform.SetGeometry(50,100,150,200);

	rowform[1].SetCentering(WDS_FORM_CENTER_XY);
	for(m=0;m<2;m++)
		{
		rowform[m].SetMode(WDS_FORM_ROW);
		rowform[m].SetSizing(EW_HORIZONTAL,WDS_SIZING_MANUAL);
		rowform[m].SetSizing(EW_VERTICAL,WDS_SIZING_MANUAL);
		rowform[m].GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_FORM_SEPARATOR,1);
		rowform[m].SetSelectability(WDS_SELECT_SINGLE);
		}

	columnform.SetMode(WDS_FORM_COLUMN);
//	columnform.SetMode(WDS_FORM_ROW);

	bform.SetMode(WDS_FORM_COLUMN);
	bform.SetSizing(EW_VERTICAL,WDS_SIZING_MANUAL);

	scrollconfirm.GetLocalLook()->SetElement(WDS_STATE_1,WDS_ELEMENT_SCROLLREGION_SCROLLABLE,WDS_SCROLLABLE_OFF);
	confirmform.SetMode(WDS_FORM_COLUMN);
	confirmform.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_FORM_SEPARATOR,4);


	//// test table ////
	WDS_TableEntryList	table_entry_list;
	WDS_TableEntry		*table_entry;
	WDS_StringList		*stringlist;
	WDS_String			*string;

	table.SetTableEntryList(&table_entry_list);
	for(m=0;m<TEST_TABLE_ROWS;m++)
		{
		table_entry=new WDS_TableEntry;
		table_entry_list.AppendNode(table_entry);

		stringlist=new WDS_StringList;
		table_entry->SetStringList(stringlist);

		for(n=0;n<TEST_TABLE_COLUMNS;n++)
			{
			if(m==0)
				sprintf(message,"Header %d",n);
			else if(n==0)
				sprintf(message,"Row %d",m);
			else
				sprintf(message,"<%d,%d>",n,m);

			string=new WDS_String;
			string->NewBuffer(message);

#if TEST_TABLE_EDIT_PARTIAL
			if( (m==0 || (m>=20 && m<=23)) && (n==0 || (n>=2 && n<=4)) )
#else
			if( !(m<TEST_TABLE_HEADERS || n<TEST_TABLE_ROWLABELS) )
#endif
				string->SetEditable(TRUE);
			else
				string->SetEditable(FALSE);

			stringlist->AppendNode(string);
			}
		}

//	table.GetTableEntryList()->Dump(0);
	table.SetEditable(TEST_TABLE_EDITABLE);
	table.SetAllSelect(TEST_TABLE_ALLSELECT);
	table.SetHeaderLabels(TEST_TABLE_HEADERS);
	table.SetRowLabels(TEST_TABLE_ROWLABELS);
	table.RebuildColumns();

	for(m=0;m<TEST_TABLE_ROWS;m+=11)
		table.AlterRowHighlight(m,TRUE);

	table.SelectRow(17);


	WDS_StringList radiostringlist;
	for(m=0;m<RADIO_CHOICES;m++)
		{
		string=new WDS_String;

		sprintf(message,"Choice %d",m);
		string->NewBuffer(message);
		radiostringlist.AppendNode(string);
		}

	radioscope.SetStringList(&radiostringlist);

#if RADIO_ANNOTATE
	WDS_Form *radioform=radioscope.GetForm();

	radioform->GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,
																	WDS_WIDGET_ANNOTATION_TOP|WDS_WIDGET_ANNOTATION_IMBED);
	radioform->SetAnnotation("Radio\nForm");
	radioform->SetLocalAnnotationSize(0,0);
#endif


	// make string list for pick
	WDS_StringList pickstringlist[2];

	for(n=0;n<2;n++)
		for(m=0;m<(n? PICKLIST_ENTRIES2:PICKLIST_ENTRIES1);m++)
			{
			string=new WDS_String;

			sprintf(message,"%sick %d",n? "P":"p",m);
			string->NewBuffer(message);
			pickstringlist[n].AppendNode(string);
			}

	partpick.GetPickList()->SetStringList(&pickstringlist[0]);
	shortpick.GetPickList()->SetStringList(&pickstringlist[0]);
	longpick.GetPickList()->SetStringList(&pickstringlist[1]);

	longpick.GetPickList()->AlterRowHighlight(22,TRUE);


#if MAKE_MENU
	// menus
	WDS_MenuNode *node1,*node2,*node3,*node4;
	for(k=0;k<3;k++)
		{
		node1=new WDS_MenuNode;
		menubar.AddWidget(node1);

		sprintf(message,"Menu %d",k);
		node1->SetLabel(message);
		node1->SetName(message);

		if(k<2)
			for(m=0;m<7;m++)
				{
				node2=new WDS_MenuNode;
				node1->AddWidget(node2);

				sprintf(message,"Sub %d,%d ",k,m);
				for(n=0;n<m;n++)
					strcat(message,"x");
				node2->SetLabel(message);
				node2->SetName(message);

				if( ((m+k)%3) == 1 )
					for(n=0;n<2;n++)
						{
						node3=new WDS_MenuNode;
						node2->AddWidget(node3);

						sprintf(message,"Sub %d,%d,%d",k,m,n);
						node3->SetLabel(message);
						node3->SetName(message);

						for(j=0;j<2;j++)
							{
							node4=new WDS_MenuNode;
							node3->AddWidget(node4);

							sprintf(message,"Sub %d,%d,%d,%d",k,m,n,j);
							node4->SetLabel(message);
							node4->SetName(message);
							}
						}
				}
		}
#endif


	//// various scattered buttons ////
	for(m=0;m<BUTTONS;m++)
		{
//		sprintf(message,"Number %d",m);
		sprintf(message,"button %d",m);
		button[m].SetName(message);
		sprintf(message,"Tip %d",m);
		button[m].SetTipText(message);
		sprintf(message,"%d Border Style",m);
		button[m].SetLabel(message);

		button[m].SetTipMode(WDS_TIP_ON);

		button[m].SetGeometry((m+1)*25,(m+1)*35,30+2*m+10*(m%2),20+m+8*((m+1)%2));

		if(m>BUTTONS*3/4)
			rowform[1].AddWidget(&button[m]);
		else if(m>BUTTONS/2)
			rowform[0].AddWidget(&button[m]);
		else if(m>8)
			scrollform.AddWidget(&button[m]);
		else if(m>3)
			tieform.AddWidget(&button[m]);
		else
			autoform.AddWidget(&button[m]);

		if(!(m%10))
			{
			button[m].SetDormant(TRUE);
			button[m].SetGreyOut(TRUE);
			}

		button[m].SetLocalElement(WDS_STATE_ALL,WDS_ELEMENT_DEPTH,6);

		button[m].SetLocalElement(WDS_STATE_0,WDS_ELEMENT_BORDER,m%32);
		button[m].SetLocalElement(WDS_STATE_1,WDS_ELEMENT_BORDER,(m+1)%32|(WDS_BEVELBOX_OMIT_RIGHT<<(m%4)));

		if(m==25)
			{
			button[m].GetLocalLook()->SetElement(WDS_STATE_0,WDS_ELEMENT_DEPTH,2);
			button[m].SetLocalElement(WDS_STATE_0,WDS_ELEMENT_BORDER,
										WDS_BEVELBOX_LABEL_LEFT|WDS_BEVELBOX_IN|WDS_BEVELBOX_SMOOTH|WDS_BEVELBOX_ELLIPTICAL);
			}

		if(m==26)
			{
			button[m].SetLocalElement(WDS_STATE_0,WDS_ELEMENT_BORDER,WDS_BEVELBOX_LABEL_LEFT|WDS_BEVELBOX_OUT);
			button[m].SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOPREF_Y);
			}
		}

	char lookname[11][32]={
		"Hard-Wired\nWin32",
		"Imbedded\nAthena",
		"Imbedded\nHybrid",
		"Imbedded\nMotif",
		"Imbedded\nSimple",
		"Imbedded\nWin32",
		"Load\nAthena",
		"Load Hybrid",
		"Load\nMotif",
		"Load\nSimple",
		"Load\nWin32"
		};

	for(m=0;m<11;m++)
		{
		abutton= new WDS_Button;
		abutton->SetLabel(lookname[m]);
		abutton->SetName(lookname[m]);
		activation.SetCode(m);
		abutton->SetStateChangeCallback(WDS_STATE_1,&look_change,&activation);
//		abutton->SetGeometry(0,0,50,32);
		abutton->SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOPREF);

		columnform.AddWidget(abutton);
		columnform.SetUniformity(WDS_UNIFORM_BOTH);
		}

	// pop-up window lay-out
	bscope.AddWidget(&bpartition);
	bpartition.AddWidget(&amenubar);
	bpartition.AddWidget(&bform);
	bform.AddWidget(&popup);
	bform.AddWidget(popwin);
	blesswindow.AddWidget(&bscope);

	// partwindow
	partwindow->AddWidget(&partscope);
	partscope.AddWidget(&fixpartition);
#if !PART_LONECHILD_TEST
	fixpartition.AddWidget(&partform);
#endif
	fixpartition.AddWidget(&controlform);
	partform.AddWidget(&partradioscope);
	partform.AddWidget(&partentryform);

	WDS_StringList partradiostringlist;
	partradioscope.SetStringList(&partradiostringlist);
	partradioscope.SetAutoSizing(TRUE);
	partradioscope.GetForm()->SetAnnotation("Radios");
	partradioscope.GetForm()->GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,
																		WDS_WIDGET_ANNOTATION_TOP|WDS_WIDGET_ANNOTATION_IMBED);

	partentryform.SetAnnotation("Entries");
	partentryform.GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,
																		WDS_WIDGET_ANNOTATION_TOP|WDS_WIDGET_ANNOTATION_IMBED);

	long prefwidth=mainwindow.StringWidth("good long space for a reasonable comment");

	activation.Reset();
	activation.SetObject(partwindow);

	char partlabel[3][32]={"Close","Increment\nCursor","EndLoop()"};

	controlform.SetUniformity(WDS_UNIFORM_BOTH);

	for(m=0;m<3;m++)
		{
		string=new WDS_String;
		sprintf(message,"Choice %d",m);
		string->NewBuffer(message);
		partradiostringlist.AppendNode(string);

		partentryform.AddWidget(&partentry[m]);

		partentry[m].SetString("String");
		partentry[m].SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE_Y);
		partentry[m].SetGeometry(0,0,prefwidth,0);

		controlform.AddWidget(&partbutton[m]);
		partbutton[m].SetLabel(partlabel[m]);
		partbutton[m].SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOPREF);

		activation.SetCode(m);
		partbutton[m].SetStateChangeCallback(WDS_STATE_0,&part_callback,&activation);
		}

	partentryform.AddWidget(&partpick);

#if PART_LONECHILD_TEST
	partscope.SetBounds(EW_BOUNDS_FILL_PARENT);
	fixpartition.SetBounds(EW_BOUNDS_FILL_PARENT);
#else
	partwindow->SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);
	partscope.SetNodeSizing(EW_NODE_SIZING_FIT_CHILD);
	fixpartition.SetNodeSizing(EW_NODE_SIZING_FIT_CHILD_X);
#endif
	fixpartition.SetDirection(WDS_VERTICAL);
	partform.SetMode(WDS_FORM_SINGLE_COLUMN);
	partentryform.SetMode(WDS_FORM_SINGLE_COLUMN);
	controlform.SetMode(WDS_FORM_SINGLE_ROW);

/* 	mainwindow.SetUnderline(TRUE); */

//	mainwindow.Refresh();
//	blesswindow.Refresh();

#if TRUE
	// under WBA, envp is not directly available
	WBA_Argument *argument;

	const char *user=NULL;
	const char *host=NULL;
	const char *hosttype=NULL;

	if(argument=WBA_ARGS.Find("USER"))
		user=argument->GetValue();
	if(argument=WBA_ARGS.Find("HOST"))
		host=argument->GetValue();
	if(argument=WBA_ARGS.Find("HOSTTYPE"))
		hosttype=argument->GetValue();

#else
	// get some environment and font information and print out
	// ewGetEnvironmentVariable() is nothing special, just convenient
	char *user=ewGetEnvironmentVariable(envp,"USER");
	char *host=ewGetEnvironmentVariable(envp,"HOST");
	char *hosttype=ewGetEnvironmentVariable(envp,"HOSTTYPE");
#endif

	sprintf(message,"test run by %s on %s (type: %s)",
			user? user: "(null)",
			host? host: "(null)",
			hosttype? hosttype: "(null)"
			);
	EW_PRINT(EW_APP,EW_LOG,message);

#if TRUE
	EW_PRINT(EW_APP,EW_LOG,"Test LOG message");
	EW_PRINT(EW_APP,EW_WARN,"Test WARN message");
	EW_PRINT(EW_APP,EW_ERROR,"Test ERROR message");
//	EW_PRINT(EW_APP,EW_FATAL,"Test FATAL message");
//	EW_PRINT(EW_APP,EW_CRASH,"Test CRASH message");
#endif

#if TRUE
	EW_PRINT(EW_APP,EW_LOG,"font height=%d ascent=%d descent=%d",
			mainwindow.FindFontAttribute(EW_FONTATTR_HEIGHT),
			mainwindow.FindFontAttribute(EW_FONTATTR_ASCENT),
			mainwindow.FindFontAttribute(EW_FONTATTR_DESCENT)
			);
#endif

	if(TYPE_CHECK)
		{
		check_widget=&check_button;

		sprintf(message,"check_widget.GetTypeCode() = 0x%x (a button as a widget)",check_widget->GetTypeCode());
		EW_PRINT(EW_APP,EW_LOG,message);
		sprintf(message,"check_button.GetTypeCode() = 0x%x",check_button.GetTypeCode());
		EW_PRINT(EW_APP,EW_LOG,message);
		sprintf(message,"WDS_Button::GetTypeCodeStatic() = 0x%x",WDS_Button::GetTypeCodeStatic());
		EW_PRINT(EW_APP,EW_LOG,message);
		sprintf(message,"check_scrollbarbutton.GetTypeCode() = 0x%x",check_scrollbarbutton.GetTypeCode());
		EW_PRINT(EW_APP,EW_LOG,message);
		sprintf(message,"WDS_ScrollBarButton::GetTypeCodeStatic() = 0x%x",WDS_ScrollBarButton::GetTypeCodeStatic());
		EW_PRINT(EW_APP,EW_LOG,message);
		sprintf(message,"check_hierarchyform.GetTypeCode() = 0x%x",check_hierarchyform.GetTypeCode());
		EW_PRINT(EW_APP,EW_LOG,message);
		sprintf(message,"WDS_HierarchyForm::GetTypeCodeStatic() = 0x%x",WDS_HierarchyForm::GetTypeCodeStatic());
		EW_PRINT(EW_APP,EW_LOG,message);
		}

	// EW_ITEM_TIMER event frequency in milliseconds
	if(timers)
		{
		event_context.AppendTimerInterval(2,1000,&mainwindow);
		event_context.AppendTimerInterval(3,400,&mainwindow);
		}

	// reset window background
	mainwindow.SetColor(EW_COLOR_BACKGROUND,button1.GetDefaultLooks()->LookupLook(WDS_TYPE_DEFAULT,0)->
																			GetElement(WDS_STATE_ALL,EW_COLOR_BACKGROUND));


	// continuous loop
	event_context.BeginLoop();

	ewGlobalOutput.PrintLine("wds_test","EW Event Loop Completed");
	}
