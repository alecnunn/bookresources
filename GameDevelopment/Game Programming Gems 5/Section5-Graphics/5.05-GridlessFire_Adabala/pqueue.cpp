/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#include"pqueue.h"

void PQ::PQupheap(float *DistArr,int *FoundArr,int k)const
{
float v;
int j;

  //printf("in PQupheap\n");
v=DistArr[k]; DistArr[0] = 999999999999999.0f;
j=FoundArr[k];

while((DistArr[k/2] <= v)&&(k>0)) {
  DistArr[k] = DistArr[k/2];
  FoundArr[k] = FoundArr[k/2];
  k=k/2;
//  printf("in PQupheap\n");
}

if (k==0) printf("Distance is %f\n",v);
DistArr[k] = v;
FoundArr[k] = j;
}


void PQ::PQInsert(float distance,int index,float *DistArr,int *FoundArr)
const
{

  //printf("in PQInsert\n");
  FoundArr[0]=FoundArr[0]+1;
  DistArr[FoundArr[0]] = distance;
  FoundArr[FoundArr[0]] = index;
  PQupheap(DistArr,FoundArr,FoundArr[0]);
}

void PQ::PQdownheap(float *DistArr,int *FoundArr,int k,int index) const
{
int j,N;
float v;
v=DistArr[k];
N = FoundArr[0];  /* tricky patch to maintain the data structure */
FoundArr[0]=index;
while (k <= N/2) {
  j=k+k;
  if (j < N && DistArr[j] <DistArr[j+1]) j++;
  if (v>=DistArr[j]) break;
  DistArr[k]=DistArr[j];
  FoundArr[k]=FoundArr[j];
  k=j;
  //printf("in PQdownheap \n");
}
DistArr[k] = v;
FoundArr[k]= index;
FoundArr[0]=N;  /* restore data struct */
}

void PQ::PQreplace(float distance,float *DistArr,int *FoundArr,int index)
const
{
  //printf("in PQreplace\n");
  DistArr[0]=distance;
  PQdownheap(DistArr,FoundArr,0,index);
}
