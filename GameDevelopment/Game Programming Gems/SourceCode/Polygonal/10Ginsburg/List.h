/* Copyright (C) Dan Ginsburg, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg, 2000"
 */
#ifndef _LIST_H_
#define _LIST_H_

///
//	Macros
//
#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE  1
#endif

///
//	Types
//
typedef struct _LIST_NODE
{
	void			  *Data;
	int				   DataSize;
	struct _LIST_NODE *Next;
	struct _LIST_NODE *Prev;
	int				   Id;


} LIST_NODE;

///
//	Classes
//
class List
{
	protected:
		LIST_NODE *First,
				  *Last;
		LIST_NODE *Iter;
		int		   ListCount;

	public:

		List();
		~List();

		///
		//	Append()
		//
		int Append(void *Data, int Size);

		///
		//	GetCount()
		//
		int GetCount();

		///
		//	GetLast()
		//
		void* GetLast();

		///
		//	BuildTable()
		//
		void** BuildTable(int *Count);

        ///
		//	BuildLinearTable()
		//
		void* BuildLinearTable(int *Count);

		///
		//	PositionAt()
		//
		void PositionAt(int Id);

		///
		//	GetCurrent()
		//
		void* GetCurrent();

		///
		//	Advance()
		//
		void Advance();
};

#endif