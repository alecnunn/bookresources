/* Copyright (C) William Leeson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2001"
 */
/*
 *   GLUT based user interface
 */
#include "main.h"
#include "HalfEdgeMesh.h"
#include "SubdivisionMesh.h"
#include "LoopMesh.h"
#include "ButterflyMesh.h"
#include "Joint.h"

#define SEGMENTS 8

/*
 * Horrible globals - quick and dirty
 */
GLScene *gp_scene = NULL; // OpenGL renderer
real g_time = 0,g_rate = 0.001,g_dd[] = { 0,0,0 };
real g_fovy = 45,g_dist = -10;
bool g_envmap = false,g_texture = true,g_object = true;
integer g_level = 1,g_max_level;
integer g_faces[] = { 0,1,2,3 };
real g_vert_tex_norm[] = 
{
  1, 1, 0, 1,    1, 1,    0,0,-1,
  1,-1, 0, 1,    1, 0,    0,0,-1,
  -1,-1, 0, 1,    0, 0,    0,0,-1,
  -1, 1, 0, 1,    0, 1,    0,0,-1
};

real g_light_pos[4]= { 0,3,-5,1 };
real g_light_col[4]= { 1,1,1,0 };

real g_pos[4]      = { 0,  2, g_dist, 1 };
real g_up[4]       = { 0,  1,  0, 1 };
real g_lookat[4]   = { 0,  0,  0, 1 };
real g_lookdir[4];

// Vertices for bones
vector<he_vertex> g_points,g_buffer,g_vertices;
vector<integer> g_indices;

HalfEdgeMesh g_mesh;
#ifdef _LINEAR_
SubdivisionMesh g_submesh[10];
#elif _BUTTERFLY_
ButterflyMesh g_submesh[10];
#else
LoopMesh g_submesh[10];
#endif
//vector<he_vertex> g_vertices;
byte g_checker[64*64*4];
Joint g_heirarchy;

/*
 * function protos
 */
void makechecker(void);

/*
 * main - it all starts here
 */
