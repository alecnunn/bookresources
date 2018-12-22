/*********************************************************************
 * bounds.h
 *
 * Borrowed heavily (almost verbatim)
 * from code written by James F. O'Brien (job@cs.berkeley.edu)
 * This defines a generic heirarchical bounding tree structure, and
 * code to compute their overlaps.
 *
 * Copyright 2003, Regents of the University of California 
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/




#ifndef BOUND_TREE_H
#define BOUND_TREE_H

//-------------------------------------------------------------------
//-------------------------------------------------------------------

#include <stdlib.h>
#include <limits.h>
#include <iostream.h>

//-------------------------------------------------------------------
// Classes from this header:

class BoundBox;
class BoundTree;

class BoundOverlapServer;

//-------------------------------------------------------------------

#include "math/primitives.h"
#include "math/geometry.h"
#include "math/raycast.h"

//-------------------------------------------------------------------

#define DefaultLeafGroupSize ( 8    )
#define DefaultOverChunkSize ( 1000 )

//-------------------------------------------------------------------
//-------------------------------------------------------------------

inline int CheckInterfere(const BoundBox &a,const BoundBox &b); 
// returns true if 'a' overlaps the area of 'b', else false.

//-------------------------------------------------------------------
 

void BuildTree(const Vector3   *cList, unsigned int numItems,
		 /* */ BoundTree &bTree, unsigned int leafGroupSize = DefaultLeafGroupSize);
// Builds a boundtree which does a spatial sort of
// its leaves based on the vector locations in cList.
// Does not modify cList.
// Bounds are uninitalized, see SmUpdateTree.

//-------------------------------------------------------------------

//void UpdateTree(const VertexList    &vList,
//		  /* */ BoundTree     &bTree,double radius);
//void UpdateTree(const VertexList    &vList,
//		  const RigidTransform    &rbPos,
//		  const RigidTransform    &rbVel,
//		  /* */ BoundTree     &bTree,double radius,double deltaT = 0.0);

// Updates the bounds in a tree after the points in pList|vList have moved.
// Also inializes the bounds for a newly constructed tree.
// The bound boxes have extra "slop" space added to them
// based on deltaT and the velocity of a point.  Depending
// on the order of the calls with respect to the integration,
// a positive or negative deltaT should be used.

//-------------------------------------------------------------------

int RayHitBound(const BoundBox &bBox, const RayCast &ray);
// Returns non-zero if ray strikes bBox.  
// From Graphics Gems I by Andrew Woo.

inline int PlaneHitBound(const BoundBox &bBox, const Plane &plane);
// Returns non-zero if the plane passes thru the 
// bound box.

inline int HalfSpaceHitBound(const BoundBox &bBox, const Plane &plane);
// Returns non-zero if any part of the bound box is on the
// negative side of the plane.

inline int SphereHitBound(const BoundBox &bBox, Vector3 cent, double rad);
// Returns non-zero if any part of the bound box is in the sphere
// centered at cent with radius rad.




//-------------------------------------------------------------------
//-------------------------------------------------------------------

class BoundBox : public AABB {
  // An axis-aligned bounding box.

public:

  //----------------------------------------------------------
  // Constructors...
  
  inline BoundBox(                      );                     
  inline BoundBox(const BoundBox &that);                     
  inline BoundBox(const Vector3 &mins,const Vector3 &maxs);
    
  //----------------------------------------------------------
  // Overloaded operators
  
  inline BoundBox &operator= (const BoundBox &that)     ;
  inline BoundBox &operator+=(const BoundBox &that)     ;
  inline int         operator==(const BoundBox &that)const;
};


inline BoundBox boundUnion(const BoundBox &a, const BoundBox &b);
inline BoundBox operator+ (const BoundBox &a, const BoundBox &b);
// Union operator

inline BoundBox transform(const RigidTransform &xf, const BoundBox &bb);
inline BoundBox transform(const Matrix3  &rm, const BoundBox &bb);

inline void setBound(const RayCast &rayCast, BoundBox &bb);

istream &operator>>(istream &buf,      BoundBox &bb);
ostream &operator<<(ostream &buf,const BoundBox &bb);


//-------------------------------------------------------------------
//-------------------------------------------------------------------

