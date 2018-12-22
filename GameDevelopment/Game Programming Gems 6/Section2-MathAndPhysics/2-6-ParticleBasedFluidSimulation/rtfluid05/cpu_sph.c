#include <memory.h>
#include <math.h>

#include <windows.h>
#include <assert.h>

#include "cpu_sph.h"
#include "sph_common.h"

//#define POOR_WATER
#define GLASS
//#define REFLECT_VEL
//#define PUSHOUT
#define RADIUS 0.03f
#define EPSILON 0.00001f

const float PI = 3.1415926535f;

#define RIGID_BODY
#ifdef RIGID_BODY
#define FOR_EACH_RIGID(r, i) for (i = sph->rigid_start[(r)]; i < sph->rigid_start[(r)] + sph->n_rigidp[(r)]; i++)
#else
#define FOR_EACH_RIGID(r, i) if (0)
#endif
#define FOR_EACH_FLUID(i) for (i = sph->fluid_start; i < sph->fluid_start + sph->n_fluidp; i++)
//#define FOR_EACH_NEIGHBOURS(i, j) Ffor (j = 1; j < nlist->sizes[i]; j++)

// test
#ifdef RIGID_BODY
#define R_LEN 4
#define R_HEIGHT 13
//#define R_LEN 4
//#define R_HEIGHT 12
//#define RIGID_MASS 3.0f*mass
//#define RIGID_DENSITY 3000
#define RIGID_MASS 0.4f*mass
#define RIGID_DENSITY 300
#else
#define R_LEN 0
#define R_HEIGHT 0
#endif


#define POS(i) sph->pos[(i)]
#define VEL(i) sph->vel[(i)]
#define ACC(i) sph->acc[(i)]
#define VELH(i) sph->vel_half[(i)]

void cpu_sph_create(cpu_sph* sph, 
					int size, const vector3* pos, const vector3* vel,
					float smoothlen, float viscosity, float mass, float stiff, float r_search, int n_loops)
{
	int i;
	float h = smoothlen;
	float* mem;
	sph->viscosity = viscosity;
	sph->n_particles = size;
	sph->smoothlen = smoothlen;
	sph->n_loops = n_loops;
	sph->stiff = stiff;

	sph_neighbour_list_create(&sph->n_list, size);

	sph_grid_create(&sph->grid, sph->n_particles, h);
	sph->grid.grid_len = r_search;
	sph->r_search = r_search;

	// Allocate memory for attributes of particles
	mem = SPH_MALLOC(49*size*sizeof(float));
	sph->mass = (float*)&mem[0];
	sph->density = (float*)&mem[size];
	sph->pressure = (float*)&mem[2*size];
	sph->pos = (vector3*)&mem[3*size];
	sph->vel = (vector3*)&mem[6*size];
	sph->vel_half = (vector3*)&mem[9*size];
	sph->acc = (vector3*)&mem[12*size];
	sph->normal = (vector3*)&mem[15*size];
	sph->curvature = (float*)&mem[18*size];

	// Set the memory domain of fluid and rigid body
	sph->n_rigid = 0;
#ifdef RIGID_BODY
	sph->n_rigid = 1;
	sph->rigid_start = (int*)SPH_MALLOC(sph->n_rigid*sizeof(int));
	sph->rigid_start[0] = 0;
	sph->n_rigidp = (int*)SPH_MALLOC(sph->n_rigid*sizeof(int));
	sph->n_rigidp[0] = R_LEN*R_LEN*R_HEIGHT;
#endif
//	sph->n_rigid = 0;*/
	sph->fluid_start = R_LEN*R_LEN*R_HEIGHT;
	sph->n_fluidp = size - sph->fluid_start;

	// Initialize attributes of particles
//	for (i = 0; i < size; i++)
#ifdef RIGID_BODY
	FOR_EACH_RIGID(0, i)
	{
		//gput_printf("rigid: %d\n", i);
		//sph->mass[i] = 0.4f*mass;
		sph->mass[i] = RIGID_MASS;//2.0f*mass;
	}
#endif
	FOR_EACH_FLUID(i)
		sph->mass[i] = mass;
	memcpy(sph->pos, pos, sph->n_particles*sizeof(vector3));
	memcpy(sph->vel, vel, sph->n_particles*sizeof(vector3));
	memcpy(sph->vel_half, vel, sph->n_particles*sizeof(vector3));

	// Precompute kernel coefficients
	sph->poly6_coef = 315.0f/(64.0f*PI*(float)pow(h, 9));
	sph->grad_poly6_coef = 945.0f/(32.0f*PI*(float)pow(h, 9));
	sph->lap_poly6_coef = 945.0f/(32.0f*PI*(float)pow(h, 9));
	sph->grad_spiky_coef = -45.0f/(PI*h*h*h*h*h*h);
	sph->lap_vis_coef = 45.0f/(PI*h*h*h*h*h*h);

	//gput_printf("%f\n", mass*sph->poly6_coef/8e10f*0.6f);

	/**** FOR TEST ****/
	mat4_set_identity(&sph->mat_col);

#ifdef RIGID_BODY
	cpu_sph_init_rigid_body(sph);
#endif

	sph_grid_clear(&sph->grid, sph->pos, 0, sph->n_particles - 1);
	sph->n_list.used_by_rigid = 0;
	sph_grid_get_neighbours(&sph->grid, sph->pos, 0, sph->fluid_start, &sph->n_list, sph->r_search);
	sph->n_list.used_by_rigid = sph->n_list.n_poolused;
//	sph_grid_get_neighbours(&sph->grid, sph->pos, 0, sph->n_particles - 1, &sph->n_list, sph->r_search);


}

