/******************************************************************************

Coral Tree wds/wwidget.h
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


// no ';' at end of each, use from call
#define WDS_DEFINE_LOOK(typename)											\
						WDS_Look		*typename::type_look_static=NULL;	\
						unsigned long	typename::type_count_static=0

#define WDS_DECLARE_LOOK(typename)											\
			public:															\
				static WDS_Look *GetGlobalLookStatic(void)					\
					{														\
					WDS_Look *look;											\
																			\
					if(GetLookCountStatic()==GetLookCounter())				\
						{													\
						look=GetGlobalLookStoredStatic();					\
						}													\
					else													\
						{													\
						look=GetLooks()->LookupLook(GetTypeCodeStatic(),0);\
						SetGlobalLookStoredStatic(look);					\
						SetLookCountStatic(GetLookCounter());				\
																			\
/*printf("lookup 0x%x (%s) -> 0x%x\n",GetTypeCodeStatic(),#typename,look);*/ \
						}													\
					return look;											\
					}														\
				virtual	WDS_Look *GetGlobalLook(void)						\
								{											\
								return GetGlobalLookStatic();				\
								};											\
																			\
				static	WDS_Look *GetGlobalLookStoredStatic(void)			\
								{											\
								return type_look_static;					\
								};											\
				virtual	WDS_Look *GetGlobalLookStored(void)					\
								{											\
								return GetGlobalLookStoredStatic();			\
								};											\
				static	void SetGlobalLookStoredStatic(WDS_Look *set)		\
								{											\
								type_look_static=set;						\
								};											\
				virtual	void SetGlobalLookStored(WDS_Look *set)				\
								{											\
								SetGlobalLookStoredStatic(set);				\
								};											\
																			\
				static	unsigned long GetLookCountStatic(void)				\
								{											\
								return type_count_static;					\
								};											\
				virtual	unsigned long GetLookCount(void)					\
								{											\
								return GetLookCountStatic();				\
								};											\
				static	void SetLookCountStatic(unsigned long set)			\
								{											\
								type_count_static=set;						\
								};											\
				virtual	void SetLookCount(unsigned long set)				\
								{											\
								SetLookCountStatic(set);					\
								};											\
			private:														\
				static	WDS_Look		*type_look_static;					\
				static	unsigned long	type_count_static

// in .cc
#define WDS_DEFINE_TYPE(typename)											\
				WDS_DEFINE_LOOK(typename);									\
				WDS_Typecode	typename::type_code_static=0


// in .h
#define WDS_DECLARE_TYPE(typename)										\
			public:															\
				static	WDS_Typecode GetTypeCodeStatic(void)				\
							{ return (WDS_Typecode)(&type_code_static);	};	\
				virtual	WDS_Typecode GetTypeCode(void)						\
							{ return GetTypeCodeStatic(); };				\
				virtual	char *GetTypeName(void)								\
							{ return #typename; };							\
			private:														\
				static	WDS_Typecode	type_code_static;					\
				GPL_OBJECT_INTERFACE(typename);								\
				WDS_DECLARE_LOOK(typename)

#define	WDS_WIDGET_IS(typename)	GetTypeCode()==typename::GetTypeCodeStatic()


// WDS_Widget::SetTipMode(?)
#define WDS_TIP_OFF			0x00
#define WDS_TIP_ON			0x01


// WDS_Widget::SetAnnotationMode(?)
#define WDS_WIDGET_ANNOTATION_OFF			0x00
#define WDS_WIDGET_ANNOTATION_ON			0x01
#define WDS_WIDGET_ANNOTATION_TOP_FLAG		0x02
#define WDS_WIDGET_ANNOTATION_TOP			(WDS_WIDGET_ANNOTATION_TOP_FLAG|\
													WDS_WIDGET_ANNOTATION_ON)
#define WDS_WIDGET_ANNOTATION_LEFT_FLAG		0x04
#define WDS_WIDGET_ANNOTATION_LEFT			(WDS_WIDGET_ANNOTATION_LEFT_FLAG|\
													WDS_WIDGET_ANNOTATION_ON)
#define WDS_WIDGET_ANNOTATION_RIGHT_FLAG	0x08
#define WDS_WIDGET_ANNOTATION_RIGHT			(WDS_WIDGET_ANNOTATION_RIGHT_FLAG|\
													WDS_WIDGET_ANNOTATION_ON)
