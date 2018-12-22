
/*

This demonstrates the constraints from the GameGems chapter.

Set the JOINT_TYPE define (below) to one of the following values, to select
the joint that will be demonstrated:

	C1: Constraint #1 - No Movement Along Z (POR in X-Y plane)
	C2: Constraint #2 - No Rotation About Z
	C3: Constraint #3 - POR in an Arbitrary Plane
	C4: Constraint #4 - No Rotation About an Arbitrary Axis
	C5: Constraint #5 - POR Constrained to a Line
	C6: Constraint #6 - Rotation About a Fixed Axis
	C7a: Constraint #7a - Fixed POR
	C7b: Constraint #7b - Fixed Rotation
	RopeAndPulley: Rope and Pulley Constraint
	Screw: Screw Constraint

The constrained body will have an oscillating force and torque applied to
it, so that you can see which degrees of freedom are being taken away.

The other joints described in the GameGems chapter (ball-and-socket,
hinge, hard contact, motor, brake) are all part of the ODE core. Look at
the file ode/src/joint.cpp to see how they are implemented.

*/

#define JOINT_TYPE Screw

//****************************************************************************

#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include "../src/joint.h"

#ifdef MSVC
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

//****************************************************************************
// some joint type numbers for the joints defined here

enum {
  dJointTypeC1 = 100,
  dJointTypeC2,
  dJointTypeC3,
  dJointTypeC4,
  dJointTypeC5,
  dJointTypeC6,
  dJointTypeC7a,
  dJointTypeC7b,
  dJointTypeRopeAndPulley,
  dJointTypeScrew
};

//****************************************************************************
// joint utility stuff

#define CAT1(x,y) x ## y
#define CAT2(x,y) CAT1(x,y)
#define MAKE_JOINT CAT2(dJointCreate,JOINT_TYPE)
#define JOINT_TYPE_NUM CAT2(dJointType,JOINT_TYPE)


extern void dJointInit (dxWorld *w, dxJoint *j);

static dxJoint *createJoint (dWorldID w, dxJoint::Vtable *vtable)
{
  dxJoint *j = (dxJoint*) dAlloc (vtable->size);
  dJointInit (w,j);
  j->vtable = vtable;
  if (vtable->init) vtable->init (j);
  j->feedback = 0;
  return j;
}

//****************************************************************************
// Constraint #1: No Movement Along Z (POR in X-Y plane)

struct dxJointC1 : public dxJoint {
  dReal zpos;				// position for body Z
};


static void c1Init (dxJointC1 *j)
{
  j->zpos = 1;
}


static void c1GetInfo1 (dxJointC1 *j, dxJoint::Info1 *info)
{
  info->m = 1;
  info->nub = 1;
}


static void c1GetInfo2 (dxJointC1 *joint, dxJoint::Info2 *info)
{
  if (joint->node[1].body) dDebug (0,"C1 joint does not support second body");

  // set jacobian
  info->J1l[2] = 1;

  // set right hand side
  dReal k = info->fps * info->erp;
  info->c[0] = k * (joint->zpos - joint->node[0].body->pos[2]);
}

  
dxJoint * dJointCreateC1 (dWorldID w)
{
  static dxJoint::Vtable __my_vtable = {
    sizeof(dxJointC1),
    (dxJoint::init_fn*) c1Init,
    (dxJoint::getInfo1_fn*) c1GetInfo1,
    (dxJoint::getInfo2_fn*) c1GetInfo2,
    dJointTypeC1};
  dAASSERT (w);
  return createJoint (w,&__my_vtable);
}

//****************************************************************************
// Constraint #2: No Rotation About Z

struct dxJointC2 : public dxJoint {
};


static void c2Init (dxJointC2 *j)
{
}


static void c2GetInfo1 (dxJointC2 *j, dxJoint::Info1 *info)
{
  info->m = 1;
  info->nub = 1;
}


