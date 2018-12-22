/******************************************************************************

Coral Tree wds/mfd.h
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



class WDS_MFD_NodeList;



/******************************************************************************
class	WDS_MFD_Navigator: public WDS_Form

******************************************************************************/
class WDS_MFD_Navigator: public WDS_TextList
	{
	public:

					WDS_MFD_Navigator(void);
virtual				~WDS_MFD_Navigator(void);

virtual	long		Operate(EW_Event *event);
virtual	void		Register(long on);
	};



/*****************************************************************************
class	WDS_MFD_Node : public GPL_DoubleNode

*****************************************************************************/
class WDS_MFD_Node : public GPL_DoubleNode
	{
	private:

		WDS_String			name;
		WDS_Widget			*widget;

	public:

					WDS_MFD_Node(void)
						{
						widget=NULL;
						};


		void		SetName(char *set)			{ name.NewBuffer(set); };
		char		*GetName(void)				{ return name.Buffer(); };

		void		SetWidget(WDS_Widget *set)	{ widget=set; };
		WDS_Widget	*GetWidget(void)			{ return widget; };
	};



/*****************************************************************************
class	WDS_MFD_NodeList : public GPL_DoubleList<WDS_MFD_Node>

*****************************************************************************/
class WDS_MFD_NodeList : public GPL_DoubleList<WDS_MFD_Node>
	{
	};



/*****************************************************************************
class	WDS_MFD_Reference : public GPL_DoubleNode

*****************************************************************************/
class WDS_MFD_Reference : public GPL_DoubleNode
	{
	private:

		WDS_String		basename;
		WDS_MFD_Node	*node;

	public:

						WDS_MFD_Reference(void)
							{
							node=NULL;
							};

		void		SetBaseName(char *set)		{ basename.NewBuffer(set); };
		char		*GetBaseName(void)			{ return basename.Buffer(); };

		void			SetNode(WDS_MFD_Node *set)	{ node=set; };
		WDS_MFD_Node	*GetNode(void)				{ return node; };
	};



/*****************************************************************************
class	WDS_MFD_ReferenceList : public GPL_DoubleList<WDS_MFD_Reference>

*****************************************************************************/
class WDS_MFD_ReferenceList : public GPL_DoubleList<WDS_MFD_Reference>
	{
	};



/*****************************************************************************
class	WDS_MFD : public WDS_Partition

*****************************************************************************/
class WDS_MFD : public WDS_Partition
	{
	WDS_DECLARE_TYPE(WDS_MFD);

	private:

		WDS_MFD_Navigator		*navigator;
		WDS_MFD_Node			*currentnode;
		WDS_Widget				*defaultwidget;

		WDS_MFD_ReferenceList	referencelist;
		WDS_MFD_NodeList		nodelist;
		WDS_StringList			stringlist;
		WDS_String				currentpath;
		long					need_change;
		long					depth;

		void	ChangeDisplay(void);
		void	ChangeList(void);

	public:

				WDS_MFD(void);
virtual			~WDS_MFD(void);

		void	CreateMFDNode(char *name,WDS_Widget *widget);
		void	DeleteMFDNode(char *name);

		void		SetDefaultWidget(WDS_Widget *set)	{ defaultwidget=set; };
		WDS_Widget	*GetDefaultWidget(void)			{ return defaultwidget; };

		void	SetDepth(long set)		{ depth=set; };
		long	GetDepth(void)			{ return depth; };

		void	SelectNodeAbsolute(char *name);
		void	SelectNodeRelative(char *name);
		void	SelectAncestor(long index);

		void	ReactToNavigation(EW_Event *event);

virtual	void	Register(long on);

virtual	long	Resize(EW_Resize *resize);

static	void	TextListDeActivationCallback(WDS_Activation *activation);
	};
