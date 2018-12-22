/*********************************************************************
 * bounds.cpp
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




extern "C" {
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <limits.h>
}

#include "bounds.h"

//-------------------------------------------------------------------

inline static
istream &eatChar(char c,istream &buf) {
  char r;
  buf >> r;
  if (strncasecmp(&c,&r,1)) {
    buf.clear(buf.rdstate() | ios::failbit);
  }
  return buf;
}


inline static
istream &eatStr(const char *s,istream &buf) {
  while (*s != '\0') {
    eatChar(*s,buf);
    s++;
  }
  return buf;
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//
//class BoundBox


istream &operator>>(istream &buf, BoundBox &bb) {
  long orgFlags = buf.setf(ios::skipws);
  eatChar('{',buf);
  buf >> bb.bmin;
  eatChar(',',buf);
  buf >> bb.bmax;
  eatChar('}',buf);
  buf.flags(orgFlags);
  return buf;
}
  
ostream &operator<<(ostream &buf, const BoundBox &bb) {
  buf << '{';
  buf << bb.bmin;
  buf << ',';
  buf << bb.bmax;
  buf << '}';
  return buf;
}
 

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//
// BoundTree


BoundTree::BoundTree() {
  _numInnerLevels  = 0;
  _innerLevelSizes = NULL;
  _leafLevelSize   = 0;
  _innerLevels = NULL;
  _leafLevel   = NULL;
}


BoundTree::BoundTree(const BoundTree &that){
  _numInnerLevels  = 0;
  _innerLevelSizes = NULL;
  _leafLevelSize   = 0;
  _innerLevels = NULL;
  _leafLevel   = NULL;

  (*this) = that;
}

BoundTree::~BoundTree(){
  register unsigned short i;

  for(i=0;i<_numInnerLevels;i++) 
    delete [] _innerLevels[i];

  delete [] _innerLevelSizes;
  delete [] _innerLevels;
  delete [] _leafLevel;
}
  


BoundTree &BoundTree::operator=(const BoundTree &that) {
  setNumInnerLevels(that._numInnerLevels);
  setLeafLevelSize (that._leafLevelSize );
  
  register unsigned int   j;
  register unsigned short i;
  for(i=0;i<_numInnerLevels;i++) {
    setInnerLevelSize(i,that._innerLevelSizes[i]);
    for(j=0;j<_innerLevelSizes[i];j++) {
      _innerLevels[i][j] = that._innerLevels[i][j];
    }
  }

  for(j=0;j<_leafLevelSize;j++) {
    _leafLevel[j] = that._leafLevel[j];
  }

  return (*this);
}


void BoundTree::updateLevel(unsigned short level) {
#ifdef DEBUG
  if (level>=_numInnerLevels) {
    cerr << "BoundTree::updateLevel range error.\n" << flush;
    abort();
  }
#endif

  register unsigned int p,pl=_innerLevelSizes[level];

  for(p=0;p<pl;p++) {

    register innerNode &parent = _innerLevels[level][p];

    if (parent.childCount == 0) {
      parent.bound.bmin =  (DBL_MAX);
      parent.bound.bmax = -(DBL_MAX);
    }else{
      register unsigned short cLevel = parent.childLevel;
      register unsigned int   cStart = parent.childStart;
      register unsigned int   cStop  = parent.childCount + cStart;
      register unsigned int   cMark  ;

      if (cLevel==_numInnerLevels) {
	// leaf level
#ifdef DEBUG
	if ((cStart >= _leafLevelSize) ||
	    (cStop  >  _leafLevelSize) ){
	  cerr << "BoundTree::updateLevel internal range error.\n" <<
	    flush;
	  abort();
	}
#endif
	parent.bound = _leafLevel[cStart].bound;
	for(cMark=cStart+1;cMark<cStop;cMark++) {
	  parent.bound += _leafLevel[cMark].bound;
	}
      }else{
	// inner level
#ifdef DEBUG
	if ((cLevel >= _numInnerLevels) ||
	    (cStart >= _innerLevelSizes[cLevel]) ||
	    (cStop  >  _innerLevelSizes[cLevel]) ){
	  cerr << "BoundTree::updateLevel internal range error.\n" << flush;
	  abort();
	}
#endif
	parent.bound = _innerLevels[cLevel][cStart].bound;
	for(cMark=cStart+1;cMark<cStop;cMark++) {
	  parent.bound += _innerLevels[cLevel][cMark].bound;
	}
      }
    }
  }
}
	  

void BoundTree::setNumInnerLevels(unsigned short numLevels) {
  register unsigned int i;
  for(i=0;i<_numInnerLevels;i++) {
    delete [] _innerLevels[i];
  }
  delete [] _innerLevels;
  delete [] _innerLevelSizes;

  _innerLevels     = new BoundTree::innerNode*[numLevels];
  _innerLevelSizes = new unsigned int[numLevels];
  _numInnerLevels  = numLevels;

  for(i=0;i<_numInnerLevels;i++) {
    _innerLevels    [i] = NULL;
    _innerLevelSizes[i] = 0   ;
  }
}

  
void BoundTree::setInnerLevelSize(unsigned short level    , 
				    unsigned int   levelSize) {
  if (level >= _numInnerLevels) {
    cerr << "BoundTree::setInnerLevelSize range error.\n" << flush;
    abort();
  }
  delete [] _innerLevels[level];
  
   _innerLevels    [level] = new BoundTree::innerNode[levelSize];
   _innerLevelSizes[level] = levelSize;
}
  

void BoundTree::setLeafLevelSize(unsigned int levelSize) {
  delete [] _leafLevel;
  _leafLevel     = new BoundTree::leafNode [levelSize];
  _leafLevelSize = levelSize;
}
				   

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------


istream &operator>>(istream &buf, BoundTree::innerNode &n) {
  long orgFlags = buf.setf(ios::skipws);
  eatChar('{',buf);
  buf >> n.bound;
  eatChar(',',buf);
  buf >> n.childLevel;
  eatChar(',',buf);
  buf >> n.childStart;
  eatChar(',',buf);
  buf >> n.childCount;
  eatChar('}',buf);
  buf.flags(orgFlags);
  return buf;
}  


ostream &operator<<(ostream &buf,const BoundTree::innerNode &n) {
  buf << '{';
  buf << n.bound;
  buf << ',';
  buf << n.childLevel;
  buf << ',';
  buf << n.childStart;
  buf << ',';
  buf << n.childCount;
  buf << '}';
  return buf;
}  


//----------------------------------------------------------


istream &operator>>(istream &buf, BoundTree::leafNode &n) {
  long orgFlags = buf.setf(ios::skipws);
  eatChar('{',buf);
  buf >> n.bound;
  eatChar(',',buf);
  buf >> n.mapIndex;
  eatChar('}',buf);
  buf.flags(orgFlags);
  return buf;
}  

ostream &operator<<(ostream &buf,const BoundTree::leafNode &n) {
  buf << '{';
  buf << n.bound;
  buf << ',';
  buf << n.mapIndex;
  buf << '}';
  return buf;
}


//----------------------------------------------------------


istream &operator>>(istream &buf, BoundTree &t) {
  long orgFlags = buf.setf(ios::skipws);

  unsigned short newNIL = 0,i;
  unsigned int   newLLS = 0,j;
  unsigned int   newILS = 0  ;

  eatStr("{BoundTree:",buf);

  buf >> newNIL;
  t.setNumInnerLevels(newNIL);

  eatChar('[',buf);
  for(i=0;i<newNIL;i++) {
    buf >> newILS;
    t.setInnerLevelSize(i,newILS);
  }
  eatChar(']',buf);
  
  buf >> newLLS;
  t.setLeafLevelSize(newLLS);
  eatChar(';',buf);
  
  for(i=0;i<newNIL;i++) {
    for(j=0;j<t.innerLevelSize(i);j++) {
      buf >> t.constructInnerLevel(i)[j];
    }
  }

  for(j=0;j<t.leafLevelSize();j++) {
    buf >> t.constructLeafLevel()[j];
  }
 
  eatChar('}',buf);

  buf.flags(orgFlags);
  return buf;
}


ostream &operator<<(ostream &buf,const BoundTree &t) {

  unsigned short i;
  unsigned int   j;

  buf << "{ BoundTree: ";
  
  buf << t.numInnerLevels();

  buf << "\n[ ";
  
  for(i=0;i<t.numInnerLevels();i++) {
    buf << t.innerLevelSize(i);
    buf << " ";
  }
  buf << " ] ";

  buf << t.leafLevelSize() << ";\n";
  
  for(i=0;i<t.numInnerLevels();i++) {
    for(j=0;j<t.innerLevelSize(i);j++) {
      buf << t(i,j) << ' ';
    }
    buf << '\n';
  }

  for(j=0;j<t.leafLevelSize();j++) {
    buf << t[j] << ' ';
  }
 
  buf << "\n}\n";

  return buf;
}



  
//----------------------------------------------------------
//----------------------------------------------------------

//void UpdateTree(const VertexList    &vList,
//		  const RigidTransform    &rbPos,
//		  const RigidTransform    &rbVel,
//		  /* */ BoundTree     &bTree,double radius,double deltaT) { 
/*
 BoundTree::leafNode *leaves = bTree.beginLeafUpdate();
 
 register unsigned int l,numL = bTree.leafLevelSize();
 
 for(l=0;l<numL;l++) {
   
   register unsigned int mapIndex = leaves->mapIndex;
   
   register const Vector3 pos1 = rbPos * vList[mapIndex].pos;

   leaves->bound.bmax = pos1;
   leaves->bound.bmin = pos1;
   
   if (deltaT != 0.0) {
     register const Vector3 pos2 = pos1 + (rbVel * vList[mapIndex].pos) * deltaT;
     leaves->bound.bmax.bmaxSet(pos2);
     leaves->bound.bmin.bminSet(pos2);
   }

   leaves->bound.bmax[0] += radius;
   leaves->bound.bmin[0] -= radius;
   
   leaves->bound.bmax[1] += radius;
   leaves->bound.bmin[1] -= radius;
   
   leaves->bound.bmax[2] += radius;
   leaves->bound.bmin[2] -= radius;
   
   leaves++;
 }

 bTree.endLeafUpdate();
}


//----------------------------------------------------------
*/
//void UpdateTree(const VertexList    &vList,
//		  /* */ BoundTree     &bTree,double radius) {
/*
  BoundTree::leafNode *leaves = bTree.beginLeafUpdate();

  register unsigned int l,numL = bTree.leafLevelSize();

  for(l=0;l<numL;l++) {
    
    register unsigned int mapIndex = leaves->mapIndex;

    register const Vector3 &pos = vList[mapIndex].pos;

    leaves->bound.bmax[0] = pos[0]+radius;
    leaves->bound.bmin[0] = pos[0]-radius;

    leaves->bound.bmax[1] = pos[1]+radius;
    leaves->bound.bmin[1] = pos[1]-radius;

    leaves->bound.bmax[2] = pos[2]+radius;
    leaves->bound.bmin[2] = pos[2]-radius;

    leaves++;
  }

  bTree.endLeafUpdate();
}*/


//----------------------------------------------------------
//----------------------------------------------------------



static 
unsigned int findEnd(short *kList) {
  register unsigned int ret;
  for(ret=0;kList[ret]<0;ret++);
  return ret;
}

//----------------------------------------------------------

static
void findCV(const Vector3 *cList,
	    unsigned int    *iList,
	    unsigned int     segB ,
	    unsigned int     segE ,
	    Vector3       &cnt  ,
	    Vector3       &var  ) {
  
  register unsigned int i;

  cnt = 0.0;
  var = 0.0;

  if ((segE-segB) <= 0) return;

  for(i=segB;i<segE;i++) {
    cnt += cList[iList[i]];
  }

  cnt /= (segE-segB);

  for(i=segB;i<segE;i++) {
    register Vector3 tmp = cnt - cList[iList[i]];
    var[0] += tmp[0] * tmp[0];
    var[1] += tmp[1] * tmp[1];
    var[2] += tmp[2] * tmp[2];
  }
}  

//----------------------------------------------------------

static 
int splitLevel(const Vector3 *cList,
	       unsigned int    *iList,
	       short           *kList,
	       unsigned int     leafGroupSize,
	       unsigned int     level) {
  
  unsigned int chunkEnd   = 0;
  unsigned int chunkBegin = 0;

  unsigned int b,e;

  Vector3 cnt;
  Vector3 var;

  int split;

  int numSplits = 0;

#ifdef DEBUG
  if (leafGroupSize < 2) {
    cerr << "BuildTree (splitLevel):: Leaf Group size is too all...\n";
    cerr << "                           should be >= 2.                \n" << flush;
    abort();
  }
#endif

  while(kList[chunkEnd] != 0) {
    
    chunkBegin = chunkEnd;
    chunkEnd   = chunkBegin+1+findEnd(&(kList[chunkBegin+1]));

    if ((chunkEnd - chunkBegin) > leafGroupSize) {

      findCV(cList,iList,chunkBegin,chunkEnd,cnt,var);

      if      ((var[0] > var[1]) && (var[0] >= var[2])) split = 0;
      else if ((var[1] > var[0]) && (var[1] >= var[2])) split = 1;
      else if ((var[2] > var[0]) && (var[2] >= var[1])) split = 2;
      else split = 2;

      if (split >= 0) {
      
	b = chunkBegin;
	e = chunkEnd-1;

	while(b<=e) {
	  register int testB = cnt[split] >= cList[iList[b]][split];
	  register int testE = cnt[split] <= cList[iList[e]][split];
	  if ((!testB) && (!testE)) {
	    register unsigned int tmp = iList[b];
	    iList[b] = iList[e];
	    iList[e] = tmp     ;
	    b++;
	    e--;
	  }else{
	    if (testB) b ++;
	    if (testE) e --;
	  }
	}	  
	
	// If group can not split, chop group in half.
	if ((b == chunkBegin) || (e == chunkEnd-1)) {
	  b = (chunkBegin + chunkEnd-1)/2;
	  e = b -1;
	}

	if ((b>0) && (b<chunkEnd)) {
	  kList[b] = level;
	  numSplits ++;
	}
      }
    }
  }
  return numSplits;
}

//----------------------------------------------------------

static
void buildLeafLevel(unsigned int *iList  ,
		    BoundTree  &bTree  ,
		    unsigned int numItems) {
  bTree.setLeafLevelSize(numItems);
  BoundTree::leafNode *leaves = bTree.constructLeafLevel();
  unsigned int i;
  for(i=0;i<numItems;i++) {
    leaves[i].mapIndex = iList[i];
  }
}

//----------------------------------------------------------

static 
void buildInnerLevels(short        *kList,
		      BoundTree  &bTree,
		      unsigned int  numItems,
		      unsigned int  numLevel,
		      unsigned int  totalSplits) {

  int i,l;

  kList ++;

  register unsigned int childStart;
  register unsigned int nodePlace ;

  unsigned int numBreaks = totalSplits;
  unsigned int listLen   = numItems   ;

  BoundTree::innerNode *nodes;

  bTree.setNumInnerLevels(numLevel);
  for(l=numLevel-1;l>=0;l--) {
    bTree.setInnerLevelSize(l,numBreaks);
    nodes = bTree.constructInnerLevel(l);

    nodePlace  = 0;
    childStart = 0;

    for(i=0;i<listLen;i++) {
      if (kList[i]>=0) {
	nodes[nodePlace].childLevel = l+1;
	nodes[nodePlace].childStart = childStart;
	nodes[nodePlace].childCount = i+1-childStart;
	
	if (kList[i]==l) {
	  kList[nodePlace] = -1;
	  numBreaks--;
	}else{
	  kList[nodePlace] = kList[i];
	}
	nodePlace++;
	childStart = i+1;	  
      }
    }
    listLen = nodePlace;
  }
}
  
//----------------------------------------------------------

void BuildTree(const Vector3   *cList, unsigned int numItems    ,
		 /* */ BoundTree &bTree, unsigned int leafGroupSize) {

  // cList is the list of centers
  // iList is a list of indexs into cList
  // kList break list

  unsigned int *iList = new unsigned int[numItems  ];
  short        *kList = new short       [numItems+1];

  //--------------------------------------------------------

  register unsigned int i;

  //--------------------------------------------------------
    
  for(i=0;i<numItems;i++) {
    iList[i] = i ;
    kList[i] = -1;
  }
  kList[numItems] = 0;

  //--------------------------------------------------------

  unsigned int level       = 1;
  unsigned int totalSplits = 1;
  unsigned int splits         ;

  while(splits = splitLevel(cList,iList,kList,leafGroupSize,level)) {
    totalSplits += splits;
    level++;
  }

  buildLeafLevel(iList,bTree,numItems);

  buildInnerLevels(kList,bTree,numItems,level,totalSplits);

  //--------------------------------------------------------

  delete []kList;
  delete []iList;
}

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

// BoundOverlapServer

BoundOverlapServer::BoundOverlapServer() :
  _chunkList   (NULL),
  _freeListHead(NULL),
  _pendListHead(NULL),
  _cacheListHead(NULL),
  _isInUse     (   0),
  _cacheUse    (BoundOverlapServer::CNotReady) {
  addChunk();
}

//----------------------------------------------------------

BoundOverlapServer::~BoundOverlapServer() {
  blowChunks();
}

//----------------------------------------------------------

int BoundOverlapServer::isReady() const {
  return (!isInUse());
}

int BoundOverlapServer::isInUse() const {
  return _isInUse;
}

//----------------------------------------------------------

void BoundOverlapServer::addChunk() {
  register BoundOverlapServer::overlapChunk *newChunk 
    = new BoundOverlapServer::overlapChunk;
  
  newChunk->chunk[newChunk->size-1].next = _freeListHead;
  _freeListHead = &(newChunk->chunk[0]);
  
  newChunk->next = _chunkList;
  _chunkList = newChunk;
}
 
//----------------------------------------------------------

void BoundOverlapServer::blowChunks(){
  if (_pendListHead != NULL) {
    cerr << "BoundOverlapServer::blowChunks() :: Error: Pending list is not empty while blowing chunks.\n";
    cerr << "BoundOverlapServer::blowChunks() :: Atempting to continue (get ready for a core dump...)  \n";
  }

  while(_chunkList!=NULL) {
    register BoundOverlapServer::overlapChunk *tmp = _chunkList;
    _chunkList = _chunkList->next;
    delete tmp;
  }

  _pendListHead  = NULL;
  _freeListHead  = NULL;
  _cacheListHead = NULL;
}

//----------------------------------------------------------

void BoundOverlapServer::dumpPending() {
  if (_pendListHead != NULL) {
    BoundOverlapServer::overlap *tail = _pendListHead;
    while (tail->next != NULL) {
      tail = tail->next;
    }
    tail -> next = _freeListHead;
    _freeListHead = _pendListHead;
    _pendListHead = NULL;
  }
}
    
//----------------------------------------------------------

void BoundOverlapServer::clearCache() {
  if (_cacheListHead != NULL) {
    BoundOverlapServer::overlap *tail = _cacheListHead;
    while (tail->next != NULL) {
      tail = tail->next;
    }
    tail -> next = _freeListHead;
    _freeListHead = _cacheListHead;
    _cacheListHead = NULL;
  }
}

//----------------------------------------------------------
//----------------------------------------------------------


// Dummy functions to send to doTraversal to that it caches leaf hits


static 
int LeafFlag(const BoundOverlapServer  &,
	     const BoundTree::leafNode &, void *,
	     const BoundTree::leafNode &, void *) { 
  abort(); return 0; 
}

static 
int SelfFlag(const BoundOverlapServer  &,
	     const BoundTree::leafNode &, 
	     const BoundTree::leafNode &, void *) { 
  abort(); return 0; 
}



static 
int RayFlag(const BoundOverlapServer  &,
	    const BoundTree::leafNode &, void *, BoundBox &,  RayCast &) { 
  abort(); return 0; 
}


static 
int PlaneFlag(const BoundOverlapServer &,
	      const BoundTree::leafNode &, void *, const Plane &) { 
  abort(); return 0; 
}


static 
int SphereFlag(const BoundOverlapServer   &,
	       const BoundTree::leafNode  &, void *, Vector3, double) {
  abort(); return 0; 
}

//----------------------------------------------------------

int BoundOverlapServer::cacheTraversal(const BoundTree &treeOne, 
					 const BoundTree &treeTwo) {
  _cacheUse = CReady;
  int ret = doTraversal(LeafFlag,treeOne,NULL,treeTwo,NULL);
  _isInUse = 1;
  return ret;
}
		   
int BoundOverlapServer::cacheSelfTraversal(const BoundTree &tree) {
  _cacheUse = CReady;
  int ret = doSelfTraversal(SelfFlag,tree,NULL);
  _isInUse = 1;
  return ret;
}
		   
int BoundOverlapServer::cacheRayTraversal(const BoundTree &tree, RayCast &rayCast) {
  _cacheUse = CReadyRay;
  int ret = doRayTraversal(RayFlag,tree,NULL,rayCast);
  _isInUse = 1;
  return ret;
}

int BoundOverlapServer::cachePlaneTraversal(const BoundTree &tree, const Plane &plane) {
  _cacheUse = CReadyPlane;
  int ret = doPlaneTraversal(PlaneFlag,tree,NULL,plane);
  _isInUse = 1;
  return ret;
}

int BoundOverlapServer::cacheSphereTraversal(const BoundTree &tree, Vector3 cent, double rad) {
  _cacheUse = CReadySphere;
  int ret = doSphereTraversal(SphereFlag,tree,NULL,cent,rad);
  _isInUse = 1;
  return ret;
}
    
//----------------------------------------------------------

void BoundOverlapServer::doneWithCache() {
  if ((_cacheUse == CNotReady) || (!_isInUse)) {
    cerr << "BoundOverlapServer::doneWithCache :: Error: Cache was not ready...\n";
    abort();
  }

  _cacheUse = CNotReady;
  _isInUse  = 0;
  
  clearCache();
}

//----------------------------------------------------------

int BoundOverlapServer::traverseCache(LeafOverlapCB cb, 
					const BoundTree &treeOne, void *uprmOne, 
					const BoundTree &treeTwo, void *uprmTwo) {

  if ((_cacheUse != CReady) || (!_isInUse)) {
    cerr << "BoundOverlapServer::traverseCache :: Error: Cache was not ready...\n";
    abort();
  }

  overlap ** pos = &_cacheListHead;

  int result = 0;

  while ((pos != NULL) && ((*pos) != NULL)) {
    register unsigned short l1 = (*pos)->level1;
    register unsigned short l2 = (*pos)->level2;
    register unsigned int   n1 = (*pos)-> node1;
    register unsigned int   n2 = (*pos)-> node2;
    
    if ((treeOne.numInnerLevels() != l1) ||
	(treeTwo.numInnerLevels() != l2)) {
      cerr << "BoundOverlapServer::traverseCache :: Error bad trees.\n";
      abort();
    }

    if (CheckInterfere(treeOne[n1].bound,treeTwo[n2].bound)) {
      result ++;
      if (cb==NULL) {
	pos = NULL;
      }else{
	int cbrv = (*cb)(*this,treeOne[n1],uprmOne,treeTwo[n2],uprmTwo);
	if (cbrv <  0) removeFromCache(pos);
	if (cbrv == 0) pos = NULL;
      }
    }
    if (pos != NULL) pos = &((*pos)->next);
  }
  return result;
}
     
//----------------------------------------------------------

int BoundOverlapServer::traverseSelfCache(SelfOverlapCB cb, 
					    const BoundTree &tree, void *uprm) {

  if ((_cacheUse != CReady) || (!_isInUse)) {
    cerr << "BoundOverlapServer::traverseSelfCache :: Error: Cache was not ready...\n";
    abort();
  }

  overlap ** pos = &_cacheListHead;

  int result = 0;

  while ((pos != NULL) && ((*pos) != NULL)) {
    register unsigned short l1 = (*pos)->level1;
    register unsigned short l2 = (*pos)->level2;
    register unsigned int   n1 = (*pos)-> node1;
    register unsigned int   n2 = (*pos)-> node2;
    
    if ((tree.numInnerLevels() != l1) ||
	(tree.numInnerLevels() != l2)) {
      cerr << "BoundOverlapServer::traverseSelfCache :: Error bad trees.\n";
      abort();
    }

    if (CheckInterfere(tree[n1].bound,tree[n2].bound)) {
      result ++;
      if (cb==NULL) {
	pos = NULL;
      }else{
	int cbrv = (*cb)(*this,tree[n1],tree[n2],uprm);
	if (cbrv <  0) removeFromCache(pos);
	if (cbrv == 0) pos = NULL;
      }
    }
    if (pos != NULL) pos = &((*pos)->next);
  }
  return result;
}
     

//----------------------------------------------------------


int BoundOverlapServer::traverseRayCache(RayOverlapCB cb, 
					   const BoundTree &tree, void *uprm, RayCast &rayCast) {
  if ((_cacheUse != CReadyRay) || (!_isInUse)) {
    cerr << "BoundOverlapServer::traverseRayCache :: Error: Cache was not ready...\n";
    abort();
  }
  overlap ** pos = &_cacheListHead;

  int result = 0;
  BoundBox rayBound;
  setBound(rayCast,rayBound);

  while ((pos != NULL) && ((*pos) != NULL)) {
    register unsigned short l1 = (*pos)->level1;
    register unsigned int   n1 = (*pos)-> node1;
    
    if (tree.numInnerLevels() != l1) {
      cerr << "BoundOverlapServer::traverseRayCache :: Error bad trees.\n";
      abort();
    }

    if (CheckInterfere(tree[n1].bound,rayBound) &&
	RayHitBound   (tree[n1].bound,rayCast )) {
      result ++;
      if (cb==NULL) {
	pos = NULL;
      }else{
	int cbrv = (*cb)(*this,tree[n1],uprm,rayBound,rayCast);
	if (cbrv <  0) removeFromCache(pos);
	if (cbrv == 0) pos = NULL;
      }
    }
    if (pos != NULL) pos = &((*pos)->next);
  }
  return result;
}
    
//----------------------------------------------------------


int BoundOverlapServer::traversePlaneCache(PlaneOverlapCB cb, 
					     const BoundTree &tree, void *uprm, const Plane &plane) {
  if ((_cacheUse != CReadyPlane) || (!_isInUse)) {
    cerr << "BoundOverlapServer::traversePlaneCache :: Error: Cache was not ready...\n";
    abort();
  }
  overlap ** pos = &_cacheListHead;
  int result = 0;

  while ((pos != NULL) && ((*pos) != NULL)) {
    register unsigned short l1 = (*pos)->level1;
    register unsigned int   n1 = (*pos)-> node1;

    if (tree.numInnerLevels() != l1) {
      cerr << "BoundOverlapServer::traversePlaneCache :: Error rotten trees.\n";
      abort();
    }

    if (HalfSpaceHitBound(tree[n1].bound,plane)) {
      result ++;
      if (cb==NULL) {
	pos = NULL;
      }else{
	int cbrv = (*cb)(*this,tree[n1],uprm,plane);
	if (cbrv <  0) removeFromCache(pos);
	if (cbrv == 0) pos = NULL;
      }
    }
    if (pos != NULL) pos = &((*pos)->next);
  }
  return result;
}

//----------------------------------------------------------




int BoundOverlapServer::traverseSphereCache(SphereOverlapCB cb, 
					      const BoundTree &tree, void *uprm, Vector3 cent, double rad) {

  if ((_cacheUse != CReadySphere) || (!_isInUse)) {
    cerr << "BoundOverlapServer::traverseSphereCache :: Error: Cache was not ready...\n";
    abort();
  }
  overlap ** pos = &_cacheListHead;
  int result = 0;

  while ((pos != NULL) && ((*pos) != NULL)) {
    register unsigned short l1 = (*pos)->level1;
    register unsigned int   n1 = (*pos)-> node1;

    if (tree.numInnerLevels() != l1) {
      cerr << "BoundOverlapServer::traverseSphereCache :: Error rotten trees.\n";
      abort();
    }

    if (SphereHitBound(tree[n1].bound,cent,rad)) {
      result ++;

            if (cb==NULL) {
	pos = NULL;
      }else{
	int cbrv = (*cb)(*this,tree[n1],uprm,cent,rad);
	if (cbrv <  0) removeFromCache(pos);
	if (cbrv == 0) pos = NULL;
      }
    }
    if (pos != NULL) pos = &((*pos)->next);
  }
  return result;
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------


// This macro is used to add items to pending list.
// it assumes that a,aMax,lA,b,limB,lB are set.

#define ADD_TOO_PEND_LIST {			\
  for(a=aMin;a<aMax;a++) {			\
    for(b=bMin;b<bMax;b++) {			\
     pushPend(1);				\
      _pendListHead->level1 = lA;		\
      _pendListHead->level2 = lB;		\
      _pendListHead-> node1 =  a;		\
      _pendListHead-> node2 =  b;		\
    }						\
  }						\
}

//----------------------------------------------------------
//----------------------------------------------------------

int BoundOverlapServer::doTraversal(LeafOverlapCB cb, 
				      const BoundTree &treeOne, void *uprmOne, 
				      const BoundTree &treeTwo, void *uprmTwo) {

  int result = 0;
  register unsigned int a,aMax,aMin,lA;
  register unsigned int b,bMax,bMin,lB;
  //--------------------------------------------------------

  if (!isReady()) {
    cerr << "BoundOverlapServer::doTraversal :: Error: Traversal attempted on not ready server.\n";
    abort();
  }

  _isInUse = 1;

  //--------------------------------------------------------
  // Fisrt initalize pending list

  if (treeOne.numInnerLevels() == 0) {
    aMax = treeOne.leafLevelSize();
  }else{
    aMax = treeOne.innerLevelSize(0);
  }
  if (treeTwo.numInnerLevels() == 0) {
    bMax = treeTwo.leafLevelSize();
  }else{
    bMax = treeTwo.innerLevelSize(0);
  }
  aMin = 0; lA = 0;
  bMin = 0; lB = 0;

  ADD_TOO_PEND_LIST;
  
  //--------------------------------------------------------
  // Loop until the pending list is empty.
  
  while(popPend()) {
    
    register unsigned short l1 = _freeListHead->level1;
    register unsigned short l2 = _freeListHead->level2;
    register unsigned int   n1 = _freeListHead-> node1;
    register unsigned int   n2 = _freeListHead-> node2;
    if (treeOne.numInnerLevels() == l1) {
      if (treeTwo.numInnerLevels() == l2) {  
	//--------------------------------------------------
	// Leaf,Leaf
	if (CheckInterfere(treeOne[n1].bound,treeTwo[n2].bound)) {
	  result ++;
	  if (cb == LeafFlag) {
	    addToCache();
	  }else{
	    if ((cb==NULL)||
		(!((*cb)(*this,treeOne[n1],uprmOne,treeTwo[n2],uprmTwo)))) {
	      dumpPending();
	    }
	  }
	}
      }else{
	//--------------------------------------------------
	// Leaf,Inner
	if (CheckInterfere(treeOne[n1].bound,treeTwo(l2,n2).bound)) {
	  lA   = l1; 
	  aMin = n1; 
	  aMax = n1+1;
	  lB   = treeTwo(l2,n2).childLevel     ;
	  bMin = treeTwo(l2,n2).childStart     ;
	  bMax = treeTwo(l2,n2).childCount+bMin;
	  ADD_TOO_PEND_LIST;
	}
      }
    }else{
      if (treeTwo.numInnerLevels() == l2) {
	//--------------------------------------------------
	// Inner,Leaf
	if (CheckInterfere(treeOne(l1,n1).bound,treeTwo[n2].bound)) {
	  lA   = treeOne(l1,n1).childLevel     ;
	  aMin = treeOne(l1,n1).childStart     ;
	  aMax = treeOne(l1,n1).childCount+aMin;
	  lB   = l2; 
	  bMin = n2; 
	  bMax = n2+1;
	  ADD_TOO_PEND_LIST;
	}
      }else{
	//--------------------------------------------------
	// Inner,Inner
	if (CheckInterfere(treeOne(l1,n1).bound,treeTwo(l2,n2).bound)) {
	  lA   = treeOne(l1,n1).childLevel     ;
	  aMin = treeOne(l1,n1).childStart     ;
	  aMax = treeOne(l1,n1).childCount+aMin;
	  lB   = treeTwo(l2,n2).childLevel     ;
	  bMin = treeTwo(l2,n2).childStart     ;
	  bMax = treeTwo(l2,n2).childCount+bMin;
	  ADD_TOO_PEND_LIST;
	}
      }
    }
  }
  _isInUse = 0;
  return result;
}
  

//----------------------------------------------------------
//----------------------------------------------------------

int BoundOverlapServer::doSelfTraversal(SelfOverlapCB cb, 
					const BoundTree &tree, void *uprm) {


  int result = 0;
  register unsigned int a,aMax,aMin,lA;
  register unsigned int b,bMax,bMin,lB;
  //--------------------------------------------------------

  if (!isReady()) {
    cerr << "BoundOverlapServer::doSelfTraversal :: Error: Traversal attempted on not ready server.\n";
    abort();
  }

  _isInUse = 1;

  //--------------------------------------------------------
  // Fisrt initalize pending list

  if (tree.numInnerLevels() == 0) {
    aMax = tree.leafLevelSize();
    bMax = tree.leafLevelSize();
  }else{
    aMax = tree.innerLevelSize(0);
    bMax = tree.innerLevelSize(0);
  }
  aMin = 0; lA = 0;
  bMin = 0; lB = 0;

  ADD_TOO_PEND_LIST;
  
  //--------------------------------------------------------
  // Loop until the pending list is empty.
  
  while(popPend()) {
    
    register unsigned short l1 = _freeListHead->level1;
    register unsigned short l2 = _freeListHead->level2;
    register unsigned int   n1 = _freeListHead-> node1;
    register unsigned int   n2 = _freeListHead-> node2;

    if ((l1 != l2) || (n2>=n1)) {
      if (tree.numInnerLevels() == l1) {
	if (tree.numInnerLevels() == l2) {  
	  //--------------------------------------------------
	  // Leaf,Leaf
	  if (n1 != n2) {
	    if (CheckInterfere(tree[n1].bound,tree[n2].bound)) {
	      result ++;
	      if (cb == SelfFlag) {
		addToCache();
	      }else{
		if ((cb==NULL)||
		    (!((*cb)(*this,tree[n1],tree[n2],uprm)))) {
		  dumpPending();
		}
	      }
	    }
	  }
	}else{
	  //--------------------------------------------------
	  // Leaf,Inner
	  if (CheckInterfere(tree[n1].bound,tree(l2,n2).bound)) {
	    lA   = l1; 
	    aMin = n1; 
	    aMax = n1+1;
	    lB   = tree(l2,n2).childLevel     ;
	    bMin = tree(l2,n2).childStart     ;
	    bMax = tree(l2,n2).childCount+bMin;
	    ADD_TOO_PEND_LIST;
	  }
	}
      }else{
	if (tree.numInnerLevels() == l2) {
	  //--------------------------------------------------
	  // Inner,Leaf
	  if (CheckInterfere(tree(l1,n1).bound,tree[n2].bound)) {
	    lA   = tree(l1,n1).childLevel     ;
	    aMin = tree(l1,n1).childStart     ;
	    aMax = tree(l1,n1).childCount+aMin;
	    lB   = l2; 
	    bMin = n2; 
	    bMax = n2+1;
	    ADD_TOO_PEND_LIST;
	  }
	}else{
	  //--------------------------------------------------
	  // Inner,Inner
	  if (CheckInterfere(tree(l1,n1).bound,tree(l2,n2).bound)) {
	    lA   = tree(l1,n1).childLevel     ;
	    aMin = tree(l1,n1).childStart     ;
	    aMax = tree(l1,n1).childCount+aMin;
	    lB   = tree(l2,n2).childLevel     ;
	    bMin = tree(l2,n2).childStart     ;
	    bMax = tree(l2,n2).childCount+bMin;
	    ADD_TOO_PEND_LIST;
	  }
	}
      }
    }
  }  
  _isInUse = 0;
  return result;
}
  

//----------------------------------------------------------
//----------------------------------------------------------

int BoundOverlapServer::doSphereTraversal(SphereOverlapCB cb, 
					    const BoundTree &tree, void *uprm, Vector3 cent, double rad) {

  int result = 0;
  register unsigned int a,aMax,aMin,lA;
  register unsigned int b,bMax,bMin,lB; // Needed by ADD_TOO_PEND_LIST macro
  
  //--------------------------------------------------------

  if (!isReady()) {
    cerr << "BoundOverlapServer::doSphereTraversal :: Error: Traversal attempted on not ready server.\n";
    abort();
  }

  _isInUse = 1;

  //--------------------------------------------------------
  // Second initalize pending list
  
  if (tree.numInnerLevels() == 0) {
    aMax = tree.leafLevelSize();
  }else{
    aMax = tree.innerLevelSize(0);
  }
  aMin = 0; lA = 0;
 
  bMax = 1;  // There is no bTree so... ignore...
  bMin = 0;
  lB   = 0;

  ADD_TOO_PEND_LIST;
  
  //--------------------------------------------------------
  // Loop until the pending list is empty.
  
  while(popPend()) {
    
    register unsigned short l1 = _freeListHead->level1;
    register unsigned int   n1 = _freeListHead-> node1;

    if (tree.numInnerLevels() == l1) {
      //--------------------------------------------------
      // Leaf
      if (SphereHitBound(tree[n1].bound,cent,rad)) {
	result++;
	if (cb == SphereFlag) {
	  addToCache();
	}else{
	  if ((cb==NULL)||
	      (!((*cb)(*this,tree[n1],uprm,cent,rad)))) {
	    dumpPending();
	  }
	}
      }
    }else{
      //--------------------------------------------------
      // Inner
      if (SphereHitBound(tree(l1,n1).bound,cent,rad)) {
	lA   = tree(l1,n1).childLevel     ;
	aMin = tree(l1,n1).childStart     ;
	aMax = tree(l1,n1).childCount+aMin;
	lB   = 0;
	bMin = 0;
	bMax = 1;  // There is no bTree so... ignore...
	ADD_TOO_PEND_LIST;
      }
    }
   }
   _isInUse = 0;
   return result;
}

//----------------------------------------------------------
//----------------------------------------------------------


int BoundOverlapServer::doPlaneTraversal(PlaneOverlapCB cb, 
					   const BoundTree &tree, void *uprm, const Plane &plane) {
  int result = 0;
  register unsigned int a,aMax,aMin,lA;
  register unsigned int b,bMax,bMin,lB; // Needed by ADD_TOO_PEND_LIST macro
  
  //--------------------------------------------------------

  if (!isReady()) {
    cerr << "BoundOverlapServer::doPlaneTraversal :: Error: Traversal attempted on not ready server.\n";
    abort();
  }

  _isInUse = 1;

  //--------------------------------------------------------
  // Second initalize pending list
  
  if (tree.numInnerLevels() == 0) {
    aMax = tree.leafLevelSize();
  }else{
    aMax = tree.innerLevelSize(0);
  }
  aMin = 0; lA = 0;
 
  bMax = 1;  // There is no bTree so... ignore...
  bMin = 0;
  lB   = 0;

  ADD_TOO_PEND_LIST;
  
  //--------------------------------------------------------
  // Loop until the pending list is empty.
  
  while(popPend()) {
    
    register unsigned short l1 = _freeListHead->level1;
    register unsigned int   n1 = _freeListHead-> node1;

    if (tree.numInnerLevels() == l1) {
      //--------------------------------------------------
      // Leaf
      if (HalfSpaceHitBound(tree[n1].bound,plane)) {
	result++;
	if (cb == PlaneFlag) {
	  addToCache();
	}else{
	  if ((cb==NULL)||
	      (!((*cb)(*this,tree[n1],uprm,plane)))) {
	    dumpPending();
	  }
	}
      }
    }else{
      //--------------------------------------------------
      // Inner
      if (HalfSpaceHitBound(tree(l1,n1).bound,plane)) {
	lA   = tree(l1,n1).childLevel     ;
	aMin = tree(l1,n1).childStart     ;
	aMax = tree(l1,n1).childCount+aMin;
	lB   = 0;
	bMin = 0;
	bMax = 1;  // There is no bTree so... ignore...
	ADD_TOO_PEND_LIST;
      }
    }
   }
   _isInUse = 0;
   return result;
}

//----------------------------------------------------------
//----------------------------------------------------------

int BoundOverlapServer::doRayTraversal(RayOverlapCB cb, 
					 const BoundTree &tree, void *uprm, RayCast &rayCast) {
  int result = 0;
  register unsigned int a,aMax,aMin,lA;
  register unsigned int b,bMax,bMin,lB; // Needed by ADD_TOO_PEND_LIST macro
  
  //--------------------------------------------------------

  if (!isReady()) {
    cerr << "BoundOverlapServer::doTraversal :: Error: Traversal attempted on not ready server.\n";
    abort();
  }

  _isInUse = 1;

  //--------------------------------------------------------
  // Fisrt initalize bound for ray:

  BoundBox rayBound;
  setBound(rayCast,rayBound);

  //--------------------------------------------------------
  // Second initalize pending list
  
  if (tree.numInnerLevels() == 0) {
    aMax = tree.leafLevelSize();
  }else{
    aMax = tree.innerLevelSize(0);
  }
  aMin = 0; lA = 0;

  bMax = 1;  // There is no bTree so... ignore...
  bMin = 0;
  lB   = 0;

  ADD_TOO_PEND_LIST;
  
  //--------------------------------------------------------
  // Loop until the pending list is empty.
  
   while(popPend()) {
    
    register unsigned short l1 = _freeListHead->level1;
    register unsigned int   n1 = _freeListHead-> node1;

    if (tree.numInnerLevels() == l1) {
      //--------------------------------------------------
      // Leaf
      if (CheckInterfere(tree[n1].bound,rayBound) &&
	  RayHitBound   (tree[n1].bound,rayCast )) {
	result++;
	if (cb == RayFlag) {
	  addToCache();
	}else{
	  if ((cb==NULL)||
	      (!((*cb)(*this,tree[n1],uprm,rayBound,rayCast)))) {
	    dumpPending();
	  }
	}
      }
    }else{
      //--------------------------------------------------
      // Inner
      if (CheckInterfere(tree(l1,n1).bound,rayBound) &&
	  RayHitBound   (tree(l1,n1).bound,rayCast )) {
	lA   = tree(l1,n1).childLevel     ;
	aMin = tree(l1,n1).childStart     ;
	aMax = tree(l1,n1).childCount+aMin;
	lB   = 0;
	bMin = 0;
	bMax = 1;  // There is no bTree so... ignore...
	ADD_TOO_PEND_LIST;
      }
    }
   }
  _isInUse = 0;
  return result;
}



  
//----------------------------------------------------------







int RayHitBound(const BoundBox &bBox, const RayCast &ray) {

  register unsigned int i;

  int inside = 1;

  Vector3 rayBeg = ray.ray.source;
  Vector3 rayDir = ray.ray.direction;
  
  int quad[3];

  for(i=0;i<3;i++) {
    if (rayBeg[i] < bBox.bmin[i]) {
      inside = 0;
      quad[i] = -1;
    }else
      if (rayBeg[i] > bBox.bmax[i]) {
	inside = 0;
	quad[i] = 1;
      }else 
	quad[i] = 0;
  }
  
  if (inside) return 1;

  double tRangeMin =  ray.tMin;
  double tRangeMax =  ray.tMax;

  double minHit;
  double maxHit;

  for(i=0;i<3;i++) {
    if (rayDir[i] != 0.0) {
      minHit = (bBox.bmin[i] - rayBeg[i])/rayDir[i];
      maxHit = (bBox.bmax[i] - rayBeg[i])/rayDir[i];

      if (minHit < maxHit) {
	if (minHit > tRangeMin) tRangeMin = minHit;
	if (maxHit < tRangeMax) tRangeMax = maxHit;
      }else{
	if (maxHit > tRangeMin) tRangeMin = maxHit;
	if (minHit < tRangeMax) tRangeMax = minHit;
      }
    }else{
      if (quad[i] != 0.0) {
	return 0; 
      }
    }

    if (tRangeMin > tRangeMax) {
      return 0;
    }
  }
  return 1;  
}
  
//----------------------------------------------------------
//----------------------------------------------------------

