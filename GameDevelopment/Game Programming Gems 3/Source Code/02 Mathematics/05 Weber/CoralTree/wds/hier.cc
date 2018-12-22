/******************************************************************************

Coral Tree wds/hier.cc
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


#define WDS_HIER_COLOR_BG_ACTIVE	EW_YELLOW
#define WDS_HIER_COLOR_BG_INACTIVE	EW_LIGHTGREY

#define HIER_NODE_MATCH_DEBUG	FALSE
#define HIER_FORM_RESIZE_DEBUG	FALSE
#define HIER_DELETE_DEBUG		FALSE

#define GPL_DISSECT_DEBUG		FALSE



long WDS_HierarchyForm::look_initialized=FALSE;
long WDS_HierarchyNode::look_initialized=FALSE;



/******************************************************************************
void	WDS_HierarchyForm::InitializeLook(long set)

Keywords:
******************************************************************************/
void WDS_HierarchyForm::InitializeLook(long set)
	{
	WDS_Look alook;

	InitializeLookCallbackInit(&alook,set);
	GetDefaultLooks()->WDS_REGISTER_LOOK(WDS_HierarchyForm,&alook);
	}



/******************************************************************************
void	WDS_HierarchyForm::InitializeLookCallbackInit(WDS_Look *alook,long set)

Keywords:
******************************************************************************/
void WDS_HierarchyForm::InitializeLookCallbackInit(WDS_Look *alook,long set)
	{
//	printf("WDS_HierarchyForm::InitializeLookCallbackInit(0x%x,%d)\n",alook,set);

	alook->Reset();

	alook->SetElement(WDS_STATE_0,	WDS_ELEMENT_BORDER,			WDS_BEVELBOX_OUT);
	alook->SetElement(WDS_STATE_1,	WDS_ELEMENT_BORDER,			WDS_BEVELBOX_IN);
	alook->SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,			WDS_HIER_COLOR_BG_INACTIVE);
	alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,	(long)WDS_Form::GetTypeCodeStatic());
	alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_CALLBACK_INIT,	(long)&WDS_HierarchyForm::InitializeLookCallbackInit);
	alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_FORM_SEPARATOR,	0);

	switch(set)
		{
		case WDS_LOOK_ATHENA:
			alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,			1);
			alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,			2);
			break;

		case WDS_LOOK_WIN32:
		case WDS_LOOK_MOTIF:
		case WDS_LOOK_CORAL:
		default:
			alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,			2);
			alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,			3);
			break;
		}
	}



/******************************************************************************
void	WDS_HierarchyNode::InitializeLook(long set)

Keywords:
******************************************************************************/
void WDS_HierarchyNode::InitializeLook(long set)
	{
	WDS_Look alook;

	InitializeLookCallbackInit(&alook,set);
	GetDefaultLooks()->WDS_REGISTER_LOOK(WDS_HierarchyNode,&alook);
	}



/******************************************************************************
void	WDS_HierarchyNode::InitializeLookCallbackInit(WDS_Look *alook,long set)

Keywords:
******************************************************************************/
void WDS_HierarchyNode::InitializeLookCallbackInit(WDS_Look *alook,long set)
	{
	alook->Reset();

	alook->SetElement(WDS_STATE_0,	EW_COLOR_WIDGET,			WDS_HIER_COLOR_BG_INACTIVE);
	alook->SetElement(WDS_STATE_1,	EW_COLOR_WIDGET,			WDS_HIER_COLOR_BG_ACTIVE);
	alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_BORDER,			WDS_BEVELBOX_BORDERLESS);
	alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_DEPTH,			0);
	alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INSET,			0);
	alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_INHERITANCE,	(long)WDS_Button::GetTypeCodeStatic());
	alook->SetElement(WDS_STATE_ALL,		WDS_ELEMENT_CALLBACK_INIT,	(long)&WDS_HierarchyNode::InitializeLookCallbackInit);

	switch(set)
		{
		case WDS_LOOK_ATHENA:
			break;

		case WDS_LOOK_WIN32:
		case WDS_LOOK_MOTIF:
		case WDS_LOOK_CORAL:
		default:
//			alook->SetElement(WDS_STATE_ALL,		EW_COLOR_WIDGET,			WDS_ELEMENT_MAP|EW_COLOR_BACKGROUND);
			break;
		}
	}



