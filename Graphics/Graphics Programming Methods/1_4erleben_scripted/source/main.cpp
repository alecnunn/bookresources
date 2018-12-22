////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002
//  Department of Computer Science Copenhagen University (DIKU).
//
//  All Rights Reserved.
//
//  This software is provided as is WITHOUT ANY WARRANTY; Use it on
//  your own risk.
//
//  Please send remarks, questions and bug reports to kenny@diku.dk
//
////////////////////////////////////////////////////////////////////////////////
#if(_MSC_VER == 1200)
#  pragma warning(disable:4244)   // No warnings on precision truncation
#  pragma warning(disable:4305)   // No warnings on precision truncation
#  pragma warning(disable:4786)   // stupid symbol size limitation
#endif

#if defined(WIN32)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <numerical/scalar.h>
#include <numerical/vectorN.h>
#include <spline/nubs.h>

bool drawPosition     = false;
bool drawVelocity     = false;
bool drawAcceleration = false;

NUBSpline * velocity = 0;
NUBSpline * space = 0;

/**
 *
 */
NUBSpline * createFancySpaceSpline(){
  int n = 5;
  VectorN * keypositions = new VectorN[n];
  for(int i=0;i<n;++i){
    keypositions[i].resize(3);
  }
  
  scalar width  = 10;
  scalar height =  5;
  scalar depth  = 10;

  keypositions[0].entry[0] = -2*width;
  keypositions[0].entry[1] = height;
  keypositions[0].entry[2] = 0*depth;

  keypositions[1].entry[0] = -1*width;
  keypositions[1].entry[1] = -height;
  keypositions[1].entry[2] = -1*depth;

  keypositions[2].entry[0] =  0;
  keypositions[2].entry[1] =  height;
  keypositions[2].entry[2] =  2*depth;

  keypositions[3].entry[0] = 1*width;
  keypositions[3].entry[1] = -height;
  keypositions[3].entry[2] = -3*depth;

  keypositions[4].entry[0] = 2*width;
  keypositions[4].entry[1] = height;
  keypositions[4].entry[2] = 4*depth;

  NUBSpline * spaceSpline = new NUBSpline();
  spaceSpline->setDataPoints(4,keypositions,n,false);

  delete [] keypositions;

  return spaceSpline;
};

/**
 *
 */
NUBSpline * createSimpleSpaceSpline(){
  int n = 5;
  VectorN * keypositions = new VectorN[n];
  for(int i=0;i<n;i++){
    keypositions[i].resize(3);
  }
  
  scalar width = 10;
  scalar height = 5;

  keypositions[0].entry[0] = -2*width;
  keypositions[0].entry[1] = height;
  keypositions[0].entry[2] = 0;

  keypositions[1].entry[0] = -1*width;
  keypositions[1].entry[1] = -height;
  keypositions[1].entry[2] = 0;

  keypositions[2].entry[0] = 0;
  keypositions[2].entry[1] = height;
  keypositions[2].entry[2] = 0;

  keypositions[3].entry[0] = 1*width;
  keypositions[3].entry[1] = -height;
  keypositions[3].entry[2] = 0;

  keypositions[4].entry[0] = 2*width;
  keypositions[4].entry[1] = height;
  keypositions[4].entry[2] = 0;

  NUBSpline * spaceSpline = new NUBSpline();
  spaceSpline->setDataPoints(4,keypositions,n,false);

  delete [] keypositions;

  return spaceSpline;
};

/**
 *
 */
NUBSpline * createConstantVelocitySpline(scalar tmax){
  int m = 2;
  VectorN * travellingdistances = new VectorN[m];
  for(int i=0;i<m;i++){
    travellingdistances[i].resize(2);
  }
  
  travellingdistances[0].entry[0] = 0.0f;
  travellingdistances[0].entry[1] = 0.0f;
  
  travellingdistances[1].entry[0] = tmax;
  travellingdistances[1].entry[1] = 1.0f;

  NUBSpline * velocitySpline = new NUBSpline();
  velocitySpline->setDataPoints(4,travellingdistances,m,false);

  delete [] travellingdistances;
  
  return velocitySpline;
};

/**
 *
 */
NUBSpline * createExageratedEaseInEaseOutVelocitySpline(scalar tmax){

  int m = 7;
  VectorN * travellingdistances = new VectorN[m];
  for(int i=0;i<m;i++){
    travellingdistances[i].resize(2);
  }

  travellingdistances[0].entry[0] = 0.0f;
  travellingdistances[0].entry[1] = 0.0f;

  travellingdistances[1].entry[0] = tmax/6.0f;
  travellingdistances[1].entry[1] = 0.01f;

  travellingdistances[2].entry[0] = tmax/3.0f;
  travellingdistances[2].entry[1] = 0.1f;

  travellingdistances[3].entry[0] = tmax/2.0f;
  travellingdistances[3].entry[1] = 0.5f;

  travellingdistances[4].entry[0] = 4.0f*tmax/6.0f;
  travellingdistances[4].entry[1] = 0.9f;

  travellingdistances[5].entry[0] = 5.0f*tmax/6.0f;
  travellingdistances[5].entry[1] = 0.99f;

  travellingdistances[6].entry[0] = tmax;
  travellingdistances[6].entry[1] = 1.0f;
  
  NUBSpline * velocitySpline = new NUBSpline();
  velocitySpline->setDataPoints(4,travellingdistances,m,false);

  delete [] travellingdistances;
  
  return velocitySpline;
};

