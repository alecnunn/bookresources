/******************************************************************************

Coral Tree wds/form.cc
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


#define WDS_FORM_DEBUG				FALSE
#define WDS_FORM_SELECT_WIRE		FALSE



/******************************************************************************
		WDS_Form::WDS_Form()

Keywords:
******************************************************************************/
WDS_Form::WDS_Form()
	{
	SetName("Form");
	SetAnnotatable(TRUE);

	SetMode(WDS_FORM_MANUAL);
	SetCentering(WDS_FORM_CENTER_NONE);

	SetSizing(0,WDS_SIZING_FIT_CHILDREN);
	SetSizing(1,WDS_SIZING_FIT_CHILDREN);

	SetInversion(TRUE);
	SetClipping(EW_CLIPPING_CHILDREN);

	SetSelectability(WDS_SELECT_NONE);
	SetSelection(NULL);
	SetClear(FALSE);
	SetUniformity(WDS_UNIFORM_NONE);

	// accept bypassed events
	SetEventMask(GetLocalEventMask());
	GetLocalEventMask()->SetWidgetFlags(WDS_BYPASS_TAKE_LOST);
	}



/******************************************************************************
long	WDS_Form::Operate(EW_Event *event)

	select child of form

Keywords:
******************************************************************************/
long WDS_Form::Operate(EW_Event *event)
	{
#if WDS_FORM_DEBUG

	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Form::Operate() event >>>>");
	event->Print((long)this);

#endif


	if(event->Is(EW_EVENT_WIDGET,WDS_ITEM_SELECTION,EW_STATE_ANY))
		{
/*
		printf("WDS_Form::Operate() (%s) deselection event >>>>\n",GetNameString()->Buffer());
		event->Print((long)this);
*/

		SetSelection(NULL);
		}

	if(event->IsLeftMousePress() && GetSelectability())
		{
		WDS_Widget *node;

		GetWidgetList()->ToHead();
		while( (node=(WDS_Widget *)GetWidgetList()->PostIncrement()) != NULL)
			if( node->InRegion(event,TRUE) )
				break;

		// NOTE: if not found, node is NULL

		if(node!=GetSelection())
			{
			SetSelection(node);
			ChangeState(WDS_STATE_1,event);	// strobe

			SetDirty(EW_DIRTY_COMPLETE);	// mark this widget as needing to be redrawn
			}
		else if(node==NULL && GetSelection())
			{
			SetSelection(node);
			ChangeState(WDS_STATE_0,event);
			SetDirty(EW_DIRTY_COMPLETE);	// mark this widget as needing to be redrawn
			}
		}

	return TRUE;
	}



