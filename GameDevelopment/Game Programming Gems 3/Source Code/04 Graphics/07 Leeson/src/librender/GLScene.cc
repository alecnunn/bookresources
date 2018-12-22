/* Copyright (C) William Leeson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2001"
 */
/****************************************************************************\
 *                         OpenGL Scene                                     *
 ****************************************************************************
 * File: GLScene.cc                                                         *
 * Date: $Date$                                                             *
 * Author: $Author$                                                         *
 * Version: $Revision$                                                      *
 * Description: An OpenGL based renderng class that allows state sorting    *
 *              and other optimisations.                                    *
\****************************************************************************/
#include "GLScene.h"


void check_opengl_error(char *cmd)
{
  GLenum error;
  while((error = glGetError()) != GL_NO_ERROR)
    {
      printf("[%s:%d] '%s' failed with error %s\n",__FILE__,__LINE__,cmd,gluErrorString(error));
    };
}

/*
 * constructor
 */
GLScene::GLScene()
{
  GLfloat amb_light[] = {0.2,0.2,0.2,1.0};
  // Setup the scene state
  glEnable(GL_LIGHTING);
  //glDisable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  // set up illumination model
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb_light);

  glPointSize(2.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_TEXTURE_2D);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  m_ff = GL_CCW;
  glFrontFace(m_ff);
  glPolygonMode(GL_FRONT,GL_FILL/* LINE *//* FILL */);
  glPolygonMode(GL_BACK,GL_LINE);

  // set clear color
  glClearColor(0,0,1,0);

  // Enable vetex array stuff
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  // light attenuation settings
  m_ca = 0.0;
  m_la = 0.0;
  m_qa = 1.0;

  // near and far clipping planes
  m_near = 1;
  m_far = 500 + 1;
  m_fovy = 45.0; //*3.14/180.0;

  // setup default viewing parameters
  m_pos[0] = 0;
  m_pos[1] = 0;
  m_pos[2] = -1;

  m_lookat[0] = 0;
  m_lookat[1] = 0;
  m_lookat[1] = 0;

  m_up[0] = 0;
  m_up[1] = 1;
  m_up[2] = 0;

  // setup frame and texture management data
  m_frame = 0;
  m_frame_threshold = 40;

  check_opengl_error("glGetError");
}

/*
 * destructor
 */
GLScene::~GLScene()
{
  // vector<material>::iterator mat;
  // vector<fragment>::iterator frag;
  vector<textureimage>::iterator image;

  cerr << "Destroying GLScene\n";
  for(image = m_images.begin();image != m_images.end();image++)
    {
      // unload texture if it's resident
      if(image->resident == true)
	{
	  cerr << "Deleteing texture image " << image->id << endl;
	  glDeleteTextures(1,&(image->id));
	}
    }
}

/*
 *  Add a list of lines to scene
 */
bool GLScene::AddLines(integer n,integer *p_indices,real *p_vertices,integer vstep)
{
  fragment frag;

  frag.type = GL_LINE_STRIP;
  frag.n = n;
  frag.p_faces = p_indices;
  frag.p_vertices = p_vertices;
  frag.vstep = vstep;
  frag.p_normals = NULL;
  frag.p_texuv = NULL;

  (m_materials.back()).fragments.push_back(frag);

  /*
  // Print out  vertices
  integer i,v;
  
  cerr << "------------------------\n";
  for(i = 0;i < n;i++)
    {
      v = p_indices[i];
      cerr << "(" << v << ") ";
      APrint4D((double *)(((byte *)(p_vertices)) + v*vstep));
      cerr << endl;
    }
  cerr << endl;
  */
  return true;
}

/*
 *  Add a list of points to scene
 */
bool GLScene::AddPoints(integer n,integer *p_indices,real *p_vertices,integer vstep)
{
  fragment frag;

  frag.type = GL_POINTS;
  frag.n = n;
  frag.p_faces = p_indices;
  frag.p_vertices = p_vertices;
  frag.vstep = vstep;
  frag.p_normals = NULL;
  frag.p_texuv = NULL;

  (m_materials.back()).fragments.push_back(frag);

  return true;
}

/*
 *  Add a list of triangles to scene
 */
