/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/* Copyright (C) James Matthews, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Matthews, 2001"
 */


//////////////////////////////////////////////////////////////////
// Class:	CAStar class (27/6/2001)
// File:	AsIncludes.h
// Author:	James Matthews
// Modified by William van der Sterren
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


#include <cassert>


class _asNode {
	public:
		_asNode(int a = -1,int b = -1) 
      : x(a), 
        y(b), 
        number(0), 
        numchildren(0),
        aim(0) 
      {
			  parent = next = NULL; dataptr = NULL;
			  memset(children, 0, sizeof(children));
		  }

  // costs
    float GetF() const { return f; }
    float GetG() const { return g; }
    float GetH() const { return h; }

    void  SetG(float aGValue)
      { 
        g = aGValue; 
      }
    void  SetH(float anHValue)
      { 
        h = anHValue; 
      }
    void UpdateF()
      {
        f = g + h;
      }

  // coordinates
    int   GetX() const { return x; }
    int   GetY() const { return y; }

    void  SetXY(int anX, int anY) 
      { 
        x = anX; 
        y = anY; 
      }

  // coordinates
    float GetThreatAimQuality() const
      { return aim; }
    void  SetThreatAimQuality(float anAimQuality)
      {
        assert( anAimQuality >= 0 );
        aim = anAimQuality;
      }

  // unique number, derived from coordinates
    int   GetID() const { return number; }
    void  SetID(int anID) 
      { 
        assert( number == 0 );
        number = anID;
      }   

  // children
    unsigned int GetNumberOfChildren() const 
                   { return numchildren; }
    _asNode*     GetChild(unsigned int aChildIdx) const
                   {
                     assert( aChildIdx < numchildren );
                     return children[aChildIdx];
                   }
    void         AddChild(_asNode* aChild)
                   {
                     assert( numchildren < (sizeof(children)/sizeof(_asNode*)) );
                     children[numchildren++] = aChild;
                   }

    _asNode*     GetParent() const
                   { return parent; }
    void         SetParent(_asNode* aParent)
                   {
                     parent = aParent;
                   } 

  protected:
		float		f;        //!< fitness
    float   g;        //!< goal
    float   h;	      //!< heuristic

    int     x;        //!< grid coordinate
    int     y;        //!< grid coordinate

		int			number;	  //!< unique id, derived from x*m_iRows+y

    float   aim;      //!< threat aim quality

  protected:
		unsigned int numchildren; //!< number of children
		_asNode*     children[8];	//!< the children, assumes square tiles
		_asNode*     parent;      //!< parent

  public:
		_asNode	*next;			// For Open and Closed lists
		void		*dataptr;		// Associated data
};

// Stack for propagation.
struct _asStack {
	_asNode	 *data;
	_asStack *next;
};

typedef int   (*_asFunc)(_asNode *, _asNode *, int, void *);