/******************************************************************************
void	WDS_Form::Draw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_Form::Draw(EW_Redraw *redraw)
	{
	DB("WDS_Form::Draw()");

	long nx,ny,nsx,nsy;
	GetTrueGeometry(&nx,&ny,&nsx,&nsy);

#if WDS_COLOR_FILL

	GetRootWindow()->RectFill(truex,truey,truex+sx-1,truey+sy-1,EW_WHITE);

#endif

#if WDS_COLOR_WIRE

	GetRootWindow()->RectWire(nx+3,ny+3,nx+nsx-4,ny+nsy-4,EW_RED);

#endif

#if FALSE
	GetRootWindow()->RectWire(nx,ny,nx+nsx-1,ny+nsy-1,EW_RED);
#endif

//	printf("WDS_Form::Draw() 0x%x\n",(long)this);

	BevelBox(GetClear(),NULL);

#if WDS_FORM_SELECT_WIRE
	WDS_Widget *node;

	GetWidgetList()->ToHead();
	while( (node=(WDS_Widget *)GetWidgetList()->PostIncrement()) != NULL)
		{
		node->GetTrueGeometry(&nx,&ny,&nsx,&nsy);
		GetRootWindow()->RectWire(nx-1,ny-1,nx+nsx,ny+nsy,EW_GREEN);
		}
#endif

	if(GetSelection() && GetSelectability())
		{
		GetSelection()->GetTrueGeometry(&nx,&ny,&nsx,&nsy);
		GetRootWindow()->RectWire(nx-1,ny-1,nx+nsx,ny+nsy,InterpretElement(GetWidgetState(),WDS_ELEMENT_FORM_COLOR_SELECT));
		}
	}



/******************************************************************************
long	WDS_Form::Resize(EW_Resize *resize)

Keywords:
******************************************************************************/
long WDS_Form::Resize(EW_Resize *resize)
	{
	WDS_Widget *node;
	EW_Resize new_resize;

	long opos[2],osize[2];		// widget's original geometry
	long pos[2],size[2];		// widget's geometry
	long minsize[2],maxsize[2];	// min, max size
	long pref[2];				// prefered size
	long fullsize[2];			// partition widget size (parent)
	long npos[2];				// next widget new position
	long limit;
	long m;
	long resized=FALSE;
	long tpos[2];				// true position
	long rpos[2],rsize[2],sdiff[2];	// region geometry

	ConfirmTypecode();

    long annmode=InterpretElement(GetWidgetState(),WDS_ELEMENT_ANNOTATION_MODE);

	SetWidgetState( (annmode&WDS_WIDGET_ANNOTATION_IMBED_FLAG)? WDS_STATE_1: WDS_STATE_0);
	long state=GetWidgetState();

	long separator=InterpretElement(state,WDS_ELEMENT_FORM_SEPARATOR);

	long indent=InterpretElement(state,WDS_ELEMENT_FORM_INDENT);
	long indent_after=InterpretElement(state,WDS_ELEMENT_FORM_INDENT_AFTER);
/*
	long edge=separator+InterpretElement(state,WDS_ELEMENT_DEPTH)
							+InterpretElement(state,WDS_ELEMENT_INSET);
*/

	long index= ((GetMode()&WDS_FORM_COLUMN) != 0);
	long single= ((GetMode()&WDS_FORM_SINGLE) != 0);

	// verify that selection is a child
	GetWidgetList()->Rewind();
	while( (node=(WDS_Widget *)(GetWidgetList()->Next())) != NULL )
		{
		if(node==GetSelection())
			break;
		}
	if(node!=GetSelection())
		SetSelection(NULL);

	// form sizes itself
//	SetBounds(EW_BOUNDS_NONE);

	// pass along any info
	osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

	// store original geometry for later comparison
	GetGeometry(&opos[0],&opos[1],&osize[0],&osize[1]);

	GetPrefSize(&pref[0],&pref[1]);
	resize->GetMaxSize(&maxsize[0],&maxsize[1]);

//	printf("0x%x maxsize=%d,%d (%s)\n",(long)this,maxsize[0],maxsize[1],GetNameString()->String());

	for(m=0;m<2;m++)
		{
		switch( sizing[m] )
			{
			case WDS_SIZING_FIT_CHILDREN:
				pref[m]=maxsize[m];
				maxsize[m]= -1;
				break;

			case WDS_SIZING_MANUAL:
				maxsize[m]= -1;
				break;

			default:
				maxsize[m]=pref[m];
				break;
			}
		}

	SetPrefSize(pref[0],pref[1]);
	SetMaxSize(maxsize[0],maxsize[1]);

	// resize form
	CheckSize(&new_resize);

	GetTrueGeometry(&tpos[0],&tpos[1],&size[0],&size[1]);
	GetOrigin(&pos[0],&pos[1]);

	// get region and convert to relative inverted coords
	GetRegion(FALSE,&rpos[0],&rpos[1],&rsize[0],&rsize[1]);
	for(m=0;m<2;m++)
		{
		rpos[m]-=tpos[m];
		rpos[m]=size[m]-(rpos[m]+rsize[m]);
		sdiff[m]=size[m]-rsize[m];
		}

//	printf("geo %d,%d %d,%d reg %d,%d %d,%d (%s)\n",opos[0],opos[1],osize[0],osize[1],rpos[0],rpos[1],rsize[0],rsize[1],GetName());

	// pass along any info
	osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

	resize->GetPrefSize(&pref[0],&pref[1]);
	resize->GetMaxSize(&maxsize[0],&maxsize[1]);

	if(sizing[index]==WDS_SIZING_FIT_CHILDREN)
		{
		fullsize[0]= (pref[0]>0)? pref[0]: maxsize[0];
		fullsize[1]= (pref[1]>0)? pref[1]: maxsize[1];
		}
	else
		{
//		fullsize[0]=size[0];
//		fullsize[1]=size[1];
		fullsize[0]=rsize[0];
		fullsize[1]=rsize[1];
		}

#if FALSE
#define	WDS_FORM_TEST	"outerform"

	long psx,psy;
	GetParentWidget()->GetSize(&psx,&psy);
	if(!GetNameString()->Compare(WDS_FORM_TEST))
		printf(" parent=%d,%d fullsize=%d,%d pref=%d,%d max=%d,%d rsize=%d,%d sdiff=%d,%d\n",
				psx,psy,fullsize[0],fullsize[1],pref[0],pref[1],maxsize[0],maxsize[1],rsize[0],rsize[1],sdiff[0],sdiff[1]);
#endif

	new_resize.SetMinSize(-1,-1);
//	pref[0]=fullsize[0];
//	pref[1]=fullsize[1];

	// limit children size to slightly smaller than our limit
	long reduction;

//	reduction=2*(InterpretElement(state,WDS_ELEMENT_DEPTH)+InterpretElement(state,WDS_ELEMENT_INSET));

	for(m=0;m<2;m++)
		{
		reduction=sdiff[m];

		if(pref[m]>0)
			{
			pref[m]-=reduction;

			if(pref[m]<0)
				pref[m]=0;
			}

		if(maxsize[m]>0)
			{
			maxsize[m]-=reduction;

			if(maxsize[m]<0)
				maxsize[m]=0;
			}
		}

	long pref_fade= ( sizing[index]!=WDS_SIZING_FIT_CHILDREN && !(GetUniformity()&(index? WDS_UNIFORM_Y: WDS_UNIFORM_X)) );

	if(pref_fade)
		{
		// subtract off size of fixed off opposing side

		long min[2],max[2];

		GetWidgetList()->ToTail();
		while( (node=(WDS_Widget *)GetWidgetList()->PostDecrement()) != NULL )
			{
			node->GetMinSize(&min[0],&min[1]);
			node->GetMaxSize(&max[0],&max[1]);

			// if will not react to pref, treat as fixed
			if(min[index]>0 && min[index]==max[index])
				{
				pref[index]-=max[index];
				}
			else if( (node->GetBounds(index? WDS_VERTICAL: WDS_HORIZONTAL)&EW_BOUNDS_IGNORE_PARENT) ||
									(GetGeneralFlags()&(index? WDS_WIDGET_FLAGS_AUTOPREF_Y: WDS_WIDGET_FLAGS_AUTOPREF_X)) )
				{
				node->CheckSize(&new_resize);
				node->GetSize(&max[0],&max[1]);
				pref[index]-=max[index];
				}
			else
				break;
			}
		}

	long fullpref=pref[index];
	long pref_loss=separator;

	new_resize.SetPrefSize(pref[0],pref[1]);
	new_resize.SetMaxSize(maxsize[0],maxsize[1]);

#ifdef WDS_FORM_TEST
	if(!GetNameString()->Compare(WDS_FORM_TEST))
		printf("0x%x new pref %d,%d max %d,%d\n",(long)this,pref[0],pref[1],maxsize[0],maxsize[1]);
#endif

	if(GetUniformity())
		{
		long biggest[2];	// largest child (x,y independent)

		biggest[0]= -1;
		biggest[1]= -1;

		GetWidgetList()->ToHead();
		while( (node=(WDS_Widget *)(GetWidgetList()->Next())) != NULL )
			{
			node->Resize(&new_resize);
			node->GetSize(&size[0],&size[1]);

			if(biggest[0]<size[0])
				biggest[0]=size[0];
			if(biggest[1]<size[1])
				biggest[1]=size[1];
			}

		if( !(GetUniformity()&WDS_UNIFORM_X) )
			biggest[0]= -1;
		if( !(GetUniformity()&WDS_UNIFORM_Y) )
			biggest[1]= -1;

		new_resize.SetMinSize(biggest[0],biggest[1]);
		new_resize.SetMaxSize(biggest[0],biggest[1]);
		new_resize.SetPrefSize(biggest[0],biggest[1]);
		}

	if(mode!=WDS_FORM_MANUAL)
		{
		// place and resize children

/*
		npos[0]=edge;
		npos[1]=edge;
*/
		npos[0]=rpos[0];
		npos[1]=rpos[1];

		limit=0;
		long count=0;
		long indentation;

		GetWidgetList()->ToHead();
		while( (node=(WDS_Widget *)(GetWidgetList()->PostIncrement())) != NULL )
			{
			if(pref_fade)
				{
				pref[index]=fullpref-pref_loss;
				new_resize.SetPrefSize(pref[0],pref[1]);
				}

			if(GetUniformity())
				{
				// limited resize - ignores some EW operations
				if( node->Resize(&new_resize) )
					resized=TRUE;
				}
			else
				{
				if( node->PreResize(&new_resize) )
					resized=TRUE;
				}

			node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);

#if FALSE
			if(!GetNameString()->Compare("confirmform"))
				{
				printf("0x%x mode=%d index=%d %d>%d && %d+%d>%d-%d %d>%d\n",	(long)this,mode,index,
																	npos[index],edge,
																	npos[index],size[index],fullsize[index],2*edge,
																	npos[index]+size[index],fullsize[index]-2*edge);
				printf("0x%x mode=%d index=%d %d>%d && %d+%d>%d-%d %d>%d\n",	(long)this,mode,index,
																	npos[index],rpos[index],
																	npos[index],size[index],fullsize[index],sdiff[index],
																	npos[index]+size[index],fullsize[index]-sdiff[index]);
				}
#endif

//			if(sizing[index]!=WDS_SIZING_FIT_CHILDREN && npos[index]>edge && npos[index]+size[index]>fullsize[index]-2*edge)
//			if(npos[index]>edge && fullsize[index]>=0 && npos[index]+size[index]>fullsize[index]-2*edge)
			if(!single && npos[index]>rpos[index] && fullsize[index]>=0 &&
													npos[index]-rpos[index]+size[index]>fullsize[index]-sdiff[index])
				{
				// next row/column

				npos[index]=rpos[index];
				npos[!index]+=limit+separator;
				limit=0;
				indent=0;
				pref_loss=separator;
				}

			indentation= (count>=indent_after)? indent: 0;

			pos[0]=npos[0];
			pos[1]=npos[1];

			pos[!index]+=indentation;

			node->SetGeometry(pos[0],pos[1],size[0],size[1]);
			node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);

			npos[index]+=size[index]+separator;
			pref_loss+=size[index]+separator;

			limit=EW_MAX(limit,size[!index]+indentation);

			count++;
			}
		}
	else
		{
		if(GetUniformity())
			{
			// limited resize - ignores some EW operations
			GetWidgetList()->ToHead();
			while( (node=(WDS_Widget *)(GetWidgetList()->Next())) != NULL )
				{
				node->Resize(&new_resize);
				}
			}
		else
			GetWidgetList()->Resize(&new_resize);
		}

	long cmin[2],cmax[2];		// child geometry limits
	long translate[2];			// translation

	// find extents of children
	GetWidgetList()->ToHead();
	GetChildrensExtents(&cmin[0],&cmin[1],&cmax[0],&cmax[1]);

	translate[0]=0;
	translate[1]=0;

	if( sizing[0]==WDS_SIZING_FIT_CHILDREN || sizing[1]==WDS_SIZING_FIT_CHILDREN || GetCentering() )
		{
		GetMinSize(&minsize[0],&minsize[1]);
		GetMaxSize(&maxsize[0],&maxsize[1]);
		GetPrefSize(&pref[0],&pref[1]);

		// pass along any info
		osdMemcpy(&new_resize,resize,sizeof(EW_Resize));

		for(m=0;m<2;m++)
			if(sizing[m]==WDS_SIZING_FIT_CHILDREN )
				{
				pref[m]= cmax[m]-cmin[m] +sdiff[m];
				minsize[m]=pref[m];
				maxsize[m]=pref[m];
				translate[m]= -cmin[m]+rpos[m];

//				maxsize[m]= -1;
				}
			else if( GetCentering() & (m? WDS_FORM_CENTER_Y: WDS_FORM_CENTER_X) )
				{
				long extent=cmax[m]-cmin[m];
				translate[m]= (rsize[m]-extent)/2;
				}

		SetMinSize(minsize[0],minsize[1]);
		SetMaxSize(maxsize[0],maxsize[1]);
		SetPrefSize(pref[0],pref[1]);

//		new_resize.SetMaxSize(maxsize[0],maxsize[1]);

		// resize form
		CheckSize(&new_resize);

		// since form size can change AFTER children are resized, must recalc child true positions
		GetWidgetList()->Rewind();
		while( (node=(WDS_Widget *)(GetWidgetList()->Next())) != NULL )
			node->CalcTruePosition(TRUE);
		}

	SetTranslation(translate[0],translate[1]);

	// check if form has changed size
	GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);
	if( pos[0]!=opos[0] || pos[1]!=opos[1] || size[0]!=osize[0] || size[1]!=osize[1] )
		{
/*
		printf("WDS_Form::Resize() 0x%x %d,%d %d,%d -> %d,%d %d,%d\n",
				(long)this,opos[0],opos[1],osize[0],osize[1],pos[0],pos[1],size[0],size[1]);
*/

		SetDirty(EW_DIRTY_COMPLETE);

		resized=TRUE;
		}

	return resized;
	}



/******************************************************************************
void	WDS_Form::RegisterExclusivity(long on)

	exclusive access on shallowest scope

******************************************************************************/
void WDS_Form::RegisterExclusivity(long on)
	{
	EW_Event event;

	event.SetSIS(EW_EVENT_WIDGET,WDS_ITEM_SELECTION,EW_STATE_ANY);
	RegisterEventBypass(on,&event,WDS_BYPASS_FLAG_TAKE,TRUE);
	}



/******************************************************************************
void	WDS_Form::SetSelection(WDS_Widget *set)

******************************************************************************/
void WDS_Form::SetSelection(WDS_Widget *set)
	{
//	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Form::SetSelection()");

	selection=set;
	if(GetSelectability()&WDS_SELECT_EXCLUSIVE)
		RegisterExclusivity(set!=NULL);

	SetDirty(EW_DIRTY_COMPLETE);
	}