bool GLScene::AddTriangles(integer n,integer *p_faces,
			   real *p_vertices,integer vstep,
			   real *p_normals,integer nstep,
			   real *p_texuv,integer tstep)
{
  fragment frag;

  frag.type = GL_TRIANGLES;
  frag.n = 3*n;
  frag.p_faces = p_faces;
  frag.p_vertices = p_vertices;
  frag.vstep = vstep;
  frag.p_normals = p_normals;
  frag.nstep = nstep;
  frag.p_texuv = p_texuv;
  frag.tstep = tstep;
  (m_materials.back()).fragments.push_back(frag);

  /*
  // Print out faces and corresponding vertices
  integer i,v;
  
  cerr << "------------------------\n";
  for(i = 0;i < 3*n;i += 3)
    {
      //cerr << "{\n";
      v = p_faces[i + 0];
      // cerr << "\t" << v << " "; 
      cerr << "(" << v << ") ";//  << AMagnitude3D((double *)(((byte *)(p_normals)) + v*nstep));
      APrint4D((double *)(((byte *)(p_vertices)) + v*vstep));
      // APrint3D((double *)(((byte *)(p_normals)) + v*nstep));
      // cerr << " "; APrint3D(p_normals + v*(nstep/8));
      // cerr << " "; APrint2D(p_texuv + v*(tstep/8));
      cerr << endl;

      v = p_faces[i + 1];
      // cerr << "\t" << v << " ";
      cerr << " (" << v << ")";// << AMagnitude3D((double *)(((byte *)(p_normals)) + v*nstep));
      APrint4D((double *)(((byte *)(p_vertices)) + v*vstep));
      //APrint3D((double *)(((byte *)(p_normals)) + v*nstep));
      // cerr << " "; APrint3D(p_normals + v*(nstep/8));
      // cerr << " "; APrint2D(p_texuv + v*(tstep/8));
      cerr << endl;

      v = p_faces[i + 2];
      // cerr << "\t" << v << " "; 
      cerr << " (" << v << ")";// << AMagnitude3D((double *)(((byte *)(p_normals)) + v*nstep));
      APrint4D((double *)(((byte *)(p_vertices)) + v*vstep));
      //APrint3D((double *)(((byte *)(p_normals)) + v*nstep));
      //cerr << " "; APrint3D(p_normals + v*(nstep/8));
      //cerr << " "; APrint2D(p_texuv + v*(tstep/8));
      cerr << endl;

      //cerr << "}\n";
    }
  cerr << "}\n";
  */
  return true;
}

/*
 *  Add a list of quadralaterals to scene
 */
bool GLScene::AddQuads(integer n,integer *p_faces,
		       real *p_vertices,integer vstep,
		       real *p_normals,integer nstep,
		       real *p_texuv,integer tstep)
{
  fragment frag;

  frag.type = GL_QUADS;
  frag.n = 4*n;
  frag.p_faces = p_faces;
  frag.p_vertices = p_vertices;
  frag.vstep = vstep;
  frag.p_normals = p_normals;
  frag.nstep = nstep;
  frag.p_texuv = p_texuv;
  frag.tstep = tstep;
  (m_materials.back()).fragments.push_back(frag);

  return true;
}

/*
 * Start a frame
 */
bool GLScene::BeginFrame(void)
{
  vector<material>::iterator mat;
  // vector<fragment>::iterator frag;
  // vector<light>::iterator light;

  // flush data from cache
  // clear materials and fragments
  for(mat = m_materials.begin();mat != m_materials.end();mat++)
    {
      mat->fragments.clear();
    }
  m_materials.clear();
  m_lights.clear();

  //cerr << "materials:" << m_materials.size() << "\n";
  // how about unloading any textures that have not been accessed

  return true;
}

/*
 * End a frame
 */
