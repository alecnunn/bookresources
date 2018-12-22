#include <memory.h>
#include "sph_render.h"
#include "gpu_util.h"
#include "sph_surf.h"

#include <GL/glut.h>


#ifdef NO_NVCG
static GLuint water_fp;
static GLuint water_vp;
#else
static CGprogram water_fragment;
static CGprogram water_vertex;
#endif


#ifdef SHADER_EMBEDDED
#include "embedded/fp_water_fragment.h"
#include "embedded/vp_water_vertex.h"
#endif

void sph_render_init_shaders()
{
}

void sph_render_create(sph_render* r)
{
	r->volume = NULL;

	r->pp = vec3_alloc(150000);
#ifdef SPH_ALPHA_REFRACT
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#endif

#ifdef SPH_SHADER
#ifdef NO_NVCG
	glGenProgramsARB(1, &water_fp);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, water_fp);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,
		GL_PROGRAM_FORMAT_ASCII_ARB,
		strlen(fp_str_water_fragment),
		(const GLubyte *)fp_str_water_fragment);
	gput_printf("Water fragment program initialized\n");

	glGenProgramsARB(1, &water_vp);
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, water_vp);
	glProgramStringARB(GL_VERTEX_PROGRAM_ARB,
		GL_PROGRAM_FORMAT_ASCII_ARB,
		strlen(vp_str_water_vertex),
		(const GLubyte *)vp_str_water_vertex);

	//set parameters for fresnel reflection
	//glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 13, 2.0f, 2.0f, 0.1f, 0.0f);

	gput_printf("Water vertex program initialized\n");

	check_gl_error();

#else
	LOAD_FRAGMENT_PROGRAM(water_fragment, "water_fragment.cg");
	LOAD_VERTEX_PROGRAM(water_vertex, "water_vertex.cg");


	check_cg_error();
#endif
#endif
}

void sph_render_destroy(sph_render* r)
{
}

void enable_water_shader()
{
#ifdef NO_NVCG
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, water_fp);
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, water_vp);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glEnable(GL_VERTEX_PROGRAM_ARB);
	gput_printf("enable water shader\n");
#else
	cgGLEnableProfile(FP_PROFILE);
	cgGLEnableProfile(VP_PROFILE);
	cgGLBindProgram(water_fragment);
	cgGLBindProgram(water_vertex);
	check_cg_error();
#endif
	check_gl_error();
}

void disable_water_shader()
{
#ifdef NO_NVCG
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
	glDisable(GL_VERTEX_PROGRAM_ARB);	
#else
	cgGLDisableProfile(FP_PROFILE);
	cgGLDisableProfile(VP_PROFILE);		
#endif
}


/**
 * Set the configuration such as resolution for Marching Cubes
 */
void sph_render_set_config(sph_render* r, float stride, int range, float iradius)
{
	float iso_radius;
	
	free(r->volume);

	r->iso_radius = iradius;
	iso_radius = iradius;
	r->mc_stride = stride;
	r->mc_range = range;

//	volume = (float*)malloc(vol_width*vol_width*vol_depth*sizeof(float));
	r->volume = (float*)malloc(100*100*100*sizeof(float));
	if (r->volume == NULL)
		printf("Volume couldn't be allocated\n");
}