/**
 *
 */
void drawScriptedMotion(NUBSpline * S,NUBSpline * V,scalar t0,scalar t1,int steps){
  VectorN p(3);
  VectorN v(3);
  VectorN a(3);

  for(int i=0;i<=steps;++i){
  
    scalar ratio = (scalar)((1.0f*i)/steps);
    scalar t = ratio*(t1-t0)+t0;
    
    S->scriptedMotion(t,*V,p,v,a);

    if(drawPosition){
    }

    if(drawVelocity){
    }

    if(drawAcceleration){
    }
  }
};

/**
 *
 */
void drawSpaceSpline(NUBSpline * S,int steps){

  VectorN * dQ = new VectorN[3];
  for(int j=0;j<3;j++){
    dQ[j].resize(3);
  }

  scalar U = S->getn() - 2;  //--- Because |X| = n-1; so U = n-2

  for(int i=0;i<=steps;++i){

    scalar ratio = (scalar)((1.0f*i)/steps);
    scalar u = ratio*U;
    S->getDerivatives(u,2,dQ);

    if(drawPosition){
    }

    if(drawVelocity){
    }

    if(drawAcceleration){
    }

  }

  delete [] dQ;
};

/**
 *
 */
void drawSpaceSplineSegments(NUBSpline * S,int steps){
  VectorN p(3);
  VectorN v(3);
  VectorN a(3);

  int substeps = steps/S->getSegmentCount();

  for(int j=0;j<S->getSegmentCount();++j){

    CubicCurve * segment = S->getSegment(j);
    //scalar scale = S->getKnotVector()[j+4] - S->getKnotVector()[j+3];

    for(int i=0;i<=substeps;++i){

      scalar ratio = (scalar)((1.0f*i)/substeps);
      scalar u = ratio;
      
      segment->getDerivativesAtParameterValue(u,p,v,a);

      if(drawPosition){
      }

      if(drawVelocity){
      }

      if(drawAcceleration){
      }
    }
  }
};

/**
 *
 */
void drawVelocitySpline(NUBSpline * V,int steps){
  VectorN * dQ = new VectorN[3];
  for(int j=0;j<3;j++){
    dQ[j].resize(2);
  }

  scalar U = V->getn() - 2;

  for(int i=0;i<=steps;i++){
  
    scalar ratio = (scalar)((1.0f*i)/steps);
    scalar u = ratio*U;
    V->getDerivatives(u,2,dQ);


    if(drawPosition){//dQ[0]
    }

    if(drawVelocity){//dQ[1]
    }

    if(drawAcceleration){//dQ[2]
    }
  }

  delete [] dQ;
};






//#ifdef __cplusplus    //--- Uncomment to use NR in C
//  extern "C" { 
//#endif
//#define NRANSI
//#include "nr/nr.h"
//#include "nr/nrutil.h"
//#ifdef __cplusplus
//  } 
//#endif


/**
 *
 */
void derivative(scalar t,scalar * y,scalar * dydt){
  VectorN p(3);
  VectorN v(3);
  VectorN a(3);

  space->scriptedMotion(t,*velocity,p,v,a);
  dydt[1] = v.entry[0];
  dydt[2] = v.entry[1];
  dydt[3] = v.entry[2];
//  dydt[0] = y[3];
//  dydt[1] = y[4];
//  dydt[2] = y[5];
  dydt[4] = a.entry[0];
  dydt[5] = a.entry[1];
  dydt[6] = a.entry[2];
};


/**
 * NOTE: Needs an ODE solver, uncommented code added that support Numerical Recipes in C
 */
