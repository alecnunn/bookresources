/** 
 * Benchmark code for performance analysis
 *
 * Copyright (c) Takashi AMADA  All Rights Reseved
 **/


#include "sph_benchmark.h"
#include <memory.h>
#include <windows.h>

int benchmark_cpu(cpu_sph* sph, int itr, float timestep)
{
	int i;
	DWORD st;

	st = GetTickCount();
	for (i = 0; i < itr; i++)
	{
		cpu_sph_elapse(sph, timestep);
	}

	return (GetTickCount() - st);
}


int benchmark_neighbour_search_cpu(cpu_sph* sph, int itr)
{
	int i;
	DWORD st;

	st = GetTickCount();
	for (i = 0; i < itr; i++)
	{
		//sph_grid_get_neighbours(&sph->grids, sph->pos, 0, sph->fluid_start, sph->n_particles, &sph->n_list, sph->r_search);
	}

	return (GetTickCount() - st);
}


void print_benchmark(cpu_sph* sph, int itr, float timestep)
{
	float cpu_search;
	float cpu_overall;

	printf("Started SPH on CPU Benchmarking ...\n");
	cpu_search = benchmark_neighbour_search_cpu(sph, itr)/(float)itr;
	cpu_overall = benchmark_cpu(sph, itr, timestep)/(float)itr;

	printf("CPU: Overall %6.2f ms.; Search %6.2f ms.; Eq. %6.2f ms.\n",
		cpu_overall, cpu_search, cpu_overall - cpu_search);
	printf("Finished Benchmarking\n");
}
