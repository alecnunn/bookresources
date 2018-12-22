#ifndef _SPH_BENCHMARK_H_
#define _SPH_BENCHMARK_H_

#include "cpu_sph.h"
#include "gpu_util.h"

int benchmark_cpu(cpu_sph* sph, int itr, float timestep);
int benchmark_neighbour_search_cpu(cpu_sph* sph, int itr);
void print_benchmark(cpu_sph* sph, int itr, float timestep);

#endif