static void c2GetInfo2 (dxJointC2 *joint, dxJoint::Info2 *info)
{
  if (joint->node[1].body) dDebug (0,"C2 joint does not support second body");

  // set jacobian
  info->J1a[2] = 1;
}

  
dxJoint * dJointCreateC2 (dWorldID w)
{
  static dxJoint::Vtable __my_vtable = {
    sizeof(dxJointC2),
    (dxJoint::init_fn*) c2Init,
    (dxJoint::getInfo1_fn*) c2GetInfo1,
    (dxJoint::getInfo2_fn*) c2GetInfo2,
    dJointTypeC2};
  dAASSERT (w);
  return createJoint (w,&__my_vtable);
}

//****************************************************************************
// Constraint #3: POR in an Arbitrary Plane

struct dxJointC3 : public dxJoint {
  dVector3 a;				// normal to plane
  dReal pos;				// POR position along 'a'
};


static void c3Init (dxJointC3 *j)
{
  j->a[0] = 1;
  j->a[1] = -1;
  j->a[2] = 0;
  j->pos = 0;
}


static void c3GetInfo1 (dxJointC3 *j, dxJoint::Info1 *info)
{
  info->m = 1;
  info->nub = 1;
}


static void c3GetInfo2 (dxJointC3 *joint, dxJoint::Info2 *info)
{
  if (joint->node[1].body) dDebug (0,"C3 joint does not support second body");

  // set jacobian
  info->J1l[0] = joint->a[0];
  info->J1l[1] = joint->a[1];
  info->J1l[2] = joint->a[2];

  // set right hand side
  dReal k = info->fps * info->erp;
  info->c[0] = k * (joint->pos - dDOT(joint->node[0].body->pos,joint->a));
}

  
dxJoint * dJointCreateC3 (dWorldID w)
{
  static dxJoint::Vtable __my_vtable = {
    sizeof(dxJointC3),
    (dxJoint::init_fn*) c3Init,
    (dxJoint::getInfo1_fn*) c3GetInfo1,
    (dxJoint::getInfo2_fn*) c3GetInfo2,
    dJointTypeC3};
  dAASSERT (w);
  return createJoint (w,&__my_vtable);
}

//****************************************************************************
// Constraint #4: No Rotation About an Arbitrary Axis

struct dxJointC4 : public dxJoint {
  dVector3 a;				// axis
};


static void c4Init (dxJointC4 *j)
{
  j->a[0] = 1;
  j->a[1] = -1;
  j->a[2] = 0;
}


static void c4GetInfo1 (dxJointC4 *j, dxJoint::Info1 *info)
{
  info->m = 1;
  info->nub = 1;
}


static void c4GetInfo2 (dxJointC4 *joint, dxJoint::Info2 *info)
{
  if (joint->node[1].body) dDebug (0,"C4 joint does not support second body");

  // set jacobian
  info->J1a[0] = joint->a[0];
  info->J1a[1] = joint->a[1];
  info->J1a[2] = joint->a[2];
}

  
dxJoint * dJointCreateC4 (dWorldID w)
{
  static dxJoint::Vtable __my_vtable = {
    sizeof(dxJointC4),
    (dxJoint::init_fn*) c4Init,
    (dxJoint::getInfo1_fn*) c4GetInfo1,
    (dxJoint::getInfo2_fn*) c4GetInfo2,
    dJointTypeC4};
  dAASSERT (w);
  return createJoint (w,&__my_vtable);
}

//****************************************************************************
// Constraint #5: POR Constrained to a Line

struct dxJointC5 : public dxJoint {
  // line of free movement is (a x b)
  dVector3 a;
  dVector3 b;
  dReal posa;			// POR position along 'a'
  dReal posb;			// POR position along 'b'
};


static void c5Init (dxJointC5 *j)
{
  j->a[0] = 1;
  j->a[1] = 0;
  j->a[2] = 0;
  j->b[0] = 0;
  j->b[1] = 1;
  j->b[2] = 0;
  j->posa = 0;
  j->posb = 0;
}


static void c5GetInfo1 (dxJointC5 *j, dxJoint::Info1 *info)
{
  info->m = 2;
  info->nub = 2;
}


