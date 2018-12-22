
/******************************************************************************

Coral Tree gpl/bag.cc
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

GPL_Tuple GPL_Bag::empty;

GPL_Bag::GPL_Bag()
{
}

GPL_Bag::~GPL_Bag()
{
	Clear();
}

void GPL_Bag::Clear()
{
	tupleList.ToHead();
	GPL_Tuple *tuple;
	while((tuple = tupleList.PostIncrement()))
	{
		tupleList.RemoveNode(tuple);
		delete tuple;
	}
}

GPL_Tuple &GPL_Bag::Match(char *format, ...)
{
	va_list ap;

	tupleList.ToHead();
	GPL_Tuple *tuple;
	while((tuple = tupleList.PostIncrement()))
	{
		va_start(ap, format);
		if(tuple->VCheckMatch(format, &ap))
		{
			va_end(ap);
			return *tuple;
		}
		va_end(ap);
	}

	return empty;
}

GPL_Tuple &GPL_Bag::Match(const GPL_Description &desc)
{
	tupleList.ToHead();
	GPL_Tuple *tuple;
	while((tuple = tupleList.PostIncrement()))
	{
		if(desc.Matches(*tuple))
		{
			return *tuple;
		}
	}

	return empty;
}


void GPL_Bag::Match(GPL_Bag &bag, char *format, ...)
{
	va_list ap;

	tupleList.ToHead();
	GPL_Tuple *tuple;
	while((tuple = tupleList.PostIncrement()))
	{
		va_start(ap, format);
		if(tuple->VCheckMatch(format, &ap))
		{
			va_end(ap);
			bag.Add(*tuple);
		}
		va_end(ap);
	}

	return;
}

void GPL_Bag::Match(GPL_Bag &bag, const GPL_Description &desc)
{
	tupleList.ToHead();
	GPL_Tuple *tuple;
	while((tuple = tupleList.PostIncrement()))
	{
		if(desc.Matches(*tuple))
		{
			bag.Add(*tuple);
		}
	}

	return;
}


void GPL_Bag::Add(const GPL_Tuple &tuple)
{
	GPL_Tuple *t;
	t = new GPL_Tuple();
	*t = tuple;
	tupleList.AppendNode(t);
}

void GPL_Bag::Add(GPL_Bag &bag)
{
	GPL_Tuple *tuple, *t;
	bag.tupleList.ToHead();
	while((tuple = bag.tupleList.PostIncrement()))
	{
		t = new GPL_Tuple();
		*t = *tuple;
		tupleList.AppendNode(t);
	}
	return;
}

long GPL_Bag::Remove(const GPL_Tuple &tuple)
{
	GPL_Tuple *t;
	tupleList.ToHead();
	while((t = tupleList.PostIncrement()))
	{
		if(tuple == *t)
		{
			tupleList.RemoveNode(t);
			delete t;
			return TRUE;
		}
	}
	return FALSE;
}

void GPL_Bag::Remove(GPL_Bag &bag)
{
	GPL_Tuple *tuple, *t;
	bag.tupleList.ToHead();
	while((tuple = bag.tupleList.PostIncrement()))
	{
		tupleList.ToHead();
		while((t = tupleList.PostIncrement()))
		{
			if(*tuple == *t)
			{
				tupleList.RemoveNode(t);
				delete t;
				break;
			}
		}
	}
	return;
}

void GPL_Bag::Get(GPL_List<GPL_Tuple> &llist)
{
	GPL_Tuple *tuple;
	tupleList.ToHead();
	while((tuple = tupleList.PostIncrement()))
	{
		llist.Append(*tuple);
	}
}

/******************************************************************************
DIRECT ACCESS FUNCTIONS
******************************************************************************/
GPL_Tuple *GPL_Bag::MatchDirect(char *format, ...)
{
	va_list ap;

	tupleList.ToHead();
	GPL_Tuple *tuple;
	while((tuple = tupleList.PostIncrement()))
	{
		va_start(ap, format);
		if(tuple->VCheckMatch(format, &ap))
		{
			va_end(ap);
			return tuple;
		}
		va_end(ap);
	}

	return NULL;
}

GPL_Tuple *GPL_Bag::MatchDirect(const GPL_Description &desc)
{
	tupleList.ToHead();
	GPL_Tuple *tuple;
	while((tuple = tupleList.PostIncrement()))
	{
		if(desc.Matches(*tuple))
		{
			return tuple;
		}
	}

	return NULL;
}

void GPL_Bag::Match(GPL_DoubleList<GPL_Tuple> &dlist, char *format, ...)
{
	va_list ap;

	tupleList.ToHead();
	GPL_Tuple *tuple;
	while((tuple = tupleList.PostIncrement()))
	{
		va_start(ap, format);
		if(tuple->VCheckMatch(format, &ap))
		{
			va_end(ap);
			dlist.AppendNode(tuple);
		}
		va_end(ap);
	}

	return;
}

void GPL_Bag::Match(GPL_DoubleList<GPL_Tuple> &dlist, const GPL_Description &desc)
{
	tupleList.ToHead();
	GPL_Tuple *tuple;
	while((tuple = tupleList.PostIncrement()))
	{
		if(desc.Matches(*tuple))
		{
			dlist.AppendNode(tuple);
		}
	}

	return;
}

void GPL_Bag::Remove(GPL_DoubleList<GPL_Tuple> &dlist)
{
	GPL_Tuple *tuple;
	dlist.ToHead();
	while((tuple = dlist.PostIncrement()))
	{
		tupleList.RemoveNode(tuple);
		dlist.RemoveNode(tuple);
		delete tuple;
	}
}

void GPL_Bag::Get(GPL_DoubleList<GPL_Tuple> &dlist)
{
	GPL_Tuple *tuple;
	tupleList.ToHead();
	while((tuple = tupleList.PostIncrement()))
	{
		dlist.AppendNode(tuple);
	}
}