bool GLScene::EndFrame(void)
{
  integer pass;
  vector<material>::iterator mat;
  vector<fragment>::iterator frag;
  vector<texture>::iterator tex;
  vector<textureimage>::iterator image;
  vector<light>::iterator light;
  GLfloat zero[] = { 0,0,0,0 };
  // clear the screenlights[i].c
  glClear(GL_COLOR_BUFFER_BIT);
  //glClear(GL_ACCUM);
  // SetUp the viewport
  glViewport(m_vx,m_vy,m_vdx,m_vdy);

  // SetUp the Camera
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(m_fovy,m_aspect,m_near,m_far);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // setup any lights that are needed
  integer c = 0;
  for(light = m_lights.begin();light != m_lights.end();light++)
    {
      if(light->resident != true)
	{
	  glEnable(GL_LIGHT0 + c);
	  glLightfv(GL_LIGHT0 + c,GL_AMBIENT,light->c);
	  glLightfv(GL_LIGHT0 + c,GL_DIFFUSE,light->c);
	  glLightfv(GL_LIGHT0 + c,GL_SPECULAR,light->c);
	  glLightfv(GL_LIGHT0 + c,GL_POSITION,light->c);
	  glLightf(GL_LIGHT0 + c,GL_CONSTANT_ATTENUATION,m_ca);
	  glLightf(GL_LIGHT0 + c,GL_LINEAR_ATTENUATION,m_la);
	  glLightf(GL_LIGHT0 + c,GL_QUADRATIC_ATTENUATION,m_qa);
	  // spot light stuff
	  // glLightf(GL_LIGHT0 + i,GL_SPOT_CUTOFF,);
	  // glLightfv(GL_LIGHT0 + i,GL_SPOT_DIRECTION,);
	  light->resident = true;
	}
      c++;
    }

  // Set up viewing parameters
  gluLookAt(   m_pos[0],   m_pos[1],   m_pos[2],
	       m_lookat[0],m_lookat[1],m_lookat[2],
	       m_up[0],    m_up[1],    m_up[2]);

  /*
    // destroy any materials not used and load any not loaded
    for(mat = m_materials.begin();mat != m_materials.end();mat++)
    {
    // remove material if it has not been used recently
    if(mat->frame < m_frame - m_frame_threshold)
    {
    // delete (*mat);
    }
    }
  */
  for(image = m_images.begin();image != m_images.end();image++) // should really use for_each
    {
      if(image->resident == true)
	{
	  // remove texture if it has not been used recently
	  if((image->frame < m_frame - m_frame_threshold))
	    {
	      cerr << "Deleting texture image" << image->id << endl;
	      glDeleteTextures(1,&(image->id));
	    }
	}
      else // Make texture resident
	{
	  glGenTextures(1,&(image->id));
	  cerr << "Making texture (" << ((void *)(image->p_texture)) << ")" 
	       << " resident as " << image->id << endl;
	  glBindTexture(GL_TEXTURE_2D,image->id);
	  /*
	    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
	    image->width,image->height,
	    0,GL_RGBA,GL_UNSIGNED_BYTE,image->p_texture);
	  */
	  gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,
			    image->width,image->height,
			    GL_RGBA,GL_UNSIGNED_BYTE,image->p_texture);
	  image->resident = true;
	}
    }

  // sort materials if they have changed so as to minimise state changes
  // sort(m_materials.begin(),m_material.end(),some kinda sorting function ere);

  // render data
  for(pass = 0;pass < 2;pass++) // render n times to acheive better effect
    {
      glClear(GL_DEPTH_BUFFER_BIT);
      for(mat = m_materials.begin();mat != m_materials.end();mat++)
	{
	  mat->frame = m_frame;
	  // cerr << "mat:" << mat << "\n" << "\ttextures:\n";
	  // setup the textures
	  // Set the material coefficients
	  switch(pass)
	    {
	    case 0: // diffuse pass
	      glDisable(GL_BLEND);
	      glMaterialfv(GL_FRONT,GL_AMBIENT,mat->r_a);
	      glMaterialfv(GL_FRONT,GL_DIFFUSE,mat->r_d);
	      glMaterialfv(GL_FRONT,GL_SPECULAR,zero);
	      glMaterialf(GL_FRONT,GL_SHININESS,0.0);
	      check_opengl_error("Diffuse Setup");
	      //glMaterialfv(GL_FRONT,GL_EMISSION,mat->e);
	      //glColor3f(0,1,0);
	      if(mat->textures.size() == 0)
		glDisable(GL_TEXTURE_2D);
	      for(tex = mat->textures.begin();tex != mat->textures.end();tex++)
		{
		  //cerr << "Binding " << (*texit)->id << endl;
		  glBindTexture(GL_TEXTURE_2D,tex->image->id);
		  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,tex->wrap_s);
		  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,tex->wrap_t);
		  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,tex->min_filter);
		  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,tex->mag_filter);
		  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,tex->mode);
		  if(tex->coord_gen == true)
		    {
		      glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,tex->gen_s);
		      glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,tex->gen_t);
		      glEnable(GL_TEXTURE_GEN_S);
		      glEnable(GL_TEXTURE_GEN_T);
		    }
		  else
		    {
		      glDisable(GL_TEXTURE_GEN_S);
		      glDisable(GL_TEXTURE_GEN_T);
		    }
		  glEnable(GL_TEXTURE_2D);
		  tex->image->frame = m_frame;
		}
	      check_opengl_error("Texture setup");
	      break;
	    case 1: // specular pass
	      glEnable(GL_BLEND);
	      glBlendFunc(GL_ONE,GL_ONE);
	      glDisable(GL_TEXTURE_2D);
	      glMaterialfv(GL_FRONT,GL_AMBIENT,zero);
	      glMaterialfv(GL_FRONT,GL_DIFFUSE,zero);
	      glMaterialfv(GL_FRONT,GL_SPECULAR,mat->r_s);
	      glMaterialf(GL_FRONT,GL_SHININESS,mat->n);
	      check_opengl_error("Specular Pass");
	      break;
	    };
	  // render the geometry
	  // cerr << "\tfragments:" << "\n";
	  for(frag = mat->fragments.begin();frag != mat->fragments.end();frag++)
	    {
	      //glColor3f(1,1,1);
	      // cerr << "\tfrag:" << frag << " { type:" << (*frag)->type << " n:" << (*frag)->n << " }\n";
	      if(frag->p_vertices != NULL)
		{
		  glEnableClientState(GL_VERTEX_ARRAY);
		  glVertexPointer(4,GL_DOUBLE,frag->vstep,frag->p_vertices);
		}
	      else
		{
		  glDisableClientState(GL_VERTEX_ARRAY);
		}
	      if(frag->p_normals != NULL)
		{
		  glNormalPointer(GL_DOUBLE,frag->nstep,frag->p_normals);
		  glEnableClientState(GL_NORMAL_ARRAY);
		}
	      else
		{
		  glDisableClientState(GL_NORMAL_ARRAY);
		}
	      if(frag->p_texuv != NULL)
		{
		  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		  glTexCoordPointer(2,GL_DOUBLE,frag->tstep,frag->p_texuv);
		}
	      else
		{
		  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	      glDrawElements(frag->type,frag->n,GL_UNSIGNED_INT,frag->p_faces);

	      check_opengl_error("glDrawElements");
	      /*
		// Print out faces and corresponding vertices
		cerr << "------------------------\n";
		integer i,v;
		for(i = 0;i < frag->n;i += 3)
		{
		//cerr << "{\n";
		v = frag->p_faces[i + 0];
		// cerr << "\t" << v << " "; 
		cerr << "\t(" << v << ") "  << AMagnitude3D((double *)(((byte *)(frag->p_normals)) + v*frag->nstep));
		APrint3D((double *)(((byte *)(frag->p_normals)) + v*frag->nstep));
		// cerr << " "; APrint3D(p_normals + v*(nstep/8));
		// cerr << " "; APrint2D(p_texuv + v*(tstep/8));
		cerr << endl;
		  
		v = frag->p_faces[i + 1];
		// cerr << "\t" << v << " ";
		cerr << " (" << v << ")" << AMagnitude3D((double *)(((byte *)(frag->p_normals)) + v*frag->nstep));
		APrint3D((double *)(((byte *)(frag->p_normals)) + v*frag->nstep));
		// cerr << " "; APrint3D(p_normals + v*(nstep/8));
		// cerr << " "; APrint2D(p_texuv + v*(tstep/8));
		cerr << endl;

		v = frag->p_faces[i + 2];
		// cerr << "\t" << v << " "; 
		cerr << " (" << v << ")" << AMagnitude3D((double *)(((byte *)(frag->p_normals)) + v*frag->nstep));
		APrint3D((double *)(((byte *)(frag->p_normals)) + v*frag->nstep));
		//cerr << " "; APrint3D(p_normals + v*(nstep/8));
		//cerr << " "; APrint2D(p_texuv + v*(tstep/8));
		cerr << endl;
		  
		//cerr << "}\n";
		}
		cerr << "+++++++++++++++++++++++++++\n";
	      */
	      /*
		integer i;
		for(i = 0;i < (*frag)->n;i++)
		{
		cerr << "\t\t (" << (*frag)->p_faces[i] << ") "; 
		APrint3D((*frag)->p_vertices + 4*((*frag)->p_faces[i])); cerr << endl;
		}
	      */
	    }
	  //glAccum(GL_ADD,1.0);
	}
    }
  //glAccum(GL_RETURN,1.0);
  glPopMatrix();
  glFlush();

  return true;
}

