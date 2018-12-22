/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#include <iostream>
#include "pqueue.h"
#include "kd_tree.h"
#include "dyn_particle_map.h"

/******************************************************
rand_get() generates a number in a normal distribution
by rejection method.
*******************************************************/

float rand_get()
{
    float t1, t2, n;

    t1=((float)rand()/RAND_MAX-0.5);
    t2=(float)rand()/RAND_MAX;
    n=0.39894228*exp(-t1*t1/2);

    while(n<t2)
      {
        t1=((float)rand()/RAND_MAX-0.5);
        t2=(float)rand()/RAND_MAX;
        n=0.39894228*exp(-t1*t1/2);
      }
    return t1;
}

/******************************************************

Class inherits from PARTICLE_MAP and defines the 
properties of the particles - namely dynamics,
velocities and updates. Also the compositions of
fire particles are initialized and updated here.

*******************************************************/

DYN_PARTICLE_MAP::DYN_PARTICLE_MAP(float **Pts,int N,VECTOR vmin,VECTOR vmax):
num_pts(N),min(vmin),max(vmax),Particles(Pts)
{ 
  width=5.0;
  spread=6.0;
  nu=0.2;
  chi=0.97;
  lambda=0.8;
  theta=0.4;
  Set_Points(Particles); 
  Set_num_particles(num_pts);
}

DYN_PARTICLE_MAP::DYN_PARTICLE_MAP(int N): num_pts(N)
{
 width=5.0;
 spread=6.0;
 nu=1.0;
 chi=0.97;
 lambda=0.8;
 theta=0.4;
 Set_num_particles(num_pts);
 Create_Particles();
 Set_Points(Particles);
}

void DYN_PARTICLE_MAP::Create_Particles()
{

   Particles=(float **)(malloc(num_pts*sizeof(float *)));

   for(int i=0;i<num_pts;i++)
    {
     Particles[i]=(float *)(malloc(3*sizeof(float)));
     if(Particles[i]==NULL)printf("We have a memory problem position allocation!!\n");
    }

   Velocity=(float **)(malloc(num_pts*sizeof(float *)));

   for(int i=0;i<num_pts;i++)
    {
     Velocity[i]=(float *)(malloc(3*sizeof(float)));
     if(Velocity[i]==NULL)printf("We have a memory problem for Velocity!!\n");
    }

  gamma=(float *)(malloc(num_pts*sizeof(float)));
    if(gamma==NULL)printf("We have a memory problem for gamma allocation!!\n");
  
  Composition=(float *)(malloc(num_pts*sizeof(float)));
    if(Composition==NULL)printf("We have a memory problem composition allocation!!\n");

   for(int i=0;i<num_pts;i++)
    {
      Particles[i][0]=((float)rand()/RAND_MAX)*width-width/2;
      Particles[i][1]=0;
      Particles[i][2]=((float)rand()/RAND_MAX)*width-width/2;
      Velocity[i][0]=(float)rand()/RAND_MAX*0.05;
      Velocity[i][1]=(float)rand()/RAND_MAX*2.0;
      Velocity[i][2]=(float)rand()/RAND_MAX*0.05;
      gamma[i]=(float)rand()/RAND_MAX*0.1;
      float t1=(nu*Particles[i][0]-floor(Particles[i][0]*nu))/(lambda);
      float t2=(nu*Particles[i][2]-floor(Particles[i][2]*nu))/(lambda);
      Composition[i]=exp(-t1*t1+ -t2*t2);
       if(Composition[i]<0.0)Composition[i]=-Composition[i];
    }
}
/*********************************************************************
Create_Memory_for_New_Particles allocates memory for updated particle
locations. Memory is also allocated to store particle locations for the
previous two time steps.
**********************************************************************/

