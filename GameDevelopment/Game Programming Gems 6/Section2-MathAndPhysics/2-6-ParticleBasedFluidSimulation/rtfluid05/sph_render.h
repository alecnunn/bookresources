#ifndef _SPH_RENDER_H_
#define _SPH_RENDER_H_

#include "glg.h"
#include "gpu_util.h"
#include "cpu_sph.h"
#include "sph_surf.h"

#define SPH_SHADER
//#define SPH_ALPHA_REFRACT


typedef struct
{
	/** for marching cubes **/
	float mc_stride;
	int mc_range;
	float iso_radius;

	float* volume;
	vector3* pp;
	int pp_size;
	vector3 volpos; /** coordinate of volume corner **/
	implicit_surface im_vol;

	int vol_width;
	int vol_height;
	int vol_depth;

	float mass;
} sph_render;

void sph_render_create(sph_render* r);
void sph_render_destroy(sph_render* r);
void enable_water_shader();
void disable_water_shader();
void sph_render_set_config(sph_render* r, float stride, int range, float iradius);

//void set_volume_resolution(sph_render* r, float stride, int range, float iradius);
void sph_render_create_implicit(sph_render* r, float k, const vector3* pos, int n_particles, const matrix4* m, cpu_sph* sph);
void sph_render_draw_surface(sph_render* r, float threshold);

void render_as_point_sprites(sph_render* r, const vector3* pos, const vector3* normal, const sph_neighbour_list* nlist, int n_particles);


#endif // _SPH_RENDER_H_