void cpu_sph_destroy(cpu_sph* sph)
{
}


void cpu_sph_init_rigid_body(cpu_sph* sph)
{
	int x, y, z, i;
	//float interval = 0.007f;
	float interval = 0.005f;

	float cx = 0.02f;
	float cy = 0.02f;

	//float cx = 0.02f;
	//float cy = 0.02f;
	vector3 rg;

	for (x = 0; x < R_LEN; x++)
		for (y = 0; y < R_LEN; y++)
			for (z = 0; z < R_HEIGHT; z++)
			{
				vec3_set(&POS(x + y*R_LEN + z*R_LEN*R_LEN), 
					interval*x + cx, interval*y + cy, interval*z - 0.01f);
				vec3_set(&VEL(x + y*R_LEN + z*R_LEN*R_LEN), 0.0f, 0.0f, 0.0f);
				vec3_set(&VELH(x + y*R_LEN + z*R_LEN*R_LEN), 0.0f, 0.0f, 0.0f);
				vec3_set(&ACC(x + y*R_LEN + z*R_LEN*R_LEN), 0.0f, 0.0f, 0.0f);
			}

	sph->init_pos = (vector3*)SPH_MALLOC(sph->n_rigidp[0]*sizeof(vector3));
	memcpy(sph->init_pos, sph->pos, sph->n_rigidp[0]*sizeof(vector3));

	vec3_set(&rg, 0, 0, 0);
	FOR_EACH_RIGID(0, i)
		vec3_add(&rg, &rg, &POS(i));
	vec3_scale(&rg, 1.0f/sph->n_rigidp[0], &rg);
	vec3_copy(&sph->c, &rg);
	vec3_copy(&sph->rg0, &rg);

	mat4_set_identity(&sph->r_rot);
	mat4_set_rotate(&sph->r_rot, 2, 0, 1, 0);
}

void cpu_sph_transform_obstacles(cpu_sph* sph, const matrix4* m)
{
	mat4_mul(&sph->mat_col, m, &sph->mat_col);
	mat4_invert(&sph->mat_inv_col, &sph->mat_col);
}

GLG_INLINE float poly6_kernel(float lensq, float h) //, float k)
{
	const float PI = 3.1415926535f;
	float h2_r2;

	h2_r2 = h*h -lensq;

	if (h2_r2 > 0.0f)
		return h2_r2*h2_r2*h2_r2;
//		return k*h2_r2*h2_r2*h2_r2;
//		return 315.0f/(64.0f*PI*pow(h, 9))*h2_r2*h2_r2*h2_r2;
	return 0.0f;
}

GLG_INLINE void grad_poly6_kernel(vector3* result, const vector3* p0, const vector3* p1, float h)
{
	float distsq;
	vector3 diff;
	float h2_r2;

	vec3_sub(&diff, p0, p1);
	distsq = vec3_lensq(&diff);
	h2_r2 = h*h - distsq;

	if (h2_r2 < 0.0f)
	{
		vec3_set(result, 0.0f, 0.0f, 0.0f);
		return;
	}

	distsq = vec3_distsq(p0, p1);

//	vec3_copy(result, &diff);
	vec3_scale(result, -945.0f/(32.0f*PI*(float)pow(h, 9))*h2_r2*h2_r2, &diff);
}