static void c5GetInfo2 (dxJointC5 *joint, dxJoint::Info2 *info)
{
  if (joint->node[1].body) dDebug (0,"C5 joint does not support second body");
  int s = info->rowskip;

  // set jacobian
  info->J1l[0] = joint->a[0];
  info->J1l[1] = joint->a[1];
  info->J1l[2] = joint->a[2];
  info->J1l[s+0] = joint->b[0];
  info->J1l[s+1] = joint->b[1];
  info->J1l[s+2] = joint->b[2];

  // set right hand side
  dReal k = info->fps * info->erp;
  info->c[0] = k * (joint->posa - dDOT(joint->node[0].body->pos,joint->a));
  info->c[1] = k * (joint->posb - dDOT(joint->node[0].body->pos,joint->b));
}

  
dxJoint * dJointCreateC5 (dWorldID w)
{
  static dxJoint::Vtable __my_vtable = {
    sizeof(dxJointC5),
    (dxJoint::init_fn*) c5Init,
    (dxJoint::getInfo1_fn*) c5GetInfo1,
    (dxJoint::getInfo2_fn*) c5GetInfo2,
    dJointTypeC5};
  dAASSERT (w);
  return createJoint (w,&__my_vtable);
}

//****************************************************************************
// Constraint #6: Rotation About a Fixed Axis

struct dxJointC6 : public dxJoint {
  // free axis is (a x b)
  dVector3 a;
  dVector3 b;
};


static void c6Init (dxJointC6 *j)
{
  j->a[0] = 1;
  j->a[1] = 0;
  j->a[2] = 0;
  j->b[0] = 0;
  j->b[1] = 1;
  j->b[2] = 0;
}


static void c6GetInfo1 (dxJointC6 *j, dxJoint::Info1 *info)
{
  info->m = 2;
  info->nub = 2;
}


static void c6GetInfo2 (dxJointC6 *joint, dxJoint::Info2 *info)
{
  if (joint->node[1].body) dDebug (0,"C6 joint does not support second body");
  int s = info->rowskip;

  // set jacobian
  info->J1a[0] = joint->a[0];
  info->J1a[1] = joint->a[1];
  info->J1a[2] = joint->a[2];
  info->J1a[s+0] = joint->b[0];
  info->J1a[s+1] = joint->b[1];
  info->J1a[s+2] = joint->b[2];
}

  
dxJoint * dJointCreateC6 (dWorldID w)
{
  static dxJoint::Vtable __my_vtable = {
    sizeof(dxJointC6),
    (dxJoint::init_fn*) c6Init,
    (dxJoint::getInfo1_fn*) c6GetInfo1,
    (dxJoint::getInfo2_fn*) c6GetInfo2,
    dJointTypeC6};
  dAASSERT (w);
  return createJoint (w,&__my_vtable);
}

//****************************************************************************
// Constraint #7a: Fixed POR

struct dxJointC7a : public dxJoint {
  dVector3 pos;			// position of POR
};


static void c7aInit (dxJointC7a *j)
{
  j->pos[0] = 0;
  j->pos[1] = 0;
  j->pos[2] = 1;
}


static void c7aGetInfo1 (dxJointC7a *j, dxJoint::Info1 *info)
{
  info->m = 3;
  info->nub = 3;
}


static void c7aGetInfo2 (dxJointC7a *joint, dxJoint::Info2 *info)
{
  if (joint->node[1].body) dDebug (0,"C7a joint does not support second body");
  int s = info->rowskip;

  // set jacobian
  info->J1l[0] = 1;
  info->J1l[s+1] = 1;
  info->J1l[2*s+2] = 1;

  // set right hand side
  dReal k = info->fps * info->erp;
  info->c[0] = k * (joint->pos[0] - joint->node[0].body->pos[0]);
  info->c[0] = k * (joint->pos[1] - joint->node[0].body->pos[1]);
  info->c[0] = k * (joint->pos[2] - joint->node[0].body->pos[2]);
}

  
dxJoint * dJointCreateC7a (dWorldID w)
{
  static dxJoint::Vtable __my_vtable = {
    sizeof(dxJointC7a),
    (dxJoint::init_fn*) c7aInit,
    (dxJoint::getInfo1_fn*) c7aGetInfo1,
    (dxJoint::getInfo2_fn*) c7aGetInfo2,
    dJointTypeC7a};
  dAASSERT (w);
  return createJoint (w,&__my_vtable);
}