int main(int argc,char *argv[])
{
  integer width  = 200,
          height = 200,
          x      = 1024/2,
          y      = 768/2,i;
  real min[3],max[3],dd[3];
  // vector<he_edge>::iterator edge;
  vector<he_vertex>::iterator vertex;
  vector<he_vertex> *p_vertices;
  // he_vertex v;
  //Joint primary;

  if(argc < 2)
    {
      cerr << argv[0] << " <filename> <levels>\n";
      return EXIT_SUCCESS;
    }

  cerr << "Loading model";
  g_mesh.Load(argv[1]); //twotris.off");
  cerr << "....done\n";

  cerr << "Subdividing mesh";
  g_submesh[0].subdivide(&g_mesh);
  g_submesh[0].compile();
  g_submesh[0].compileneighbours();
  g_max_level = atoi(argv[2]);
  for(i = 1;i < g_max_level;i++)
    {
      g_submesh[i].subdivide(&(g_submesh[i - 1]));
      g_submesh[i].compile();
      g_submesh[i].compileneighbours();
      g_submesh[i].compileweights();
      g_submesh[i].update();
    }
  cerr << "....done\n";

  g_submesh[1].determine_volume(min,max);
  ASubtract3D(max,min,dd);
  cerr << "Bounding volume:"; APrint3D(min); cerr << "-"; APrint3D(max); cerr << endl;
  ///*
  g_lookat[0] = min[0] + dd[0]/2;
  g_lookat[1] = min[1] + dd[1]/2;
  g_lookat[2] = min[2] + dd[2]/2;
  g_lookat[3] = 1.0;
  cerr << "look at:"; APrint4D(g_lookat); cerr << endl;
  g_pos[0] = min[0] + dd[0]/2;
  g_pos[1] = min[1] + dd[1]/2;
  g_pos[2] = min[2] + dd[2]/2 - 10;
  g_pos[3] = 1.0;
  cerr << "pos:"; APrint4D(g_pos); cerr << endl;
  //*/
  // setup bone hierarchy
  cerr << "Setting up heirarchy";
  Joint *p_joints[SEGMENTS];
  vector<j_vertex> *p_jvertices[SEGMENTS];
  vector<Joint> *p_children;
  j_vertex jv;
  Joint *p_joint;
  keyframe kf;
  real axis[4] = { 0,0,1,0 };
  real origin[4],t,displ[4];
  integer count;
  he_vertex hev;

  ASubtract3D(max,min,dd);
  
  kf.disp[0] = 0;
  kf.disp[1] = 0;
  kf.disp[2] = 0;
  kf.disp[3] = 0;

  origin[0] = min[0] + dd[0]/2;
  origin[1] = min[1];
  origin[2] = min[2] + dd[2]/2;
  origin[3] = 1.0;
  
  displ[0] = 0.0;
  displ[1] = dd[1]/SEGMENTS;
  displ[2] = 0.0;
  displ[3] = 0.0;

  // assign keyframes to each joint
  p_joint = &g_heirarchy;
  g_points.reserve(SEGMENTS + 1);
  g_buffer.reserve(SEGMENTS + 1);
  ACopy4D(origin,hev.V);
  cerr << "origin:"; APrint4D(origin); cerr << endl;
  g_points.push_back(hev);
  g_buffer.push_back(hev);
  g_indices.push_back(0);
  for(i = 0;i < SEGMENTS;i++)
    {
      p_joints[i] = p_joint;
      p_jvertices[i] = p_joint->GetVertices();
      p_joint->SetOrigin(origin);
      
      if(i != 0)
	p_joint->SetDisplacement(displ);
      else
	p_joint->SetDisplacement(origin);
      
      if(i != 0)
	{
	  kf.time = 0.0;
	  AA2Quat(axis,-M_PI/4,kf.orient);
	  p_joint->GetKeyFrames()->push_back(kf);
	  
	  kf.time = 0.25;
	  AA2Quat(axis,0,kf.orient);
	  p_joint->GetKeyFrames()->push_back(kf);

	  kf.time = 0.5;
	  AA2Quat(axis,M_PI/4,kf.orient);
	  p_joint->GetKeyFrames()->push_back(kf);
	  
	  kf.time = 0.75;
	  AA2Quat(axis,0,kf.orient);
	  p_joint->GetKeyFrames()->push_back(kf);
	  
	  kf.time = 1.0;
	  AA2Quat(axis,-M_PI/4,kf.orient);
	  p_joint->GetKeyFrames()->push_back(kf);
	}
      else
	{
	  kf.time = 0.0;
	  QIdentity(kf.orient);
	  p_joint->GetKeyFrames()->push_back(kf);
	  
	  kf.time = 1.0;
	  QIdentity(kf.orient);
	  p_joint->GetKeyFrames()->push_back(kf);
	}
      cerr << "bone:"; APrint4D(kf.disp); cerr << endl;

      p_children = p_joint->GetChildren();
      origin[1] += dd[1]/real(SEGMENTS);

      ACopy4D(origin,hev.V);
      g_points.push_back(hev);
      g_buffer.push_back(hev);
      g_indices.push_back(i + 1);      
      jv.source = g_points.back().V;
      jv.dest = g_buffer.back().V;
      jv.weight = 1.0;
      p_jvertices[i]->push_back(jv);

      if(i < SEGMENTS - 1)
	{
	  p_children->push_back(Joint());
	  p_joint = p_children->end() - 1;
	}
    }
  cerr << "done\n";

  cerr << "Assign vertices to bones";
  real V[4];
  p_vertices = g_submesh[1].GetVertices();
  g_vertices.reserve(p_vertices->size());
  for(vertex = p_vertices->begin();vertex != p_vertices->end();vertex++)
    {
      g_vertices.push_back(*vertex);

      jv.source = g_vertices.back().V;
      jv.dest = vertex->V;
      jv.weight = 1.0;

      // determine segment
      i = (integer)(SEGMENTS*(vertex->V[1] - min[1])/(max[1] - min[1]));
      if(i >= SEGMENTS)
	i--;
      t = (vertex->V[1] - min[1] - (i*dd[1])/SEGMENTS)/(dd[1]/SEGMENTS);
      count = 1;
      //i = SEGMENTS - i - 1;
      ///*
      //cerr << "t:" << t;
      // << " dd:" << dd[1]/SEGMENTS << " dy:" << (i*dd[1])/SEGMENTS << " V:" << vertex->V[1] << endl;
      ///*
      if((t > 0.5) && (i < SEGMENTS - 1))
	{
	  count++;
	  jv.weight = 0.5;
	  p_jvertices[i + 1]->push_back(jv);
	  cerr << " " << i + 1;
	}
      if((t <= 0.5) && (i > 0))
	{
	  count++;
	  jv.weight = 0.5;
	  p_jvertices[i - 1]->push_back(jv);
	  cerr << " " << i - 1;
	}
      //*/
      p_jvertices[i]->push_back(jv);
      cerr << " " << i << endl;
    }
  cerr << "done\n";

  // create texture
  makechecker();

  cerr << "Initialising GLUT\n";
  glutInit(&argc,argv);
  cerr << "width:" << width << " height:" << height << "\n";
  glutInitWindowSize(width,height);
  cerr << "x:" << y << " y:" << y << "\n";
  glutInitWindowPosition(x,y);
  cerr << "setting display mode:\n";
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  cerr << "Creating window\n";
  glutCreateWindow("SubDivision");
  cerr << "Setting handler functions:\n";
  glutKeyboardFunc(keyboard);
  cerr << "\tkeyboard\n";
  glutMouseFunc(press);
  cerr << "\tmouse buttons\n";
  glutMotionFunc(motion);
  cerr << "\tmouse motion\n";
  glutDisplayFunc(display);
  cerr << "\tdisplay\n";
  glutReshapeFunc(reshape);
  cerr << "\tidle\n";
  glutIdleFunc(idle);
  //glutTimerFunc(10,tick,10); // set timer to tick once every 60 seconds
  cerr << "\treshape\n";
  cerr << "About to start the main loop\n";

  // g_submesh[1].printvertexlist();

  // Create the OpenGL scene
  gp_scene = new GLScene;

  glutMainLoop();

  delete gp_scene;
}