void DYN_PARTICLE_MAP::Create_Memory_for_New_Particles(int n_pts)
{
   new_num_pts=n_pts;

   new_Particles=(float **)(malloc(new_num_pts*sizeof(float *)));

   for(int i=0;i<new_num_pts;i++)
    {
     new_Particles[i]=(float *)(malloc(3*sizeof(float)));
     if(new_Particles[i]==NULL)printf("We have a memory Problem!!\n");
    }

   old_Particles=(float **)(malloc(new_num_pts*sizeof(float *)));

   for(int i=0;i<new_num_pts;i++)
    {
     old_Particles[i]=(float *)(malloc(3*sizeof(float)));
     if(old_Particles[i]==NULL)printf("We have a memory Problem!!\n");
    }

   oldold_Particles=(float **)(malloc(new_num_pts*sizeof(float *)));

   for(int i=0;i<new_num_pts;i++)
    {
     oldold_Particles[i]=(float *)(malloc(3*sizeof(float)));
     if(oldold_Particles[i]==NULL)printf("We have a memory Problem!!\n");
    }

   new_Velocity=(float **)(malloc(new_num_pts*sizeof(float *)));

   for(int i=0;i<new_num_pts;i++)
    {
     new_Velocity[i]=(float *)(malloc(3*sizeof(float)));
     if(new_Velocity[i]==NULL)printf("We have a memory Problem!!\n");
    }

   new_gamma=(float *)(malloc(new_num_pts*sizeof(float)));
    if(new_gamma==NULL)printf("We have a memory Problem!!\n");

   for(int i=0;i<num_pts;i++)
     new_gamma[i]=gamma[i];

   for(int i=0;i<num_pts;i++)
     {
      new_Particles[i][0]=Particles[i][0];
      new_Particles[i][1]=Particles[i][1];
      new_Particles[i][2]=Particles[i][2];
      new_Velocity[i][0]=Velocity[i][0];
      new_Velocity[i][1]=Velocity[i][1];
      new_Velocity[i][2]=Velocity[i][2];
     }

  if(new_num_pts>num_pts)
   {
      printf("More particles being introduced\n");
      printf("new_num_points = %d old num_points = %d\n", new_num_pts, num_pts);

   for(int i=num_pts;i<new_num_pts;i++)
    {
      new_Particles[i][0]=((float)rand()/RAND_MAX)*width-width/2;
      new_Particles[i][1]=0;
      new_Particles[i][2]=((float)rand()/RAND_MAX)*width-width/2;
      new_Velocity[i][0]=(float)rand()/RAND_MAX*0.05;
      new_Velocity[i][1]=(float)rand()/RAND_MAX*2.0;
      new_Velocity[i][2]=(float)rand()/RAND_MAX*0.05;
      new_gamma[i]=(float)rand()/RAND_MAX*0.1;
    }

   }
}
  
/***********************************************************

Update_Dynamics computes the updated location of particles 
using the Stochastic Lagraginan Approach

************************************************************/

void DYN_PARTICLE_MAP::Update_Dynamics(const int *NNoptfound, float dt, int current)const
{
  float En_U[3],En_Omega,dW[3],dW_s;
  float dU[3], dOmega; 

  //Updates the dynamics 'current' particle 
  //Compute the Ensemble Averages in the neighbouring region
     En_U[0]=0; En_U[1]=0; En_U[2]=0;
     En_Omega=0;
     for(int j=1;j<=NNoptfound[0]; j++)
	{
	  for(int k=0;k<3;k++)
              {
		  En_U[k]+=Velocity[NNoptfound[j]][k];
		  En_Omega+=gamma[NNoptfound[j]];
              }
	}

   En_U[0]/=NNoptfound[0]; En_U[1]/=NNoptfound[0]; En_U[2]/=NNoptfound[0];
   En_Omega/=NNoptfound[0];

   dW[0]=rand_get(); dW[1]=rand_get(); dW[2]=rand_get();
   dW_s=rand_get();

   dU[0]=-1.575*En_Omega*(Velocity[current][0]-En_U[0])*dt+1.7748*sqrt(En_Omega)*dW[0];
   dU[1]=-1.575*En_Omega*(Velocity[current][1]-En_U[1])*dt+1.7748*sqrt(En_Omega)*dW[1];
   dU[2]=-1.575*En_Omega*(Velocity[current][2]-En_U[2])*dt+1.7748*sqrt(En_Omega)*dW[2];

   dOmega=-(gamma[current]-En_Omega)*dt-sqrt(0.5*En_Omega*gamma[current]*En_Omega)*dW_s;

   new_Velocity[current][0]+=dU[0];
   new_Velocity[current][1]+=dU[1];
   new_Velocity[current][2]+=dU[2];

   new_gamma[current]+=dOmega*0.1;

   Composition[current]=Composition[current]*(chi+rand_get());

   new_Particles[current][0]+=Velocity[current][0]*dt;
   new_Particles[current][1]+=Velocity[current][1]*dt;
   new_Particles[current][2]+=Velocity[current][2]*dt;
}

/**********************************************************************
Restart_Particles re-initializes particles position, velocity
gamma and composition.
***********************************************************************/