//****************************************************************************
// Constraint #7b: Fixed Rotation

struct dxJointC7b : public dxJoint {
};


static void c7bInit (dxJointC7b *j)
{
}


static void c7bGetInfo1 (dxJointC7b *j, dxJoint::Info1 *info)
{
  info->m = 3;
  info->nub = 3;
}


static void c7bGetInfo2 (dxJointC7b *joint, dxJoint::Info2 *info)
{
  if (joint->node[1].body) dDebug (0,"C7b joint does not support second body");
  int s = info->rowskip;

  // set jacobian
  info->J1a[0] = 1;
  info->J1a[s+1] = 1;
  info->J1a[2*s+2] = 1;
}

  
dxJoint * dJointCreateC7b (dWorldID w)
{
  static dxJoint::Vtable __my_vtable = {
    sizeof(dxJointC7b),
    (dxJoint::init_fn*) c7bInit,
    (dxJoint::getInfo1_fn*) c7bGetInfo1,
    (dxJoint::getInfo2_fn*) c7bGetInfo2,
    dJointTypeC7b};
  dAASSERT (w);
  return createJoint (w,&__my_vtable);
}

//****************************************************************************
// Rope and Pulley Constraint

struct dxJointRopeAndPulley : public dxJoint {
  dVector3 a;
  dVector3 b;
};


static void ropeAndPulleyInit (dxJointRopeAndPulley *j)
{
  j->a[0] = 0;
  j->a[1] = 0;
  j->a[2] = 1;
  j->b[0] = 0;
  j->b[1] = 0;
  j->b[2] = 1;
}


static void ropeAndPulleyGetInfo1 (dxJointRopeAndPulley *j, dxJoint::Info1 *info)
{
  info->m = 1;
  info->nub = 1;
}


static void ropeAndPulleyGetInfo2 (dxJointRopeAndPulley *joint, dxJoint::Info2 *info)
{
  if (!joint->node[1].body) dDebug (0,"RopeAndPulley joint needs second body");

  // set jacobian
  info->J1l[0] = joint->a[0];
  info->J1l[1] = joint->a[1];
  info->J1l[2] = joint->a[2];
  info->J2l[0] = joint->b[0];
  info->J2l[1] = joint->b[1];
  info->J2l[2] = joint->b[2];
}

  
dxJoint * dJointCreateRopeAndPulley (dWorldID w)
{
  static dxJoint::Vtable __my_vtable = {
    sizeof(dxJointRopeAndPulley),
    (dxJoint::init_fn*) ropeAndPulleyInit,
    (dxJoint::getInfo1_fn*) ropeAndPulleyGetInfo1,
    (dxJoint::getInfo2_fn*) ropeAndPulleyGetInfo2,
    dJointTypeRopeAndPulley};
  dAASSERT (w);
  return createJoint (w,&__my_vtable);
}

//****************************************************************************
// Screw Constraint

struct dxJointScrew : public dxJoint {
  dVector3 a;
  dVector3 q;
};


static void screwInit (dxJointScrew *j)
{
  j->a[0] = 0;
  j->a[1] = 0;
  j->a[2] = 4;
  j->q[0] = 0;
  j->q[1] = 0;
  j->q[2] = 1;
}


static void screwGetInfo1 (dxJointScrew *j, dxJoint::Info1 *info)
{
  info->m = 1;
  info->nub = 1;
}


static void screwGetInfo2 (dxJointScrew *joint, dxJoint::Info2 *info)
{
  if (joint->node[1].body) dDebug (0,"Screw joint does not support second body");

  // set jacobian
  info->J1l[0] = joint->a[0];
  info->J1l[1] = joint->a[1];
  info->J1l[2] = joint->a[2];
  info->J1a[0] = joint->q[0];
  info->J1a[1] = joint->q[1];
  info->J1a[2] = joint->q[2];
}

  
dxJoint * dJointCreateScrew (dWorldID w)
{
  static dxJoint::Vtable __my_vtable = {
    sizeof(dxJointScrew),
    (dxJoint::init_fn*) screwInit,
    (dxJoint::getInfo1_fn*) screwGetInfo1,
    (dxJoint::getInfo2_fn*) screwGetInfo2,
    dJointTypeScrew};
  dAASSERT (w);
  return createJoint (w,&__my_vtable);
}

