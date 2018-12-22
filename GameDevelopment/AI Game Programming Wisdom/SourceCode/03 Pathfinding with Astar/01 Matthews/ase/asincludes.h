//////////////////////////////////////////////////////////////////
// Class:	CAStar class (27/6/2001)
// File:	AsIncludes.h
// Author:	James Matthews
//
// Implements the A* algorithm.
// 
//
// Please visit http://www.generation5.org/ for the latest
// in Artificial Intelligence news, interviews, articles and
// discussion forums.
//

#ifndef NULL
#define NULL 0
#endif

#define ASNL_ADDOPEN		0
#define ASNL_STARTOPEN		1
#define ASNL_DELETEOPEN		2
#define ASNL_ADDCLOSED		3

#define ASNC_INITIALADD		0
#define ASNC_OPENADD_UP		1
#define ASNC_OPENADD		2
#define ASNC_CLOSEDADD_UP	3
#define ASNC_CLOSEDADD		4
#define ASNC_NEWADD			5

class _asNode {
	public:
		_asNode(int a = -1,int b = -1) : x(a), y(b), number(0), numchildren(0) {
			parent = next = NULL; dataptr = NULL;
			memset(children, 0, sizeof(children));
		}

		int			f,g,h;			// Fitness, goal, heuristic.
		int			x,y;			// Coordinate position
		int			numchildren;
		int			number;			// x*m_iRows+y
		_asNode		*parent;
		_asNode		*next;			// For Open and Closed lists
		_asNode		*children[8];	// Assumes square tiles
		void		*dataptr;		// Associated data
};

// Stack for propagation.
struct _asStack {
	_asNode	 *data;
	_asStack *next;
};

typedef int(*_asFunc)(_asNode *, _asNode *, int, void *);