/*
 * Begin Material reset material data to default
 */
bool GLScene::BeginMaterial(void)
{
  material mat;

  mat.r_s[0] = mat.r_s[1] = mat.r_s[2] = 0.1; mat.r_s[3] = 1.0;
  mat.r_d[0] = mat.r_d[1] = mat.r_d[2] = 0.7; mat.r_d[3] = 1.0;
  mat.r_a[0] = mat.r_a[1] = mat.r_a[2] = 0.2; mat.r_a[3] = 1.0;
  mat.n = 100.0;

  m_materials.push_back(mat);

  return true;
}

/*
 * EndMaterial revert back to previous material
 */
bool GLScene::EndMaterial(void)
{
  vector<texture> tex;
  //material *p_material;
  // blank for now
  // push back old material
  //p_material = m_materials.pop_back();
  // need to change or unbind texture set
  //for(tex = m_materialstextures.begin();tex != p_material->textures.end();tex++)
  //  {
      // not sure how to do this yet but only need it when multi-texturing is involved
  //  }

  return true;
}

/*
 *  Phong shading model
 */
bool GLScene::Phong(real r_a,real r_d,real r_s,real n)
{
  material mat;

  mat.r_a[0] = mat.r_a[1] = mat.r_a[2] = r_a; mat.r_a[3] = 1.0;
  mat.r_d[0] = mat.r_d[1] = mat.r_d[2] = r_d; mat.r_d[3] = 1.0;
  mat.r_s[0] = mat.r_s[1] = mat.r_s[2] = r_s; mat.r_s[3] = 1.0;
  mat.n = n;
  
  m_materials.push_back(mat);

  return true;
}

