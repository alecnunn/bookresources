/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#ifndef RAYPP_KD_TREE
#define RAYPP_KD_TREE

#include <stdio.h>
#include <math.h>
#include "vector.h"
#include "pqueue.h"

#define BUCKETSIZE 50

/* Used to create a new tree in the k-d tree */
#define TESTTREE(PP)  ((PP) = (kd_TreeNode *)malloc(sizeof(kd_TreeNode)))
#define NEWTREE(PP)  if (TESTTREE(PP)==NULL) \
                         {printf("memory error\n");return(NULL);}


/* Structure of kd Tree Node */
typedef struct kd_TreeNode
     {
       int index; /*stores index of point in points array*/
       int discrim; /* stores which dimension of the tree partitions data*/
       float cutval; /*stores the value which partions loson and hison*/
       int bucket; /*is 1 if node is a bucket node*/
       int lopt,hipt; /*the lowest and highest value of discriminant in
                        bucket node*/
       struct kd_TreeNode *loson, *hison; /*pointers to sons of node*/
     }treenode;


class PQ;

class KD_TREE
{
  private:

    bool initialized;
    mutable kd_TreeNode* Root;
    float** Points;
    /* Nearest Neighbour Distance Array */

    /* permutation array */
    mutable int *perm;

    /* Variables that store the Nearest Neighbour (NN) Query Results */
    mutable int Count;
    mutable int *optfound;
    mutable int *NNoptfound;
    mutable float NNRadius; /*Radius of NN query of region search*/

    void Selection(int l, const int N, const int  k,
                   const int discrim)const;

    int Find_Max_Spread(const int l,const int u,const int dimension)const;

    kd_TreeNode* Build_Opt_Tree(const int l,const int u,
                              const int dimension)const ;

    void rnnEuclidean(const kd_TreeNode* p,const float *querpoint,
            const int dimension, const int numpoints) const;

    void Kill_kd_Tree(kd_TreeNode* P);

 public:

    mutable float *nndist;

    KD_TREE();

    void Build_kd_Tree(const int numPoints, const int dimension);

    int *kdOptNNQuery(const int dimension,
                      float* querpoint,const int numNN, const float NNR, int current)const;


    void Set_Points(float** Pts);

    ~KD_TREE();
};



#endif