#define WDS_WIDGET_ANNOTATION_IMBED_FLAG	0x10
#define WDS_WIDGET_ANNOTATION_IMBED			(WDS_WIDGET_ANNOTATION_IMBED_FLAG|\
													WDS_WIDGET_ANNOTATION_ON)
#define WDS_WIDGET_ANNOTATION_SQUARE_WIDGET	0x20



#define WDS_WIDGET_ANNOTATE_EVERYTHING	FALSE
#define WDS_WIDGET_ANNOTATE_EVERY_MINY	32


// WDS_Widget::SetGeneralFlags(?)
#define WDS_WIDGET_FLAGS_NONE		0

#define WDS_WIDGET_FLAGS_AUTOPREF_X	1
#define WDS_WIDGET_FLAGS_AUTOFIX_X	2
#define WDS_WIDGET_FLAGS_AUTOSIZE_X	(WDS_WIDGET_FLAGS_AUTOPREF_X| \
										WDS_WIDGET_FLAGS_AUTOFIX_X)

#define WDS_WIDGET_FLAGS_AUTOPREF_Y	4
#define WDS_WIDGET_FLAGS_AUTOFIX_Y	8
#define WDS_WIDGET_FLAGS_AUTOSIZE_Y	(WDS_WIDGET_FLAGS_AUTOPREF_Y| \
										WDS_WIDGET_FLAGS_AUTOFIX_Y)

#define WDS_WIDGET_FLAGS_AUTOPREF	( WDS_WIDGET_FLAGS_AUTOPREF_X | \
										WDS_WIDGET_FLAGS_AUTOPREF_Y )
#define WDS_WIDGET_FLAGS_AUTOFIX	( WDS_WIDGET_FLAGS_AUTOFIX_X | \
										WDS_WIDGET_FLAGS_AUTOFIX_Y )
#define WDS_WIDGET_FLAGS_AUTOSIZE	( WDS_WIDGET_FLAGS_AUTOSIZE_X | \
										WDS_WIDGET_FLAGS_AUTOSIZE_Y )

#define WDS_WIDGET_FLAGS_DEFAULT	WDS_WIDGET_FLAGS_NONE


// EW_Event EW_EVENT_WIDGET psuedo-items
#define WDS_ITEM_EXCLUDE_BUTTON	EW_ITEM(0)
#define WDS_ITEM_SELECTION		EW_ITEM(1)
#define WDS_ITEM_POPUP			EW_ITEM(2)



/******************************************************************************
class	WDS_Activation

	sent to activation/deactivation callbacks

Keywords:
******************************************************************************/
class WDS_Activation
	{
	private:

		WDS_Widget	*this_widget;
		WDS_Widget	*target_widget;
		EW_Event	*cause;

		void		*object;

		long		state;
		long		code;

	public:

					WDS_Activation(void)	{ Reset(); };

		void		Reset(void)
						{
						this_widget=NULL;
						target_widget=NULL;
						cause=NULL;
						object=NULL;
						code=0;
						state=0;
						};

		void		SetThisWidget(WDS_Widget *set)	{ this_widget=set; };
		WDS_Widget	*GetThisWidget(void)			{ return this_widget; };

		void		SetTargetWidget(WDS_Widget *set){ target_widget=set; };
		WDS_Widget	*GetTargetWidget(void)			{ return target_widget; };

		void		SetEvent(EW_Event *set)			{ cause=set; };
		EW_Event	*GetEvent(void)					{ return cause; };

		void		SetObject(void *set)			{ object=set; };
		void		*GetObject(void)				{ return object; };

		void		SetCode(long set)				{ code=set; };
		long		GetCode(void)					{ return code; };

		void		SetState(long set)				{ state=set; };
		long		GetState(void)					{ return state; };

		void		CopyFrom(WDS_Activation *source)
						{
						SetThisWidget(source->GetThisWidget());
						SetTargetWidget(source->GetTargetWidget());
						SetEvent(source->GetEvent());
						SetObject(source->GetObject());
						SetCode(source->GetCode());
						SetState(source->GetState());
						};

		void		Dump(long arg);
	};