/*
 * GLUT callback function for when window is resized
 */
void reshape(int width,int height)
{
  gp_scene->Viewport(0,0,width,height);
}

/*
 * GLUT callback for keyboard events
 */
void keyboard(unsigned char key,int x,int y)
{
  switch(key)
    {
    case 'O':
    case 'o':
      g_object = !g_object;
      break;
    case 'T':
    case 't':
      g_texture = !g_texture;
      break;
    case '[':
	g_rate -= 0.0001;
	if(g_rate < 0.0)
	  g_rate = 0;
      break;
    case ']':
      g_rate += 0.0001;
      break;
    case 'Q':
    case 'q':
      exit(1);
      break;
    case 'R':
    case 'r':
      glutPostRedisplay();
      break;
    case ',':
      g_fovy++;
      break;
    case '.':
      g_fovy--;
      break;
    case 'Z':
    case 'z':
      g_dist++;
      break;
    case 'X':
    case 'x':
      g_dist--;
      break;
    case '=':
      if(g_level < g_max_level - 1)
	g_level++;
      break;
    case '-':
      if(g_level > 1)
	g_level--;
      break;
    case 'S':
    case 's':
      gp_scene->SmoothShade();
      break;
    case 'F':
    case 'f':
      gp_scene->FlatShade();
      break;
    case 'B':
    case 'b':
      gp_scene->FlipBackFace();
      break;
    case 'W':
    case 'w':
      gp_scene->WireFrame();
      break;
    case 'L':
    case 'l':
      gp_scene->SolidFill();
      break;
    case 'E':
    case 'e':
      g_envmap = !g_envmap;
      break;
    default:
      cerr << "key unbound\n";
      break;
    };
}

/*
 * GLUT Mouse button callback function
 */
void press(int button,int state,int x,int y)
{
}

/*
 * GLUT Mouse motion callback function
 */
void motion(int x,int y)
{
  
}

/*
 * GLUT display function
 */