class BoundTree {
  // Stores a bounding tree built from a list of
  // objects.  This class does not actually store
  // the triangles (or whatever) so it only has
  // meaning when considered with the object from
  // which it was build.
  // 
  // A tree is of depth "n" if it has n+1 levels.
  // There are n inner levels and 1 leaf level.
  // The inner levels are levels 0..(n-1) and
  // the leaf level is level n. The root level 
  // is inner level 0.
  //
  // Inner nodes hold a child level field, a
  // start child field, and a numChildren field.
  //
  // Leaf nodes holds and index which holds the
  // position index in a list of the things from
  // which the BoundTree was built.

public:

  //-------------------------------------------------------------------
  // internal classes.

  class innerNode {
  public:
    BoundBox     bound;
    unsigned short childLevel;
    unsigned short childCount;
    unsigned int   childStart;

    // Operators:
    inline innerNode           (                     ); 
    inline innerNode           (const innerNode &that); 
    inline innerNode &operator=(const innerNode &that); 
  };

  class leafNode {
  public:
    BoundBox     bound;
    unsigned int   mapIndex;

    // Operators:
    inline leafNode           (                    ); 
    inline leafNode           (const leafNode &that); 
    inline leafNode &operator=(const leafNode &that); 
  };

  //----------------------------------------------------------
  // Constructors & destructor...

  BoundTree(                       );
  BoundTree(const BoundTree &that);
 ~BoundTree(                       );
  
  //----------------------------------------------------------
  // Size querries
  
  inline unsigned short numInnerLevels()                   const; 

  inline unsigned int innerLevelSize(unsigned short level) const; 
  inline unsigned int  leafLevelSize(                    ) const; 

  //----------------------------------------------------------
  // Access operators
  
  inline const  leafNode &operator[](unsigned int   index) const; 
  inline const innerNode &operator()(unsigned short level, 
				     unsigned int   index) const; 

  //----------------------------------------------------------

  BoundTree &operator=(const BoundTree &that);

  //----------------------------------------------------------
  // Root bound access

  inline const BoundBox rootBound() const;
  // If there is no root node for some reason, then 
  // an empty bound ((0,0,0),(0,0,0)) will be returned.


  //----------------------------------------------------------
  // Level Access

  inline const  leafNode * leafLevel(                    ) const; 
  inline const innerNode *innerLevel(unsigned short level) const; 

  //----------------------------------------------------------
  // Level update

  inline void updateTree ();              
  inline void updateAbove(unsigned short level); 
  void        updateLevel(unsigned short level);  
  // The update tree will update all inner nodes.
  // updateAbove will update all innerLevels starting
  // at level 'level' to level 0.
  // updateLevel will only update the innerlevel 'level'.

  inline leafNode *beginLeafUpdate()                 ; 
  inline void        endLeafUpdate(){ updateTree(); }; 


//protected:

  //----------------------------------------------------------
  // Size operations
  // Using these calls invalidate the tree. They
  // are for use durring construction or the tree.

  void setNumInnerLevels(unsigned short numLevels);
  
  void setInnerLevelSize(unsigned short level, unsigned int levelSize);
  void  setLeafLevelSize(                      unsigned int levelSize);
  
  inline leafNode  *constructLeafLevel (                    );
  inline innerNode *constructInnerLevel(unsigned short level);
  
protected:

  //----------------------------------------------------------
  // Data members

  unsigned short  _numInnerLevels;
  unsigned int   *_innerLevelSizes;
  unsigned int    _leafLevelSize;

  innerNode **_innerLevels;
  leafNode   *_leafLevel  ;

};

//-------------------------------------------------------------------

istream &operator>>(istream &buf,      BoundTree::innerNode &n);
ostream &operator<<(ostream &buf,const BoundTree::innerNode &n);

istream &operator>>(istream &buf,      BoundTree::leafNode &n);
ostream &operator<<(ostream &buf,const BoundTree::leafNode &n);

istream &operator>>(istream &buf,      BoundTree       &t);
ostream &operator<<(ostream &buf,const BoundTree       &t);



//-------------------------------------------------------------------
//-------------------------------------------------------------------

typedef int (* LeafOverlapCB)(const BoundOverlapServer  &oe,
				const BoundTree::leafNode &indexOne, void *uprmOne,
				const BoundTree::leafNode &indexTwo, void *uprmTwo);


typedef int (* SelfOverlapCB)(const BoundOverlapServer  &oe,
				const BoundTree::leafNode &indexOne, 
				const BoundTree::leafNode &indexTwo, void *uprm);

