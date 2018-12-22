/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#include "vector.h"
#include "pqueue.h"
#include "kd_tree.h"

/***************************************************
Data Structure Class of kd - tree

Reference paper: 

Contains only nearest neighbour search function to
find the nearest neighbours to a query location.

Uses class Priority Queue (PQ) for nearest neighbour 
searches.

****************************************************/

class PQ;

KD_TREE::KD_TREE()
{
 Count=0;
 perm=NULL;
 Root=NULL;
 optfound=NULL;
 NNoptfound=NULL;
 nndist=NULL;
 initialized=true;
}

void KD_TREE::Selection(int l, const int N, const int  k,
                        const int discrim)const
{	
	if(!initialized) return;
	
	float v;
	int t,i,j,r;
	
	r=N;
	
	while(r>l)
	{
		v=Points[perm[r]][discrim]; i=l-1; j=r;
		for (;;)
		{
			while (Points[perm[++i]][discrim] < v);
			while (Points[perm[--j]][discrim] > v && j>l);
			if (i >= j) break;
			t=perm[i]; perm[i] = perm[j]; perm[j]=t;
		}
		t=perm[i]; perm[i] = perm[r]; perm[r]=t;
		if (i>=k) r=i-1;
		if (i<=k) l=i+1;
	}
}

int KD_TREE::Find_Max_Spread(const int l,const int u,const int dimension)const
{
	
	if(!initialized) 
	{
		int i,j,maxdim=0;
		float mmax       =-999999999.0f,
			mmin       = 999999999.0f,
			maxspread =-999999999.0f;
		
		for (i=0; i < dimension; i++)
		{
			mmax =-999999999.0f;
			mmin = 999999999.0f;
			for (j=l; j <= u; j++)
			{
				if (mmax < Points[perm[j]][i])
					mmax = Points[perm[j]][i];
				if (mmin > Points[perm[j]][i])
					mmin = Points[perm[j]][i];
				if (maxspread < fabs(mmax-mmin))
				{
					maxspread = (float)fabs(mmax-mmin);
					maxdim = i;
				}
			}
		}
		return(maxdim);
	}
	return -1;
}

kd_TreeNode*  KD_TREE::Build_Opt_Tree(const int l, const int u, const int dimension)const
{
	if(initialized) 
	{
		kd_TreeNode* p;
		int m;
		
		NEWTREE(p);
		if (u-l+1 <= BUCKETSIZE)
		{
			p->bucket = 1;
			p->lopt = l;
			p->hipt = u;
			p->loson = NULL;
			p->hison = NULL;
		}
		else
		{
			p->bucket =0;
			p->discrim = Find_Max_Spread(l,u,dimension);
			m=(l+u)/2;
			Selection(l,u,m,p->discrim);
			p->cutval = Points[perm[m]][p->discrim];
			p->loson = Build_Opt_Tree(l,m,dimension);
			p->hison = Build_Opt_Tree(m+1,u,dimension);
		}
		return(p);
	}
	return NULL;
}



void KD_TREE::Build_kd_Tree(const int numPoints,
                                const int dimension) 
{
  if(!initialized) return;
  if(perm!=NULL){free(perm);perm=0;}
  if(Root!=NULL)
  {
   Kill_kd_Tree(Root);Root=0;
   //printf("Deleted Old kd-tree\n");
  }

  /* initialize perm array */
  perm = (int *) malloc(numPoints*sizeof(int));
  if (perm == NULL) {
    printf("We have a memory problem perm\n");
  }

  for (int j=0; j < numPoints; j++) {
    perm[j]=j;
  }
  Root=Build_Opt_Tree(0,numPoints-1,dimension);
}

void KD_TREE::rnnEuclidean(const kd_TreeNode* p,const float *querpoint,
        const int dimension,const int numpoints)const
{
  if(!initialized) return;
  int i,j;
  float d,thisdist,val;
  PQ PQueue;

  if (p->bucket)
  {
	  for (i=p->lopt; i <= p->hipt; i++)
      {
		  thisdist=0.0;
		  for (j=0; j<dimension; j++)
		  {
			  d=(querpoint[j]-Points[perm[i]][j]);
			  thisdist=thisdist+d*d;
		  }
		  if((thisdist<NNRadius)&&(thisdist>0))
		  {
			  //cout<<thisdist<<" Distance fo particle \n";
			  if (NNoptfound[0] < numpoints)
				  PQueue.PQInsert(thisdist,perm[i],nndist,NNoptfound);
			  else
				  PQueue.PQreplace(thisdist,nndist,NNoptfound,perm[i]);
		  }
      }
  }
  else
  {
	  val = querpoint[p->discrim] - p->cutval;
	  if (val < 0)
	  {
		  rnnEuclidean(p->loson,querpoint,dimension,numpoints);
		  if (nndist[1] >= val*val)
			  rnnEuclidean(p->hison,querpoint,dimension,numpoints);
	  }
	  else
	  {
		  rnnEuclidean(p->hison,querpoint,dimension,numpoints);
		  if (nndist[1] >= val*val)
			  rnnEuclidean(p->loson,querpoint,dimension,numpoints);
	  }
  }
}

int * KD_TREE::kdOptNNQuery(const int dimension,
                float* querpoint,const int numNN,const float NNR, int current)const
{
	if(initialized)
	{
		int j;
		
		if(NNoptfound==NULL)
		{
			NNoptfound = (int *) malloc((numNN*100)*sizeof(int));
		}
		if(NNoptfound==NULL) printf("we have memory problem for NNoptfound\n");
		
		NNoptfound[0]=0;  /* for now */
 
		/* nndist is a priority queue of the distances of the nearest neighbors found */
		if(nndist==NULL)
		{
			nndist = (float *)malloc((numNN*100)*sizeof(float));
		}
		if(nndist==NULL)printf("we have memory problem for nndist\n");
		
		for (j=0; j < numNN; j++) {
			nndist[j] = 99999999999.0f;
		}
		
                //Insert current particle into the found list
                NNoptfound[0]=1;
                nndist[0]=0.0;
                nndist[1]=0.0;
                NNoptfound[1]=current;
		
		NNRadius=NNR;
		rnnEuclidean(Root,querpoint,dimension,numNN);
                
		return(NNoptfound);
	}
	else
		return NULL;
}

void KD_TREE::Kill_kd_Tree(kd_TreeNode* P)
{
  if (perm != NULL) {
    free(perm);
  }  /* free permutation array */

  if (P==NULL) {
    return;
  } /* just to be sure */
  if (P->loson != NULL) {
    Kill_kd_Tree(P->loson);
  }

  if (P->hison != NULL) {
    Kill_kd_Tree(P->hison);
  }

  free(P);

}

void KD_TREE::Set_Points( float** Pts)
{
  Points = Pts;
}


KD_TREE::~KD_TREE()
{
	if(initialized)
	{
//		cout<<"In Destructor of kd-Tree\n";
		if(Root!=NULL)
		{
			Kill_kd_Tree(Root);
			Root=0;
		}
		if(perm!=NULL)
		{
			delete [] perm;
			perm=0;
		}
		delete [] NNoptfound;
		NNoptfound=0;
		if(nndist!=NULL)
		delete [] nndist;
		nndist=0;
		if(optfound!=NULL)
		delete [] optfound;
		optfound=0;
		initialized=false;
//		cout<<"Deinited the kd-tree \n";
	}
}