GLG_INLINE float lap_viscosity_kernel(float r, float h, float k)
{
	const float PI = 3.1415926535f;

	if (h > r)
		return k*(h - r);
//		return 45.0f/(PI*h*h*h*h*h*h)*(h - r);
	return 0.0f;
}

void cpu_sph_compute_density(cpu_sph* sph, int first)
{
	int i;
	int j;

	int nindex;
	sph_neighbour_list* nlist;

	memset(sph->density, 0, sph->n_particles*sizeof(float));

	nlist = &sph->n_list;

	if (first)
	{
		for (i = 0; i < sph->n_particles; i++)
		{
			for (j = 0; j < nlist->sizes[i]; j++)
			{
				float density;
				float distsq;
				float h2 = sph->smoothlen*sph->smoothlen;

				nindex = nlist->p[i][j].index;
				distsq = nlist->p[i][j].distsq;

				if (h2 > distsq)
				{
					float h2_r2;
					h2_r2 = h2 - distsq;
//					density = sph->mass[nindex]*h2_r2*h2_r2*h2_r2;
					density = h2_r2*h2_r2*h2_r2;
					sph->density[i] += sph->mass[nindex]*density;
					if (i != nindex)
						sph->density[nindex] += sph->mass[i]*density;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < sph->n_particles; i++)
		{
			for (j = 0; j < nlist->sizes[i]; j++)
			{
				float density;
				float distsq;
				float h2 = sph->smoothlen*sph->smoothlen;

				nindex = nlist->p[i][j].index;
				distsq = vec3_distsq(&POS(i), &POS(nindex));
				nlist->p[i][j].distsq = distsq;

				if (h2 > distsq)
				{
					float h2_r2;
					h2_r2 = h2 - distsq;
//					density = sph->mass[nindex]*h2_r2*h2_r2*h2_r2;
					density = h2_r2*h2_r2*h2_r2;
					sph->density[i] += sph->mass[nindex]*density;
					if (i != nindex)
						sph->density[nindex] += sph->mass[i]*density;
				}
			}
		}
	}

#ifdef RIGID_BODY
	FOR_EACH_RIGID(0, i)
	{
		sph->density[i] *= sph->poly6_coef;
	//	sph->density[i] = RIGID_DENSITY;
		//if (i == 0)
			//gput_printf("rho=%f\n", sph->density[i]);
		//sph->pressure[i] = sph->repulsive*(sph->density[i] - RIGID_DENSITY);
		sph->pressure[i] = 1.0f*sph->stiff*max(sph->density[i] - RIGID_DENSITY, 0.0f);
		//sph->pressure[i] = 0.0f;
		sph->density[i] = 1.0f/sph->density[i];
	}
#endif

	FOR_EACH_FLUID(i)
	//for (i = 0; i < sph->n_particles; i++)
	{
		sph->density[i] *= sph->poly6_coef;
		sph->pressure[i] = sph->stiff*(sph->density[i] - 1000.0f);//, 0.0f);
		//sph->pressure[i] = sph->repulsive*max(sph->density[i] - 1000.0f, 0.0f);
		sph->density[i] = 1.0f/sph->density[i];
	}
}


void cpu_sph_compute_force(cpu_sph* sph)
{
	int i;
	int j;
	float h;

	vector3 vdiff;

	sph_neighbour_list* nlist;

	nlist = &sph->n_list;


	h = sph->smoothlen;
	memset(sph->acc, 0, sph->n_particles*sizeof(vector3));


	FOR_EACH_FLUID(i)
		for (j = 1; j < nlist->sizes[i]; j++)
		{
			vector3 force;
			float r;
			int nindex;

			nindex = nlist->p[i][j].index;

			r = glg_sqrt(nlist->p[i][j].distsq);//r);

			
			if (r < h)
			{
				float h_r;

				vector3 diff;

#ifdef RIGID_BODY
                float pres;
#endif
				// Compute force due to pressure and viscosity
				// force = m_j*(h - r)/rho_i/rho_j*(-0.5*(p_i + p_j)*grad_spiky_coef*(h - r)/r + (v_j - v_i)*mu*lap_vis_coef)
				h_r = h - r;
				vec3_sub(&diff, &POS(i), &POS(nindex));

#ifdef RIGID_BODY
				if ((i < sph->n_rigidp[0]) || (nindex < sph->n_rigidp[0]))
					pres = max(sph->pressure[i], 0.0f) + max(sph->pressure[nindex], 0.0f);
				else pres = sph->pressure[i] + sph->pressure[nindex];
				vec3_scale(&force, -0.5f*pres*sph->grad_spiky_coef*h_r/r, &diff);
#else
				vec3_scale(&force, -0.5f*(sph->pressure[i] + sph->pressure[nindex])*sph->grad_spiky_coef*h_r/r, &diff);
#endif
				vec3_sub(&vdiff, &VEL(nindex), &VEL(i));

/*
				if ((i < sph->n_rigidp[0]) || (nindex < sph->n_rigidp[0]))
				vec3_scale(&vdiff, 0.0f*sph->viscosity*sph->lap_vis_coef, &vdiff);
				else vec3_scale(&vdiff, sph->viscosity*sph->lap_vis_coef, &vdiff);
				*/
vec3_scale(&vdiff, sph->viscosity*sph->lap_vis_coef, &vdiff);

				vec3_add(&force, &force, &vdiff);


				vec3_scale(&force, h_r*sph->density[i]*sph->density[nindex], &force);

				// Apply force
				vec3_scaleadd(&ACC(i), &ACC(i), sph->mass[nindex], &force);
				vec3_scaleadd(&ACC(nindex), &ACC(nindex), -sph->mass[i], &force);

			}
		}

}


_inline void compute_col(vector3* p, vector3* col,
						 const vector3* vel, const vector3* n, 
						 float diff, float stiff, float damp)
{
	float v0;
	float reverse;

	v0 = vec3_dot(n, vel);
#ifdef PUSHOUT
	//mat4_mulvec3(&n_w, &mat_col, &n);
	vec3_scaleadd(p, p, diff, n);
#endif
#ifdef REFLECT_VEL
	vec3_scaleadd(col, col, -1.9*v0, n);
#else
	reverse = stiff*diff - damp*v0;

	vec3_scaleadd(col, col, reverse, n);
#endif
}
/**
 * col is ref_v if REFLECT_VEL is defined, col_force if REFLECT_VEL is not defined
 */


_inline void cpu_sph_glass_collision(vector3* p, vector3* col, const vector3* vel,
									 const matrix4* mat, const matrix4* mat_inv, 
									 float radius, float stiff, float damp)
{
	float GLASS_R = 0.05f;
	//		float GLASS_R = 0.045;
	float GLASS_BOTTOM = -0.08f;
#ifdef POOR_WATER
	float GLASS_TOP = 0.04f;
#else
	float GLASS_TOP = 0.06f;
	//float GLASS_TOP = 0.18;
	//float GLASS_TOP = 0.26;
#endif
	float GLASS_THICKNESS = 0.01f;

	vector3 p_col;
	vector3 n;
	float diff; /** Distance between object and fluid particle **/

	mat4_mulvec3(&p_col, mat_inv, p);

	diff =
        2.0f*radius - (GLASS_R - (float)sqrt(p_col.x*p_col.x + p_col.y*p_col.y));

	if (((diff < 8.0f*radius) && (diff > EPSILON)) && (p_col.z < GLASS_TOP))
	{
		vec3_set(&n, -p_col.x, -p_col.y, 0.0f);
		vec3_normalize(&n, &n);

		mat4_mulvec3_as_mat3(&n, mat, &n);
		compute_col(p, col, vel, &n, diff, stiff, damp);
	}

	diff = 2.0f*radius - (p_col.z - GLASS_BOTTOM);

	if (diff > EPSILON)
	{
		vec3_set(&n, 0.0f, 0.0f, 1.0f);

		mat4_mulvec3_as_mat3(&n, mat, &n);
		compute_col(p, col, vel, &n, diff, stiff, damp);
	}

#ifndef POOR_WATER
	diff = 2.0f*radius - (GLASS_TOP - p_col.z);

	if (diff > EPSILON)
	{
		vec3_set(&n, 0.0f, 0.0f, -1.0f);;

		mat4_mulvec3_as_mat3(&n, mat, &n);
		compute_col(p, col, vel, &n, diff, stiff, damp);
	}
#endif
}

void cpu_sph_process_collision(cpu_sph* sph)
{
	int i;
	float e;
	float sphere_radius;
	float stiff;
	float damp;
//	const float EPSILON = 0.000001f;

//	stiff = 5000.0f;
	//stiff = 10000.0f;
	stiff = 30000.0f;
	damp = 128.0f;
	//damp = 64.0f;
	
	//sphere_radius = RADIUS;
	sphere_radius = 0.004f;
	e = 1.0f;


	for (i = 0; i < sph->n_particles; i++)
	{

		//float BOX_W = 0.08f; /** Width of box **/
		//float BOX_BOTTOM = -0.13f; /** Bottom of box **/
		float BOX_W = 0.065f; /** Width of box **/
		float BOX_BOTTOM = -0.08f; /** Bottom of box **/
//		_ALIGNED vector3 p; /** Position in collision coordinate **/
		//_ALIGNED vector3 vel; /** Velocity in collision coordinate **/

//        _ALIGNED vector3 n_w; /** Normal in world coordinate **/
		_ALIGNED vector3 pre_p; /** Predicted pos **/
		vector3 col;

#ifdef POOR_WATER
		vector3 n;
		float diff;
#endif

		vec3_set(&col, 0.0f, 0.0f, 0.0f);
		vec3_scaleadd(&pre_p, &POS(i), sph->timestep, &VELH(i));

#ifdef RIGID_BODY
		if (i < sph->n_rigidp[0])
			stiff = 50000.0f;
		else stiff = 30000.0f;
#endif

#ifdef GLASS
		cpu_sph_glass_collision(&pre_p, &col, &VEL(i),
			&sph->mat_col, &sph->mat_inv_col, sphere_radius, stiff, damp);
#endif
	

		/** Box **/
#ifdef POOR_WATER
		diff = 2.0f*sphere_radius - (pre_p.z - BOX_BOTTOM);
		if (diff > EPSILON)
		{
			vec3_set(&n, 0.0f, 0.0f, 1.0f);
			compute_col(&pre_p, &col, &VEL(i), &n, diff, stiff, damp);
		}

		diff = 2.0f*sphere_radius - (BOX_W - pre_p.x);
		if (diff > EPSILON)
		{
			vec3_set(&n, -1.0f, 0.0f, 0.0f);
			compute_col(&pre_p, &col, &VEL(i), &n, diff, stiff, damp);
		}

		diff = 2.0f*sphere_radius - (BOX_W + pre_p.x);
		if (diff > EPSILON)
		{
			vec3_set(&n, 1.0f, 0.0f, 0.0f);
			compute_col(&pre_p, &col, &VEL(i), &n, diff, stiff, damp);
		}

		diff = 2.0f*sphere_radius - (BOX_W - pre_p.y);
		if (diff > EPSILON)
		{
			vec3_set(&n, 0.0f, -1.0f, 0.0f);
			compute_col(&pre_p, &col, &VEL(i), &n, diff, stiff, damp);
		}

		diff = 2.0f*sphere_radius - (BOX_W + pre_p.y);
		if (diff > EPSILON)
		{
			vec3_set(&n, 0.0f, 1.0f, 0.0f);
			compute_col(&pre_p, &col, &VEL(i), &n, diff, stiff, damp);
		}
#endif

	
		vec3_add(&ACC(i), &ACC(i), &col);

	}

	return;
}


#ifdef RIGID_BODY
void cpu_sph_compute_rigid_body_motion(cpu_sph* sph, float t)
{
	int i;
	vector3 rg;
	vector3 T;
	vector3 R;
	vector3* q;
	vector3* temp_vel;
	vector3 g;
	float I;

	vec3_set(&g, 0.0f, 0.0f, -9.8f);

	q = (vector3*)malloc(sph->n_rigidp[0]*sizeof(vector3));
	temp_vel = (vector3*)malloc(sph->n_rigidp[0]*sizeof(vector3));

	// Compute new velocity
	FOR_EACH_RIGID(0, i)
	{
		_ALIGNED vector3 final_acc;

		vec3_add(&final_acc, &sph->acc[i], &g);
		vec3_scaleadd(&temp_vel[i], &VELH(i), t, &final_acc);
	}
	

	// Compute translational velocity
	vec3_set(&T, 0, 0, 0);
	FOR_EACH_RIGID(0, i)
		vec3_add(&T, &T, &temp_vel[i]);
	vec3_scale(&T, 1.0f/sph->n_rigidp[0], &T);
	//vec3_scaleadd(&T, &T, t, &g);

	// Update translational velocity
	//vec3_scaleadd(&sph->v_trans_half, &sph->v_trans_half, t, &T);

	// Compute center of gravity 
	vec3_set(&rg, 0.0f, 0.0f, 0.0f);
	FOR_EACH_RIGID(0, i)
		vec3_add(&rg, &rg, &POS(i));
	vec3_scale(&rg, 1.0f/sph->n_rigidp[0], &rg);

	// Compute angular velocity
	FOR_EACH_RIGID(0, i)
		vec3_sub(&q[i], &POS(i), &rg);

	I = 0;
	vec3_set(&R, 0.0f, 0.0f, 0.0f);
	FOR_EACH_RIGID(0, i)
	{
		vector3 c;
		I += vec3_lensq(&q[i]);

//		vec3_cross(&c, &temp_vel[i], &q[i]);
		vec3_cross(&c, &q[i], &temp_vel[i]);
		vec3_add(&R, &R, &c);
	}
	vec3_scale(&R, 1.0f/I, &R);

	
	// Compute rotation matrix
	{
		vector3 axis;
		matrix4 dr;
		float r = vec3_len(&R);
		vec3_scale(&axis, 1.0f/r, &R);
		mat4_set_rotate(&dr, t*r/M_PI*180.0f, axis.x, axis.y, axis.z);
		mat4_mul(&sph->r_rot, &dr, &sph->r_rot);
	}

	
	// Update velocity of each rigid body particles
	FOR_EACH_RIGID(0, i)
	{
		_ALIGNED vector3 v_half;

//		vec3_cross(&v_half, &q[i], &R);
		vec3_cross(&v_half, &R, &q[i]);
		vec3_add(&v_half, &v_half, &T);

		//vec3_scaleadd(&sph->pos[i], &sph->pos[i], t, &v_half);

		vec3_add(&VEL(i), &VELH(i), &v_half);
		vec3_scale(&VEL(i), 0.5f, &VEL(i));

		vec3_copy(&VELH(i), &v_half);
	}

	vec3_scaleadd(&rg, &rg, t, &T);

	
	FOR_EACH_RIGID(0, i)
	{
		_ALIGNED vector3 tempp;

		vec3_sub(&tempp, &sph->init_pos[i], &sph->rg0);
		mat4_mulvec3_as_mat3(&tempp, &sph->r_rot, &tempp);
		vec3_add(&POS(i), &tempp, &rg);
	}
	
	

	//vec3_scaleadd(&sph->r, &sph->r, t, &sph->w_half);
	vec3_copy(&sph->c, &rg);


	free(temp_vel);
	free(q);
}
#endif // RIGID_BODY



void cpu_sph_elapse(cpu_sph* sph, float t)
{
	int i;
	int j;
	_ALIGNED vector3 gravity;

	sph->timestep = t;

	vec3_set(&gravity, 0.0f, 0.0f, -9.8f);

	for (j = 0; j < sph->n_loops; j++)
	{
		if (j == 0)
		{
			sph_grid_clear(&sph->grid, sph->pos, 0, sph->n_particles - 1);
			sph_grid_alloc(&sph->grid, sph->pos, 0, sph->fluid_start - 1);
			sph_grid_get_neighbours(&sph->grid, sph->pos, sph->fluid_start, sph->n_particles - 1, &sph->n_list, sph->r_search);
		}


		cpu_sph_compute_density(sph, (j == 0));
		cpu_sph_compute_force(sph);
		cpu_sph_process_collision(sph);

#ifdef RIGID_BODY
		cpu_sph_compute_rigid_body_motion(sph, t);
#endif
		FOR_EACH_FLUID(i)
		{
			_ALIGNED vector3 v_half;
			_ALIGNED vector3 final_acc;

			vec3_add(&final_acc, &sph->acc[i], &gravity);

			vec3_scaleadd(&v_half, &VELH(i), t, &final_acc);
			vec3_scaleadd(&POS(i), &POS(i), t, &v_half);

			vec3_add(&VEL(i), &VELH(i), &v_half);
			vec3_scale(&VEL(i), 0.5f, &VEL(i));

			vec3_copy(&VELH(i), &v_half);
		}
	}
}

void cpu_sph_get_pos(cpu_sph* sph, vector3* pos)
{
	memcpy(pos, sph->pos, sph->n_particles*sizeof(vector3));
}