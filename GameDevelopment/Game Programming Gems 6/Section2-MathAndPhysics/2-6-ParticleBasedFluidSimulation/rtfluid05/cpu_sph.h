#ifndef _SPH_CPU_H_
#define _SPH_CPU_H_

#include "glg.h"
#include "sph_common.h"

//#define N_RIGID (R_LEN*R_LEN*R_LEN)

/*
typedef struct
{
	vector3 p;  // position
	vector3 v;  // velocity
	vector3 vh; // velocity half timestep
	vector3 a;  // acceleration
	float m;    // mass
	float pres; // pressure
	float dens; // density
} sph_particle;
*/

typedef struct
{
	int start;
	int end;
	matrix4 rot; // Rotation matrix
	vector3 rg;  // Center of gravity
} sph_rigid_body;

typedef struct
{
	// Global parametesr of fluid
	float viscosity; // Viscosity of fluid
	float smoothlen; // Smoothing length
	float timestep;  // Timestep
	float stiff;     // Stiffness
	int n_particles; // Number of particles;
	float r_search;  // Search radius of neighbour list

	// Region of particles attribures
	// 0 -> (fluid_start - 1)           : rigid body particles
	// fluid_start -> (n_particles - 1) : fluid particles
	int* rigid_start;  // Primary index of rigid body i particles
	int* n_rigidp;     // Number of rigid body i particles
	int n_rigid;       // Number of rigid bodies

	int fluid_start;   // Primary index of fluid particle
	int n_fluidp;      // Number of fluid particles

	// Attributes of particles
	vector3* normal;   // Normal
	vector3* pos;      // Position
	vector3* vel;      // Velocity
	vector3* vel_half; // Velocity half steps forwarded
	vector3* acc;      // Acceleration
	float* curvature;  // Curvature at particle location
	float* density;    // Density and its reciprocal at particle location
	float* pressure;   // Pressure at particle location
	float* mass;       // Mass of particle

	sph_neighbour_list n_list; // Neighbour list

	sph_grid grid; // Grid for neighbour search

	int n_loops; /* n_loops equation-solving for 1 neighbour search performed */


	// For rigid body test
	vector3* init_pos;
	vector3 c; // translation
//	vector3 r; // rotation
	vector3 rg0;
	vector3 q;
	matrix4 r_rot;
	//vector3 a0;
	//vector3 a1;

	// Coefficients for kernel
	float poly6_coef;
	float lap_poly6_coef;
	float grad_poly6_coef;
	float grad_spiky_coef;
	float lap_vis_coef;

	// Collision handling
	matrix4 mat_col;
	matrix4 mat_inv_col;

//	int n_rigid;
} cpu_sph;

/**
 * Create sph simulation context
 */
void cpu_sph_create(cpu_sph* sph, 
					int size, const vector3* pos, const vector3* vel,
					float smoothlen, float viscosity, float mass, float stiff, float search_radius, int n_loops);


void cpu_sph_destroy(cpu_sph* sph);
void cpu_sph_check_overlap(cpu_sph* sph);
void cpu_sph_elapse(cpu_sph* sph, float t);
void cpu_sph_transform_obstacles(cpu_sph* sph, const matrix4* m);

void cpu_sph_get_pos(cpu_sph* sph, vector3* pos);

void cpu_sph_init_rigid_body(cpu_sph* sph);

#endif