/*
 *  AddTexture - Add a texture to the current material
 */
bool GLScene::AddTexture(integer width,integer height,byte *p_tex)
{
  texture t;
  textureimage i;
  vector<textureimage>::iterator image;
  
  // save texture map properties
  t.mode = GL_MODULATE;
  t.wrap_s = GL_REPEAT;
  t.wrap_t = GL_REPEAT;
  t.min_filter = GL_LINEAR_MIPMAP_LINEAR;
  t.mag_filter = GL_LINEAR;
  t.coord_gen = false;

  // check to see that texture image does not already exist
  for(image = m_images.begin();image != m_images.end();image++)
    {
      if((image->width = width) && (image->height = height) && (image->p_texture = p_tex))
	{
	  image->refs++;
	  t.image = image;
	  (m_materials.back()).textures.push_back(t);
	  return true;
	}
    }

  // otherwise add texture image to texture list
  cerr << "Adding texture (" << ((void *)p_tex) << ") " << width << "x" << height << endl;
  i.refs = 1;
  i.frame = 0;
  i.width = width;
  i.height = height;
  i.p_texture = p_tex;
  i.resident = false;
  m_images.push_back(i);

  t.image = m_images.end() - 1;
  // also add texture to current material
  (m_materials.back()).textures.push_back(t);

  return true;
}

/*
 *  AddEnvironmentMap - Add a texture to the current material with automatic spherical texture
 *                      generation.
 */
bool GLScene::AddEnvironmentMap(integer width,integer height,byte *p_tex)
{
    texture t;
  textureimage i;
  vector<textureimage>::iterator image;
  
  // save texture map properties
  t.mode = GL_MODULATE;
  t.wrap_s = GL_REPEAT;
  t.wrap_t = GL_REPEAT;
  t.min_filter = GL_LINEAR_MIPMAP_LINEAR;
  t.mag_filter = GL_LINEAR;
  //t.min_filter = GL_LINEAR;
  //t.mag_filter = GL_LINEAR;
  t.coord_gen = true;
  t.gen_s = GL_SPHERE_MAP;
  t.gen_t = GL_SPHERE_MAP;

  // check to see that texture image does not already exist
  for(image = m_images.begin();image != m_images.end();image++)
    {
      if((image->width = width) && (image->height = height) && (image->p_texture = p_tex))
	{
	  image->refs++;
	  t.image = image;
	  (m_materials.back()).textures.push_back(t);
	  return true;
	}
    }

  // otherwise add texture image to texture list
  cerr << "Adding texture (" << ((void *)p_tex) << ") " << width << "x" << height << endl;
  i.refs = 1;
  i.frame = 0;
  i.width = width;
  i.height = height;
  i.p_texture = p_tex;
  i.resident = false;
  m_images.push_back(i);

  t.image = m_images.end() - 1;
  // also add texture to current material
  (m_materials.back()).textures.push_back(t);

  return true;
}

