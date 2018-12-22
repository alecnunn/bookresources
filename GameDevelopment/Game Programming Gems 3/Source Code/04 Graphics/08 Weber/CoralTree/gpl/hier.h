/******************************************************************************

Coral Tree gpl/hier.h
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

#ifndef GPL_HIER_H
#define GPL_HIER_H

#include <gpl/str.h>

/***********************************************************************
class GPL_Node

Node in a hierarchy.
***********************************************************************/
class GPL_Node
{
	public:
		GPL_Node(void) {}
		virtual ~GPL_Node(void) {}

		const GPL_String & GetName(void) const { return name; }
		GPL_Node *GetHierarchy (void) const { return hierarchy; }
		virtual GPL_List<GPL_Node *> GetNodes(void)
		{
			return GPL_List<GPL_Node *>();
		}

		void SetName(const GPL_String & n) { name = n; }
		void SetHierarchy(GPL_Node *h) { hierarchy = h; }

		virtual GPL_List<GPL_Node *> FindNodes(const GPL_String & n);

	protected:
		GPL_String name;
		GPL_Node *hierarchy;
};

/***********************************************************************
class GPL_Hierarchy

Hierarchy of nodes.
***********************************************************************/
class GPL_Hierarchy : public GPL_Node
{
	public:
		GPL_Hierarchy(void) {}
		virtual ~GPL_Hierarchy(void) {}

		virtual GPL_List<GPL_Node *> GetNodes(void) { return nodes; }

		virtual void InsertNode(GPL_Node *n, const uint32 i = 0)
		{
			nodes.Insert(n, i);
		}
		virtual void RemoveNode(GPL_Node *n)
		{
			register const int32 i = nodes.Find(n);
			if (-1 != i)
			{
				nodes.Remove(i);
			}
		}

	protected:
		GPL_List<GPL_Node *> nodes;
};

/***********************************************************************
class GPL_HierarchySearcher

Hierarchy searcher.
***********************************************************************/
class GPL_HierarchySearcher
{
	public:
		GPL_HierarchySearcher(void) {}
		virtual ~GPL_HierarchySearcher(void) {}

		virtual void SetHierarchy(GPL_Hierarchy *h) { hierarchy = h; }
		virtual void SetDelimiter(const GPL_String & d) { delimiter = d; }

		virtual GPL_List<GPL_Node *> FindNodes(const GPL_String & n) const
		{
			return FindNodes(hierarchy, n);
		}

	protected:
		GPL_Hierarchy *hierarchy;
		GPL_String delimiter;

		virtual GPL_List<GPL_Node *> FindNodes(GPL_Node *h,
												const GPL_String & n) const;
};

#endif /* GPL_HIER_H */
