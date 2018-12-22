/*********************************************************************
 * UI.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Defines a generic interface from GLUT into a framework program.
 *
 * Copyright 2003, Regents of the University of California 
 *
 *********************************************************************/

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


/****************************
 * Sorry for this unnecessary complicated layer between the main
 * program and the simulation.  It abstracts the top layer interface
 * (whether it's GLUT or PS2 stuff).  It also used to have several
 * menu modes, in its PS2 incarnation.  Now it just has one, and
 * everything looks really complicated because of it.
 ***************************/


#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include "UI.h"
#include "env.h"
#include "Simulation.h"
#include "SimAnim.h"

#define mStartTimer(x)
#define mStopTimer(x)



void draw_cursor(int mouse_x, int mouse_y)
{
  glBegin(GL_POINTS);
  glVertex3f(mouse_x, mouse_y, 0);

  glVertex3f(mouse_x+2, mouse_y, 0);
  glVertex3f(mouse_x+4, mouse_y, 0);
  glVertex3f(mouse_x-2, mouse_y, 0);
  glVertex3f(mouse_x-4, mouse_y, 0);

  glVertex3f(mouse_x, mouse_y+2, 0);
  glVertex3f(mouse_x, mouse_y+4, 0);
  glVertex3f(mouse_x, mouse_y-2, 0);
  glVertex3f(mouse_x, mouse_y-4, 0);
  glEnd();
}



void set_light_positions();
void DrawPoint(const vec3_t v, float_t size = 1.0);



//the abstract base class for all menu types
struct BaseMenu
{
  virtual const char* Name () const = 0;
  virtual void Init() {}
  virtual void Reset() {}
  virtual void Activate() {}
  virtual void Deactivate() {}
  virtual void MoveTo(float x, float y) {}
  virtual void Drag(int button, float x, float y) {}
  virtual void Rotate(float dx, float dy) {
    app->viewport.yaw += dx;
    app->viewport.pitch += dy;
  }
  virtual void Zoom(float dz) {
    if(app->viewport.view == VIEW_PERSPECTIVE)
      app->viewport.push(-dz);
    else
      app->viewport.zoom(-dz);

    if(app->viewport.dist < 100.0)
      app->viewport.dist = 100.0;
  }
  virtual void Click(int button, int state) {}
  virtual void Nav(bool up, bool down, bool left, bool right) {}
  virtual void NextTool() {}
  virtual void PrevTool() {}
  virtual void Update() {}
  virtual void Draw() {
    app->viewport.set_mode();
    set_light_positions();
  }
  virtual void ObjectTransform(matrix4_t m) {
    matrix4_identity(m);
  }
  virtual void Key(unsigned char c) {
  }

  DeformMain* app;
};



CSimulation dyn_sim;
double gTime_Step = 1.0/60;
int movie_save_steps = 0;

#define EPSILON 0.001



/*******************DRAWING**********************/

void drawPlane(const vec3_t n, float_t ofs, float_t divsize = 200.0, int ndivs = 10 )
{
  vec3_t orig, u, v;
  vec3_multiply(orig, n, ofs);
  if(fabs(fabs(n[2])-1.0) < EPSILON) {
    vec3_make(u, 1,0,0);
    vec3_make(v, 0,1,0);
  }
  else {
    vec3_t up = {0,0,1};
    vec3_cross(u,up,n);
    vec3_normalize(u);
    vec3_cross(v,u,n);
  }
  vec3_multiply(u, u, divsize);
  vec3_multiply(v, v, divsize);
  vec3_madd(orig, u, -ndivs);
  vec3_madd(orig, v, -ndivs);

  glNormal3f(n[0], n[1], n[2]);

  for(int i=-ndivs; i<ndivs; i++) {
    vec3_t old_orig;
    vec3_equal(old_orig, orig);
    glBegin(GL_TRIANGLE_STRIP);
    for(int j=-ndivs; j<ndivs; j++) {
      glVertex3f(orig[0], orig[1], orig[2]);
      glVertex3f(orig[0]+u[0], orig[1]+u[1], orig[2]+u[2]);
      vec3_add(orig, orig, v);
    }
    glVertex3f(orig[0], orig[1], orig[2]);
    glVertex3f(orig[0]+u[0], orig[1]+u[1], orig[2]+u[2]);
    glEnd();
    vec3_equal(orig, old_orig);
    vec3_add(orig, orig, u);
  }
}

void drawPlane(const Plane& p, float_t divsize = 200.0, int ndivs = 10) {
  drawPlane(p.normal,-p.offset,divsize,ndivs);
}