/*
 * AddPointLight - Add a Point light source to the scene
 */
bool GLScene::AddLight(real *p_pos,real *p_col)
{
  light l;

  l.pos[0] = GLfloat(p_pos[0]);
  l.pos[1] = GLfloat(p_pos[1]);
  l.pos[2] = GLfloat(p_pos[2]);
  l.pos[3] = GLfloat(p_pos[3]);

  l.c[0] = GLfloat(p_col[0]);
  l.c[1] = GLfloat(p_col[1]);
  l.c[2] = GLfloat(p_col[2]);
  l.c[3] = GLfloat(p_col[3]);

  l.resident = false;

  m_lights.push_back(l);

  return true;
}

/*
 * AddSpotLight(
 */
/*
 * Position - Set the viewing position
 */
bool GLScene::Position(real *pos)
{
  ACopy3D(pos,m_pos);
  // cerr << "eye:"; APrint3D(m_pos); cerr << endl;

  return true;
}

/*
 * LookAt - Set the position to look at
 */
bool GLScene::LookAt(real *pos)
{
  ACopy3D(pos,m_lookat);
  // cerr << "look at:"; APrint3D(m_lookat); cerr << endl;

  return true;
}

/*
 * Up - Set the up direction
 */
bool GLScene::Up(real *up)
{
  ACopy3D(up,m_up);
  // cerr << "up:"; APrint3D(m_up); cerr << endl;

  return true;
}

/*
 * LookDirection - Set the look direction direction
 */
bool GLScene::LookDirection(real *lookdir)
{
  ACopy3D(lookdir,m_lookdir);

  return true;
}

/*
 * Viewport - Set the viewing area of the window
 */
bool GLScene::Viewport(integer minx,integer miny,integer maxx,integer maxy)
{
  m_vx = GLint(minx);
  m_vy = GLint(miny);

  m_vdx = GLint(maxx - minx);
  m_vdy = GLint(maxy - miny);

  m_aspect = GLdouble(m_vdx)/GLdouble(m_vdy);

  return true;
}

/*
 * 
 */
bool GLScene::FieldOfView(real fovy)
{
  m_fovy = fovy; //*3.1415926535897932384626433832795)/180.0;
  // cerr << "fovy:" << m_fovy << "\n";

  return true;
}

/*
 * BeginObject - start and object (part of a transformation heirarchy)
 */
bool GLScene::BeginObject(void)
{
  return true;
}

/*
 * EndObject - end an object
 */
bool GLScene::EndObject(void)
{
  return true;
}

/*
 * Rotate - rotate all in current object
 */
bool GLScene::Rotate(real angle,real *axis)
{
  return true;
}

/*
 * Translate - translate all in current object
 */
bool GLScene::Translate(real x,real y,real z)
{
  return true;
}

/*
 * Scale - scale all in current scene
 */
bool GLScene::Scale(real x,real y,real z)
{
  return true;
}

/*
 * WireFrame - render everything as wire frame model rather that solid
 */
bool GLScene::WireFrame(void)
{
  glPolygonMode(GL_FRONT,GL_LINE);

  return true;
}

/*
 * Solid - render everything as a solid model
 */
bool GLScene::SolidFill(void)
{
  glPolygonMode(GL_FRONT,GL_FILL);

  return true;
}

/*
 * SmoothShade - interpolate normals
 */
bool GLScene::SmoothShade(void)
{
  glShadeModel(GL_SMOOTH);

  return true;
}

/*
 * FlatShade - don't interpolate normals
 */
bool GLScene::FlatShade(void)
{
  glShadeModel(GL_FLAT);

  return true;
}

bool GLScene::FlipBackFace(void)
{
  if(m_ff == GL_CCW)
    m_ff = GL_CW;
  else
    m_ff = GL_CCW;

  glFrontFace(m_ff);

  return true;
}






