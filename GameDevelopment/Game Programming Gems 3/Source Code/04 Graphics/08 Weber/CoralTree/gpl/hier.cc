/******************************************************************************

Coral Tree gpl/hier.cc
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

#include <gpl/gpl.h>

/***********************************************************************
class GPL_Node
***********************************************************************/
GPL_List<GPL_Node *> GPL_Node::FindNodes(const GPL_String & n)
{
	GPL_List<GPL_Node *> nodes = GetNodes();
	register uint32 i = 0;
	while (nodes.GetCount() > i)
	{
		if (n != nodes[i]->GetName())
		{
			nodes.Remove(i);
		}
		else
		{
			++i;
		}
	}
	return nodes;
}

/***********************************************************************
class GPL_HierarchySearcher
***********************************************************************/
GPL_List<GPL_Node *> GPL_HierarchySearcher::FindNodes(GPL_Node *h,
												const GPL_String & n) const
{
	GPL_List<GPL_Node *> nodes;
	if ((GPL_Node *) GPL_BAD_ADDRESS != h)
	{
		GPL_String prefix;
		GPL_String postfix;
		n.Split(delimiter, prefix, postfix);

		if (0 == postfix.GetLength())
		{
			nodes = FindNodes(n);
		}
		else
		{
			const GPL_List<GPL_Node *> temp = FindNodes(prefix);
			for (register uint32 i = 0; temp.GetCount() > i; ++i)
			{
				nodes.Concatenate(FindNodes(temp[i], postfix));
			}
		}
	}
	return nodes;
}
