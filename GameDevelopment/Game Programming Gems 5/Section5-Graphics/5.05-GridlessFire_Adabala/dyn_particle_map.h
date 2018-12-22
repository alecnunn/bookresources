/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#ifndef DYN_PARTICLE_MAP_H
#define DYN_PARTICLE_MAP_H

#include <stdio.h>
#include <math.h>
#include "kd_tree.h"
#include "particle_map.h"

class DYN_PARTICLE_MAP:public PARTICLE_MAP
 {
  private:

     float* new_gamma;
     float* gamma;
     
  public:
	
    int num_pts,new_num_pts;
    mutable VECTOR min,max;
    float** Particles;
    float* Composition;
    float** Velocity;
    float** new_Particles;
    float** old_Particles;
    float** oldold_Particles;
    float** new_Velocity;

    float width;
    float spread;

    //Parameters to control the appearance of fire
    float nu;  // relates to number of flame brushes
    float chi;  // controls the rate of flicker along with theta
    float theta; // threshold at which the global extiction occurs
    float lambda;  // relates to width of flame brushes

    DYN_PARTICLE_MAP(float **Pts, int N, VECTOR vmin, VECTOR vmax);

    DYN_PARTICLE_MAP(int N); 

    void Create_Particles();

    void Create_Memory_for_New_Particles(int size);

    void Update_Dynamics(const int *NNoptfound, float dt, int current)const;

    void Exchange_Updated_Particles();

    void Restart_Particle(int i);

    ~DYN_PARTICLE_MAP();

 };

#endif
