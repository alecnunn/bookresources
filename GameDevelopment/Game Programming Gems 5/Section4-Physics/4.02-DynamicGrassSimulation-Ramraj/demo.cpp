//----------------------------------------------------------------------------
//
//demo.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <stdlib.h>
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <time.h>
#include <memory.h>

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )

#include "demo.h"
#include "win_wrap.h"

#include "water.h"
#include "texture.h"
#include "grass.h"
#include "leaves.h"
#include "cs_ch3d.h"
#include "particles.h"

//··········································································//
//    Source :: Definitions
//··········································································//

#define MILLI_PER_RIPPLE 115

//··········································································//
//    Source :: Structures
//··········································································//

//··········································································//
//    Source :: Class Defs
//··········································································//

//··········································································//
//    Source :: Private Function Defs
//··········································································//

void draw_grass(void);
void draw_leaf(void);
void draw_z(Particle * cur_part);
void draw_flare(Particle * cur_part);

//··········································································//
//    Source :: Private Vars
//··········································································//

//input control
float y_rot = 0;
float x_rot = 0;
float z_trans = -50.0f;

//update control
int cur_milli = 0;

//lighting
float l_amb[4] = {	0.2f, 0.2f, 0.2f, 1.0f	};
float l_diff[4] = {	0.2f, 0.2f, 0.2f, 1.0f	};
float l_spec[4]= {	0.2f, 0.2f, 0.2f, 1.0f	};
float l_pos[4]= {	121.0f, 120.0f, -70.0f, 1.0f};

//scenery
water ocean;
GLuint ocean_tex;
int num_leaves;
leaves tree_leaves;
grass island_grass;
float wind[3] = {-5.0f, 0.1f, 5.0f};

cs_am3d_c scene_mdl;
cs_am3d_c tree_mdl;
cs_am3d_c leaf_mdl;
cs_am3d_c grass_blade_mdl;

cs_am3d_c Rishi_am;
cs_sk3d_c Rishi_sk;
cs_kf3d_c Rishi_kf;
cs_ch3d_c Rishi;
cs_am3d_c z_mdl;
particle_emitter zzz;

//··········································································//
//    Source :: External Function Implementation
//··········································································//