void sph_render_create_implicit(sph_render* r, float k, const vector3* pos, int n_particles, const matrix4* m, cpu_sph* sph)
{
	int i;
	float inv_stride;
	float stride;
	float stride2;
	//float v;
	int x;
	int y;
	int z;

	float fmin_x;
	float fmax_x;
	float fmin_y;
	float fmax_y;
	float fmin_z;
	float fmax_z;

	int min_x;
	int max_x;
	int min_y;
	int max_y;
	int min_z;
	int max_z;
	
	int vol_width;
	int vol_height;
	int vol_depth;

	
	//vector3 volpos;

	vector3 piv; /* position in volume */

#ifdef LIMIT_VOLUME
	vector3 x_obs;
	vector3 y_obs;
	vector3 z_obs;
	vec3_set(&x_obs, 1.0f, 0.0f, 0.0f);
	vec3_set(&y_obs, 0.0f, 1.0f, 0.0f);
	vec3_set(&z_obs, 0.0f, 0.0f, 1.0f);
		mat4_mulvec3_as_mat3(&x_obs, m, &x_obs);
		mat4_mulvec3_as_mat3(&y_obs, m, &y_obs);
		mat4_mulvec3_as_mat3(&z_obs, m, &z_obs);
#endif

	stride = r->mc_stride;
	stride2 = stride*stride;
	inv_stride = 1.0f/stride;

	fmin_x = fmin_y = fmin_z = 1000;
	fmax_x = fmax_y = fmax_z = -1000;
	for (i = 0; i < n_particles; i++)
	{
		const vector3* p = &pos[i];
		if (fmin_x > p->x)
			fmin_x = p->x;
		if (fmax_x < p->x)
			fmax_x = p->x;
		if (fmin_y > p->y)
			fmin_y = p->y;
		if (fmax_y < p->y)
			fmax_y = p->y;
		if (fmin_z > p->z)
			fmin_z = p->z;
		if (fmax_z < p->z)
			fmax_z = p->z;
	}

	//printf("%.4f, %.4f, %.4f\n", fmax_x, fmax_y, fmax_z);
	vec3_set(&r->volpos, fmin_x - r->mc_range*stride, fmin_y - r->mc_range*stride, fmin_z - r->mc_range*stride);
//	vec3_set(&r->volpos, fmin_x, fmin_y, fmin_z);
	min_x = 0;
	min_y = 0;
	min_z = 0;
	inv_stride = 1.0f/stride;
	max_x = (fmax_x - fmin_x)*inv_stride;
	max_y = (fmax_y - fmin_y)*inv_stride;
	max_z = (fmax_z - fmin_z)*inv_stride;

	if ((min_x < 0) || (min_y < 0) || (min_z < 0))
		printf("min_x=%d, min_y=%d, min_z=%d\n", min_x, min_y, min_z);

	
	vol_width = max_x + r->mc_range*2+ 6;
	vol_height = max_y + r->mc_range*2 + 6;
	vol_depth = max_z + r->mc_range*2 + 6;
	


	memset(r->volume, 0, vol_width*vol_height*vol_depth*sizeof(float));
	
	r->im_vol.density = r->volume;
	r->im_vol.width = vol_width;
	r->im_vol.height = vol_height;
	r->im_vol.depth = vol_depth;
	r->im_vol.voxelsize = stride;


	//printf("\n---------------------------\n");
	for (i = 0; i < n_particles; i++)
	{
#ifdef LIMIT_VOLUME
		vector3 pos_obs;

#endif
		float bias = 1.0f;

		vec3_sub(&piv, &pos[i], &r->volpos);
		vec3_scale(&piv, 1.0f/stride, &piv);


		for (z = -r->mc_range; z <= r->mc_range; z++)
		{
			//vindex += vol_width*vol_height;
			for (y = -r->mc_range; y <= r->mc_range; y++)
			{
			//	vindex += vol_width;
				for (x = -r->mc_range; x <= r->mc_range; x++)
				{
					int vindex;
					float v;
					float h2_r2;

					float dx;
					float dy;
					float dz;


					dx = ((int)piv.x)*stride + r->volpos.x - pos[i].x + stride*x;
					dy = ((int)piv.y)*stride + r->volpos.y - pos[i].y + stride*y;
					dz = ((int)piv.z)*stride + r->volpos.z - pos[i].z + stride*z;

					h2_r2 = max(r->iso_radius*r->iso_radius - (dx*dx + dy*dy + dz*dz), 0.0f);

					v = k*h2_r2*h2_r2*h2_r2;

					
					vindex = (int)(piv.x + x - min_x + 2) 
						+ ((int)(piv.y + y - min_y + 2))*vol_width
						+ ((int)(piv.z + z - min_z + 2))*vol_width*vol_height;
						
					if ((vindex < 0) || (vindex > vol_width*vol_height*vol_depth))
						continue;

					r->volume[vindex] += v;
				}
			}
		}
	}
	r->vol_width = vol_width;
	r->vol_height = vol_height;
	r->vol_depth = vol_depth;
}

/**
 * Draw the fluid surface
 */
void sph_render_draw_surface(sph_render* r, float threshold)
{
			
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-0.01f, -0.01f, -0.01f);
	glTranslatef(r->volpos.x, r->volpos.y, r->volpos.z);


//	glColor3f(1.0f, 1.0f, 1.0f);

#ifdef SPH_SHADER
#ifdef NO_NVCG
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glEnable(GL_VERTEX_PROGRAM_ARB);
#else
	cgGLEnableProfile(FP_PROFILE);
	cgGLEnableProfile(VP_PROFILE);
	cgGLBindProgram(water_fragment);
	cgGLBindProgram(water_vertex);
	check_cg_error();
#endif
#endif	
	
#ifdef SPH_ALPHA_REFRACT
	glEnable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
#endif

	//glDisable(GL_CULL_FACE);
	//glutSolidTeapot(0.05f);
	
	render_marching_cubes(&r->im_vol,
		0, r->vol_width,
		0, r->vol_height,
		0, r->vol_depth,
		threshold);


#ifdef SPH_SHADER
#ifdef NO_NVCG
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
	glDisable(GL_VERTEX_PROGRAM_ARB);	
#else
	cgGLDisableProfile(FP_PROFILE);
	cgGLDisableProfile(VP_PROFILE);		
#endif
#endif

#ifdef SPH_ALPHA_REFRACT
	glDisable(GL_BLEND);
#endif
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	//glMatrixMode(GL_TEXTURE);
	//glPopMatrix();
}

