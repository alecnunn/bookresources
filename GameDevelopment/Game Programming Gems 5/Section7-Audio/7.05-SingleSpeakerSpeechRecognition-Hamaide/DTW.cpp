#include "DTW.h"
#include <stdio.h>
#include <math.h>

//Do not use real distance, but square one
float EuclidianDistanceSquare(float* a, float *b, int size)
{
	float temp = 0;

    for(int i = 0; i< size; i++)
	{
		temp +=  (*a - *b) * (*a - *b);
		a++; b++;
	}
	return sqrtf(temp);
}


float DTW(features *sample, int samplesize, features *reference, int referencesize)
{

	FILE *out = fopen("test.log", "w");

	//Not optimum, but for simplicity, just take the reference size has computed vectors

	float *global_dist, *prev_global_dist;
	int i,j;

	global_dist = new float[referencesize];
	prev_global_dist = new float[referencesize];


	//Init vectors
	global_dist[0] = EuclidianDistanceSquare((float*)(&sample[0]), (float*)(&reference[0]), sizeof(features)/sizeof(float));
	for(i= 1; i< referencesize; i++)
	{
		global_dist[i] = global_dist[i-1] + EuclidianDistanceSquare((float*)(&sample[0]), (float*)(&reference[i]), sizeof(features)/sizeof(float));
	}

	for(i = 0; i< referencesize;i++)
	{
		fprintf(out, "%.4f  ", global_dist[i]);
	}
	fprintf(out, "\n");

	swap(global_dist, prev_global_dist);

	for(i = 1; i< samplesize; i++)
	{
		float local_dist = EuclidianDistanceSquare((float*)(&sample[i]), (float*)(&reference[0]), sizeof(features)/sizeof(float));
		global_dist[0] = prev_global_dist[0] + local_dist;
		for(j = 1; j< referencesize; j++)
		{
			float min;
			float penalty;
			local_dist = EuclidianDistanceSquare((float*)(&sample[i]), (float*)(&reference[j]), sizeof(features)/sizeof(float));
			//Test 3 cases

			//First, the diagonal case (-1,-1)
			min = prev_global_dist[j-1];
			penalty = 1.0f;

			//now, left one (-1,0)
			if (min > prev_global_dist[j])
			{
				min=prev_global_dist[j];
				penalty = 2;
			}

			//now the bottom one

			if (min > global_dist[j-1])
			{
				min = global_dist[j-1];
				penalty = 2;
			}

			global_dist[j] = min + penalty * local_dist;

		}
		//ok, swap old one with new one


		for(j = 0; j< referencesize;j++)
		{
			fprintf(out, "%.4f  ", global_dist[j]);
		}
		fprintf(out, "\n");

		swap(prev_global_dist, global_dist);
	}

	//ok, return the score

	fclose(out);

	return prev_global_dist[referencesize-1];
}