typedef int (* RayOverlapCB)(const BoundOverlapServer  &oe,
				const BoundTree::leafNode &index, void *uprm,
				/* */BoundBox &rayBound,  RayCast &rayCast);

typedef int (* PlaneOverlapCB)(const BoundOverlapServer   &oe,
				const BoundTree::leafNode  &index, void *uprm, const Plane &plane);

typedef int (* SphereOverlapCB)(const BoundOverlapServer   &oe,
				 const BoundTree::leafNode  &index, void *uprm, Vector3 cent, double rad);


class BoundOverlapServer {
  // This class provides a set of methods for preforming travrsals
  // of bound tree pairs searching for overlaping bound boxes.
  // Note that while both trees may be the SAME tree (ie to find self
  // overlap) this is not an efficent way to do things.
  // This object uses an internal structure to track
  // its recursion.  For effency reasons, the structures
  // are alowcated in blocks so that the server can avoid the
  // overhead of memory allocation durring traversal.  If 
  // many traversals will be done (ie once a time step) it
  // it more effecient to reuse the server rather than constantly
  // creating and disposing of them.
  
public:

  //----------------------------------------------------------
  // Constructors...
  
  BoundOverlapServer();
 ~BoundOverlapServer();
 
  //----------------------------------------------------------
  // Traversal related calls

  int isReady() const;
  // return true if the overlapServer is in a state valid to
  // start a traversal.

  int isInUse() const;
  // returns true if the overlapServer is in the midst
  // of doing a traversal.


  //----------------------------------------------------------
  // Actuall traversal calls

  int doTraversal(LeafOverlapCB cb, 
		  const BoundTree &treeOne, void *uprmOne, 
		  const BoundTree &treeTwo, void *uprmTwo);
  // Does a traversal looking for overlaping bound boxes at the leaf
  // level.  Each time overlapping leaves are found, cb will be called.  
  // If cb returns  true, the traversal will continue.  The return
  // from this call will be the number of overlaping bounds  found
  // at the leaf levels. If cb is NULL then the traversal will be
  // stoped at the first overlaping leaf pair.

  //----------------------------------------------------------

  int doSelfTraversal(SelfOverlapCB cb, 
		      const BoundTree &tree, void *uprmOne); 
  // Does a self traversal looking for overlaping bound boxes at the
  // leaf level.  Each time overlapping leaves are found, cb will be
  // called.  If cb returns true, the traversal will continue.  The
  // return from this call will be the number of overlaping bounds
  // found at the leaf levels. If cb is NULL then the traversal will
  // be stoped at the first overlaping leaf pair.

  //----------------------------------------------------------

  int doRayTraversal(RayOverlapCB cb, 
		     const BoundTree &tree, void *uprm, RayCast &rayCast);
  // Does a ray traversal looking for boundBoxes which 
  // are struck by the ray.  When leaves are found to be hit
  // by ray, cb will be called. If cb returns true, the traversal will
  // continue. The return from this call will be number of  overlaping 
  // bounds found at the leaf levels. If cb is NULL then the 
  // traversal will be stoped at the first overlaping leaf bound.
  // If the ray of rayCast is altered by cb, then it cb should
  // also adjust the boundBox.  In general, the box should shrink,
  // not grow, or else the results will be unpredictable. 
  // Further, the actuall RAY (dir & org) should NOT change,
  // beacuse info is cached in the traversal.

  //----------------------------------------------------------

  int doPlaneTraversal(PlaneOverlapCB cb, 
		       const BoundTree &tree, void *uprm, const Plane &plane);
  // Does a plane traversal looking for boundBoxes which are "under"
  // the plane (ie wrong side of normal).  When leaves are found cb
  // will be called. If cb returns true, the traversal will
  // continue. The return from this call will be number of overlaping
  // bounds found at the leaf levels.  If cb is NULL then the
  // traversal will be stoped at the first overlaping leaf bound.

  //----------------------------------------------------------

  int doSphereTraversal(SphereOverlapCB cb, 
			const BoundTree &tree, void *uprm, Vector3 cent, double rad);
			
  // Does a sphere traversal looking for boundBoxes which are inside
  // the sphere (partly inside).  When leaves are found cb will be
  // called. If cb returns true, the traversal will continue. The
  // return from this call will be number of overlaping bounds found
  // at the leaf levels.  If cb is NULL then the traversal will be
  // stoped at the first overlaping leaf bound.


