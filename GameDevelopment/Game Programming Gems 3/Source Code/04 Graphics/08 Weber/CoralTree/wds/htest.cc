
/******************************************************************************

Coral Tree wds/htest.cc
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



#define HIER_TEST_ENTRIES	8



/******************************************************************************
void	WDS_HierarchyTest::ShowEntry(void)

	build the child widgets

Keywords:
******************************************************************************/
void WDS_HierarchyTest::ShowEntry(void)
	{
	char names[HIER_TEST_ENTRIES][20]=
		{
		"Alpha",
		"Bravo",
		"Charlie",
		"Delta",
		"Echo",
		"Foxtrot",
		"Golf",
		"Hotel",
		};

	WDS_HierarchyTest *child;
	char message[256];

	long margin=parent_form.GetWidgetList()->GetNumberNodes()-1;

	parent_form.GetWidgetList()->ToHead();
	parent_form.GetWidgetList()->PostIncrement();
	parent_form.DeleteChildren(margin);

	long entry;
	for(entry=0;entry<HIER_TEST_ENTRIES;entry++)
		{
		child= new WDS_HierarchyTest;

		child->hier_string.NewBuffer(names[entry]);

		// set label & names
		sprintf(message,"Node %s",child->hier_string.Buffer());
		child->SetLabel(message);
		sprintf(message,"test hier-node %s",child->hier_string.Buffer());
		child->SetName(message);
		sprintf(message,"test hier-form %s",child->hier_string.Buffer());
		child->GetHierForm()->SetName(message);

		parent_form.AddWidget(child);
		}
	}