bool dem_init(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glAlphaFunc(GL_GREATER,0.1f);
	glEnable(GL_ALPHA_TEST);
	
	//ipicture produces artifcats if rendered right side up
	//this is compensation
	glMatrixMode(GL_TEXTURE);
	glScalef(1.0f,-1.0f,1.0f);
	glMatrixMode(GL_MODELVIEW);

	glLightfv(GL_LIGHT0, GL_AMBIENT,  l_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  l_diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l_spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l_pos);	

	l_amb[0] = l_amb[1] = l_amb[2] = 0.75f;
	l_spec[0] = l_spec[1] = l_spec[2] = 0.65f;
	l_diff[0] = l_diff[1] = l_diff[2] = 0.15f;
	glLightfv(GL_LIGHT1, GL_AMBIENT,  l_amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  l_diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l_spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l_pos);

	l_amb[0] = l_amb[1] = l_amb[2] = 0.35f;
	l_spec[0] = l_spec[1] = l_spec[2] = 1.0f;
	l_diff[0] = l_diff[1] = l_diff[2] = 1.0f;
	glLightfv(GL_LIGHT2, GL_AMBIENT,  l_amb);
	glLightfv(GL_LIGHT2, GL_DIFFUSE,  l_diff);
	glLightfv(GL_LIGHT2, GL_SPECULAR, l_spec);
	glLightfv(GL_LIGHT2, GL_POSITION, l_pos);

	//sphere mapping
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	srand( (unsigned)time( NULL ) );

	if(!scene_mdl.load("data/scene/scene.am3d"))
		return false;
	if(!tree_mdl.load("data/tree/tree.am3d"))
		return false;
	if(!leaf_mdl.load("data/tree/leaf.am3d"))
		return false;	

	if(!grass_blade_mdl.load("data/grass/grass.am3d"))
		return false;
	if(!island_grass.init(65,65,0.1f,2,wind, 0.3f))
		return false;
	if(!island_grass.load_scale_map("data/grass/map.jpg", 3.0f, -0.2f))
		return false;
	if(!island_grass.load_height_map("data/grass/map.jpg", 13.0f))
		return false;
	
	if(!ocean.init(240,240,0.01f))
		return false;
	if(!ipic_build_tex("data/scene/ocean.jpg", ocean_tex))
		return false;

	if(!Rishi_am.load("data/Rishi/Rishi.am3d"))
		return false;
	if(!Rishi_sk.load("data/Rishi/Rishi.sk3d"))
		return false;
	if(!Rishi_kf.load("data/Rishi/Rishi.kf3d"))
		return false;
	if(!z_mdl.load("data/Rishi/z.am3d"))
		return false;

	l_pos[0] = 0.843f;
	l_pos[1] = 5.7f;
	l_pos[2] = 1.4f;
	zzz.set_pos(l_pos);
	l_pos[0] = 0.0f;
	l_pos[1] = 0.01f;
	l_pos[2] = 0.0f;
	zzz.set_vel(l_pos);
	l_pos[0] = 0.0f;
	l_pos[1] = 0.0001f;
	l_pos[2] = 0.0f;
	zzz.set_grav(l_pos);
	l_pos[0] = 0.1f;
	l_pos[1] = 0.0f;
	l_pos[2] = 0.1f;
	zzz.set_spread(l_pos);
	zzz.set_fade(0.05f);
	zzz.set_slowdown(0.03f);

	if(!zzz.ResetEmitter(7))
		return false;

	//generate leaf positions
	{
		word * sel_vert;		

		sel_vert = tree_mdl.gel_sel_vertices(num_leaves);
		if(!num_leaves || !sel_vert)
			return false;

		float * leaf_pos = new float[num_leaves * 3];
		if(!leaf_pos)
			return false;
		
		for(int loop1 = 0; loop1 < num_leaves; loop1++)
			memcpy((void *)&(leaf_pos[loop1 * 3]), (void*)tree_mdl.get_vertex(sel_vert[loop1]).vertex, sizeof(float)*3);

		if(!tree_leaves.init(num_leaves,leaf_pos,wind,1.5f, 0.15f))
			return false;		

		delete[] leaf_pos;
		delete[] sel_vert;
	}

	//Build animation
	Rishi_am.apply_skel(&Rishi_sk);
	Rishi_sk.del_abs();
	Rishi.set_mdl(&Rishi_am);
	Rishi.set_anim(Rishi_kf.get_anim_ptr((int)0));
	Rishi.set_anim(true);
	Rishi.set_loop(true);
	Rishi.set_scale(1.0f);
	Rishi.update(0);

	return true;
}
void dem_update(int milliseconds)
{
	if(wnd_get_key_press(VK_ESCAPE))
		wnd_post_quit();
	if(wnd_get_key_press(VK_F1))
		wnd_post_tgl_full_scr();
	if(wnd_get_key_press(VK_UP))
		z_trans += (float)milliseconds/50.0f;	
	if(wnd_get_key_press(VK_DOWN))
		z_trans -= (float)milliseconds/50.0f;

	if(z_trans > -5.0f)
		z_trans = -5.0f;

	if(z_trans < -115.0f)
		z_trans = -115.0f;

	cur_milli += milliseconds;

	if(cur_milli > MILLI_PER_RIPPLE)
	{
		int x = (int)( ((float)rand()/(float)RAND_MAX) * 4 );
		for(int y = 0; y < x; y++)
			ocean.add_ripple(
				(int)( ( (float)rand()/(float)RAND_MAX ) * 238 + 1 ),
				(int)( ( (float)rand()/(float)RAND_MAX ) * 238 + 1 ),
				( ( (float)rand()/(float)RAND_MAX ) * -3.5f) );
	
		cur_milli = 0;

		x = (int)( ((float)rand()/(float)RAND_MAX) * 2 );
		for(y = 0; y < x; y++)
			island_grass.add_ripple(
				(int)( ( (float)rand()/(float)RAND_MAX ) * 63 + 1 ),
				(int)( ( (float)rand()/(float)RAND_MAX ) * 63 + 1 ),
				( ( (float)rand()/(float)RAND_MAX ) * 30 ) );

		x = (int)( ((float)rand()/(float)RAND_MAX) * 30 );
		for(y = 0; y < x; y++)
			tree_leaves.add_ripple(
				(int)(((float)rand()/(float)RAND_MAX) * num_leaves) - 1 ,
				(float)rand()/(float)RAND_MAX * 3 );	
	}
	
	x_rot = (float)wnd_get_mouse_y()/(float)wnd_get_scr_height();
	x_rot = (1.0f - x_rot)*50.0f - 2.5f;
	y_rot = (float)wnd_get_mouse_x() /(float)wnd_get_scr_width();
	y_rot = 360 * y_rot - 180;

	ocean.update(milliseconds);
	island_grass.update(milliseconds);	
	tree_leaves.update(milliseconds);
	zzz.UpdateEmitter(milliseconds);
	Rishi.update(milliseconds);	
}
void dem_draw(void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();
	
	//Position
	glScalef(0.1f,0.1f,0.1f);
	glTranslatef( 0.0f, -5.0f, z_trans);	
	glRotatef( x_rot, 1.0f, 0.0f, 0.0f);
	glRotatef( y_rot, 0.0f, 1.0f, 0.0f);

	glColor4f(1.0f,1.0f,1.0f,1.0f);	
	glDisable(GL_LIGHTING);	

	scene_mdl.render_no_anim();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
		glTranslatef(-10.1f,1.8f,-9.8f);		
		island_grass.draw_scaled_height(draw_grass);
	glPopMatrix();

	tree_mdl.render_no_anim();	
	
	glDisable(GL_LIGHT0);	

	glEnable(GL_LIGHT1);
	Rishi.render();
	glDisable(GL_LIGHT1);

	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glEnable(GL_NORMALIZE);

	glPushMatrix();
		
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);		
		glBindTexture(GL_TEXTURE_2D,ocean_tex);		
		glColor4f(0.4f,0.9f,1.0f,0.35f);
		glTranslatef(-120.0f,2.5f,-120.0f);
		ocean.draw_sph();		
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);

	glPopMatrix();

	glDepthMask(GL_TRUE);
	glDisable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT2);
	glColor4f(1.0f,1.0f,1.0f,1.0f);	
	tree_leaves.draw(draw_leaf);
	glDisable(GL_LIGHT2);

	glDisable(GL_LIGHTING);
	zzz.RenderEmitter(draw_z);	

	glFlush ();
}
void dem_deinit(void)
{
}

//··········································································//
//    Source :: External Class Implementation
//··········································································//

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

void draw_grass(void)
{
	grass_blade_mdl.render_no_anim();
}
void draw_leaf(void)
{
	leaf_mdl.render_no_anim();
}
void draw_z(Particle * cur_part)
{
	glTranslatef(cur_part->x, cur_part->y, cur_part->z);
	glRotatef(-y_rot,0.0f, 1.0f, 0.0f);
	if(cur_part->life)
	{
		float scale = 1.0f - cur_part->life;
		glScalef(scale, scale, scale);		
	}

	glColor4f(1.0f,1.0f,1.0f,cur_part->life);	
	z_mdl.render_no_anim();
}

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//demo.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
