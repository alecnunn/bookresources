
/******************************************************************************

Coral Tree wds/htest.h
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
class	WDS_HierarchyTest

Keywords:
******************************************************************************/
class WDS_HierarchyTest:public WDS_HierarchyNode
	{
	GPL_OBJECT_INTERFACE(WDS_HierarchyTest);

	private:

	public:

				WDS_HierarchyTest()
					{
					SetName("HierarchyTest");

					// set selectabilty at one selection per entire hierarchy
//					SetSelectability(WDS_SELECT_SINGLE|WDS_SELECT_EXCLUSIVE);
					};

		void	ShowEntry(void);
	};