  //----------------------------------------------------------
  //  Traversal cache 

  int cacheTraversal      (const BoundTree &treeOne,
			   const BoundTree &treeTwo);
  int cacheSelfTraversal  (const BoundTree &tree);
  int cacheRayTraversal   (const BoundTree &tree, RayCast &rayCast);		     
  int cachePlaneTraversal (const BoundTree &tree, const Plane &plane);
  int cacheSphereTraversal(const BoundTree &tree, Vector3 cent, double rad);
			
  // These calls are like the doTraversal version except they bult an
  // internal list of the overlaping bounds.  Once a traversal has
  // been done, the appropriate call to traverseCache can be used many
  // times.  The return value and other behavior is the same as the
  // equivalent doTraversal call.

  int traverseCache(LeafOverlapCB cb, 
		    const BoundTree &treeOne, void *uprmOne, 
		    const BoundTree &treeTwo, void *uprmTwo);
  int traverseSelfCache(SelfOverlapCB cb, 
			const BoundTree &tree, void *uprm);
  int traverseRayCache(RayOverlapCB cb, 
		       const BoundTree &tree, void *uprm, RayCast &rayCast);
  int traversePlaneCache(PlaneOverlapCB cb, 
			 const BoundTree &tree, void *uprm, const Plane &plane);
  int traverseSphereCache(SphereOverlapCB cb, 
			  const BoundTree &tree, void *uprm, Vector3 cent, double rad);

  // Like an equivalent call to doTraversal, but on the leaf results
  // of a previous call to cacheTraversal.  The return value of the
  // callback is interpreted slightly differently.  As with the
  // doTraversal method, a positive value means continue and a zero
  // value will stop.  Additionaly, a negative value can be returned
  // to indicate that the traversaal should continue, but that the
  // current pair could be removed from the cache.

  void doneWithCache();
  // Clears out the caches traversal and prepares the server for a
  // call to cacheTraversal or doTraversal.


  //----------------------------------------------------------

protected:

  class overlap {
  public:
    unsigned short level1,level2;
    unsigned int    node1, node2;
    BoundOverlapServer::overlap *next;
  };

  class overlapChunk {
  public:
    inline  overlapChunk(unsigned int size = DefaultOverChunkSize);
    inline ~overlapChunk(                                          );
    BoundOverlapServer::overlapChunk *next ;
    BoundOverlapServer::overlap      *chunk;
    unsigned int                   size ;
  };
    
private:

  BoundOverlapServer::overlapChunk * _chunkList;
  BoundOverlapServer::overlap      * _freeListHead;
  BoundOverlapServer::overlap      * _pendListHead;
  BoundOverlapServer::overlap      * _cacheListHead;
  
  int  _isInUse;

  enum CacheUse { CNotReady , CReady , CReadyPlane , CReadyRay , CReadySphere } _cacheUse;

protected:
  void addChunk();
  void blowChunks();  // hehe, aka freeChunks

  void dumpPending();

  inline int pushPend(int allowGrowth = 1);
  inline int  popPend(                   );
  
  inline int addToCache(int allowGrowth = 1);
  inline void removeFromCache(overlap **node);

  void clearCache();
};

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------

// Inline member functions

