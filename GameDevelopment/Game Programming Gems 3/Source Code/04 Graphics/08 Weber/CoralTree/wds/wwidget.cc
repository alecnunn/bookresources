/******************************************************************************

Coral Tree wds/wwidget.cc
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



#include "wds.h"


#define WDS_INTERPRET_DEBUG			FALSE
#define WDS_ANNOTATION_FILL			FALSE

#define WDS_FIND_DEBUG				FALSE
#define WDS_DENYTIP_DEBUG			FALSE
#define WDS_WIDGET_REGISTER_DEBUG	FALSE


WBA_INIT_DEFINITION(WDS);

WDS_DEFINE_LOOK(WDS_Widget);



WDS_TipWindow	*WDS_Widget::tipwindow=NULL;
WDS_Looks		WDS_Widget::default_looks;
WDS_Looks		*WDS_Widget::current_looks=NULL;
long			WDS_Widget::looks_initialized=FALSE;
long			WDS_Widget::mask_initialized=FALSE;
unsigned long	WDS_Widget::lookcounter=1;


/******************************************************************************
void	WDS_Activation::Dump(long arg)

******************************************************************************/
void WDS_Activation::Dump(long arg)
	{
	EW_PRINT(EW_WIDGET,EW_LOG,"this=0x%x arg=0x%x this_widget=0x%x target=0x%x object=0x%x code=%d state=%d on cause:",
									(long)this,arg,this_widget,target_widget,object,code,state);
	cause->Print(0);
	}


/******************************************************************************
	WDS_Widget::WDS_Widget(void)

******************************************************************************/
WDS_Widget::WDS_Widget(void)
	{
	SetName(GetTypeName());
	SetWidgetState(WDS_STATE_0);
	SetGeneralFlags(WDS_WIDGET_FLAGS_DEFAULT);
	SetTipMode(WDS_TIP_OFF);

	tielist = NULL;

	long m;
	for(m=0;m<WDS_STATES;m++)
		{
		activator[m]=NULL;
		image[m]=NULL;
		}

#if WDS_WIDGET_ANNOTATE_EVERYTHING
	GetLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_ANNOTATION_MODE,WDS_WIDGET_ANNOTATION_TOP);
	SetLocalAnnotationSize(0,16);

	long minx,miny;
	GetMinSize(&minx,&miny);
	SetMinSize(minx,WDS_WIDGET_ANNOTATE_EVERY_MINY);
#endif

	SetAnnotatable(FALSE);

	if(!looks_initialized)
		{
		looks_initialized=TRUE;

		default_looks.Initialize(WDS_LOOK_DEFAULT,NULL);
		}

	if(!mask_initialized)
		{
		GetDefaultEventMask()->SetSource(EW_EVENT_COMMON);
		GetDefaultEventMask()->SetWidgetFlags(EW_WIDGET_FLAGS_NONE);
		}

	if(tipwindow==NULL)
		{
		// prevent loop
		tipwindow=(WDS_TipWindow *)0x01;

		tipwindow=new WDS_TipWindow();

		// should get deleted when event context destructs
		}
	}



/******************************************************************************
	WDS_Widget::~WDS_Widget(void)

******************************************************************************/
WDS_Widget::~WDS_Widget(void)
	{
	if(GetTipWindow())
		{
		if(GetTipWindow()->GetSource()==this)
			GetTipWindow()->Deactivate();
		if(GetTipWindow()->GetForbidden()==this)
			GetTipWindow()->SetForbidden(NULL);
		}

	tielist=NULL;

#if FALSE
	// as a safety precaution, deregister bypasses from any scopes we can find
	EW_Event event;

	event.SetSIS(EW_EVENT_ANY,EW_ITEM_ALL,EW_STATE_ANY);

	RegisterEventBypass(FALSE,&event,0,TRUE);
	RegisterEventBypass(FALSE,&event,0,FALSE);
#endif
	}


/******************************************************************************
long	WDS_Widget::CanSmartDraw(void)

******************************************************************************/
long WDS_Widget::CanSmartDraw(void)
	{
	return (WDS_SMART_REDRAW && GetRootWindow()->GetMapped() && !GetRootWindow()->GetDoubleBuffer());
	}



/******************************************************************************
void	WDS_Widget::IncrementLookCounter(void)

******************************************************************************/
void WDS_Widget::IncrementLookCounter(void)
	{
	lookcounter++;
	if(lookcounter==0)
		lookcounter=1;
	}



/******************************************************************************
void	WDS_Widget::ConfirmTypecode(void)

******************************************************************************/
void WDS_Widget::ConfirmTypecode(void)
	{
	// update NULL typecode in look list
	WDS_Look *look=GetGlobalLook();
	if(look==NULL)
		{
		// if found, invalidate cache
		if(GetLooks()->AssignTypecodeToTypeName(GetTypeCode(),GetClassId(),GetTypeName()))
			SetLookCount(0);
		}
	}



