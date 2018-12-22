
/******************************************************************************

Coral Tree wds/tie.h
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
class	WDS_TieValue

Keywords:
******************************************************************************/
class WDS_TieValue: public GPL_DoubleNode
	{
	private:

		float value;

	public:

			WDS_TieValue(void)
				{
				value=0.0;
				};

	void	SetValue(float set)	{ value=set; };
	float	GetValue(void)		{ return value; };
	};



/******************************************************************************
class	WDS_TieValues

Keywords:
******************************************************************************/
class WDS_TieValues: public GPL_IDoubleList<WDS_TieValue>
	{
	public:
				WDS_TieValues(void)		{};
virtual			~WDS_TieValues(void)	{};
	};



/******************************************************************************
class	WDS_TieList

Keywords:
******************************************************************************/
class WDS_TieList
	{
	private:

		GPL_DoubleList<WDS_Widget>   tielist;

	public:

virtual			~WDS_TieList(void)				{ ClearTies(); };

        void    AddTie(WDS_Widget *wwidget)		{ tielist.AppendNode(wwidget); };

        void    RemoveTie(WDS_Widget *wwidget)	{ tielist.RemoveNode(wwidget); };

        void    ClearTies(void)
                    {
                    WDS_Widget *tie,*last_tie=NULL;

                    tielist.ToTail();
                    while( (tie=(WDS_Widget *)tielist.Current()) != NULL )
                        {
                        // inform each tied widget

						if(tie==last_tie)
							{
							EW_PRINT(EW_WIDGET,EW_ERROR,"WDS_TieList::ClearTies() couldn't release tie to widget");
							tielist.PostDecrement();
							}
						else
							{
							tie->SetTieList(NULL);
							last_tie=tie;
							}
                        };
                    };

        void    ChangeTieValues(WDS_TieValues *tievalues)
                    {
                    WDS_Widget *tie;

                    tielist.ToHead();
                    while( (tie=(WDS_Widget *)tielist.PostIncrement()) != NULL )
                        {
                        // inform each tied widget

						tie->TieValuesChanged(tievalues);
                        };
                    };
	};