inline
int CheckInterfere(const BoundBox &a,const BoundBox &b){
  return !(((a.bmin[0] > b.bmax[0]) || (b.bmin[0] > a.bmax[0])) ||
	   ((a.bmin[1] > b.bmax[1]) || (b.bmin[1] > a.bmax[1])) ||
	   ((a.bmin[2] > b.bmax[2]) || (b.bmin[2] > a.bmax[2])));
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

inline 
BoundBox::BoundBox()
{
  AABB::minimize();
}

inline 
BoundBox::BoundBox(const BoundBox &that) :
  AABB(that.bmin, that.bmax)
{}

inline 
BoundBox::BoundBox(const Vector3 &mins,const Vector3 &maxs) :
  AABB(mins,maxs)
{}

inline 
BoundBox &BoundBox::operator=(const BoundBox &that) {
  AABB::operator = (that);
  return (*this);
}

inline
BoundBox &BoundBox::operator+=(const BoundBox &that) {
  AABB::expand(that);
  return (*this);
}


inline
int BoundBox::operator==(const BoundBox &that) const {
  return (bmin == that.bmin) && (bmax == that.bmax);
}
    
//-------------------------------------------------------------------


inline
BoundBox boundUnion(const BoundBox &a, const BoundBox &b){
  BoundBox c;
  c.set_expand(a,b);
  return c;
}

inline
BoundBox operator+(const BoundBox &a, const BoundBox &b) {
  return boundUnion(a,b);
}


inline 
BoundBox transform(const RigidTransform &xf, const BoundBox &bb) {
  register BoundBox ret(transform(xf.R,bb));
  ret.bmin+=xf.t;
  ret.bmax+=xf.t;
  return ret;
}

inline 
BoundBox transform(const Matrix3  &rm, const BoundBox &bb) {
  register Vector3 l;   // low|min
  register Vector3 h;   //  hi|max

  register double m;      // matrix value
  
  if((m=rm(0,0))>0){l[0] =m*bb.bmin[0];h[0] =m*bb.bmax[0];}else{l[0] =m*bb.bmax[0];h[0] =m*bb.bmin[0];}
  if((m=rm(0,1))>0){l[1] =m*bb.bmin[1];h[1] =m*bb.bmax[1];}else{l[1] =m*bb.bmax[1];h[1] =m*bb.bmin[1];}
  if((m=rm(0,2))>0){l[2] =m*bb.bmin[2];h[2] =m*bb.bmax[2];}else{l[2] =m*bb.bmax[2];h[2] =m*bb.bmin[2];}

  if((m=rm(1,0))>0){l[0]+=m*bb.bmin[0];h[0]+=m*bb.bmax[0];}else{l[0]+=m*bb.bmax[0];h[0]+=m*bb.bmin[0];}
  if((m=rm(1,1))>0){l[1]+=m*bb.bmin[1];h[1]+=m*bb.bmax[1];}else{l[1]+=m*bb.bmax[1];h[1]+=m*bb.bmin[1];}
  if((m=rm(1,2))>0){l[2]+=m*bb.bmin[2];h[2]+=m*bb.bmax[2];}else{l[2]+=m*bb.bmax[2];h[2]+=m*bb.bmin[2];}

  if((m=rm(2,0))>0){l[0]+=m*bb.bmin[0];h[0]+=m*bb.bmax[0];}else{l[0]+=m*bb.bmax[0];h[0]+=m*bb.bmin[0];}
  if((m=rm(2,1))>0){l[1]+=m*bb.bmin[1];h[1]+=m*bb.bmax[1];}else{l[1]+=m*bb.bmax[1];h[1]+=m*bb.bmin[1];}
  if((m=rm(2,2))>0){l[2]+=m*bb.bmin[2];h[2]+=m*bb.bmax[2];}else{l[2]+=m*bb.bmax[2];h[2]+=m*bb.bmin[2];}

  return BoundBox(l,h);
}


inline 
void setBound(const RayCast &rayCast, BoundBox &bb) {
  Vector3 rayMaxPos(rayCast.maxPos());
  Vector3 rayMinPos(rayCast.minPos());

  if (rayMaxPos[0] > rayMinPos[0]) {
    bb.bmin[0] = rayMinPos[0];
    bb.bmax[0] = rayMaxPos[0];
  }else{
    bb.bmin[0] = rayMaxPos[0];
    bb.bmax[0] = rayMinPos[0];
  }

  if (rayMaxPos[1] > rayMinPos[1]) {
    bb.bmin[1] = rayMinPos[1];
    bb.bmax[1] = rayMaxPos[1];
  }else{
    bb.bmin[1] = rayMaxPos[1];
    bb.bmax[1] = rayMinPos[1];
  }

  if (rayMaxPos[2] > rayMinPos[2]) {
    bb.bmin[2] = rayMinPos[2];
    bb.bmax[2] = rayMaxPos[2];
  }else{
    bb.bmin[2] = rayMaxPos[2];
    bb.bmax[2] = rayMinPos[2];
  }
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

inline
BoundTree::innerNode::innerNode() :
  childCount(0) {
}


inline
BoundTree::innerNode::innerNode(const BoundTree::innerNode &that) :
  bound(that.bound),
  childLevel(that.childLevel),
  childStart(that.childStart),
  childCount(that.childCount) {
}
 

inline
BoundTree::innerNode &
BoundTree::innerNode::operator=(const BoundTree::innerNode &that) {
  bound = that.bound;
  childLevel = that.childLevel;
  childCount = that.childCount;
  childStart = that.childStart;
  return (*this);
}


inline
BoundTree::leafNode::leafNode() {
}


inline   
BoundTree::leafNode::leafNode(const BoundTree::leafNode &that) :
  bound(that.bound),
  mapIndex(that.mapIndex) {
}


inline   
BoundTree::leafNode &
BoundTree::leafNode::operator=(const BoundTree::leafNode &that) {
  bound = that.bound;
  mapIndex = that.mapIndex;
  return (*this);
}


inline 
unsigned short BoundTree::numInnerLevels() const {
  return _numInnerLevels;
}
  

inline 
unsigned int BoundTree::innerLevelSize(unsigned short level) const {
#ifdef DEBUG
  if (level>=_numInnerLevels) {
    cerr << "BoundTree::innerLevelSize range error.\n" << flush;
    abort();
  }
#endif
  return _innerLevelSizes[level];
}
  

inline
unsigned int BoundTree::leafLevelSize() const {
  return _leafLevelSize;
}

inline 
const BoundTree::leafNode &BoundTree::operator[](unsigned int index) const {
#ifdef DEBUG
  if (index>=_leafLevelSize) {
    cerr << "BoundTree::operator[] range error.\n" << flush;
    abort();
  }
#endif
  return _leafLevel[index];
}


inline
const BoundTree::innerNode &BoundTree::operator()(unsigned short level, 
						      unsigned int   index) const {
#ifdef DEBUG
  if ((level>=_numInnerLevels) || (index>=_innerLevelSizes[level])) {
    cerr << "BoundTree::operator() range error.\n" << flush;
    abort();
  }
#endif
  return _innerLevels[level][index];
}

inline 
const BoundBox BoundTree::rootBound() const {
  if (numInnerLevels() == 0) {
    if (leafLevelSize() == 1) {
      return (*this)[0].bound;
    }
  }else{
    if (innerLevelSize(0) == 1) {
      return (*this)(0,0).bound;
    }
  }
#ifdef DEBUG
  cerr << "BoundTree::rootBound() root bound is not defined.\n" << flush;
#endif
  return BoundBox(Vector3(0,0,0),Vector3(0,0,0));
}


inline
const BoundTree::leafNode *BoundTree::leafLevel() const {
  return _leafLevel;
}

inline       
const BoundTree::innerNode *BoundTree::innerLevel(unsigned short level) const {
#ifdef DEBUG
  if (level>=_numInnerLevels) {
    cerr << "BoundTree::innerLevel range error.\n" << flush;
    abort();
  }
#endif
  return _innerLevels[level];
}


inline
void BoundTree::updateTree() {
  updateAbove(_numInnerLevels-1);
}

inline    
void BoundTree::updateAbove(unsigned short level) {
#ifdef DEBUG
  if (level>=_numInnerLevels) {
    cerr << "BoundTree::updateAbove range error.\n" << flush;
    abort();
  }
#endif
  register unsigned short i;
  for(i=level;;i--) {
    updateLevel(i);
    if (i==0) break;
  }
}


inline
BoundTree::leafNode *BoundTree::beginLeafUpdate() {
  return _leafLevel;
}

inline
BoundTree::leafNode *BoundTree::constructLeafLevel() {
  return _leafLevel;
}

inline
BoundTree::innerNode *BoundTree::constructInnerLevel(unsigned short level) {
#ifdef DEBUG
  if (level>=_numInnerLevels) {
    cerr << "BoundTree::constructInnerLevel range error.\n" << flush;
    abort();
  }
#endif
  return _innerLevels[level];
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

inline BoundOverlapServer::overlapChunk::overlapChunk(unsigned int size) {
  this->next  = NULL;
  this->size  = size;
  chunk = new BoundOverlapServer::overlap[size];

  register int i;

  for(i=1;i<size;i++) {
    chunk[i-1].next = &(chunk[i]);
  }
  chunk[size-1].next = NULL;
}

inline BoundOverlapServer::overlapChunk::~overlapChunk() {
  delete [] chunk;
}


int BoundOverlapServer::pushPend(int allowGrowth) {
  register int ret;

  if (( _freeListHead == NULL ) && (allowGrowth)) {
    addChunk();
  }

  if ( _freeListHead == NULL ) {
    ret = 0;
  }else{
    BoundOverlapServer::overlap *hold = _freeListHead;
    _freeListHead = _freeListHead->next;

    hold->next = _pendListHead;
    _pendListHead = hold;

    ret = 1;
  }

  return ret;
}
  
int BoundOverlapServer::popPend() {
  register int ret;
  if ( _pendListHead == NULL ) {
    ret = 0;
  }else{
    BoundOverlapServer::overlap *hold = _pendListHead;
    _pendListHead = _pendListHead->next;

    hold->next = _freeListHead;
    _freeListHead = hold;

    ret = 1;
  }
  return ret;
}

int BoundOverlapServer::addToCache(int allowGrowth) {
  register int ret;

  if (( _freeListHead == NULL ) && (allowGrowth)) {
    addChunk();
  }

  if ( _freeListHead == NULL ) {
    ret = 0;
  }else{
    BoundOverlapServer::overlap *hold = _freeListHead;
    _freeListHead = _freeListHead->next;

    hold->next = _cacheListHead;
    _cacheListHead = hold;

    ret = 1;
  }
  return ret;
}

void BoundOverlapServer::removeFromCache(overlap **node) {
  
  if ((node == NULL) && ((*node) != NULL)) {
    BoundOverlapServer::overlap * hold = *node;
    (*node) = hold->next;
    hold->next = _freeListHead;
    _freeListHead = hold;
  }
}
 
//----------------------------------------------------------
//----------------------------------------------------------

inline int PlaneHitBound(const BoundBox &bBox, const Plane &plane) {
  Vector3 min = bBox.bmin * plane.normal;
  Vector3 max = bBox.bmax * plane.normal;
  double mmms = min[0] + min[1] + min[2] + plane.offset;
  if (mmms>0) {
    if ((min[0] + min[1] + max[2] + plane.offset) <= 0) return 1;
    if ((min[0] + max[1] + min[2] + plane.offset) <= 0) return 1;
    if ((min[0] + max[1] + max[2] + plane.offset) <= 0) return 1;
    if ((max[0] + min[1] + min[2] + plane.offset) <= 0) return 1;
    if ((max[0] + min[1] + max[2] + plane.offset) <= 0) return 1;
    if ((max[0] + max[1] + min[2] + plane.offset) <= 0) return 1;
    if ((max[0] + max[1] + max[2] + plane.offset) <= 0) return 1;
  }else{
    if (mmms<0) {
      if ((min[0] + min[1] + max[2] + plane.offset) >= 0) return 1;
      if ((min[0] + max[1] + min[2] + plane.offset) >= 0) return 1;
      if ((min[0] + max[1] + max[2] + plane.offset) >= 0) return 1;
      if ((max[0] + min[1] + min[2] + plane.offset) >= 0) return 1;
      if ((max[0] + min[1] + max[2] + plane.offset) >= 0) return 1;
      if ((max[0] + max[1] + min[2] + plane.offset) >= 0) return 1;
      if ((max[0] + max[1] + max[2] + plane.offset) >= 0) return 1;
    }else{
      return 1;
    }
  }
  return 0;
}

inline int HalfSpaceHitBound(const BoundBox &bBox, const Plane &plane) {
  Vector3 min = bBox.bmin * plane.normal;
  if ((min[0] + min[1] + min[2] + plane.offset) <= 0) return 1;
  Vector3 max = bBox.bmax * plane.normal;
  if ((min[0] + min[1] + max[2] + plane.offset) <= 0) return 1;
  if ((min[0] + max[1] + min[2] + plane.offset) <= 0) return 1;
  if ((min[0] + max[1] + max[2] + plane.offset) <= 0) return 1;
  if ((max[0] + min[1] + min[2] + plane.offset) <= 0) return 1;
  if ((max[0] + min[1] + max[2] + plane.offset) <= 0) return 1;
  if ((max[0] + max[1] + min[2] + plane.offset) <= 0) return 1;
  if ((max[0] + max[1] + max[2] + plane.offset) <= 0) return 1;
  return 0;
}


inline int SphereHitBound(const BoundBox &bBox, Vector3 cent, double rad) {

  if ((cent[0] - bBox.bmax[0]) > rad) return 0;
  if ((cent[1] - bBox.bmax[1]) > rad) return 0;
  if ((cent[2] - bBox.bmax[2]) > rad) return 0;

  if ((bBox.bmin[0] - cent[0]) > rad) return 0;
  if ((bBox.bmin[1] - cent[1]) > rad) return 0;
  if ((bBox.bmin[2] - cent[2]) > rad) return 0;

  return 1;
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#endif