void derivTest(NUBSpline * S,NUBSpline * V,scalar t0,scalar t1,int steps){
  VectorN p(3);
  VectorN v(3);
  VectorN a(3);

  space = S;
  velocity = V;

  S->scriptedMotion(t0,*V,p,v,a);

  int nvar = 6;
  scalar * y = 0;//scalar * y = vector(1,nvar); //--- Uncomment to use NR in C

  y[1] = p.entry[0];
  y[2] = p.entry[1];
  y[3] = p.entry[2];
  y[4] = v.entry[0];
  y[5] = v.entry[1];
  y[6] = v.entry[2];

  if(drawPosition){
  }

  if(drawVelocity){
  }


  scalar deltaT = (t1-t0)/steps;

  for(int i=0;i<steps;i++){

    scalar tstart = i*deltaT+t0;
    scalar tend = tstart + deltaT;

    int nok        = 0;                   //--- On return holds the number of OK steps.
    int nbad       = 0;                   //--- On return holds the number of BAD steps.
    float eps      = 0.1f;                //--- Accuracy of integration.
    float hguess   = (tend-tstart)/steps; //--- Guessed step size.
    if(hguess>(tend-tstart))
      hguess = (tend-tstart);
    float hmin = hguess/10.0f;           //--- Minimum allowed step size.

    //odeint(y,nvar,tstart,tend,eps,hguess,hmin,&nok,&nbad,derivative,rkqs); //--- Uncomment to use NR in C

    p.entry[0] = y[1];
    p.entry[1] = y[2];
    p.entry[2] = y[3];
    
    v.entry[0] = y[4];
    v.entry[1] = y[5];
    v.entry[2] = y[6];

    if(drawPosition){
    }

    if(drawVelocity){
    }


  }

  //free_vector(y,1,nvar);   //--- Uncomment to use NR in C
};

//#undef NRANSI   //--- Uncomment to use NR in C


/**
 *
 *
 */
void test1(void){
  scalar tmax = 5.0f;
  NUBSpline * spaceSpline    = createFancySpaceSpline();
  NUBSpline * velocitySpline = createConstantVelocitySpline(tmax);
  int steps = 44;
  drawScriptedMotion(spaceSpline,velocitySpline,0,tmax,steps);
  delete spaceSpline;
  delete velocitySpline;
};

/**
 *
 */
void test2(){
  scalar tmax = 5.0f;
  NUBSpline * spaceSpline = createFancySpaceSpline();
  NUBSpline * velocitySpline = createExageratedEaseInEaseOutVelocitySpline(tmax);
  int steps = 44;
  drawScriptedMotion(spaceSpline,velocitySpline,0,tmax,steps);
  delete spaceSpline;
  delete velocitySpline;
};

/**
 *
 */
void test3(){
  scalar tmax = 5.0f;
  NUBSpline * spaceSpline    = createSimpleSpaceSpline();
  NUBSpline * velocitySpline = createConstantVelocitySpline(tmax);
  int steps = 24;
  drawScriptedMotion(spaceSpline,velocitySpline,0,tmax,steps);
  delete spaceSpline;
  delete velocitySpline;
};

/**
 *
 */
void test4(){
  scalar tmax = 5.0f;
  NUBSpline * spaceSpline    = createSimpleSpaceSpline();
  NUBSpline * velocitySpline = createExageratedEaseInEaseOutVelocitySpline(tmax);
  int steps = 24;
  drawScriptedMotion(spaceSpline,velocitySpline,0,tmax,steps);
  delete spaceSpline;
  delete velocitySpline;
};

/**
 *
 */
void test5(){
  scalar tmax = 5.0f;
  NUBSpline * spaceSpline    = createFancySpaceSpline();
  NUBSpline * velocitySpline = createConstantVelocitySpline(tmax);
  int steps = 44;
  drawScriptedMotion(spaceSpline,velocitySpline,0,tmax-0.01f,steps);
  derivTest(spaceSpline,velocitySpline,0,tmax-0.01f,steps);
  delete spaceSpline;
  delete velocitySpline;
};

/**
 *
 */
void test6(){
  scalar tmax = 5.0f;
  NUBSpline * spaceSpline    = createSimpleSpaceSpline();
  NUBSpline * velocitySpline = createConstantVelocitySpline(tmax);
  int steps = 24;
  drawScriptedMotion(spaceSpline,velocitySpline,0,tmax,steps);
  derivTest(spaceSpline,velocitySpline,0,tmax,steps);
  delete spaceSpline;
  delete velocitySpline;
};

/**
 *
 */
void test7(){
  scalar tmax = 5.0f;
  NUBSpline * spaceSpline    = createFancySpaceSpline();
  NUBSpline * velocitySpline = createExageratedEaseInEaseOutVelocitySpline(tmax);
  int steps=44;
  drawScriptedMotion(spaceSpline,velocitySpline,0,tmax-0.01f,steps);
  derivTest(spaceSpline,velocitySpline,0,tmax-0.01f,steps);
  delete spaceSpline;
  delete velocitySpline;
};

/**
 *
 */
void test8(){
  scalar tmax = 5.0f;
  NUBSpline * spaceSpline    = createSimpleSpaceSpline();
  NUBSpline * velocitySpline = createExageratedEaseInEaseOutVelocitySpline(tmax);
  int steps=24;
  drawScriptedMotion(spaceSpline,velocitySpline,0,tmax-0.01f,steps);
  derivTest(spaceSpline,velocitySpline,0,tmax-0.01f,steps);
  delete spaceSpline;
  delete velocitySpline;
};

/**
 * Main Application Entry Point.
 *
 * This small demonstration application shows
 * how an end user should interact with the
 * QAD2003 simulator.
 *
 */
int main(){

  drawPosition = true;
  drawVelocity = true;
  drawAcceleration = true;

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  
  return 0;
};