/******************************************************************************
void	WDS_Widget::AdjustSizeByFlags(void)

******************************************************************************/
void WDS_Widget::AdjustSizeByFlags(void)
	{
	long state=GetWidgetState();
	ConfirmTypecode();

	long size[2],psize[2],minsize[2],maxsize[2];
	GetPrefSize(&psize[0],&psize[1]);
	GetMinSize(&minsize[0],&minsize[1]);
	GetMaxSize(&maxsize[0],&maxsize[1]);

	long lookx=InterpretElement(state,WDS_ELEMENT_PREF_X);
	long looky=InterpretElement(state,WDS_ELEMENT_PREF_Y);

	if(lookx)
		psize[0]=lookx;
	if(looky)
		psize[1]=looky;

	long annmode=InterpretElement(state,WDS_ELEMENT_ANNOTATION_MODE);

	if((GetGeneralFlags()&WDS_WIDGET_FLAGS_AUTOSIZE) || annmode)
		{
		GetSize(&size[0],&size[1]);
		FindSizeByFlags(state,GetGeneralFlags(),size);
		SetSize(size[0],size[1]);
		}

	if( GetGeneralFlags() & WDS_WIDGET_FLAGS_AUTOPREF_X )
		{
		psize[0]=size[0];
		}
	if( GetGeneralFlags() & WDS_WIDGET_FLAGS_AUTOFIX_X )
		{
		minsize[0]=size[0];
		maxsize[0]=size[0];
		}
	if( GetGeneralFlags() & WDS_WIDGET_FLAGS_AUTOPREF_Y )
		{
		psize[1]=size[1];
		}
	if( GetGeneralFlags() & WDS_WIDGET_FLAGS_AUTOFIX_Y )
		{
		minsize[1]=size[1];
		maxsize[1]=size[1];
		}

	SetPrefSize(psize[0],psize[1]);
	SetMinSize(minsize[0],minsize[1]);
	SetMaxSize(maxsize[0],maxsize[1]);
	}



/******************************************************************************
void WDS_Widget::FindSizeByFlags(long state,long general_flags,long *size)

******************************************************************************/
void WDS_Widget::FindSizeByFlags(long state,long general_flags,long *size)
	{
	WDS_Look_Flash lookflash;

	long annmode=InterpretElementFast(&lookflash,state,WDS_ELEMENT_ANNOTATION_MODE);

	long region[4];
	long isize[2];

	GetRegionFast(&lookflash,FALSE,&region[0],&region[1],&region[2],&region[3]);
	GetMaxImageSize(&isize[0],&isize[1]);

	long annox=0,annoy=0;

	long cly=GetRootWindow()->GetFontHeight();

	if(annmode)
		{
		long annx=InterpretElementFast(&lookflash,state,WDS_ELEMENT_ANNOTATION_X);
		long anny=InterpretElementFast(&lookflash,state,WDS_ELEMENT_ANNOTATION_Y);

		GetAnnotationSizeFast(&lookflash,annx,anny,-1,-1,&annox,&annoy);
		}

	long maxwidth= (general_flags&WDS_WIDGET_FLAGS_AUTOSIZE_X)? -1: region[2];

	// stringsize[0]=number of lines
	// stringsize[1]=width of widest line in pixels
	long stringsize[2];
	GetLabelString()->GetStringSpace(GetRootWindow(),maxwidth,&stringsize[0],&stringsize[1]);

	long insetx=0;
	long subinsetx=0;
	long depth=0;

	if( general_flags & WDS_WIDGET_FLAGS_AUTOSIZE )
		{
		insetx=InterpretElementFast(&lookflash,state,WDS_ELEMENT_INSET);
		subinsetx=InterpretElementFast(&lookflash,state,WDS_ELEMENT_SUBINSET);
		depth=InterpretElementFast(&lookflash,state,WDS_ELEMENT_DEPTH);
		}

	if( general_flags & WDS_WIDGET_FLAGS_AUTOSIZE_X )
		{
		long indent=InterpretElementFast(&lookflash,state,WDS_ELEMENT_INDENT);
		long outdent=InterpretElementFast(&lookflash,state,WDS_ELEMENT_OUTDENT);
		long sub_indent=InterpretElementFast(&lookflash,state,WDS_ELEMENT_SUBINDENT);
		long sub_outdent=InterpretElementFast(&lookflash,state,WDS_ELEMENT_SUBOUTDENT);

		size[0]=stringsize[0];

		if(size[0]<isize[0])
			size[0]=isize[0];
		size[0]+=indent+sub_indent+2*(insetx+depth+subinsetx)+outdent+sub_outdent;
		if(annmode&WDS_WIDGET_ANNOTATION_LEFT_FLAG || annmode&WDS_WIDGET_ANNOTATION_RIGHT_FLAG)
			size[0]+=annox;
		if(size[0]<annox && (annmode&WDS_WIDGET_ANNOTATION_TOP_FLAG))
			size[0]=annox;
		}

	if( general_flags & WDS_WIDGET_FLAGS_AUTOSIZE_Y )
		{
		long insety=insetx+InterpretElementFast(&lookflash,state,WDS_ELEMENT_INSET_MODY);
		long subinsety=subinsetx+InterpretElementFast(&lookflash,state,WDS_ELEMENT_SUBINSET_MODY);

		size[1]=stringsize[1];

		if(size[1]<isize[1])
			size[1]=isize[1];
		size[1]+=2*(insety+depth+subinsety);
		if(annmode&WDS_WIDGET_ANNOTATION_TOP_FLAG)
			size[1]+=annoy;
		if(size[1]<annoy && (annmode&WDS_WIDGET_ANNOTATION_LEFT_FLAG || annmode&WDS_WIDGET_ANNOTATION_RIGHT_FLAG))
			size[1]=annoy;
		}

	// widget must be at least as large as annotation
	if(size[0]>=0 && size[0]<annox)
		size[0]=annox;
	if(size[1]>=0 && size[1]<annoy)
		size[1]=annoy;
	}



/******************************************************************************
long	WDS_Widget::PreOperate(EW_Event *event)

	virtual

******************************************************************************/
long WDS_Widget::PreOperate(EW_Event *event)
	{
//	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Window::PreOperate() 0x%x",this);

	CheckForTip(event);

	if( (event->IsExpective() || event->IsFocusChange() || event->GetMouseButtons()) &&
				GetTipWindow()->GetSource() && !event->GetWidgetFlags() )
		{
		GetTipWindow()->SetForbidden(GetTipWindow()->GetSource());
		GetTipWindow()->Deactivate();
		}

	return EW_Widget::PreOperate(event);
	}