/******************************************************************************
	WDS_HierarchyNode::WDS_HierarchyNode(void)

******************************************************************************/
WDS_HierarchyNode::WDS_HierarchyNode(void)
	{
	SetName("HierarchyNode");

	// turn off 3D text-style (it takes twice as long)
	GetLabelString()->SetStyleExcluding(WDS_STRING_STYLE_3D);

	SetFlags(WDS_BUTTON_FLAGS_TOGGLE|WDS_BUTTON_FLAGS_EXCLUSIVE);
	SetGeneralFlags(WDS_WIDGET_FLAGS_AUTOSIZE);

	showing=FALSE;
	SetShowable(2);

	toggle.SetDormant(TRUE);

	if(FALSE && !look_initialized)
		{
		look_initialized=TRUE;
		InitializeLook(WDS_LOOK_DEFAULT);
		};
	};



/******************************************************************************
void	WDS_HierarchyNode::SetShow(long show)

******************************************************************************/
void WDS_HierarchyNode::SetShow(long show)
	{
	if(show!=showing)
		{
		EW_Window *root=GetRootWindow();
		WDS_Widget *real_parent;

		if(show)
			{
			// insert our parent_form inbetween us and real parent

			// this is not our owned parent form
			real_parent=(WDS_Widget *)GetParentWidget();

			if(real_parent)
				{
				real_parent->GetWidgetList()->SetCurrent(this);
				real_parent->RemoveWidget(this);

				real_parent->GetWidgetList()->InsertBefore(&parent_form);
				parent_form.SetParentWidget(real_parent);

				real_parent->SetDirty(EW_DIRTY_COMPLETE);
				}
			else if(root)
				{
				root->RemoveWidget(this);
				root->AddWidget(&parent_form);

				root->GetWidgetList()->SetCurrent(this);
				root->RemoveWidget(this);

				root->GetWidgetList()->InsertBefore(&parent_form);
				parent_form.SetRootWindow(root);

				parent_form.SetDirty(EW_DIRTY_COMPLETE);
				}
			else
				EW_PRINT(EW_WIDGET,EW_WARN,"WDS_HierarchyNode::SetShow(%d) widget 0x%x has no parent",show,(long)this);

			parent_form.GetWidgetList()->ToHead();
			parent_form.GetWidgetList()->InsertBefore(this);
			this->SetParentWidget(&parent_form);

			ShowEntry();


			EW_Resize resize;	// dummy resize

			parent_form.Resize(&resize);
			}
		else
			{
			// remove said parent_form from between us and real parent

			real_parent=(WDS_Widget *)parent_form.GetParentWidget();

			parent_form.RemoveWidget(this);

			if(real_parent)
				{
				real_parent->GetWidgetList()->SetCurrent(&parent_form);
				real_parent->RemoveWidget(&parent_form);

				real_parent->GetWidgetList()->InsertBefore(this);
				this->SetParentWidget(real_parent);

				real_parent->SetDirty(EW_DIRTY_COMPLETE);
				}
			else if(root)
				{
				root->GetWidgetList()->SetCurrent(&parent_form);
				root->RemoveWidget(&parent_form);

				root->GetWidgetList()->InsertBefore(this);
				this->SetRootWindow(root);

				SetDirty(EW_DIRTY_COMPLETE);
				}
			else
				EW_PRINT(EW_WIDGET,EW_WARN,"WDS_HierarchyNode::SetShow(%d) widget 0x%x has no parent",show,(long)this);
			}

		showing=show;
		}
	}



