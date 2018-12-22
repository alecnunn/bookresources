/******************************************************************************

Coral Tree ew/node.cc
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


#include "ew.h"



#define EW_NUDE_DEBUG		FALSE
#define EW_CONSTRAIN_DEBUG	FALSE



#if	EW_TRACK_NEWED
/******************************************************************************
EW_Node *EW_Node::last_newed;

Since EW_Node::operator new() occurs before the constructor and there
is no way to pass the absence of information, the flag EW_Node::newed
can *not* indicate allocation at constructor invocation.  Each newed
address needs to be stored so the constructor can determine if its
object was newed or not.  THIS IS NOT RE-ENTRANT!  If re-entrancy is
desired, switch to a set of addresses.
******************************************************************************/

EW_Node *EW_Node::last_newed=(EW_Node *)0;

#endif /* EW_TRACK_NEWED */



/******************************************************************************
		EW_Node::EW_Node(void)


Keywords:
******************************************************************************/
EW_Node::EW_Node(void)
	{
#if	EW_TRACK_NEWED
//	newed=(last_newed==this);
	newed=(last_newed!=NULL);

#if EW_NUDE_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"EW_Node::EW_Node() 0x%p/0x%p newed=%d",(long)this,last_newed,newed);

	long difference= ((long)this-(long)last_newed);
	if(difference>0 && newed)
		EW_PRINT(EW_WIDGET,EW_LOG,"EW_Node::EW_Node() %sdifference = %d%s",ewColorCode(3),difference,ewColorCode(0));
#endif

	last_newed=NULL;

#endif /* EW_TRACK_NEWED */

	widgetlist = new EW_WidgetList();

//	SetDirty(EW_DIRTY_NOT);
	dirty[0]=EW_DIRTY_NOT;
	dirty[1]=EW_DIRTY_NOT;

	SetNodeSizing(EW_NODE_SIZING_NULL);
	}



/******************************************************************************
		EW_Node::~EW_Node(void)

Keywords:
******************************************************************************/
EW_Node::~EW_Node(void)
	{
	if(widgetlist)
		delete widgetlist;
	else
		EW_PRINT(EW_WINDOW,EW_ERROR,"EW_Node::~EW_Node() can't delete NULL widgetlist");
	}



#if	EW_TRACK_NEWED
/******************************************************************************
void	*EW_Node::operator new(size_t n)

******************************************************************************/
void *EW_Node::operator new(size_t n)
	{
	register void *p=::operator new(n);

	last_newed=(EW_Node *)p;

#if EW_NUDE_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"new 0x%p",p);
#endif

	return p;
	}



/******************************************************************************
void	EW_Node::operator delete(void *p)

******************************************************************************/
void EW_Node::operator delete(void *p)
	{
	long was_newed=((EW_Node *) p)->GetNewed();

#if EW_NUDE_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"delete 0x%p newed=%d",p,was_newed);
#endif

	if(was_newed)
		::operator delete(p);
	else
		EW_PRINT(EW_APP,EW_ERROR,"EW_Node::operator delete() can't delete a non-nude node 0x%p",p);
	}
#endif /* EW_TRACK_NEWED */



/******************************************************************************
void    EW_Node::AddWidgetProtected(EW_Widget *widget,long placement)

Keywords:
******************************************************************************/
void EW_Node::AddWidgetProtected(EW_Widget *widget,long placement)
    {
	switch(placement)
		{
		case EW_ADD_BEGINNING:
			widgetlist->ToHead();
			widgetlist->InsertBefore(widget);
			break;

		case EW_ADD_BEFORE:
			widgetlist->InsertBefore(widget);
			break;

		case EW_ADD_AFTER:
			widgetlist->InsertAfter(widget);
			break;

		case EW_ADD_END:
		default:
			widgetlist->AppendNode(widget);
			break;
		}

	widget->SetRootWindow(this->GetRootWindow());
	widget->SetParentWidget( (EW_Widget *)(IsWindow()? NULL: this) );
	SetDirty(EW_DIRTY_COMPLETE);
	}



/******************************************************************************
void    EW_Node::RemoveWidgetProtected(EW_Widget *widget)

Keywords:
******************************************************************************/
void EW_Node::RemoveWidgetProtected(EW_Widget *widget)
    {
	widgetlist->RemoveNode(widget);
	widget->SetRootWindow((EW_Window *)NULL);
	widget->SetParentWidget((EW_Widget *)NULL);
	SetDirty(EW_DIRTY_COMPLETE);
    }



