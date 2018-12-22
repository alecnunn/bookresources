/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#ifndef PQ_H
#define PQ_H

#include <stdio.h>
#include <math.h>

class PQ
{

 private:

   void PQupheap(float *DistArr,int *FoundArr,int k)const;
   void PQdownheap(float *DistArr,int *FoundArr,int k,int index)const;


 public:

   void PQInsert(float distance,int index,float *DistArr,int *FoundArr)const;
   void PQreplace(float distance,float *DistArr,int *FoundArr,int index)const;

};

#endif

