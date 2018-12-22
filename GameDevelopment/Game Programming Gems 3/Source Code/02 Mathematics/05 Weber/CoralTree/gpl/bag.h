
/******************************************************************************

Coral Tree gpl/bag.h
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


class GPL_Bag
{
	private:
		static GPL_Tuple			empty;
	protected:
		GPL_DoubleList<GPL_Tuple>	tupleList;
	public:
		GPL_Bag();
		~GPL_Bag();

		/* MAIN FUNCTIONS */
		GPL_Tuple 	&Match(char *format, ...);
		GPL_Tuple 	&Match(const GPL_Description &desc);
		void 		Match(GPL_Bag &bag, char *format, ...);
		void 		Match(GPL_Bag &bag, const GPL_Description &desc);

		void 		Add(const GPL_Tuple &tuple);
		void 		Add(GPL_Bag &bag);

		long 		Remove(const GPL_Tuple &tuple);
		void 		Remove(GPL_Bag &bag);

		void		Get(GPL_List<GPL_Tuple> &llist);

		/* DIRECT ACCESS FUNCTIONS */
		GPL_Tuple	*MatchDirect(char *format, ...);
		GPL_Tuple 	*MatchDirect(const GPL_Description &desc);
		void 		Match(GPL_DoubleList<GPL_Tuple> &dlist, char *format, ...);
		void 		Match(GPL_DoubleList<GPL_Tuple> &dlist, const GPL_Description &desc);
		void 		Remove(GPL_DoubleList<GPL_Tuple> &dlist);
		void		Get(GPL_DoubleList<GPL_Tuple> &dlist);

		void		Clear();

};