/******************************************************************************
long	WDS_Widget::Resize(EW_Resize *resize)

	virtual

******************************************************************************/
long WDS_Widget::Resize(EW_Resize *resize)
	{
//	AdjustSizeByFlags();

	return EW_Widget::Resize(resize);
	}



/******************************************************************************
long	WDS_Widget::CheckSize(EW_Resize *resize)

	virtual

******************************************************************************/
long WDS_Widget::CheckSize(EW_Resize *resize)
	{
	AdjustSizeByFlags();

	return EW_Widget::CheckSize(resize);
	}



/******************************************************************************
void	WDS_Widget::ModifyPointDirection(long state,long vertical,long back)

Keywords:
******************************************************************************/
void WDS_Widget::ModifyPointDirection(long state,long vertical,long back)
	{
	long flag_copy=FALSE;

	// only if the WDS_BEVELBOX_POINT bit is set
	if( !(InterpretElement(state,WDS_ELEMENT_BORDER)&WDS_BEVELBOX_POINT) )
		return;

	// if default or was copy, set to default and mark new as copy
	if(WDS_ELEMENT_IS_DEFAULT(GetLocalLook()->GetElement(state,WDS_ELEMENT_BORDER)) ||
			WDS_ELEMENT_IS_COPY(InterpretElement(state,WDS_ELEMENT_BORDER)))
		{
		flag_copy=TRUE;
		GetLocalLook()->SetElement(state,WDS_ELEMENT_BORDER,WDS_ELEMENT_DEFAULT_INIT);
		}

	// only if the WDS_BEVELBOX_POINT bit is STILL set
	if( !(InterpretElement(state,WDS_ELEMENT_BORDER)&WDS_BEVELBOX_POINT) )
		return;

	long setting=InterpretElement(state,WDS_ELEMENT_BORDER);

	if(vertical)
		setting|=WDS_BEVELBOX_POINT_VERTICAL;
	else
		setting^= (setting&WDS_BEVELBOX_POINT_VERTICAL);

	if(back)
		setting|=WDS_BEVELBOX_POINT_BACK;
	else
		setting^= (setting&WDS_BEVELBOX_POINT_BACK);

	if(flag_copy)
		setting|=WDS_ELEMENT_COPY;

	GetLocalLook()->SetElement(state,WDS_ELEMENT_BORDER,setting);
	}



/******************************************************************************
WDS_Widget	*WDS_Widget::FindWidgetType(WDS_Typecode code,long shallowest)

	find first/shallowest (or last/deepest) ancestor that matches type code

Keywords:
******************************************************************************/
WDS_Widget *WDS_Widget::FindWidgetType(WDS_Typecode code,long shallowest)
	{
	/* search up the tree */

#if WDS_FIND_DEBUG
	printf("0x%x (%s) searching for type 0x%x shallowest=%d\n",(long)this,this->GetNameString()->Buffer(),code,shallowest);
#endif

	if(GetParentWidget())
		{
		WDS_Widget *wds_widget=(WDS_Widget *)GetParentWidget();

#if WDS_FIND_DEBUG
		printf(" on parent 0x%x (%s) type 0x%x\n",wds_widget,wds_widget->GetNameString()->Buffer(),wds_widget->GetTypeCode());
#endif

		if(wds_widget->GetTypeCode()==code)
			{
			if(shallowest)
				{
#if WDS_FIND_DEBUG
				printf(" shallow match\n");
#endif

				/* first one is fine */
				return wds_widget;
				}
			else
				{
#if WDS_FIND_DEBUG
				printf(" search further\n");
#endif

				WDS_Widget *further;

				/* see if there is a further one, and send it instead */
				further=wds_widget->FindWidgetType(code,shallowest);

#if WDS_FIND_DEBUG
				if(!further)
					printf("0x%x (%s) was furthest\n",wds_widget,wds_widget->GetNameString()->Buffer());
#endif

				return (further)? further: wds_widget;
				}
			}
		else
			{
#if WDS_FIND_DEBUG
			printf(" no match\n");
#endif

			/* no match, keep trying */
			return wds_widget->FindWidgetType(code,shallowest);
			}
		}
	else
		{
		/* down to the root */
		return (WDS_Widget *)NULL;
		}
	}



/******************************************************************************
void	WDS_Widget::GetRegion(long include_border,
									long *rx,long *ry,long *rsx,long *rsy,
									long subx,long suby,long subsx,long subsy)

	region that bevel-box will contain

	note: uses true coordinates

******************************************************************************/
void WDS_Widget::GetRegion(long include_border,long *rx,long *ry,long *rsx,long *rsy,long subx,long suby,long subsx,long subsy)
	{
	GetRegionFast(NULL,include_border,rx,ry,rsx,rsy,subx,suby,subsx,subsy);
	}