/******************************************************************************
class	WDS_TipHandler

******************************************************************************/
class WDS_TipHandler : public EW_WSHandler
{
	protected:
		EW_Event *ws_event;
		WDS_Widget *ws_widget;
	public:
		WDS_TipHandler()
		{
			ws_event = NULL;
			ws_widget = NULL;
		}
virtual	~WDS_TipHandler(){}

		EW_Event *GetEvent(){return ws_event;}
		void SetEvent(EW_Event *e){ ws_event = e; }

		WDS_Widget *GetWidget(){return ws_widget;}
		void SetWidget(WDS_Widget *w){ ws_widget = w; }
		
};


/******************************************************************************
class	WDS_TypeCodeBase

******************************************************************************/
class WDS_TypeCodeBase
	{
	public:

virtual	WDS_Typecode	GetTypeCode(void)	{ return WDS_TYPE_UNDEFINED; };
virtual	char			*GetTypeName(void)	{ return "Undefined"; };
	};



/******************************************************************************
class	WDS_Validifier : public long, public DoubleNode

******************************************************************************/
class WDS_Validifier : public GPL_DoubleNode
	{
	private:
		long	valid;

	public:
				WDS_Validifier(void)	{ valid=FALSE; };

		void	SetValid(long set);
		long	GetValid(void)			{ return valid; };
	};



/******************************************************************************
class	WDS_ValidifierList : public GPL_DoubleList<WDS_Validifier>

******************************************************************************/
class WDS_ValidifierList : public GPL_DoubleList<WDS_Validifier>
	{
	public:
				WDS_ValidifierList(void) { };
virtual			~WDS_ValidifierList(void);

		void	Prune(void);
	};



