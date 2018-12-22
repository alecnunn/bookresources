/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#ifndef PARTICLE_MAP_H
#define PARTICLE_MAP_H

#include <stdio.h>
#include <math.h>
#include "kd_tree.h"


class PARTICLE_MAP:public KD_TREE
 {
  private:

     char* filename;
     int num_particles;

  public:

    VECTOR mmin,mmax;
    float** Particles;

    PARTICLE_MAP(){}

    PARTICLE_MAP(float **Pts, int N, VECTOR vmin, VECTOR vmax);

    PARTICLE_MAP(char *Name, int N); 

    void Read_Particle_Locations();

    void Build_Particle_Map(int dim);
  
    void Set_num_particles(int n);

    ~PARTICLE_MAP();

 };


#endif
