/******************************************************************************

Coral Tree wds/mfd.cc
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


#define	WDS_MFD_DEBUG	FALSE



/******************************************************************************
		WDS_MFD_Navigator::WDS_MFD_Navigator(void)

******************************************************************************/
WDS_MFD_Navigator::WDS_MFD_Navigator(void)
	{
	SetName("WDS_MFD_Navigator");

	SetMinSize(128,-1);
	SetMaxSize(128,-1);
	SetPrefSize(128,-1);
	SetBounds(WDS_HORIZONTAL,EW_BOUNDS_IGNORE_PARENT);

	// accept bypassed events
	SetEventMask(GetLocalEventMask());
	GetLocalEventMask()->SetSource(EW_EVENT_COMMON|EW_EVENT_FOCUS);
	GetLocalEventMask()->SetWidgetFlags(WDS_BYPASS_FLAGS);
	}



/******************************************************************************
		WDS_MFD_Navigator::~WDS_MFD_Navigator(void)

******************************************************************************/
WDS_MFD_Navigator::~WDS_MFD_Navigator(void)
	{
	}



/******************************************************************************
void	WDS_MFD_Navigator::Register(long on)

******************************************************************************/
void WDS_MFD_Navigator::Register(long on)
	{
	// register for keyboard
	EW_Event keymask;

	keymask.SetSIS(EW_EVENT_KEYBOARD,EW_KEY_ALL,EW_STATE_PRESS);
	RegisterEventBypass(on,&keymask,WDS_BYPASS_FLAG_NULL,FALSE);
	}



/******************************************************************************
long	WDS_MFD_Navigator::Operate(EW_Event *event)

******************************************************************************/
long WDS_MFD_Navigator::Operate(EW_Event *event)
	{
	WDS_TextList::Operate(event);

	// ignore non-keyboard events
	if(!event->IsKeyPressed(EW_KEY_ALL))
		return FALSE;

	// ignore used events
	if(event->GetUsed())
		return FALSE;

	// ignore non-bypassed events
	if(!event->GetWidgetFlags()&WDS_BYPASS_EVENT)
		return FALSE;

	// ignore bypass-focus events
	if(event->GetWidgetFlags()&WDS_BYPASS_IN_FOCUS)
		return FALSE;

	WDS_Widget *parent=(WDS_Widget *)GetParentWidget();

	if(parent && parent->WDS_WIDGET_IS(WDS_MFD))
		{
		WDS_MFD *mfd=(WDS_MFD *)parent;

		mfd->ReactToNavigation(event);
		}

	return FALSE;
	}



/******************************************************************************
void    WDS_MFD::TextListDeActivationCallback(WDS_Activation *activation)

******************************************************************************/
void WDS_MFD::TextListDeActivationCallback(WDS_Activation *activation)
    {
    WDS_TextList *thislist=(WDS_TextList *)activation->GetThisWidget();
    WDS_MFD *mfd=(WDS_MFD *)activation->GetTargetWidget();

	long newstate=activation->GetState();

//	printf("WDS_MFD::TextListDeActivationCallback() newstate=%d (%s)\n",newstate,thislist->GetSelectedString());

	if(newstate==4)
		{
		long index=thislist->GetSelectedNumber();

		if(index<mfd->GetDepth())
			mfd->SelectAncestor(index);
		else
			mfd->SelectNodeRelative(thislist->GetSelectedString());
		}

//	activation->GetEvent()->Print(0);
	}



/******************************************************************************
		WDS_MFD::WDS_MFD(void)

******************************************************************************/
WDS_MFD::WDS_MFD(void)
	{
	SetName("WDS_MFD");

	currentnode=NULL;
	defaultwidget=NULL;
	need_change=TRUE;

	navigator=new WDS_MFD_Navigator();
	AddWidget(navigator);

	navigator->SetStringList(&stringlist);

    SetDirection(WDS_HORIZONTAL);
    SetFlags(WDS_PARTITION_FLAGS_ATTACH);

	WDS_Activation activation;
	activation.SetTargetWidget(this);

	navigator->SetStateChangeCallback(WDS_STATE_ALL,&WDS_MFD::TextListDeActivationCallback,&activation);
	}



/******************************************************************************
		WDS_MFD::~WDS_MFD(void)

******************************************************************************/
WDS_MFD::~WDS_MFD(void)
	{
	}



/******************************************************************************
void	WDS_MFD::Register(long on)

******************************************************************************/
void WDS_MFD::Register(long on)
	{
	navigator->Register(on);
	}



