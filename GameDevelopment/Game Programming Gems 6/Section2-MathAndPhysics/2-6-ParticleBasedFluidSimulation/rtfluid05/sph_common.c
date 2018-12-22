#include <memory.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include "gpu_util.h"

#include "sph_common.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))


void sph_mem_pool_create(sph_mem_pool* pool, int size)
{
	pool->capacity = size;
	pool->prealloc = SPH_MALLOC(size);
	pool->used_size = 0;
}

void* sph_mem_pool_alloc(sph_mem_pool* pool, int size)
{
	void* ret;
	if (size + pool->used_size > pool->capacity)
	{
		gput_printf("sph_mem_pool_alloc: Out of memory pool\n");
	}

	ret = &(((char*)pool->prealloc)[pool->used_size]);
	pool->used_size += size;

	return ret;
}

void sph_mem_pool_free(sph_mem_pool* pool)
{
	pool->used_size = 0;
}

void sph_mem_pool_destroy(sph_mem_pool* pool)
{
}

/*
void sph_mem_pool_ensure(sph_mem_pool* pool, int size)
{
}
*/

vector3* alloc_vec3array(int size)
{
	return (vector3*)malloc(size*sizeof(vector3));
}

void free_vec3array(vector3* a)
{
	free(a);
}

/*
int* sph_grid_malloc(sph_grid* g, int size)
{
}
*/

void sph_neighbour_list_create(sph_neighbour_list* l, int n_particles)
{
	l->sizes = (int*)SPH_MALLOC(n_particles*sizeof(int));
	l->pool = (void*)SPH_MALLOC(250*n_particles*sizeof(sph_neighbour));
	//sph_mem_pool_create(&l->pool, 120*n_particles*sizeof(sph_neighbour));
	l->p = (sph_neighbour**)SPH_MALLOC(n_particles*sizeof(sph_neighbour*));
}

void sph_neighbour_list_create_gpu(sph_neighbour_list* l, int n_particles)
{
	int i;
	l->sizes = (int*)SPH_MALLOC(n_particles*sizeof(int));
	l->pool = (void*)SPH_MALLOC(220*n_particles*sizeof(sph_neighbour));
	l->p = (sph_neighbour**)SPH_MALLOC(n_particles*sizeof(sph_neighbour*));
	for (i = 0; i < n_particles; i++)
	{
		l->p[i] = &((sph_neighbour*)l->pool)[i*220];
	}
}

void sph_grid_create(sph_grid* g, int n_particles, float grid_len)
{
	int i;
	int g_size;

	g->capacity = 40*40*40;
	g_size = g->capacity;

//	g->sizes = (int*)malloc(g_size*sizeof(int));
	g->sizes = (int*)malloc(g_size*sizeof(particle_index));
	g->caps = (int*)malloc(g_size*sizeof(int));

	g->particles = (int**)malloc(g_size*sizeof(int*));
	
	//g->particles[0] = (int*)malloc(INITIAL_GRID_CAPACITY*g_size*sizeof(int));

	for (i = 1; i < g_size; i++)
	{
		//g->particles[i] = &(g->particles[0][i*INITIAL_GRID_CAPACITY]);
		g->caps[i] = 0;//INITIAL_GRID_CAPACITY;
	}

	g->grid_len = grid_len;

	sph_mem_pool_create(&g->mempool, sizeof(particle_index)*80*80*INITIAL_GRID_CAPACITY);

#ifdef EXP_SORT
	indices = (int*)malloc(n_particles*sizeof(int));
#endif
}

void sph_grid_destroy(sph_grid* g)
{
	free(g->particles[0]);
	free(g->particles);
	free(g->sizes);
	free(g->caps);
}


/**
 * Clear grids hash and resize grids from the distribution of particles.
 */
void sph_grid_clear(sph_grid* g, const vector3* pos, int start, int end)
{
	float fmin_x;
	float fmax_x;
	float fmin_y;
	float fmax_y;
	float fmin_z;
	float fmax_z;
	int i;

	fmin_x = fmin_y = fmin_z = MAX_FLOAT;//1000;
	fmax_x = fmax_y = fmax_z = -MAX_FLOAT;//-1000;
	for (i = start; i <= end; i++)
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

	g->width  = (int)((fmax_x - fmin_x + g->grid_len)/g->grid_len);
	g->height = (int)((fmax_y - fmin_y + g->grid_len)/g->grid_len);
	g->depth  = (int)((fmax_z - fmin_z + g->grid_len)/g->grid_len);

	g->minx = fmin_x;
	g->miny = fmin_y;
	g->minz = fmin_z;

	if (g->width*g->height*g->depth > g->capacity)
	{
		printf("Out of Capacity\n");
	}

	memset(g->sizes, 0, g->width*g->height*g->depth*sizeof(particle_index));
	memset(g->caps, 0, g->width*g->height*g->depth*sizeof(int));
	sph_mem_pool_free(&g->mempool);
}