/******************************************************************************
class	WDS_Widget

	WDS-specific extensions to standard EW_Widget class

Keywords:
******************************************************************************/
class WDS_Widget : public EW_Widget, public GPL_Object, public WDS_TypeCodeBase
    {
	GPL_OBJECT_INTERFACE(WDS_Widget);

	public:

static	WDS_Typecode	GetTypeCodeStatic(void)	{ return WDS_TYPE_DEFAULT; };
	
	WDS_DECLARE_LOOK(WDS_Widget);

    private:

		WDS_Look			look;			// per-widget color/geometry specs
static	WDS_Looks			*current_looks;	// general & per class specs
static	WDS_Looks			default_looks;	// general & per class specs

static	long				looks_initialized;
static	long				mask_initialized;

static	unsigned long		lookcounter;

static	WDS_TipWindow		*tipwindow;

		// array of function pointers
		void	(*activator[WDS_STATES])(WDS_Activation *activation);

		WDS_Image			*image[WDS_STATES];

		WDS_TieList			*tielist;			// list of tie widgets
		WDS_ValidifierList	validifierlist;

		WDS_Help			help;				// help context

		long				tipmode;
		WDS_String			tipstring;			// annotation string
		WDS_String			annotation;			// annotation string
		WDS_String			label;				// label string
		long				annotatable;

		long				general_flags;
		long				widgetstate;

		void		ConvertName(long maxlen,char *in,char *out);

	protected:

		WDS_Activation activation[WDS_STATES];

					// to be set in a widget's constructor
		void		SetAnnotatable(long set)	{ annotatable=set; };

		void 		LimitDimensionsToScreen(long *srcx,long *srcy,
								long *destx,long *desty,long *lenx,long *leny);

    public:

					WDS_Widget(void);
virtual				~WDS_Widget(void);

virtual	WDS_Typecode	GetTypeCode(void)		{ return WDS_TYPE_DEFAULT; };
virtual	char			*GetTypeName(void)		{ return "WDS_Widget"; };

		long		GetAnnotatable(void)		{ return annotatable; };

		void		GetAnnotationGeometry(long *x,long *y,long *sx,long *sy);
		void		GetAnnotationGeometryFast(WDS_Look_Flash *lookflash,
									long *x,long *y,long *sx,long *sy,
									long subx,long suby,long subsx,long subsy);

		void		GetAnnotationSize(long annx,long anny,long maxx,long maxy,
							long *annox,long *annoy);
		void		GetAnnotationSizeFast(WDS_Look_Flash *lookflash,
							long annx,long anny,long maxx,long maxy,
							long *annox,long *annoy);

virtual	void		Draw(EW_Redraw *redraw)		{ BevelBox(TRUE,TRUE,&label); };
virtual	long		PreOperate(EW_Event *event);
virtual	long		Resize(EW_Resize *resize);
virtual	long		CheckSize(EW_Resize *resize);
		void		ConfirmTypecode(void);

		void		AdjustSizeByFlags(void);
		void		FindSizeByFlags(long state,long general_flags,long *size);

		WDS_String	*GetLabelString(void)		{ return &label; };
		char		*GetLabel(void)				{ return label.Buffer(); };
		void		SetLabel(char *a_string)
						{
						if(label.NewBuffer(a_string) == FALSE)
							EW_PRINT(EW_OS,EW_ERROR,
										"could not allocate string memory");

						};

		long		CanSmartDraw(void);

		void		ModifyPointDirection(long state,long vertical,long back);

		WDS_Widget	*FindWidgetType(WDS_Typecode code,long shallowest);

		void		SetWidgetState(long set)	{ widgetstate=set; };
		long		GetWidgetState(void)		{ return widgetstate; };

		long		GetGeneralFlags(void)		{ return general_flags; };
		void		SetGeneralFlags(long set)	{ general_flags=set; };
		void		SetGeneralFlagsIncluding(long set)
									{ general_flags |= set; };
		void		SetGeneralFlagsExcluding(long set)
									{ general_flags ^= (general_flags&set); };

		void		SetTipMode(long set)	{ tipmode=set; };
		long		GetTipMode(void)		{ return tipmode; };

		void		SetTipText(char *text)	{ tipstring.NewBuffer(text); };
		char		*GetTipText(void)		{ return tipstring.Buffer(); };
		WDS_String	*GetTipString(void)		{ return &tipstring; };

		void		CheckForTip(EW_Event *idleevent);

static	WDS_TipWindow	*GetTipWindow(void)		{ return tipwindow; };
static	void			NullifyTipWindow(void)	{ tipwindow=NULL; };

		WDS_Help	*GetHelp(void)				{ return &help; };
		long		SpawnHelp(void)
						{
						if(GetRootWindow())
							return help.Spawn(
									GetRootWindow()->GetDisplayIdentifier());
						else
							return FALSE;
						};

static	void			IncrementLookCounter(void);
static	unsigned long	GetLookCounter(void)		{ return lookcounter; };

		WDS_Look	*GetLocalLook(void)			{ return &look; };

static	void		SetLooks(WDS_Looks *set)	{ current_looks=set; };
static	WDS_Looks	*GetDefaultLooks(void)		{ return &default_looks; };
static	WDS_Looks	*GetLooks(void)
						{
						if(current_looks)
							return current_looks;
						else
							return &default_looks;
						};

		long		InterpretElement(long state,long element);
		long		InterpretElementFast(WDS_Look_Flash *lookflash,
													long state,long element);

		void		SetAnnotation(char *set)
						{ annotation.NewBuffer(set); };
		WDS_String	*GetAnnotation(void)
						{
#if WDS_WIDGET_ANNOTATE_EVERYTHING
						annotation.NewBuffer("Widget");
#endif
						return &annotation;
						};


					// annotation convenience
		void		SetLocalAnnotationSize(long x,long y)
						{
						GetLocalLook()->SetElement(WDS_STATE_ALL,
												WDS_ELEMENT_ANNOTATION_X,x);
						GetLocalLook()->SetElement(WDS_STATE_ALL,
												WDS_ELEMENT_ANNOTATION_Y,y);
						};


		void		GetRegion(long include_border,
									long *rx,long *ry,long *rsx,long *rsy,
									long subx,long suby,long subsx,long subsy);
		void		GetRegionFast(WDS_Look_Flash *lookflash,long include_border,
									long *rx,long *ry,long *rsx,long *rsy,
									long subx,long suby,long subsx,long subsy);
		void		GetRegion(long include_border,
							long *rx,long *ry,long *rsx,long *rsy)
						{
						GetRegionFast(NULL,include_border,rx,ry,rsx,rsy);
						};
		void		GetRegionFast(WDS_Look_Flash *lookflash,
							long include_border,
							long *rx,long *ry,long *rsx,long *rsy)
						{
						long tx,ty,tsx,tsy;

						GetTrueGeometry(&tx,&ty,&tsx,&tsy);
						GetRegionFast(lookflash,include_border,rx,ry,rsx,rsy,
																tx,ty,tsx,tsy);
						};

		long		InRegion(EW_Event *event,long include_border,
									long subx,long suby,long subsx,long subsy)
						{
						long rx,ry,rsx,rsy;

						GetRegion(include_border,&rx,&ry,&rsx,&rsy,
														subx,suby,subsx,subsy);
						return event->InBounds(rx,ry,rsx,rsy);
						};
		long		InRegion(EW_Event *event,long include_border)
						{
						long tx,ty,tsx,tsy;

						GetTrueGeometry(&tx,&ty,&tsx,&tsy);
						return InRegion(event,include_border,tx,ty,tsx,tsy);
						};

		void		BevelBox(long force_clear,long annotate,
									WDS_String *bevelstring,
									long subx,long suby,long subsx,long subsy);
		void		BevelBox(long force_clear,long annotate,
									WDS_String *bevelstring)
						{
						long tx,ty,tsx,tsy;

						GetTrueGeometry(&tx,&ty,&tsx,&tsy);
						BevelBox(force_clear,annotate,bevelstring,
																tx,ty,tsx,tsy);
						};
		void		BevelBox(long force_clear,WDS_String *bevelstring)
						{
						BevelBox(force_clear,TRUE,bevelstring);
						};

		void		ClearBackground(void)
						{
						EW_Widget::ClearBackground(InterpretElement(
										GetWidgetState(),EW_COLOR_BACKGROUND));
						};


		void		SetTieList(WDS_TieList *set);
		WDS_TieList	*GetTieList(void)				{ return tielist; };

		void		ChangeTieValues(WDS_TieValues *tievalues);
virtual	void		TieValuesChanged(WDS_TieValues *tievalues)
						{
						// hush
						tievalues=tievalues;

						EW_PRINT(EW_WIDGET,EW_WARN,
						"WDS_Widget::TieValuesChanged() virtual not replaced");
						}

		void		ChangeStateIfDifferent(long newstate,EW_Event *event)
						{
						if(newstate!=widgetstate)
							ChangeState(newstate,event);
						};
virtual	void		ChangeState(long newstate,EW_Event *event)
						{
						widgetstate=newstate;

						activation[widgetstate].SetThisWidget(this);
						activation[widgetstate].SetState(widgetstate);
						activation[widgetstate].SetEvent(event);

						if(activator[widgetstate])
							(*activator[widgetstate])(&activation[widgetstate]);
						};

		void		SetStateChangeCallback(long for_state,
							void (*fp)(WDS_Activation *),WDS_Activation *act)
						{
						long m,start=for_state,end=for_state+1;

						if(for_state==WDS_STATE_ALL)
							{
							start=0;
							end=WDS_STATES;
							}

						for(m=start;m<end;m++)
							{
							activator[m]=fp;
							if(act)
								activation[m].CopyFrom(act);
							}
						};

		void		SetImage(long for_state,WDS_Image *set)
						{
						long m,start=for_state,end=for_state+1;

						if(for_state==WDS_STATE_ALL)
							{
							start=0;
							end=WDS_STATES;
							}

						for(m=start;m<end;m++)
							image[m]=set;
						};
		void		GetMaxImageSize(long *x,long *y)
						{
						long lx,ly;

						*x=0;
						*y=0;

						long m;
						for(m=0;m<WDS_STATES;m++)
							if(image[m])
								{
								image[m]->GetSize(&lx,&ly);

								if(*x<lx)
									*x=lx;
								if(*y<ly)
									*y=ly;
								}
						};

virtual	void		DumpString(void);
virtual	void		DumpInfo(long level);

					// allow widgets to suggest others to change thier prefsize
					// by default, ignore
virtual	void		SuggestPrefSize(long suggestx,long suggesty)	{ };

/////////////////// convenience functions ///////////////////

		void		RegisterEventBypass(long on,EW_Event *event,
											long bypassflags,long shallowest);

		void		SetPrefGeometry(long x,long y,long w,long h)
						{
						SetGeometry(x,y,w,h);
//						SetPrefSize(w,h);
						};

		void		SetLocalElement(long forstate,long attribute,long set)
						{ GetLocalLook()->SetElement(forstate,attribute,set); };
		long		GetLocalElement(long forstate,long attribute)
						{ return GetLocalLook()->GetElement(forstate,attribute); };
	};