/******************************************************************************
void	WDS_Widget::GetRegionFast(WDS_Look_Flash *lookflash,long include_border,
									long *rx,long *ry,long *rsx,long *rsy,
									long subx,long suby,long subsx,long subsy)

******************************************************************************/
void WDS_Widget::GetRegionFast(WDS_Look_Flash *lookflash,long include_border,long *rx,long *ry,long *rsx,long *rsy,
																				long subx,long suby,long subsx,long subsy)
	{
	EW_Window *root=GetRootWindow();

	long index_store[EW_COLOR_NUMBER];
	long m,index;
	long state=GetWidgetState();

	long indent=InterpretElementFast(lookflash,state,WDS_ELEMENT_INDENT);
	long outdent=InterpretElementFast(lookflash,state,WDS_ELEMENT_OUTDENT);
	long insetx=InterpretElementFast(lookflash,state,WDS_ELEMENT_INSET);
	long insety=insetx+InterpretElementFast(lookflash,state,WDS_ELEMENT_INSET_MODY);

	long depth;

/*
	long inset_depthx= (include_border)? insetx: insetx+depth+subinsetx;
	long inset_depthy= (include_border)? insety: insety+depth+subinsety;
	long inset_left= (include_border)? 0: sub_indent;
	long inset_right= (include_border)? 0: sub_outdent;
*/

	long inset_depthx=	insetx;
	long inset_depthy=	insety;
	long inset_left=	0;
	long inset_right=	0;

	if(!include_border)
		{
		depth=InterpretElementFast(lookflash,state,WDS_ELEMENT_DEPTH);

		long sub_indent=InterpretElementFast(lookflash,state,WDS_ELEMENT_SUBINDENT);
		long sub_outdent=InterpretElementFast(lookflash,state,WDS_ELEMENT_SUBOUTDENT);
		long subinsetx=InterpretElementFast(lookflash,state,WDS_ELEMENT_SUBINSET);
		long subinsety=subinsetx+InterpretElementFast(lookflash,state,WDS_ELEMENT_SUBINSET_MODY);

		inset_depthx+=	depth+subinsetx;
		inset_depthy+=	depth+subinsety;
		inset_left+=	sub_indent;
		inset_right+=	sub_outdent;
		}

	*rx=subx+inset_depthx+indent+inset_left;
	*ry=suby+inset_depthy;
	*rsx=subsx-2*inset_depthx-indent-outdent-inset_right;
	*rsy=subsy-2*inset_depthy;

	long annmode=InterpretElementFast(lookflash,state,WDS_ELEMENT_ANNOTATION_MODE);

	if(GetAnnotatable() && annmode)
		{
		// if not yet interpreted
		if(include_border)
			depth=InterpretElementFast(lookflash,state,WDS_ELEMENT_DEPTH);

		long annx=InterpretElementFast(lookflash,state,WDS_ELEMENT_ANNOTATION_X);
		long anny=InterpretElementFast(lookflash,state,WDS_ELEMENT_ANNOTATION_Y);

		long annx2,anny2;
		GetAnnotationSizeFast(lookflash,annx,anny,subsx,subsy,&annx2,&anny2);

		if(annmode&WDS_WIDGET_ANNOTATION_SQUARE_WIDGET)
			{
			long diff=EW_MAX(0,subsy-anny);
			*ry+=diff/2;
			*rsy-=diff;

			annx2=EW_MAX(0,subsx-anny);
			}
			
		if(annmode&WDS_WIDGET_ANNOTATION_TOP_FLAG)
			{
			if(annmode&WDS_WIDGET_ANNOTATION_IMBED_FLAG)
				{
				long diff;

				if(include_border)
					diff=(anny2-depth)/2;
				else
					diff=anny2-depth;

				if(diff>0)
					*rsy-=diff;
				}
			else
				*rsy-=anny2;
			}
		if(annmode&WDS_WIDGET_ANNOTATION_LEFT_FLAG)
			{
			*rx+=annx2;
			*rsx-=annx2;
			}

		if(annmode&WDS_WIDGET_ANNOTATION_RIGHT_FLAG)
			{
//			*rx+=annx2;
			*rsx-=annx2;
			}
		}
	}



/******************************************************************************
void	WDS_Widget::GetAnnotationGeometry(long *x,long *y,long *sx,long *sy)

	external version

******************************************************************************/
void WDS_Widget::GetAnnotationGeometry(long *x,long *y,long *sx,long *sy)
    {
	long tx,ty,tsx,tsy;
	GetTrueGeometry(&tx,&ty,&tsx,&tsy);
    GetAnnotationGeometryFast(NULL,x,y,sx,sy,tx,ty,tsx,tsy);
    }



/******************************************************************************
void WDS_Widget::GetAnnotationGeometryFast(WDS_Look_Flash *lookflash,
					long *x,long *y,long *sx,long *sy,
					long subx,long suby,long subsx,long subsy)

	internal version

******************************************************************************/
void WDS_Widget::GetAnnotationGeometryFast(WDS_Look_Flash *lookflash,long *x,long *y,long *sx,long *sy,
																				long subx,long suby,long subsx,long subsy)
	{
	long state=GetWidgetState();

	long rx,ry,rsx,rsy;
	GetRegionFast(lookflash,TRUE,&rx,&ry,&rsx,&rsy,subx,suby,subsx,subsy);

	long depth=InterpretElementFast(lookflash,state,WDS_ELEMENT_DEPTH);
	long annx=InterpretElementFast(lookflash,state,WDS_ELEMENT_ANNOTATION_X);
	long anny=InterpretElementFast(lookflash,state,WDS_ELEMENT_ANNOTATION_Y);
	long annmode=InterpretElementFast(lookflash,state,WDS_ELEMENT_ANNOTATION_MODE);

	long annx2,anny2;
	GetAnnotationSizeFast(lookflash,annx,anny,subsx,subsy,&annx2,&anny2);

	long ax,ay;
	long xmax,ymax;

	ax=subx;
	ay=suby+subsy-anny2;

	if(annmode&WDS_WIDGET_ANNOTATION_IMBED_FLAG)
		{
		ax=rx+depth+anny2/2;
		ay=ry+rsy-anny2/2-depth/2;
		xmax=annx2;
		ymax=anny2;
		}
	else if(annmode&WDS_WIDGET_ANNOTATION_TOP_FLAG)
		{
		xmax=subsx;
		ymax=anny2;
		}
	else
		{
		if(annmode&WDS_WIDGET_ANNOTATION_RIGHT_FLAG)
			{
			ax=rx+rsx;

			if(annmode&WDS_WIDGET_ANNOTATION_SQUARE_WIDGET)
				{
				ax+=annx;
				annx2=subx+subsx-ax;
				}
			}

		ay=suby;
		xmax=annx2;
		ymax=subsy;
		}

	*x=ax;
	*y=ay;
	*sx=xmax;
	*sy=ymax;
	}