/**
 * Allocate particles to grids
 */
void sph_grid_alloc(sph_grid* g, const vector3* pos, int start, int end)
{
	int i;

	float inv_glen = 1.0f/g->grid_len;

	for (i = start; i <= end; i++)
	{
		int gx;
		int gy;
		int gz;
		int gindex;
/*
		n_list->p[i] = &((sph_neighbour*)n_list->pool)[n_poolused];
		n_list->p[i][0].index = i;
		n_list->p[i][0].distsq = 0;
		n_list->sizes[i] = 1;
		*/

		gx = (int)((pos[i].x - g->minx) *inv_glen);
		gy = (int)((pos[i].y - g->miny) *inv_glen);
		gz = (int)((pos[i].z - g->minz) *inv_glen);
		
		gindex = gx + gy*g->width + gz*g->width*g->height;

		if (g->sizes[gindex] == g->caps[gindex])
		{
			g->particles[gindex] = (particle_index*)sph_mem_pool_alloc(&g->mempool, 50*sizeof(particle_index));
			g->caps[gindex] = 50;
			//gput_printf("sph_grid_get_neighbours: Out of Grid Capacity\n");
			//exit(1);
		}

//		gput_printf("%d %d %d\n", gx, gy, gz);

		g->particles[gindex][g->sizes[gindex]] = i;
		g->sizes[gindex]++;
	}
}


void sph_grid_get_neighbours(sph_grid* g, const vector3* pos, int start, int end,
							 sph_neighbour_list* n_list, float search_radius)
{
	int i;
	int j;
	int neighbour_grid;
	int gx;
	int gy;
	int gz;
	int gindex; /* grid index */
	int pindex; /* particle index */
	float search_radius2;

	float inv_glen;

	//int n_poolused; /** Used size of memory pool of neighbour list **/

	n_list->n_poolused = n_list->used_by_rigid;
	search_radius2 = search_radius*search_radius;

	inv_glen = 1.0f/g->grid_len;

	for (i = start; i <= end; i++)
	{
		n_list->p[i] = &((sph_neighbour*)n_list->pool)[n_list->n_poolused];
		n_list->p[i][0].index = i;
		n_list->p[i][0].distsq = 0;
		n_list->sizes[i] = 1;

		gx = (int)((pos[i].x - g->minx)*inv_glen);
		gy = (int)((pos[i].y - g->miny)*inv_glen);
		gz = (int)((pos[i].z - g->minz)*inv_glen);
		
		gindex = gx + gy*g->width + gz*g->width*g->height;

		for (gz = -1; gz <= 1; gz++)
			for (gy = -1; gy <= 1; gy++)
				for (gx = -1; gx <= 1; gx++)
				{
					neighbour_grid = gindex + g->width*g->height*gz + g->width*gy + gx;
					if ((neighbour_grid < 0) || (neighbour_grid >= g->width*g->depth*g->height))
						continue;

					for (j = 0; j < g->sizes[neighbour_grid]; j++)
					{
						float distsq;

						pindex = g->particles[neighbour_grid][j];
						distsq = vec3_distsq(&pos[i], &pos[pindex]);

						if (distsq < search_radius2)
						{
							int nindex = n_list->sizes[i];
							n_list->p[i][nindex].index = pindex;
							n_list->p[i][nindex].distsq = distsq;
							n_list->sizes[i]++;
						}
					}
				}

		if (g->sizes[gindex] == g->caps[gindex])
		{
//			g->particles[gindex] = (particle_index*)sph_mem_pool_alloc(&g->mempool, 50*sizeof(particle_index));
			g->particles[gindex] = (particle_index*)sph_mem_pool_alloc(&g->mempool, 150*sizeof(particle_index));
			g->caps[gindex] = 150;
			//gput_printf("sph_grid_get_neighbours: Out of Grid Capacity\n");
			//exit(1);
		}

		n_list->n_poolused += n_list->sizes[i];
		g->particles[gindex][g->sizes[gindex]] = i;
		g->sizes[gindex]++;
	}

	//gput_printf("Num. of interactions = %d\n", n_list->n_poolused);
}