void drawPenetration(const CConstraint& p, float_t scale = 5.0) {
  Vector3 center;
  p.a->GetBodyPointPosition_World(p.p1, center);
  if(draw_penetration) {
    vec3_t end;
    p.fn.normal.get(end);
    vec3_multiply(end, end, scale*p.pen);
    glBegin(GL_LINES);
    glVertex3f(center[0], center[1], center[2]);
    glVertex3f(center[0]+end[0], center[1]+end[1], center[2]+end[2]);
    glEnd();
  }
  glPointSize(5);
  glBegin(GL_POINTS);
  glVertex3f(p.p[0], p.p[1], p.p[2]);
  glEnd();
}

void drawConstraint(const CConstraint& p) {
  Vector3 c;
  glBegin(GL_POINTS);
  p.a->GetBodyPointPosition_World(p.p1, c);
  glVertex3f(c.x, c.y, c.z);
  if(p.b) {
    p.b->GetBodyPointPosition_World(p.p2, c);
    glVertex3f(c.x, c.y, c.z);
  }
  glEnd();
}

void drawCoordinates(const Vector3& p, const Matrix3& r)
{
  glBegin(GL_LINES);
  //x axis
  glColor3f(1, 0, 0);
  glVertex3d(p[0], p[1], p[2]);
  glVertex3d(p[0] + r(0,0), p[1]+r(1,0), p[2]+r(2,0));
  //y axis
  glColor3f(0, 1, 0);
  glVertex3d(p[0], p[1], p[2]);
  glVertex3d(p[0] + r(0,1), p[1]+r(1,1), p[2]+r(2,1));
  //z axis
  glColor3f(0, 0, 1);
  glVertex3d(p[0], p[1], p[2]);
  glVertex3d(p[0] + r(0,2), p[1]+r(1,2), p[2]+r(2,2));
  glEnd();
}

void drawRay(const Vector3& p, const Vector3& v, double len = 1.0)
{
  glBegin(GL_LINES);
  glVertex3d(p[0], p[1], p[2]);
  glVertex3d(p[0]+v[0]*len, p[1]+v[1]*len, p[2]+v[2]*len);
  glEnd();
}



void drawFrame(const CRigidState& state)
{
  Matrix3 rScaled;
  rScaled.mul(state.rotMat,10.0);
  drawCoordinates(state.trnVec, rScaled);
  
  //velocity: orange
  glColor3f(1, 0.5, 0);
  drawRay(state.trnVec, state.tVel);

  //accel: yellow
  glColor3f(1, 1, 0);
  drawRay(state.trnVec, state.rotMat*state.netFrc);

  //avelocity: cyan
  glColor3f(0, 1, 1);
  drawRay(state.trnVec, state.aVel*100.0);

  //accel: purple
  glColor3f(1, 0, 1);
  drawRay(state.trnVec, state.rotMat*state.netMom*100.0);
}



void drawAnimation(const CRigidBodyAnimation& a, int num_path_frames = 40) {
  glDisable(GL_LIGHTING);
  glColor3f(1, 0, 0);
  //translation keys: red dots
  glBegin(GL_POINTS);
  for(int i=0; i<a.position_track.num_keys; i++) {
    glVertex3d(a.position_track.keys[i][0],a.position_track.keys[i][1],a.position_track.keys[i][2]);
  }
  glEnd();

  //rotation keys: axes
  for(int i=0; i<a.rotation_track.num_keys; i++) {
    AnimationPosition p;
    a.position_track.Interpolate(a.rotation_track.times[i], p);
    Matrix3 r;
    a.rotation_track.keys[i].getMatrix(r);
    r.inplaceScale(20);

    drawCoordinates(p, r);
  }

  //path: white
  double tbegin, tend;
  tbegin = min(a.rotation_track.times[0], a.position_track.times[0]);
  tend = max(a.rotation_track.times[a.rotation_track.num_keys-1],
	     a.position_track.times[a.position_track.num_keys-1]);
  double dt = (tend - tbegin)/num_path_frames;
  
  glColor3f(1,1,1);
  glLineWidth(2.0);
  glBegin(GL_LINE_STRIP);
  //glBegin(GL_POINTS);
  for(double t=tbegin; t<=tend; t+=dt) {
    AnimationPosition p;
    a.position_track.Interpolate(t, p);
    glVertex3d(p[0], p[1], p[2]);
  }
  glEnd();
  
  /*
  glLineWidth(1.0);
  for(double t=tbegin; t<=tend; t+=dt) {
    CRigidState f;
    a.GetState(t, f);
    drawFrame(f);
  }
  */


  glEnable(GL_LIGHTING);
}