//****************************************************************************
// simulation support stuff

// select correct drawing functions
#ifdef dDOUBLE
#define dsDrawBox dsDrawBoxD
#endif

// some constants
#define SIDE (0.5f)	// side length of a box - don't change this
#define MASS (1.0)	// mass of a box
#define STEPSIZE 0.05

// global dynamics objects
static dWorldID world;
static dBodyID body[2];
static dJointID joint;


// start simulation and set the viewpoint

static void start()
{
  static float xyz[3] = {1.0382f,-1.0811f,1.4700f};
  static float hpr[3] = {135.0000f,-19.5000f,0.0000f};
  dsSetViewpoint (xyz,hpr);
}


// simulation loop

static void simLoop (int pause)
{
  const dReal kld = 0.1;		// linear damping coefficient
  const dReal kad = 0.01;		// angular damping coefficient
  
  if (!pause) {
    // apply damped oscillating forces and torques to both bodies
    static dReal a = 0;
    a += 0.01;
    if (a > 12*M_PI) a -= 12*M_PI;

    const dReal *v = dBodyGetLinearVel (body[0]);
    const dReal *w = dBodyGetAngularVel (body[0]);
    dReal fx = 0.01*sin (1.0*a + M_PI/2) - kld*v[0];
    dReal fy = 0.01*sin (1.5*a + M_PI/2) - kld*v[1];
    dReal fz = 0.01*sin (2.0*a + M_PI/2) - kld*v[2];
    dReal tx = 0.002*sin (1.5*a + M_PI/2) - kad*w[0];
    dReal ty = 0.002*sin (2.0*a + M_PI/2) - kad*w[1];
    dReal tz = 0.002*sin (2.5*a + M_PI/2) - kad*w[2];

    if (JOINT_TYPE_NUM==dJointTypeRopeAndPulley || JOINT_TYPE_NUM==dJointTypeScrew) {
      dBodyAddForce (body[0],0,0,5*fz);
    }
    else {
      dBodyAddForce (body[0],fx,fy,fz);
      dBodyAddTorque (body[0],tx,ty,tz);
    }
    
    // take a step
    dWorldStep (world,STEPSIZE);
  }

  dReal sides[3] = {SIDE,SIDE,SIDE};
  dsSetTexture (DS_WOOD);
  dsSetColor (1,1,0);
  dsDrawBox (dBodyGetPosition(body[0]),dBodyGetRotation(body[0]),sides);
  if (body[1]) {
    dsSetColor (0,1,1);
    dsDrawBox (dBodyGetPosition(body[1]),dBodyGetRotation(body[1]),sides);
  }
}

//****************************************************************************
// main

int main (int argc, char **argv)
{
  // setup pointers to drawstuff callback functions
  dsFunctions fn;
  fn.version = DS_VERSION;
  fn.start = &start;
  fn.step = &simLoop;
  fn.command = 0;
  fn.stop = 0;
  fn.path_to_textures = "../../drawstuff/textures";

  // create world
  world = dWorldCreate();

  // create one or more bodies
  dMass m;
  dMassSetBox (&m,1,SIDE,SIDE,SIDE);
  dMassAdjust (&m,MASS);
  body[0] = dBodyCreate (world);
  body[1] = 0;
  dBodySetMass (body[0],&m);
  dBodySetPosition (body[0], 0,0,1);
  if (JOINT_TYPE_NUM==dJointTypeRopeAndPulley) {
    body[1] = dBodyCreate (world);
    dBodySetMass (body[1],&m);
    dBodySetPosition (body[1], 0.5,0.5,1);
  }

  // create the joint
  joint = MAKE_JOINT (world);
  if (JOINT_TYPE_NUM==dJointTypeRopeAndPulley) {
    dJointAttach (joint,body[0],body[1]);
  }
  else {
    dJointAttach (joint,body[0],0);
  }

  // run simulation
  dsSimulationLoop (argc,argv,640,480,&fn);

  // done
  dWorldDestroy (world);

  return 0;
}