/******************************************************************************
void	WDS_MFD::ReactToNavigation(EW_Event *event)

******************************************************************************/
void WDS_MFD::ReactToNavigation(EW_Event *event)
	{
	switch(event->GetItem())
		{
		case '/':
			if(currentpath.GetLength())
				{
				currentpath.NewLength(0);
				currentnode=NULL;
				need_change=TRUE;
				}
			return;

		case EW_KEY_ESC:
			if(GetDepth()>1)
				SelectAncestor(GetDepth()-2);
			return;

		default:
			WDS_MFD_Reference *reference;

			long m=GetDepth();
			long ascii=(event->GetItem()&255);

			referencelist.ToIndex(m);
			while( (reference=referencelist.PostIncrement()) != NULL)
				{
				long ref_ascii=reference->GetBaseName()[0];

				if(ref_ascii>='A' && ref_ascii<='Z')
					ref_ascii+='a'-'A';

#if WDS_MFD_DEBUG
				printf("compare '%c' '%c' (%s)\n",ref_ascii,ascii,reference->GetNode()->GetName());
#endif

				if(ref_ascii==ascii)
					{
					SelectNodeRelative(stringlist.ToIndex(m)->Buffer());
					return;
					}

				m++;
				}
			break;
		}

	event->Print((long)this);
	}



/******************************************************************************
void	WDS_MFD::CreateMFDNode(char *name,WDS_Widget *widget)

******************************************************************************/
void WDS_MFD::CreateMFDNode(char *name,WDS_Widget *widget)
	{
	// if not a child already, add
	if(widget && !GetWidgetList()->SetCurrent(widget))
		AddWidget(widget);

	// create association
	WDS_MFD_Node *mfdnode=new WDS_MFD_Node();

	mfdnode->SetName(name);
	mfdnode->SetWidget(widget);

	nodelist.AppendNode(mfdnode);

	need_change=TRUE;
	}
	


/******************************************************************************
void	WDS_MFD::DeleteMFDNode(char *name)

******************************************************************************/
void WDS_MFD::DeleteMFDNode(char *name)
	{
	}



/******************************************************************************
void	WDS_MFD::SelectAncestor(long index)

******************************************************************************/
void WDS_MFD::SelectAncestor(long index)
	{
	// set up ancestry
	char *point1=currentpath.Buffer();
	char *point2=point1;

	long m=0;
	while(m<index && point2 && (point2-point1)<currentpath.GetLength())
		{
		point2=strstr(point2,"->");

		if(point2 && m<index-1)
			point2+=2;

		m++;
		}

	if(point2)
		{
		currentpath.NewBuffer(point1);
		currentpath.NewLength(point2-point1);
		}

	currentnode=NULL;

#if WDS_MFD_DEBUG
	printf("SelectAncestor(%d) currentpath (%s)\n",index,currentpath.Buffer());
#endif
	need_change=TRUE;
	}



/******************************************************************************
void	WDS_MFD::SelectNodeAbsolute(char *name)

******************************************************************************/
void WDS_MFD::SelectNodeAbsolute(char *name)
	{
	}



/******************************************************************************
void	WDS_MFD::SelectNodeRelative(char *newname)

******************************************************************************/
void WDS_MFD::SelectNodeRelative(char *newname)
	{
#if WDS_MFD_DEBUG
	printf("SelectNodeRelative(%s) currentpath (%s)\n",newname,currentpath.Buffer());
#endif

	WDS_String fullname;

	if(currentpath.GetLength())
		{
		fullname.CopyFromString(&currentpath);
		fullname.InsertBuffer(fullname.GetLength(),"->");
		}

	fullname.InsertBuffer(fullname.GetLength(),newname);

//	printf("SelectNodeRelative() (%s)\n",fullname.Buffer());

	// find node
	WDS_Widget *lastwidget=currentnode? currentnode->GetWidget(): defaultwidget;
	WDS_MFD_Node *node;

	currentnode=NULL;

	nodelist.ToHead();
	while( (node=nodelist.PostIncrement()) != NULL)
		{
		if(!fullname.Compare(node->GetName()))
			{
			currentnode=node;
			break;
			}
		}

	// check if has children
	long has_children=FALSE;

	if(currentnode)
		{
		long currentlen=strlen(fullname.Buffer());

		nodelist.ToHead();
		while( (node=nodelist.PostIncrement()) != NULL)
			{
			char *name=node->GetName();

#if WDS_MFD_DEBUG
			printf("name (%s)\n",name);
#endif

			if(!strncmp(fullname.Buffer(),name,currentlen) && (!currentlen || !strncmp(&name[currentlen],"->",2)) )
				{
#if WDS_MFD_DEBUG
				printf("  child\n");
#endif

				has_children=TRUE;
				break;
				}
			}
		}

	if(has_children)
		{
		currentpath.CopyFromString(&fullname);
		need_change=TRUE;
		}

	if( (!currentnode && lastwidget) || (currentnode && currentnode->GetWidget()!=lastwidget) )
		{
		need_change=TRUE;
		}
	}