void drawSim(const CSimulation& sim) {
  for(int i=0; i<sim.numBodies; i++) {
    sim.bodies[i]->geometry->Draw();

    if(draw_animation) {
      //animations
      if(sim.bodies[i]->animationPath) {
	drawAnimation(*sim.bodies[i]->animationPath);
      }
    }
  }

  glPointSize(5.0);
  glEnable(GL_POINT_SMOOTH);

    float color [4] = { 1.0, 1.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);

    glDisable(GL_CULL_FACE);

    for(int i=0; i<sim.numPlanes; i++)
      drawPlane(sim.planes[i]);

    glEnable(GL_CULL_FACE);

    float ccolor [4] = { 0.0, 0.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ccolor);

    //penetrations
    if(draw_constraints) {
      CConstraint* p = NULL;
      while((p = sim.constraints.next(p))) {
	if(p->IsPenetration()) {
	  drawPenetration(*p);
	}
	else {
	  drawConstraint(*p);
	}
      }
    }
}





#include <signal.h>
void simulation_exit(int sig) {
  printf("\n******EXITING******\n");
  printf("%f seconds\n", dyn_sim.sim_time);
  printf("%d total forward steps.\n", dyn_sim.sim_steps);
  printf("%d constrained steps.\n", dyn_sim.constraint_steps);
  printf("%d constraint solves.\n", dyn_sim.constraint_loops);
  printf("%d rollback steps.\n", dyn_sim.rollback_steps);

/*  if(sim_logging) {
    sim_log.LogEnd();
    sim_log.Save(sim_log_file);
    printf("Saving log to %s\n", sim_log_file); 
  }*/

  if(sig == SIGINT) {
    printf("Interrupt.\n");
    exit(0);
  }
  if(sig == SIGABRT) {
    printf("Aborted.\n");
    exit(-1);
  }
}



struct SimulationMenu : public BaseMenu
{
  const char* Name () const { return "Simulation"; }
  void Init() {
    signal(SIGINT, simulation_exit);
    signal(SIGABRT, simulation_exit);
    dyn_setup();
    simulate = false;
    cur_step = 0;
  }

  void Reset() {
    dyn_reset();
    simulate = false;
  }

  void Activate() {
    app->viewport.pitch = -60;
    app->viewport.f = 10000;
    simulate = false;
  }

  void Deactivate() {
    dyn_reset();
  }

  void GetWorldRay(Ray& r) {
      vec3_t wsource,wdirection;
      app->viewport.get_click_source(click_x,click_y,wsource);
      app->viewport.get_click_vector(click_x,click_y,wdirection);
      vec3_equal(r.source, wsource);
      vec3_equal(r.direction, wdirection);
  }

  /*void GetLocalRay(Ray& r) {
      vec3_t wsource,wdirection;
      app->viewport.get_click_source(click_x,click_y,wsource);
      app->viewport.get_click_vector(click_x,click_y,wdirection);
      dyn_obj.Position_WorldToLocal(wsource, r.source);
      dyn_obj.Vector_WorldToLocal(wdirection, r.direction);
  }*/

  void MoveTo(float x, float y) {
	  click_x = (int)x;
	  click_y = (int)y;
  }

  void Drag(int button, float dx, float dy) {
    click_x += (int)dx;
    click_y += (int)dy;
    if(button == 0)
    {
      vec3_t ofs;
      app->viewport.get_movement_vector(dx,dy, ofs);
      vec3_multiply(ofs, ofs, app->viewport.scale);
      vec3_sub(app->viewport.position, app->viewport.position, ofs);
    }
    if(button == 1) {
      Zoom(dy);
    }
    if(button == 2)
    {
      Rotate(dx, dy);
    }
  }

  void Click(int button, int state) {
  }
  void Update() {
    if(simulate) {
/*      if(sim_playback)
	sim_log.PlaybackAdvance(dyn_sim, dyn_sim.sim_time + gTime_Step);
      else*/
      dyn_sim.simulate(gTime_Step);

      cur_step++;
    }
  }
  void Draw() {
    app->viewport.set_mode();

    set_light_positions();

    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    drawSim(dyn_sim);
  }

  void Key(unsigned char c) {
    switch(c) {
    case ' ':
      simulate = !simulate; //(state == 1);
      break;
    case 'r':
      dyn_reset();
      break;
    }
  }