/******************************************************************************
void WDS_Widget::GetAnnotationSize(long annx, long anny,long maxx,long maxy,
                                                    long *annox,long *annoy)

******************************************************************************/
void WDS_Widget::GetAnnotationSize(long annx, long anny,long maxx,long maxy,long *annox,long *annoy)
    {
    GetAnnotationSizeFast(NULL,annx,anny,maxx,maxy,annox,annoy);
    }



/******************************************************************************
void WDS_Widget::GetAnnotationSizeFast(WDS_Look_Flash *lookflash,
			long annx,long anny,long maxx,long maxy,long *annox,long *annoy)

	annx,anny is a previously determined size, <0 means not yet looked up
	maxx,maxy is a maximum if >0

	annox,annoy is the returned size

******************************************************************************/
void WDS_Widget::GetAnnotationSizeFast(WDS_Look_Flash *lookflash,long annx, long anny,long maxx,long maxy,
																									long *annox,long *annoy)
	{
	long state=GetWidgetState();

	if(annx<0)
		annx=InterpretElementFast(lookflash,state,WDS_ELEMENT_ANNOTATION_X);
	if(anny<0)
		anny=InterpretElementFast(lookflash,state,WDS_ELEMENT_ANNOTATION_Y);

	long size[2];
	GetSize(&size[0],&size[1]);

//		long maxwidth= (GetGeneralFlags()&WDS_WIDGET_FLAGS_AUTOSIZE_X)? -1: annx;
	long maxwidth= -1;

	long annmode=InterpretElementFast(lookflash,state,WDS_ELEMENT_ANNOTATION_MODE);
	long cly=GetRootWindow()->GetFontHeight();

	long strx,stry;
	GetAnnotation()->GetStringSpace(GetRootWindow(),maxwidth,&strx,&stry);

	// extra space
//		strx+=3;
//		stry+=5;


	*annox=EW_MAX(annx,strx);
	*annoy=EW_MAX(anny,stry);

	// only if max>=0
	if(maxx>=0)
		*annox=EW_MIN(*annox,maxx);
	if(maxy>=0)
		*annoy=EW_MIN(*annoy,maxy);
	}



/******************************************************************************
void WDS_Widget::BevelBox(long force_clear,long annotate,
			WDS_String *bevelstring,long subx,long suby,long subsx,long subsy)

******************************************************************************/
void WDS_Widget::BevelBox(long force_clear,long annotate,WDS_String *bevelstring,long subx,long suby,long subsx,long subsy)
	{
	WDS_Look_Flash lookflash;
	EW_Window *root=GetRootWindow();
	long state=GetWidgetState();

	long index_store[EW_COLOR_NUMBER];
	long m,index;
	long rx,ry,rsx,rsy;
	long stx,sty,stsx,stsy;
	GetRegionFast(&lookflash,TRUE,&rx,&ry,&rsx,&rsy,subx,suby,subsx,subsy);
	GetRegionFast(&lookflash,FALSE,&stx,&sty,&stsx,&stsy,subx,suby,subsx,subsy);

	// change to widget color overrides (store old if changed)
	for(m=0;m<EW_COLOR_NUMBER;m++)
		{
		index=InterpretElementFast(&lookflash,state,m);

		if(WDS_ELEMENT_IS_DEFAULT(index))
			index_store[m]=index;
		else
			{
			index_store[m]=root->GetColor(m);

			if(index<0)
				root->SetColor(m,root->GetColor(-index));
			else
				root->SetColor(m,index);
			}
		}

/*
        x3=x+inset+subinsetx+labelx;
        y3=y+inset+subinsety+labely;
        x4=dx-2*(inset+subinsetx)-labelx;
        y4=dy-2*(inset+subinsety)-labely;
*/

	long depth=InterpretElementFast(&lookflash,state,WDS_ELEMENT_DEPTH);
	long border=InterpretElementFast(&lookflash,state,WDS_ELEMENT_BORDER);
	long labelx=InterpretElementFast(&lookflash,state,WDS_ELEMENT_LABEL_X);
	long labely=InterpretElementFast(&lookflash,state,WDS_ELEMENT_LABEL_Y);

//	wdsBevelBox(root,rx,ry,rsx,rsy,depth,border,force_clear,subinsetx1,subinsetx2,subinsety,labelx,labely,bevelstring);
	wdsBevelBox(root,this,rx,ry,rsx,rsy,depth,border,force_clear,stx+labelx,sty+labely,stsx,stsy,bevelstring);

	// if image specified, draw it
	if(image[state])
		{
		long subinsetx=InterpretElementFast(&lookflash,state,WDS_ELEMENT_SUBINSET);
		long subinsety=subinsetx+InterpretElementFast(&lookflash,state,WDS_ELEMENT_SUBINSET_MODY);

		image[state]->Draw(root,rx+depth+subinsetx,ry+depth+subinsety);
		}

	long annmode=InterpretElementFast(&lookflash,state,WDS_ELEMENT_ANNOTATION_MODE);

	if(annotate && GetAnnotatable() && annmode)
		{
		long ax,ay;
		long xmax,ymax;

		GetAnnotationGeometryFast(&lookflash,&ax,&ay,&xmax,&ymax,subx,suby,subsx,subsy);

		if(annmode&WDS_WIDGET_ANNOTATION_IMBED_FLAG)
			root->RectFill(ax-1,ay-1,ax+xmax,ay+ymax,InterpretElementFast(&lookflash,state,EW_COLOR_WIDGET));

#if WDS_ANNOTATION_FILL
		root->RectWire(ax,ay,ax+xmax-1,ay+ymax-1,EW_PINK);
#endif

		if(GetAnnotation()->Buffer()!=NULL )
			GetAnnotation()->Operate(root,ax,ay,xmax,ymax,0,0,WDS_STRING_DRAW);
		}

	// restore root coloring
	for(m=0;m<EW_COLOR_NUMBER;m++)
		if( !WDS_ELEMENT_IS_DEFAULT(index_store[m]) )
			root->SetColor(m,index_store[m]);
	}