/******************************************************************************
EW_Node		*EW_Node::GetParent(void)

******************************************************************************/
EW_Node *EW_Node::GetParent(void)
	{
	EW_Node *node;

	if(( node=(EW_Node *)((void *)GetParentWidget()) ))
		return node;
	else
		return (EW_Node *)((void *)GetRootWindow());
	}



/******************************************************************************
void    EW_Node::GetChildrensExtents(long *mincx,long *mincy,
                                                        long *maxcx,long *maxcy)

    return limits of immediate children

    does not pre-rewind widgetlist; starts from current position

Keywords:
******************************************************************************/
void EW_Node::GetChildrensExtents(long *mincx,long *mincy,long *maxcx,long *maxcy)
	{
	EW_Widget *node;

	long pos[2],size[2];
	long min[2],max[2];
	long m;
	long first=TRUE;

	min[0]=0;
	min[1]=0;
	max[0]=0;
	max[1]=0;

	while( (node=widgetlist->Next()) != NULL)
		{
		node->CalcTruePosition();

		node->GetGeometry(&pos[0],&pos[1],&size[0],&size[1]);

//  printf("%x geom %d,%d %d,%d\n",(long)node,pos[0],pos[1],size[0],size[1]);

		for(m=0;m<2;m++)
			{
			if(first || min[m]>pos[m])
				min[m]=pos[m];

			if(first || max[m]<pos[m]+size[m])
				max[m]=pos[m]+size[m];
			}

		first=FALSE;
		}

//  printf("%x extents %d,%d %d,%d\n",(long)this,min[0],min[1],max[0],max[1]);

	*mincx=min[0];
	*mincy=min[1];
	*maxcx=max[0];
	*maxcy=max[1];
	}



/******************************************************************************
long	EW_Node::ConstrainSize(long hold_top,long maxx,long maxy)

	if hold_top is TRUE, scale using bottom edge, not top

	if bound is TRUE, move as neccessary to place in bounds
		(such as for window open)

	returns TRUE if constrained (implies fixed)

******************************************************************************/
long EW_Node::ConstrainSize(long hold_top,long bound,long maxx,long maxy)
	{
	if(nodesizing)
		{
		long cmin[2],cmax[2];
		long pos[2],len[2];
		long npos[2],nlen[2];
		long margin[2];

		margin[0]=8;
		margin[1]=24;

		widgetlist->ToHead();
		GetChildrensExtents(&cmin[0],&cmin[1],&cmax[0],&cmax[1]);

		GetGeometry(&pos[0],&pos[1],&len[0],&len[1]);

		long m;
		for(m=0;m<2;m++)
			{
			npos[m]=pos[m];
			nlen[m]=len[m];

			if(nodesizing&(m? EW_NODE_SIZING_FIT_CHILD_Y: EW_NODE_SIZING_FIT_CHILD_X))
				nlen[m]=cmax[m];

			if(m && hold_top)
				{
#if EW_CONSTRAIN_DEBUG
			if(maxx>0)
				printf("reduce %d by %d  ",npos[m],nlen[m]-len[m]);
#endif
				npos[m]-= (nlen[m]-len[m]);
				}

			long limit=(m? maxy: maxx)-margin[m];
			long excess=0;
			if(limit>0)
				excess= npos[m]+nlen[m]-limit;

			limit=nlen[m]-len[m];
			if(!bound && excess>limit)
				excess=limit;

#if EW_CONSTRAIN_DEBUG
			if(maxx>0)
				printf("excess[%d]=%d ",m,excess);
#endif

			if(excess>0)
				npos[m]-=excess;
/*
			if(npos[m]<0)
				npos[m]=0;
*/
			}

#if EW_CONSTRAIN_DEBUG
		if(maxx>0)
			{
			printf("extent min %d,%d max %d,%d ",cmin[0],cmin[1],cmax[0],cmax[1]);
			printf("maxx,maxy %d,%d ",maxx,maxy);
			printf("node from %d,%d %d,%d to %d,%d %d,%d\n",pos[0],pos[1],len[0],len[1],npos[0],npos[1],nlen[0],nlen[1]);
			}
#endif

		SetGeometry(npos[0],npos[1],nlen[0],nlen[1]);

/*
		if(maxx>0)
			{
			GetGeometry(&pos[0],&pos[1],&len[0],&len[1]);
			printf("confirmed %d,%d %d,%d\n",pos[0],pos[1],len[0],len[1]);
			}
*/

		return TRUE;
		}

	return FALSE;
	}



/******************************************************************************
void	EW_Node::SetDirty(long index,long set)


******************************************************************************/
void EW_Node::SetDirty(long index,long set)
	{
//	printf("SetDirty(%d,%d) 0x%x 0x%x\n",index,set,this,GetRootWindow());
	dirty[index]=set;
	}