/******************************************************************************
void	WDS_HierarchyNode::Draw(EW_Redraw *redraw)

******************************************************************************/
void WDS_HierarchyNode::Draw(EW_Redraw *redraw)
	{
	if(InterpretElement(GetWidgetState(),WDS_ELEMENT_HNODE_LINES))
		{
		EW_Window *root=GetRootWindow();

		long parx,pary,parsx,parsy;
		long prevx,prevy,prevsx,prevsy;
		long nextx,nexty,nextsx,nextsy;
		long tx,ty,sx,sy;
		long first=FALSE;

		WDS_Widget *parent=(WDS_Widget *)GetParentWidget();
		if(parent)
			parent->GetTrueGeometry(&parx,&pary,&parsx,&parsy);

		WDS_Widget *next=(WDS_Widget *)Next();
		WDS_Widget *previous=(WDS_Widget *)Prev();
		if(!previous)
			{
			first=TRUE;
			if(parent)
				{
				previous=(WDS_Widget *)parent->Prev();
				next=(WDS_Widget *)parent->Next();
				}
			else
				{
				previous=NULL;
				next=NULL;
				}
			}

		long hnode_code=WDS_HierarchyNode::GetTypeCodeStatic();
		long hform_code=WDS_HierarchyForm::GetTypeCodeStatic();

		// if referencing a hier-form, re-reference its first child
		if( next && next->GetTypeCode()==hform_code )
			next=(WDS_Widget *)next->GetWidgetList()->ToHead();
		if( previous && previous->GetTypeCode()==hform_code )
			previous=(WDS_Widget *)previous->GetWidgetList()->ToHead();

		// don't reference unless a hier-node or hier-form
		if( next && next->GetTypeCode()!=hnode_code)
			next=NULL;
		if( previous && previous->GetTypeCode()!=hnode_code)
			previous=NULL;

		GetTrueGeometry(&tx,&ty,&sx,&sy);

		if(next)
			next->GetTrueGeometry(&nextx,&nexty,&nextsx,&nextsy);
		if(previous)
			previous->GetTrueGeometry(&prevx,&prevy,&prevsx,&prevsy);

		long colorindex=GetHierToggle()->InterpretElement(GetWidgetState(),EW_COLOR_PEN);
		InterpretElement(GetWidgetState(),WDS_ELEMENT_FORM_SEPARATOR);

		long togx,togy,togsx,togsy;
		toggle.GetRegion(TRUE,&togx,&togy,&togsx,&togsy);

		long rx,ry,rsx,rsy;
		GetRegion(TRUE,&rx,&ry,&rsx,&rsy);

		long linex=togx+togsx/2;
		long nodex=rx-2;	// one pixel gap
		long centery=togy+togsy/2;
		long top=ty+sy-1;
		long liney= (next && nextx==tx)? nexty+nextsy: centery;
		long endy= (previous)? prevy-1: top;

		// potential in multi-column
		if(endy<top)
			endy=top;
		if(next && nextx!=tx)
			liney=ty;

		long clipped=FALSE;

		EW_RectangleList rectanglelist;

		// backup clipping stack one level
		if(parent && parent->GetClipping())
			{
			clipped=TRUE;

			root->CopyClipRegionTo(&rectanglelist);
			root->PopClipRegion();
			}

		root->Line(linex,centery,nodex,centery,colorindex);
		root->Line(linex,liney,linex,endy,colorindex);

/*
		root->Point(linex,liney,EW_GREEN);
		root->Point(linex,endy,EW_BLUE);
*/

		// restore clipping
		if(clipped)
			{
			root->PushClipRegion();
			root->CopyClipRegionFrom(&rectanglelist);
			}
		}

	WDS_Button::Draw(redraw);
	}