/******************************************************************************
long WDS_Widget::InterpretElementFast(WDS_Look_Flash *lookflash,
													long state,long element)

	if been set:	return previous value
	otherwise:		interpret, set, and return

******************************************************************************/
long WDS_Widget::InterpretElementFast(WDS_Look_Flash *lookflash,long state,long element)
	{
	if(lookflash==NULL)
		return InterpretElement(state,element);

	long value=lookflash->GetElement(element);

	if(value==WDS_ELEMENT_DEFAULT_INIT)
		{
		value=InterpretElement(state,element);
		lookflash->SetElement(element,value);
		}

	return value;
	}



/******************************************************************************
long	WDS_Widget::InterpretElement(long state,long element)

	return actual color to be used for this element on this widget

	GetElement() will return potentially coded values.
	This functions interprets said values to real colors.

	This function differs from WDS_Widget::InterpretElement()
	in that negative values are interpreted as the negative of the
	element index to use from a previous level:
	ie. -EW_COLOR_BACKGROUND means use the color set as the background
	in the underlying level.

******************************************************************************/
long WDS_Widget::InterpretElement(long state,long element)
	{
	// colors can be mapped using negation as a flag
	long is_color= (element<EW_COLOR_NUMBER);

	// delete me
	long map_negative= is_color;

	// reading the per-instance specs

//	long value=GetLook()->GetElement(state,element);
	long value=GetLocalLook()->GetElement(state<0? 0: state,element);

#if WDS_INTERPRET_DEBUG
	long debug= (state<0 && !strcmp(GetNameString()->Buffer(),"Scroll Bar Button"));
	if(debug)
		{
		printf("(%s) ",GetNameString()->Buffer());
		printf("0x%x element=%d value=%d\n",(long)this,element,value);
		}
#endif

	long typecode=GetTypeCode();

	// interpreting through inheritance
	value=GetLooks()->SearchForElement(GetGlobalLook(),GetTypeCode(),value,state,element);

#if WDS_INTERPRET_DEBUG
	if(debug)
		printf("type 0x%x -> %d",typecode,value);
#endif

	// interpreting through absolute defaults, like the window colors
	if(WDS_ELEMENT_IS_DEFAULT(value))
		{
		if(is_color)
			{
			// current setting of element in window
			value=GetRootWindow()->GetColor(element);
			}
		else
			value=WDS_ELEMENT_DEFAULT_VALUE;
		}
	else if(map_negative && value<0)
		{
		if(is_color)
			{
			// mapping to different element in window
			value=GetRootWindow()->GetColor(-value);
			}
		else
			value=WDS_ELEMENT_DEFAULT_VALUE;
		}

#if WDS_INTERPRET_DEBUG
	if(debug)
		printf(" -> %d\n",value);
#endif

	if(WDS_ELEMENT_IS_DEFAULT(value))
		{
		char message[256];
		sprintf(message,"WDS_Widget::InterpretElement(0x%x,%d) unresolved default",state,element);
		EW_PRINT(EW_WIDGET,EW_WARN,message);
		}

	return value;
	}



/******************************************************************************
void	WDS_Widget::CheckForTip(EW_Event *idleevent)

******************************************************************************/
void WDS_Widget::CheckForTip(EW_Event *idleevent)
	{
	if( /* deny_tips || */ !idleevent->IsIdleMouse())
		return;

	if(GetRootWindow()->GetWSHandler())
	{
		WDS_TipHandler *th=(WDS_TipHandler *)(GetRootWindow()->GetWSHandler());
		th->SetWidget(this);
		th->SetEvent(idleevent);
		th->WSHandle();
		return;
	}

	if(!tipwindow->GetEventContext())
		GetRootWindow()->GetEventContext()->AddWindow(tipwindow);

	tipwindow->CheckTip(this,idleevent);
	}



/******************************************************************************
void	WDS_Widget::SetTieList(WDS_TieList *set)

******************************************************************************/
void WDS_Widget::SetTieList(WDS_TieList *set)
	{
	if(set==tielist)
		return;

	if(tielist)
		tielist->RemoveTie(this);

	if(set)
		set->AddTie(this);

	tielist=set;
	}



/******************************************************************************
void	WDS_Widget::ChangeTieValues(WDS_TieValues *tievalues)

******************************************************************************/
void WDS_Widget::ChangeTieValues(WDS_TieValues *tievalues)
	{
	if(tielist)
		tielist->ChangeTieValues(tievalues);
	}