  void Read_Setup_File(const char *project)
  {
    //1) read external stuff

    ifstream fsetup;
    fsetup.open(project);
    if (!fsetup) {
      cout << "Can't open file " << project << endl;
      exit(1);
    }

    int frame;
    fsetup >> frame;
    gTime_Step = 1.0/(double)frame;


    //2) pass to CSimulation
    dyn_sim.readSetupFile(fsetup);

    fsetup.close();
  }

  void dyn_setup()
  {
/*    if(sim_playback) {
      sim_log.Load(sim_log_file);
      sim_log.PlaybackInitialize(dyn_sim);
    }
    else*/
      Read_Setup_File(scene_file);
  }
  
  void dyn_reset()
  {
/*    if(sim_playback)
      sim_log.PlaybackRewind(dyn_sim);*/
  }


  int click_x, click_y;
  bool simulate;
  int cur_step;
};


#define NUM_MENUS 1
BaseMenu* menus [NUM_MENUS];

DeformMain::DeformMain()
{
  //  menus[0] = new SettingsMenu;
  menus[0] = new SimulationMenu;

  for(int i=0; i<NUM_MENUS; i++) {
    menus[i]->app = this;
    menus[i]->Init();
  }
  i_current_menu = 0;
  current_menu = menus[0];

  InitViewports();
  
  //light_system = CreateDefaultLightSystem();
  //SetActiveLights(light_system);

  //tsLoadFont();
}

DeformMain::~DeformMain()
{
  //CleanupLightSystem(light_system);
  //objects.clear();
}

void DeformMain::Reset()
{
  current_menu->Reset();
}




void DeformMain::Click(int button, int state)
{
  current_menu->Click(button, state);
}



void DeformMain::Drag(int button, float dx, float dy)
{ 
  current_menu->Drag(button, dx,dy);
}

void DeformMain::MoveTo(float dx, float dy)
{ 
  current_menu->MoveTo(dx,dy);
}


void DeformMain::Rotate(float dx, float dy)
{
  current_menu->Rotate(dx,dy);
}

void DeformMain::Zoom(float dz)
{
  current_menu->Zoom(dz);
}




float_t mesh_diameter(const surface_mesh& m)
{
  float_t bb [6];
  m.get_aabb(bb);
  float_t dmax = 0.0;
  dmax = max(max(bb[3]-bb[0], bb[4]-bb[1]), bb[5]-bb[2]);
  return dmax;
}

void AutoScaleViewport(editor_viewport* v, float_t diameter)
{
	float_t vmin = (float_t)min(v->w, v->h);

	if(v->view == VIEW_PERSPECTIVE) {
	  v->scale = vmin / diameter * 0.8f;
	  v->dist = 600;
	}
	else {
	  v->scale = vmin / diameter * 0.8f;
	}
}






void DeformMain::InitViewports()
{
  //viewport.set_view(VIEW_FRONT);
  viewport.set_view(VIEW_PERSPECTIVE);

  viewport.x = 0;
  viewport.y = 0;
  viewport.w = 640; //SCREEN_W;
  viewport.h = 480; //SCREEN_H;

  viewport.dist = 500;
  viewport.scale = 1.0;
}



void DeformMain::Draw()
{
  //printf("drawing mesh... \n");
  
  mStartTimer("Draw");

  current_menu->Draw();

/*
  tsResetMargins();
  tsResetCursor();
  
  mStartTimer("UI Draw");
  for(int i=0; i<NUM_MENUS; i++) {
    if(current_menu == menus[i])
      tsDrawString("> ");
    else
      tsDrawString(". ");
    tsDrawString(menus[i]->Name());
    tsDrawString(".\n");
  }
  mStopTimer("UI Draw");
   */
  mStopTimer("Draw");
}

void DeformMain::Nav(bool up, bool down, bool left, bool right)
{
	current_menu->Nav(up, down, left, right);
}

void DeformMain::NextTool()
{
  current_menu->NextTool();
}

void DeformMain::PrevTool()
{
  current_menu->PrevTool();
}

void DeformMain::NextMenu()
{ 
  current_menu->Deactivate();
  i_current_menu = (i_current_menu+1)%(NUM_MENUS);
  current_menu = menus[i_current_menu];
  current_menu->Activate();
}
void DeformMain::PrevMenu()
{
  current_menu->Deactivate();
  i_current_menu = (i_current_menu+NUM_MENUS-1)%(NUM_MENUS);
  current_menu = menus[i_current_menu];
  current_menu->Activate();
}



int iter = 0;

void DeformMain::Update() 
{
  current_menu->Update();
}

void DeformMain::Key(unsigned char c)
{
  current_menu->Key(c);
}





