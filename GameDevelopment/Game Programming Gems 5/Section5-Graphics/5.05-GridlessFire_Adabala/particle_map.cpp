/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

/************************************************
Class that inherits from the abstract kd_tree 
structure and creates the data structure of 
particle maps by including properties related 
to the particles namely positions. 
*************************************************/

#include <fstream>

#include "vector.h"
#include "pqueue.h"
#include "kd_tree.h"
#include "particle_map.h"

PARTICLE_MAP::PARTICLE_MAP(float **Pts,int N,VECTOR vmin,VECTOR vmax):
filename(" "), num_particles(N),mmin(vmin),mmax(vmax),Particles(Pts)
{ 
  Set_Points(Particles); 
}

PARTICLE_MAP::PARTICLE_MAP(char* name,int N):filename(name),
num_particles(N)
{
 Read_Particle_Locations();
}

void PARTICLE_MAP::Read_Particle_Locations()
{
    FILE *infile;

   Particles=(float **)(malloc(num_particles*sizeof(float *)));


   for(int i=0;i<num_particles;i++)
    {
      Particles[i]=(float *)(malloc(3*sizeof(float)));
      if(Particles[i]==NULL)printf("We have a memory Problem!!\n");
    }


   if(filename==" ")
	   printf("Error no file name\n");

   infile=fopen(filename,"r");

    if(infile==NULL)
         printf("Unable to open file specified!\n");

 for(int i=0;i<num_particles;i++)
 {
fscanf(infile,"%f %f %f",&Particles[i][0],&Particles[i][1],&Particles[i][2]);

  Particles[i][0]=Particles[i][0]/1.5f;
  Particles[i][1]=Particles[i][1]/3.5f;
  Particles[i][2]=Particles[i][2]/2.0f;

 }

    float minx,miny,minz,maxx,maxy,maxz;


    fscanf(infile,"%f %f %f",&minx,&miny,&minz);
    fscanf(infile,"%f %f %f",&maxx,&maxy,&maxz);

   VECTOR t(0.2,0.2,0.2);
   mmin.x=(double)minx/1.5; mmax.x=(double)maxx/1.5;
   mmin.y=(double)miny/3.5; mmax.y=(double)maxy/3.5;
   mmin.z=(double)minz/2; mmax.z=(double)maxz/2;

   mmin=mmin-t;
   mmax=mmax+t;

   Set_Points(Particles);
}

void PARTICLE_MAP::Build_Particle_Map(int dim) 
{
 Build_kd_Tree(num_particles,dim);
}

void PARTICLE_MAP::Set_num_particles(int n)
{
   num_particles=n;
}


PARTICLE_MAP::~PARTICLE_MAP()
{
   if(Particles!=NULL)
   {
   for(int i=0;i<num_particles;i++)
    {
     free(Particles[i]);
     Particles[i]=0;
    }
   free(Particles);
   Particles=0;
   }
}

