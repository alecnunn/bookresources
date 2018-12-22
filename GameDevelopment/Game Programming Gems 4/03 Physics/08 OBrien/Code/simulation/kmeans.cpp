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

#include "math/primitives.h"
#include <float.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************
 * mykmeans()
 * A variation on kmeans where all clustered pts must
 * be within mindist.
 *
 * pts: a list of points (size nPts)
 * nPts: the # of points in pts
 * nClusters: the desired k # of clusters
 * mindist: minimum distance of cluster
 * maxIters: maximum # of iterations
 * clusters: (output) array of clusters (size nClusters)
 * nclustered: (output) array of # of clustered points (size nClusters)
 * membership: (output) array of point-cluster membership (size nPts)
 *     stores -1 if i'th point is unclustered
 *************************************************/
int mykmeans(const Vector3* pts, int nPts, int nClusters,
	    double mindist, int maxIters,
	    Vector3* clusters, int* nclustered, int* membership)
{
  int i,j,k;

  k = nClusters;

  double* dist = new double [nPts];
  int num_valid_clusters = 0;

  //we'll use distance squared
  mindist = mindist*mindist;

  Vector3 temp;

  //initialize clusters to k random points
  for(i=0; i<k; i++) {
    clusters[i] = pts[rand()%nPts];
  }

  for(int iter = 0; iter < maxIters; iter++) {
    //we have new clusters, initialize their inclusion to 0
    for(i=0; i<k; i++)
      nclustered[i] = 0;

    //calculate membership
    for(i=0; i<nPts; i++) {
      membership[i] = -1;
      dist[i] = mindist;
      for(j=0; j<k; j++) {
	//calc distance from this cluster, if it can/should be clustered, do so
	temp = pts[i] - clusters[j];
	double cdist = dot(temp,temp);
	if(cdist < dist[i]) {
	  membership[i] = j;
	  dist[i] = cdist;
	}
      }
      if(membership[i] != -1)
	nclustered[membership[i]]++;
    }
    
    //update clusters with the mean of the values clustered by them
    //if a cluster has no membership, toss it and find a new one
    for(i=0; i<nPts; i++) {
      if(membership[i] != -1)
	clusters[membership[i]] += pts[i];
    }
    num_valid_clusters = 0;
    for(i=0; i<k; i++) {
      if(nclustered[i]) {    //some clustered points in this cluster, keep it
	//its new position is the mean of all clustered points
	clusters[i] /= nclustered[i]+1;
	num_valid_clusters++;
      }
      else {
	clusters[i] = pts[rand()%nPts];
      }
    }
  }

  delete [] dist;

  //We're done!

  return nClusters;
}