/******************************************************************************
void	WDS_Widget::LimitDimensionsToScreen(long *srcx,long *srcy,
								long *destx,long *desty,long *lenx,long *leny)

******************************************************************************/
void WDS_Widget::LimitDimensionsToScreen(long *srcx,long *srcy,long *destx,long *desty,long *lenx,long *leny)
	{
	long display[2];
	GetRootWindow()->GetScreenSize(&display[0],&display[1]);
	long wpos[2];
	GetRootWindow()->GetOrigin(&wpos[0],&wpos[1]);

//	printf("x %d vs %d-%d-%d=%d\n",*lenx,display[0],wpos[0],*srcx,display[0]-wpos[0]- *srcx);
//	printf("y %d vs %d-%d-%d=%d\n",*leny,display[1],wpos[1],*srcy,display[1]-wpos[1]- *srcy);

	if(*lenx>display[0]-wpos[0]- *srcx)
		*lenx=display[0]-wpos[0]- *srcx;
	if(*leny>display[1]-wpos[1]- *srcy)
		*leny=display[1]-wpos[1]- *srcy;

	if(wpos[0]<0)
		{
		*srcx-=wpos[0];
		*destx-=wpos[0];
		*lenx+=wpos[0];
		}
	if(wpos[1]<0)
		{
		*srcy-=wpos[1];
		*desty-=wpos[1];
		*leny+=wpos[1];
		}

//	printf("to %d,%d %d,%d\n",*srcx,*srcy,*destx,*desty,*lenx,*leny);
	}



/******************************************************************************
void	WDS_Widget::RegisterEventBypass(long on,EW_Event *event,
											long bypassflags,long shallowest)

******************************************************************************/
void WDS_Widget::RegisterEventBypass(long on,EW_Event *event,long bypassflags,long shallowest)
	{
#if WDS_WIDGET_REGISTER_DEBUG
	printf("0x%x register on=%d bypassflags=%d shallowest=%d event:\n",(long)this,on,bypassflags,shallowest);
	event->Print((long)this);
#endif

	WDS_Typecode scope_code=WDS_Scope::GetTypeCodeStatic();

	WDS_Scope *bypass_scope=(WDS_Scope *)FindWidgetType(scope_code,shallowest);

	if(bypass_scope)
		{
#if WDS_WIDGET_REGISTER_DEBUG
		printf("scope is (%s)\n",bypass_scope->GetNameString()->Buffer());
#endif

		WDS_Bypass bypass;

		// specify destination and type of event we need
		bypass.CopyDataFromEvent(event);
		bypass.SetDestination(this);
		bypass.SetFlags(bypassflags);

		if(on)
			{
			/* register with scope */
			WDS_Validifier *validifier=bypass_scope->RegisterBypass(&bypass);

			if(validifier)
				{
//				EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Widget::RegisterEventBypass() append validifier 0x%x (%s)",validifier,GetName());
				validifierlist.AppendNode(validifier);
				}
			}
		else
			{
			/* deregister with scope */
			bypass_scope->DeRegisterBypass(&bypass);
			}
		}
	else if(on)
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Widget::RegisterEventBypass() no scope found, could not %sregister",on? "": "de");

	validifierlist.Prune();
	}



/******************************************************************************
void	WDS_Widget::DumpString(void)

******************************************************************************/
void WDS_Widget::DumpString(void)
	{
	char headerstring[4][1024]=
		{
"%s         %s              %s             %s      %sChild   In  %s   %sautosize from         %s%s   invisible    %s      %sclippable%s%s%s%s%s",
"%sgeometry %s    widget    %slast resize  %s      %s    Out     %s   %sself (pref,fix,auto)  %s%sdormant greyout %s      %s    clip not/child/all%s%s%s%s%s",
"%srel->abs %s[pref min:max|%spref min:max]%sstate %sC/- O/- I/- %sXY %s-/p/f/a(2) %si/p/m/n(2) %sd/- i/- g/- %st/f trans %sC/- n/c/a {clip geo} %stype %sname%s(label) %sann,mode(anno) %stipmode(tip)",
"%s   ~ for inverse %s%s                   %s      %sdescend     %snode-autosize%s %sbound:ign/par/max/no %s%s  translatable or fixed%s%s%s%s%s%s"
		};

	long m=0;
	for(m=0;m<4;m++)
		EW_PRINT(EW_WINDOW,EW_LOG,headerstring[m],
										ewColorCode(1),
										ewColorCode(2),
										ewColorCode(5),
										ewColorCode(6),
										ewColorCode(0),
										ewColorCode(1),
										ewColorCode(2),
										ewColorCode(5),
										ewColorCode(6),
										ewColorCode(1),
										ewColorCode(2),
										ewColorCode(0),
										ewColorCode(5),
										ewColorCode(6),
										ewColorCode(1),
										ewColorCode(0));
	}



