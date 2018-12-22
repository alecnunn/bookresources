/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef double noise_vector3[3];
typedef double noise_vector2[2];


class CNoise
{
public:
    CNoise(); // need to call init
    CNoise(int size, int np);
    ~CNoise();

    // call the noise function for 1,2 the 3 dimensions
    double noise1 (double arg);
    double noise2 (double vec[2]);
    double noise3 (double vec[3]);

    double turbulence2(double v[2], double freq);
    double turbulence3(double v[3], double freq);
    double gain(double a, double b);
    double bias(double a, double b);




    void Init(int size, int np);
    void Release(); // any memory
    void init_noise();

private:
    int random ();
    void normalize2 (double v[2]);
    void normalize3 (double v[3]);

    bool m_bInitCalled;

    int B, BM, N, NP;
    
    noise_vector3 * g3;
    noise_vector2 * g2;
    double * g1;
    int *p;
};