void display(void)
{
  integer i;
  real M[16];
  vector<he_vertex> *p_vertices;

  //ANormalise3D(axis,axis);
  gp_scene->FieldOfView(g_fovy);
  gp_scene->BeginFrame();
  // Add lights
  gp_scene->AddLight(g_light_pos,g_light_col);

  // Add Camera
  gp_scene->Position(g_pos);
  gp_scene->LookAt(g_lookat);
  gp_scene->Up(g_up);

  // Setup First material
  gp_scene->BeginMaterial();
  gp_scene->Phong(0.1,0.4,0.5,100.0);
  if(g_texture == true)
    {
      if(g_envmap == true)
	gp_scene->AddEnvironmentMap(64,64,g_checker);
      else
	gp_scene->AddTexture(64,64,g_checker);
    }
  gp_scene->EndMaterial();

  /*
  // draw firsxt fragment
  //gp_scene->AddTriangles(1,faces,vertices,normals,texuv);
  // Lets rotate the quad about the x-axis


  g_angle = fmod(g_angle + g_rate,360.0);
  MRotate4x4((g_angle*M_PI)/180,axis,Mr);
  p_vertices = g_submesh[1].GetVertices();
  vs = g_vertices.begin();
  vd = p_vertices->begin();
  */
  //g_submesh[1].printvertexlist();
  // clear vertices before acculumation
  vector<he_vertex>::iterator vertex;
  p_vertices = g_submesh[1].GetVertices();
  for(vertex = p_vertices->begin();vertex != p_vertices->end();vertex++)
      vertex->V[0] = vertex->V[1] = vertex->V[2] = vertex->V[3] = 0.0;
  for(vertex = g_buffer.begin() + 1;vertex != g_buffer.end();vertex++)
      vertex->V[0] = vertex->V[1] = vertex->V[2] = vertex->V[3] = 0.0;

  //cerr << "update heirarchy";
  g_time = fmod(g_time + g_rate,1);

  MIdentity4x4(M);
  g_heirarchy.update(0,g_time,M);
  // propagate changes
  for(i = 2;i <= g_level;i++)
    g_submesh[i].update();
  //g_submesh[1].draw(g_lookdir,1,gp_scene);
  if(g_object)
    g_submesh[g_level].draw(g_lookdir,1,gp_scene);

  // cerr << "buffer:" << g_buffer.size() << endl;
  
  gp_scene->AddLines(g_buffer.size(),g_indices.begin(),g_buffer.begin()->V,sizeof(he_vertex));
  gp_scene->AddPoints(g_buffer.size(),g_indices.begin(),g_buffer.begin()->V,sizeof(he_vertex));
  // gp_scene->AddLines(g_points.size(),g_indices.begin(),g_points.begin()->V,sizeof(he_vertex));
  // gp_scene->AddPoints(g_points.size(),g_indices.begin(),g_points.begin()->V,sizeof(he_vertex));
  /*
  for(i = 0;i < 4;i++)
    {
      MVMultiply4x4(Mr,buffer + i*4,vertices + i*9);
    }
  gp_scene->BeginMaterial();
  gp_scene->Phong(0.1,0.6,0.2,100.0);
  gp_scene->AddTexture(64,64,g_checker);
  gp_scene->EndMaterial();

  gp_scene->AddQuads(1,g_faces,
		     g_vert_tex_norm + 0,9*sizeof(double),
		     g_vert_tex_norm + 6,9*sizeof(double),
		     g_vert_tex_norm + 4,9*sizeof(double));
  */
  gp_scene->EndFrame();

  // Swop in the frame
  glutSwapBuffers();
}

/*
 * GLUT timer function
 */
void tick(int value)
{
  cerr << "timer func:" << value << "\n";
  glutPostRedisplay();
}

/*
 * GLUT idle function
 */
void idle(void)
{
  glutPostRedisplay();
}

/*
 * checker image texture
 */
void makechecker(void)
{
  int i,j,col,off;

  for(i = 0;i < 64;i++)
    {
      for(j = 0;j < 64;j++)
	{
	  col = ((((i & 0x8)==0)^((j & 0x8)) == 0))*127 + 127;
	  off = (i*64 + j)*4;
	  g_checker[off + 0] = (GLubyte) col;
	  g_checker[off + 1] = (GLubyte) col;
	  g_checker[off + 2] = (GLubyte) col;
	  g_checker[off + 3] = (GLubyte) 255;
	}
    }
}
