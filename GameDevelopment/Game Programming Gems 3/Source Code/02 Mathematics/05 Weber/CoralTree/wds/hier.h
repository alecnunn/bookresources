
/******************************************************************************

Coral Tree wds/hier.h
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
class WDS_HierarchyForm:public WDS_Form

	WARNING: destruction of a WDS_HierarchyForm belonging to a showing
		WDS_HierarchyNode can cause a fault.  The WDS_HierarchyNode
		must be unshown first.

Keywords:
******************************************************************************/
class WDS_HierarchyForm:public WDS_Form
    {
	WDS_DECLARE_TYPE(WDS_HierarchyForm);

	private:

static	long	look_initialized;

	public:
				WDS_HierarchyForm(void);

virtual			~WDS_HierarchyForm(void)
					{
//					printf("destroy WDS_HierarchyForm 0x%x\n",(long)this);
					DeleteChildren(-1);		// delete all children
					};

		void	DeleteChildren(long number);

		long    Resize(EW_Resize *resize);

		void	InitializeLook(long set);
static	void	InitializeLookCallbackInit(WDS_Look *look,long set);
	};



/******************************************************************************
class	WDS_HierarchyToggle:public WDS_Button

******************************************************************************/
class WDS_HierarchyToggle:public WDS_Button
    {
	WDS_DECLARE_TYPE(WDS_HierarchyToggle);

	public:
				WDS_HierarchyToggle(void)	{};
virtual	void	Draw(EW_Redraw *redraw);
	};



/******************************************************************************
class	WDS_HierarchyNode:public WDS_Button

	WARNING: when showing, a WDS_HierarchyNode is the first child of it's own
		WDS_HierarchyForm parent_form which it inserts between itself
		and the original parent.  Any WDS_HierarchyNode must be unshown
		before deletion or a cyclic destruction can occur.
		The destructor for WDS_HierarchyNode accounts for this,
		but the destructor for WDS_HierarchyForm can only do this for open
		nodes below it, not if it is the form of an open node.
		This latter case can cause the fault.

Keywords:
******************************************************************************/
class WDS_HierarchyNode:public WDS_Button
    {
	WDS_DECLARE_TYPE(WDS_HierarchyNode);

    private:

static	long	look_initialized;

		long	showing;
		long	showable;

	protected:

		WDS_HierarchyForm	parent_form;	// not a pointer, we own the parent
		WDS_HierarchyToggle	toggle;			// toggle for showing
		WDS_String			hier_string;	// name to match against

    public:
				WDS_HierarchyNode(void);

virtual			~WDS_HierarchyNode(void)
					{
//					printf("destroy WDS_HierarchyNode 0x%x\n",(long)this);

					SetShow(FALSE);
					};

		long	Operate(EW_Event *event);

		void	InitializeLook(long set);
static	void	InitializeLookCallbackInit(WDS_Look *look,long set);

		void	SetShowable(long set)		{ showable=set;	};
		long	GetShowable(void)			{ return showable; };

		void	SetShow(long show);
		long	GetShow(void)				{ return showing; };

		// for comparisons
		WDS_String			*GetHierString(void)	{ return &hier_string; };
		WDS_HierarchyForm	*GetHierForm(void)		{ return &parent_form; };
		WDS_HierarchyToggle	*GetHierToggle(void)	{ return &toggle; };

		// this basically is just to add widgets and set labels
		// using whatever type-specific data/actions are neccessary
virtual	void	ShowEntry(void)
					{
					EW_PRINT(EW_WIDGET,EW_WARN,
						"virtual WDS_HierarchyNode::ShowEntry not replaced");
					};

virtual	void	Draw(EW_Redraw *redraw);
virtual	long	Resize(EW_Resize *resize);

		WDS_HierarchyNode *MatchChild(long margin,char *text);
	};