/******************************************************************************
void	WDS_Widget::DumpInfo(long level)

******************************************************************************/
void WDS_Widget::DumpInfo(long level)
	{
	long ix,iy,sx,sy,truex,truey;
	long clipx,clipy,clipw,cliph;
	long minsize[2],maxsize[2],prefsize[2];
	long lminsize[2],lmaxsize[2],lprefsize[2];
	long transx,transy;
	long bounds[2];

	char string[256];
	char boundcode[8][2]={ "n", "m", "p", "?", "i", "?", "?", "?" };
	char clipcode[3][2]={ "n", "c", "a" };
	char autocode[4][2]={ "-", "p", "f", "a" };
	char *name_in,name_out[256];;
	char *label_in,label_out[256];
	char *anno_in,anno_out[256];
	char *tip_in,tip_out[256];

	GetGeometry(&ix,&iy,&sx,&sy);
	GetTrueGeometry(&truex,&truey,&sx,&sy);
	GetMinSize(&minsize[0],&minsize[1]);
	GetMaxSize(&maxsize[0],&maxsize[1]);
	GetLastResize()->GetPrefSize(&lprefsize[0],&lprefsize[1]);
	GetLastResize()->GetMinSize(&lminsize[0],&lminsize[1]);
	GetLastResize()->GetMaxSize(&lmaxsize[0],&lmaxsize[1]);
	GetPrefSize(&prefsize[0],&prefsize[1]);
	GetTranslation(&transx,&transy);
	GetClipRegion(&clipx,&clipy,&clipw,&cliph);

	long dormant=GetDormant();
	long invisible=GetInvisible();
	long greyout=GetGreyOut();
	long translate=GetTranslatable();
	long clippable=GetClippable();
	long clipping=GetClipping();
	long descend=GetDescendMode();
	long annmode=InterpretElement(GetWidgetState(),WDS_ELEMENT_ANNOTATION_MODE);
	long nodesizing=GetNodeSizing();
	long widgetflags=GetGeneralFlags();
	long autox=(widgetflags&WDS_WIDGET_FLAGS_AUTOSIZE_X);
	long autoy=(widgetflags&WDS_WIDGET_FLAGS_AUTOSIZE_Y)>>2;
	bounds[0]=GetBounds(0);
	bounds[1]=GetBounds(1);

	name_in=GetNameString()->Buffer();
	label_in=GetLabel();
	anno_in=GetAnnotation()->Buffer();
	tip_in=GetTipText();

	long maxlen=16;

	ConvertName(maxlen,name_in,name_out);
	ConvertName(maxlen,label_in,label_out);
	ConvertName(maxlen,anno_in,anno_out);
	ConvertName(maxlen,tip_in,tip_out);



	sprintf(string,
"%8x%*s%s%d,%d %d,%d%s>%d,%d%s[%d,%d %d,%d:%d,%d|%s%d,%d %d,%d:%d,%d]%s%d%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%d,%d%s%s%s{%d,%d %d,%d}%s%s %s%s%s(%s)%s%d,%d(%s)%s%d(%s)",
										(long)this,
										level*2,"",
										ewColorCode(1),
										ix,iy,sx,sy,
										GetInversion()? "~":"-",
										truex,truey,
										ewColorCode(2),
										prefsize[0],prefsize[1],
										minsize[0],minsize[1],
										maxsize[0],maxsize[1],
										ewColorCode(5),
										lprefsize[0],lprefsize[1],
										lminsize[0],lminsize[1],
										lmaxsize[0],lmaxsize[1],
										ewColorCode(6),
										GetWidgetState(),
										ewColorCode(0),
										(descend&EW_DESCEND_IN_CHILD_WIDGET)? "C": "-",
										(descend&EW_DESCEND_OUT_WINDOW)? "O": "-",
										(descend&EW_DESCEND_IN_WINDOW)? "I": "-",
										ewColorCode(1),
										(nodesizing&EW_NODE_SIZING_FIT_CHILD_X)? "X": "-",
										(nodesizing&EW_NODE_SIZING_FIT_CHILD_Y)? "Y": "-",
										ewColorCode(2),
										autocode[autox&3],
										autocode[autoy&3],
										ewColorCode(5),
										boundcode[bounds[0]&7],
										boundcode[bounds[1]&7],
										ewColorCode(6),
										dormant? "d":"-",
										invisible? "i":"-",
										greyout? "g":"-",
										ewColorCode(1),
										translate? "t":"f",transx,transy,
										ewColorCode(2),
										clippable? "C":"-",
										clipcode[clipping&3],clipx,clipy,clipw,cliph,
										ewColorCode(0),
										GetTypeName(),
										ewColorCode(5),
										name_out,
										ewColorCode(6),
										label_out,
										ewColorCode(1),
										GetAnnotatable(),annmode,
										anno_out,
										ewColorCode(0),
										GetTipMode(),
										tip_out
										);

	EW_PRINT(EW_WINDOW,EW_LOG,string);
	}



/******************************************************************************
void	WDS_Widget::ConvertName(long maxlen,char *in,char *out)

	convert newlines to just characters

******************************************************************************/
void WDS_Widget::ConvertName(long maxlen,char *in,char *out)
	{
	long m=0,n=0;
	for(m=0;m<strlen(in);m++)
		{
		if(in[m]=='\n')
			{
			out[n++]='\\';
			out[n++]='n';
			}
		else
			out[n++]=in[m];
		}

	out[n]=0;

	if(maxlen>1 && maxlen<n)
		out[maxlen-1]='*';
		out[maxlen]=0;
	}



/******************************************************************************
	WDS_ValidifierList::~WDS_ValidifierList(void)

	for all valid validifiers, decrement and leave undeleted
	if already invalid, let this list delete it

******************************************************************************/
WDS_ValidifierList::~WDS_ValidifierList(void)
	{
//	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ValidifierList::~WDS_ValidifierList() %d nodes",GetNumberNodes());

	WDS_Validifier *node;

	ToHead();
	while( (node=GetCurrent()) != NULL )
		{
		if(node->GetValid())
			{
			node->SetValid(node->GetValid()-1);

//			EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ValidifierList::~WDS_ValidifierList() invalidating 0x%x -> releasing",node);

			// don't let this destruction delete the node
			RemoveNode(node);
			}
		else
			{
//			EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ValidifierList::~WDS_ValidifierList() invalid 0x%x -> will destruct",node);

			PostIncrement();
			}
		}

//	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ValidifierList::~WDS_ValidifierList() done");
	}



/******************************************************************************
void	WDS_ValidifierList::Prune(void)

******************************************************************************/
void WDS_ValidifierList::Prune(void)
	{
	WDS_Validifier *node;

	ToHead();
	while( (node=PostIncrement()) != NULL )
		{
		if(node->GetValid()<=0)
			{
//			EW_PRINT(EW_WIDGET,EW_LOG,"WDS_ValidifierList::Prune() deleting invalidated 0x%x",node);

			RemoveNode(node);
			delete node;
			}
		}
	}



/******************************************************************************
void	WDS_Validifier::SetValid(long set)

******************************************************************************/
void WDS_Validifier::SetValid(long set)
	{
//	printf("0x%x %d->%d\n",(long)this,valid,set);

	valid=set;
	};