/******************************************************************************
void	WDS_MFD::ChangeList(void)

******************************************************************************/
void WDS_MFD::ChangeList(void)
	{
	WDS_String *string;
	WDS_MFD_Reference *reference;

#if WDS_MFD_DEBUG
	printf("ChangeList() currentpath (%s)\n",currentpath.Buffer());
#endif

	long currentlen=strlen(currentpath.Buffer());

	stringlist.Clear();
	referencelist.Clear();

	// set up ancestry
	string=new WDS_String();
	string->NewBuffer("Application");
	stringlist.AppendNode(string);

	reference=new WDS_MFD_Reference();
	referencelist.AppendNode(reference);

	if(currentlen)
		{
		char *point1=currentpath.Buffer();
		char *point2;

		while(point1!=NULL)
			{
			point2=strstr(point1,"->");

			string=new WDS_String();
			string->NewBuffer(point1);

			if(point2)
				{
				string->NewLength(point2-point1);
				point1=point2+2;
				}
			else
				point1=NULL;

			stringlist.AppendNode(string);

			reference=new WDS_MFD_Reference();
			referencelist.AppendNode(reference);
			}
		}

	stringlist.ToTail();
	stringlist.GetCurrent()->InsertBuffer(stringlist.GetCurrent()->GetLength(),"    <-");
	stringlist.GetCurrent()->SetHighlight(WDS_HIGHLIGHT_LINE_OUT);

	SetDepth(stringlist.GetNumberNodes());

	// find direct children
	WDS_MFD_Node *node;

	nodelist.ToHead();
	while( (node=nodelist.PostIncrement()) != NULL)
		{
		char *name=node->GetName();

#if WDS_MFD_DEBUG
		printf("name (%s)\n",name);
#endif

		// if starts with path
		if(!strncmp(currentpath.Buffer(),name,currentlen) && (!currentlen || !strncmp(&name[currentlen],"->",2)) )
			{
#if WDS_MFD_DEBUG
			printf("  in path\n");
#endif

			// if not a sub within current
			if(!strstr(&name[currentlen+2],"->"))
				{
#if WDS_MFD_DEBUG
				printf("  no sub\n");
#endif

				string=new WDS_String();
				string->NewBuffer(&name[currentlen? currentlen+2: 0]);

				stringlist.AppendNode(string);

				reference=new WDS_MFD_Reference();
				reference->SetNode(node);
				reference->SetBaseName(string->Buffer());
				referencelist.AppendNode(reference);
				}
			}
		}
	}



/******************************************************************************
void	WDS_MFD::ChangeDisplay(void)

******************************************************************************/
void WDS_MFD::ChangeDisplay(void)
	{
	WDS_Widget *widget;

	GetWidgetList()->ToHead();
	while( (widget=(WDS_Widget *)GetWidgetList()->PostIncrement()) != NULL)
		widget->SetInvisible(TRUE);

	navigator->SetInvisible(FALSE);

	if(currentnode)
		{
		widget=currentnode->GetWidget();

		if(!widget)
			widget=defaultwidget;

		if(widget)
			widget->SetInvisible(FALSE);
		}
	else if(defaultwidget)
		defaultwidget->SetInvisible(FALSE);
	}



/******************************************************************************
long	WDS_MFD::Resize(EW_Resize *resize)

******************************************************************************/
long WDS_MFD::Resize(EW_Resize *resize)
	{
	if(need_change)
		{
		need_change=FALSE;

		navigator->SetSelectedNumber(-1);

		ChangeList();
		ChangeDisplay();
		SetDirty(EW_DIRTY_COMPLETE);
		}

	// if navigator is not first widget, adjust
	if(GetWidgetList()->ToHead()!=navigator)
		{
		if(GetWidgetList()->SetCurrent(navigator))
			RemoveWidget(navigator);

		AddWidget(navigator,EW_ADD_BEGINNING);
		}

	return WDS_Partition::Resize(resize);
	}