/******************************************************************************
void	WDS_HierarchyToggle::Draw(EW_Redraw *redraw)

	replaces WDS_Button::Draw() with a three-state ability

Keywords:
******************************************************************************/
void WDS_HierarchyToggle::Draw(EW_Redraw *redraw)
	{
    // store
    long save_state=GetWidgetState();

    long figure_state=save_state+3;
    
    BevelBox(TRUE,TRUE,GetLabelString());
    
    // bevel figure
    SetWidgetState(figure_state);
    BevelBox(InterpretElement(figure_state,WDS_ELEMENT_BORDER)!=WDS_BEVELBOX_BORDERLESS,FALSE,NULL);
    
    // restore
    SetWidgetState(save_state);
	}



/******************************************************************************
long	WDS_HierarchyNode::Operate(EW_Event *event)

	NOTICE: this widget can move itself in the hierarchy
	since it may receive the same event twice, it sets the event used
	and ignores used events

Keywords:
******************************************************************************/
long WDS_HierarchyNode::Operate(EW_Event *event)
	{
	if(event->GetUsed())
		{
		// may be an event already seen
//		return FALSE;
		}
	else if( event->IsRightMouseRelease() ||
			(event->IsLeftMousePress() && (event->GetRepetition() || toggle.InRegion(event,TRUE)) ))
		{
		if(GetShowable())
			{
			SetShow(!GetShow());
			event->SetUsed(EW_USED_ALL);
			}
		}
	else
		{
		// send unutilized events to the parent class
		WDS_Button::Operate(event);
		}

	return FALSE;
	}



/******************************************************************************
long	WDS_HierarchyNode::Resize(EW_Resize *resize)

Keywords:
******************************************************************************/
long WDS_HierarchyNode::Resize(EW_Resize *resize)
	{
	// if first is not toggle
	if(&toggle != GetWidgetList()->ToHead())
		{
		// if toggle on list remove
		if(GetWidgetList()->SetCurrent(&toggle))
			RemoveWidget(&toggle);

		// force-add widget to beginning
		AddWidget(&toggle,EW_ADD_BEGINNING);
		}

	if(GetShowable())
		toggle.SetWidgetState(GetShow()? WDS_STATE_2: WDS_STATE_1);
	else
		toggle.SetWidgetState(WDS_STATE_0);

	// set node's miny to child toggle's prefy
	long minx,miny;
	long togsx,togsy;

	toggle.GetPrefSize(&togsx,&togsy);
	GetMinSize(&minx,&miny);
	SetMinSize(minx,togsy);

	WDS_Button::Resize(resize);

	long prefx,prefy;
	GetPrefSize(&prefx,&prefy);

	prefy=GetRootWindow()->GetFontHeight();

#if FALSE
	prefx=GetRootWindow()->StringWidth(GetLabel()) + 2*InterpretElement(GetWidgetState(),WDS_ELEMENT_DEPTH)
														+ 2*InterpretElement(GetWidgetState(),WDS_ELEMENT_INSET);
#endif

	if(showing)
		prefy*=2;

//	SetPrefSize(prefx,prefy);

	long resized=CheckSize(resize);

	// center toggle in y
	long sx,sy;

	GetSize(&sx,&sy);
	toggle.GetSize(&togsx,&togsy);
	toggle.SetOrigin(0,(sy-togsy)/2);

	// actually, there should be no direct children of a WDS_HierarchyNode
	if( GetWidgetList()->Resize(resize) )
		resized=TRUE;

	if(resized)
		SetDirty(EW_DIRTY_COMPLETE);

	return FALSE;
	}