void DYN_PARTICLE_MAP::Restart_Particle(int i)
{
  Particles[i][0]=((float)rand()/RAND_MAX)*width-width/2;
  Particles[i][1]=0;
  Particles[i][2]=((float)rand()/RAND_MAX)*width-width/2;
  Velocity[i][0]=(float)rand()/RAND_MAX*0.05;
  Velocity[i][1]=(float)rand()/RAND_MAX*2.0;
  Velocity[i][2]=(float)rand()/RAND_MAX*0.05;
  gamma[i]=(float)rand()/RAND_MAX*0.1;
  float t1=(nu*Particles[i][0]-floor(Particles[i][0]*nu))/(lambda);
  float t2=(nu*Particles[i][2]-floor(Particles[i][2]*nu))/(lambda);
  Composition[i]=exp(-t1*t1+ -t2*t2);
  if(Composition[i]<0.0)Composition[i]=-Composition[i];
  old_Particles[i][0]=Particles[i][0];
  old_Particles[i][1]=Particles[i][1];
  old_Particles[i][2]=Particles[i][2];
  oldold_Particles[i][0]=Particles[i][0];
  oldold_Particles[i][1]=Particles[i][1];
  oldold_Particles[i][2]=Particles[i][2];
}

/******************************************************************
Exchange_Updated_Particles performs the maintanance operations 
between successive time steps.
*******************************************************************/

void DYN_PARTICLE_MAP::Exchange_Updated_Particles()
{

   for(int i=0;i<num_pts;i++)
     {
      old_Particles[i][0]=Particles[i][0];
      old_Particles[i][1]=Particles[i][1];
      old_Particles[i][2]=Particles[i][2];
      oldold_Particles[i][0]=old_Particles[i][0];
      oldold_Particles[i][1]=old_Particles[i][1];
      oldold_Particles[i][2]=old_Particles[i][2];
     }

   for(int i=0;i<num_pts;i++)
    {
     free(Particles[i]);
     Particles[i]=0;
    }
   free(Particles);
   Particles=0;

   for(int i=0;i<num_pts;i++)
    {
     free(Velocity[i]);
     Velocity[i]=0;
    }
   free(Velocity);
   Velocity=0;
   
   free(gamma);
   gamma=0;

  num_pts=new_num_pts;
  Particles=new_Particles;
  Velocity=new_Velocity;
  gamma=new_gamma;

   for (int i=0; i<num_pts; i++)
     {
       if (gamma[i]>5.0)
        gamma[i]=gamma[i]-floor(gamma[i]);
     } 
   
   float fuzzy_edge=(float)rand()/RAND_MAX*3.0; 
   for (int i=0; i<num_pts; i++)
     {
       if ((Particles[i][0]>(width*0.1+width/spread+fuzzy_edge))||(Particles[i][0]<(-1.0-width/spread-fuzzy_edge)))
         Restart_Particle(i);
       if (Particles[i][1]<0.0)
          Restart_Particle(i);
       if ((Particles[i][2]>(width*0.1+width/spread+fuzzy_edge))||(Particles[i][2]<(-1.0-width/spread-fuzzy_edge)))
        Restart_Particle(i);
     } 
   
static int num =0;

   float ComSum=0.0;
   for (int i=0; i<num_pts; i++)
     {
       ComSum+=fabs(Composition[i]);
     } 

if(ComSum/num_pts<theta)
{
   for (int i=0; i<num_pts; i++)
     {
          Restart_Particle(i);
     }
        num++;
}

  Set_Points(Particles);
  Set_num_particles(num_pts);
  Build_Particle_Map(3);
}

DYN_PARTICLE_MAP::~DYN_PARTICLE_MAP()
{
   for(int i=0;i<num_pts;i++)
    {
     free(Particles[i]);
     Particles[i]=0;
    }
   free(Particles);
   Particles=0; 

   for(int i=0;i<num_pts;i++)
    {
     free(Velocity[i]);
     Velocity[i]=0;
    }
   free(Velocity);
   Velocity=0; 

   for(int i=0;i<num_pts;i++)
    {
     free(old_Particles[i]);
     old_Particles[i]=0;
    }
   free(old_Particles);
   old_Particles=0; 

   for(int i=0;i<num_pts;i++)
    {
     free(oldold_Particles[i]);
     oldold_Particles[i]=0;
    }
   free(oldold_Particles);
   oldold_Particles=0; 

   if(gamma!=0)
   free(gamma);
   gamma=0;

   if(Composition!=0)
   free(Composition);
   Composition=0;
}