/******************************************************************************
WDS_HierarchyNode *WDS_HierarchyNode::MatchChild(long margin,char *text)

	match against a child using string text and hiertype of

	margin indicates number of widgets at beginning of list to match against
	matched entries are moved to the end of the list

Keywords:
******************************************************************************/
WDS_HierarchyNode *WDS_HierarchyNode::MatchChild(long margin,char *text)
	{
	WDS_HierarchyNode *child=NULL;
	WDS_Widget *node;

#if HIER_NODE_MATCH_DEBUG
	printf("Match `%s' with margin %d\n",text,margin);
#endif

	parent_form.GetWidgetList()->ToHead();			// reset list
	parent_form.GetWidgetList()->PostIncrement();	// skip past header entry

	long search=0;
	while(search<margin)
		{
		long is_form;

		node=(WDS_Widget *)parent_form.GetWidgetList()->PostIncrement();

		// if this previous entry is a hierarchy node AND the names match, re-use

		// if is a hier-form (like owned parent_form) OR a hier node (like *this)
		if( (is_form=(node->GetTypeCode()==GetHierForm()->GetTypeCode()))  || node->GetTypeCode()==this->GetTypeCode() )
			{
			WDS_HierarchyNode *hnode;

			if(is_form)
				hnode=(WDS_HierarchyNode *)( ((WDS_HierarchyForm *)node)->GetWidgetList()->ToHead() );
			else
				hnode=(WDS_HierarchyNode *)node;

#if HIER_NODE_MATCH_DEBUG
				printf("VS %s\n",hnode->GetHierString()->Buffer());
#endif

			if( hnode->GetTypeCode()==this->GetTypeCode() && !( hnode->GetHierString()->Compare(text) ) )
				{
#if HIER_NODE_MATCH_DEBUG
				printf("found\n");
#endif

				parent_form.GetWidgetList()->RemoveNode(node);
				parent_form.GetWidgetList()->AppendNode(node);

				child=hnode;

				break;
				}
			}

		search++;
		}

	return child;
	}



/******************************************************************************
	WDS_HierarchyForm::WDS_HierarchyForm(void)

******************************************************************************/
WDS_HierarchyForm::WDS_HierarchyForm(void)
	{
	SetName("HierarchyForm");
	SetMode(WDS_FORM_COLUMN);
	SetCompound(TRUE);

	if(FALSE && !look_initialized)
		{
		look_initialized=TRUE;
		InitializeLook(WDS_LOOK_DEFAULT);
		}
	}



/******************************************************************************
long WDS_HierarchyForm::Resize(EW_Resize *resize)

Keywords:
******************************************************************************/
long WDS_HierarchyForm::Resize(EW_Resize *resize)
	{
#if HIER_FORM_RESIZE_DEBUG

	long pos[2],size[2];
	GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);
	printf("WDS_HierarchyForm::Resize() 0x%x %d,%d %d,%d\n",(long)this,pos[0],pos[1],size[0],size[1]);

#endif

	this->WDS_Form::Resize(resize);

#if HIER_FORM_RESIZE_DEBUG

	GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);
	printf("                            0x%x %d,%d %d,%d\n",(long)this,pos[0],pos[1],size[0],size[1]);

#endif

	// clear form if parent is not another WDS_HierarchyForm
	SetClear( !(((WDS_Widget *)GetParentWidget())->WDS_WIDGET_IS(WDS_HierarchyForm)) );

	return FALSE;
	}



/******************************************************************************
void	WDS_HierarchyForm::DeleteChildren(long number)

	delete specfied number of children from current widgetlist setting

	if number<0, reset current and delete all children

	all non-hierarchy children are removed from the list, not deleted

Keywords:
******************************************************************************/
void WDS_HierarchyForm::DeleteChildren(long number)
	{
	WDS_Widget *node;
	WDS_Widget *first;
	char comment[256];

	if(HIER_DELETE_DEBUG)
		printf("0x%x WDS_HierarchyForm::DeleteChildren(%d)\n",(unsigned int)this,(int)number);

	if(number<0)
		{
		number=GetWidgetList()->GetNumberNodes();

		GetWidgetList()->ToHead();

//no	GetWidgetList()->PostIncrement();	// skip past header entry
		}

	long counter=number;

	first=(WDS_Widget *)GetWidgetList()->GetCurrent();

	if(HIER_DELETE_DEBUG)
		printf(" unshow %d\n",(int)counter);

	if(counter>0)
		{
		// unshow any child hierarchy nodes (under their form)
		while( (counter--)>0 )
			{
			node=(WDS_Widget *)GetWidgetList()->PostIncrement();
			if(!node)
				{
				sprintf(comment,"WDS_HierarchyForm::DeleteChildren(%d) only %d nodes (pass 1)",(int)number,(int)(number-counter));
				EW_PRINT(EW_APP,EW_ERROR,comment);
				break;
				}

			if(HIER_DELETE_DEBUG)
				printf(" checking 0x%x\n",(unsigned int)node);

			if( node->WDS_WIDGET_IS(WDS_HierarchyNode) && ((WDS_HierarchyNode *)node)->GetHierForm() == this )
				{
				// is a hier-node and is *this

				EW_PRINT(EW_WIDGET,EW_ERROR,"WDS_HierarchyForm::DeleteChildren() form's owner node still showing");
				}
			else if( node->GetTypeCode() == this->GetTypeCode() )
				{
				// is a hier-form

				WDS_HierarchyNode *hnode=(WDS_HierarchyNode *)( ((WDS_HierarchyForm *)node)->GetWidgetList()->ToHead() );

				if(hnode->WDS_WIDGET_IS(WDS_HierarchyNode))
					{
					// backup one since the current will be replaced
					GetWidgetList()->PostDecrement();

					if(HIER_DELETE_DEBUG)
						printf(" unshow 0x%x under 0x%x (first=0x%x)\n",(unsigned int)hnode,(unsigned int)node,(unsigned int)first);

					hnode->SetShow(FALSE);

					if(first==node)
						first=hnode;
					}
				else
					{
					sprintf(comment,"WDS_HierarchyForm::DeleteChildren() first child 0x%x of hier form 0x%x not a hier node",
																											(unsigned int)node,(unsigned int)hnode);
					EW_PRINT(EW_WIDGET,EW_ERROR,comment);
					}
				}
			}
		}

	// delete all child hierarchy nodes

	counter=number;
	GetWidgetList()->SetCurrent(first);

	if(HIER_DELETE_DEBUG)
		printf(" delete %d\n",(int)counter);

	if(counter>0)
		{
		while( (counter--)>0 )
			{
			node=(WDS_Widget *)GetWidgetList()->GetCurrent();
			if(!node)
				{
				sprintf(comment,"WDS_HierarchyForm::DeleteChildren(%d) only %d nodes (pass 2)",(int)number,(int)(number-counter));
				EW_PRINT(EW_APP,EW_ERROR,comment);
				break;
				}

			// if not our owner
			if( node->WDS_WIDGET_IS(WDS_HierarchyNode) && ((WDS_HierarchyNode *)node)->GetHierForm() == this )
				{
				EW_PRINT(EW_WIDGET,EW_ERROR,"WDS_HierarchyForm::DeleteChildren() form's owner node still showing");
				continue;
				}

			if(HIER_DELETE_DEBUG)
				printf(" removing old node 0x%x\n",(unsigned int)node);

			GetWidgetList()->RemoveNode(node);

			// don't have the right to delete non-hierarchy widgets

			if( node->WDS_WIDGET_IS(WDS_HierarchyNode) )
				{
				if(HIER_DELETE_DEBUG)
					printf(" deleting old hierarchy node 0x%x\n",(unsigned int)node);

				delete (WDS_HierarchyNode *)node;
				}
			else if( node->GetTypeCode()==this->GetTypeCode() )
				{
				sprintf(comment,
	"WDS_HierarchyForm::DeleteChildren()\n hier form 0x%x still contains hier form 0x%x on second pass\n POTENTIAL MEMORY LEAK",
																							(unsigned int)this,(unsigned int)node);
				EW_PRINT(EW_WIDGET,EW_ERROR,comment);
				}
			}
		}

	if(HIER_DELETE_DEBUG)
		printf("0x%x WDS_HierarchyForm::DeleteChildren() complete\n",(unsigned int)this);
	}
