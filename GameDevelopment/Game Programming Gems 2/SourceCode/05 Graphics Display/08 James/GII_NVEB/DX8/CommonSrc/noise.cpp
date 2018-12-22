/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
/* coherent noise function over 1, 2 or 3 dimensions */
/* (derived from code posted by Ken Perlin) 

  http://www.noisemachine.com/talk1/index.html

*/

#include <noise.h>



/*
    generates a square noise function
    noise functions return a value from -1 to +1

*/


CNoise::CNoise()
{
    //requires Init to be called
    m_bInitCalled = false;

    B= 0;
    BM=0; 
    N=0;
    NP = 0;
    
    g3 = 0;
    g2 = 0;
    g1 = 0;
    p = 0;

}

// allows calling int multiple times
// pass in size for square sampling and power (12 for example)
void CNoise::Init(int size, int np)
{
    m_bInitCalled = true;

    if ((B != size) || (NP != np))
    {
        Release();
    
        B = size;
        BM = B - 1;
     
        NP = np;
        N = (int)pow(2, np);
        
        p = new int[B + B + 2];
        g3 = new noise_vector3[B + B + 2];
        g2 = new noise_vector2[B + B + 2];
        g1 = new double [B + B + 2];
    }    

    init_noise();

}

void CNoise::Release()
{

    B = 0;
    BM = 0; 
    N = 0;
    NP = 0;
    
    if (g3)
        delete [] g3;
    g3 = 0;

    if (g2)
        delete [] g2;
    g2 = 0;

    if (g1)
        delete [] g1;
    g1 = 0;

    if (p)
        delete [] p;
    p = 0;
    
}

CNoise::CNoise(int size, int np)
{
    B = 0;
    BM = 0; 
    N = 0;
    NP = 0;
    
    g3 = 0;
    g2 = 0;
    g1 = 0;
    p = 0;

    Init(size, np);

}


CNoise::~CNoise()
{
    delete [] p;

    delete [] g3;
    delete [] g2;
    delete [] g1;
}



int CNoise::random ()
{
    return rand ();
}

#define s_curve(t) ( t * t * (3. - 2. * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;

/*
   1D Noise function
*/

double CNoise::noise1 (double arg)
{
    int bx0, bx1;
    double rx0, rx1, sx, t, u, v, vec[1];

    vec[0] = arg;

    if (m_bInitCalled == false)
        return 0;


    setup (0, bx0, bx1, rx0, rx1);

    sx = s_curve (rx0);

    u = rx0 * g1[p[bx0]];
    v = rx1 * g1[p[bx1]];

    return lerp (sx, u, v);
}

/*
   2D Noise function
*/

double CNoise::noise2 (double vec[2])
{
    int bx0, bx1, by0, by1, b00, b10, b01, b11;
    double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
    register i, j;

    if (m_bInitCalled == false)
        return 0;

    setup (0, bx0, bx1, rx0, rx1);
    setup (1, by0, by1, ry0, ry1);

    i = p[bx0];
    j = p[bx1];

    b00 = p[i + by0];
    b10 = p[j + by0];
    b01 = p[i + by1];
    b11 = p[j + by1];

    sx = s_curve (rx0);
    sy = s_curve (ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

    q = g2[b00];
    u = at2 (rx0, ry0);
    q = g2[b10];
    v = at2 (rx1, ry0);
    a = lerp (sx, u, v);

    q = g2[b01];
    u = at2 (rx0, ry1);
    q = g2[b11];
    v = at2 (rx1, ry1);
    b = lerp (sx, u, v);

    return lerp (sy, a, b);
}

/*
   3D Noise function
*/
double CNoise::noise3 (double vec[3])
{
    int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
    double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
    register i, j;

    if (m_bInitCalled == false)
        return 0;

    setup (0, bx0, bx1, rx0, rx1);
    setup (1, by0, by1, ry0, ry1);
    setup (2, bz0, bz1, rz0, rz1);

    i = p[bx0];
    j = p[bx1];

    b00 = p[i + by0];
    b10 = p[j + by0];
    b01 = p[i + by1];
    b11 = p[j + by1];

    t = s_curve (rx0);
    sy = s_curve (ry0);
    sz = s_curve (rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

    q = g3[b00 + bz0];
    u = at3 (rx0, ry0, rz0);
    q = g3[b10 + bz0];
    v = at3 (rx1, ry0, rz0);
    a = lerp (t, u, v);

    q = g3[b01 + bz0];
    u = at3 (rx0, ry1, rz0);
    q = g3[b11 + bz0];
    v = at3 (rx1, ry1, rz0);
    b = lerp (t, u, v);

    c = lerp (sy, a, b);

    q = g3[b00 + bz1];
    u = at3 (rx0, ry0, rz1);
    q = g3[b10 + bz1];
    v = at3 (rx1, ry0, rz1);
    a = lerp (t, u, v);

    q = g3[b01 + bz1];
    u = at3 (rx0, ry1, rz1);
    q = g3[b11 + bz1];
    v = at3 (rx1, ry1, rz1);
    b = lerp (t, u, v);

    d = lerp (sy, a, b);

    return lerp (sz, c, d);
}

void CNoise::normalize2 (double v[2])
{
    double s;

    s = sqrt (v[0] * v[0] + v[1] * v[1]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
}

void CNoise::normalize3 (double v[3])
{
    double s;

    s = sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
    v[2] = v[2] / s;
}

void CNoise::init_noise()
{
    int i, j, k;

    for (i = 0; i < B; i++)
    {
        p[i] = i;

        g1[i] = (double) ((random () % (B + B)) - B) / B;

        for (j = 0; j < 2; j++)
            g2[i][j] = (double) ((random () % (B + B)) - B) / B;
        normalize2 (g2[i]);

        for (j = 0; j < 3; j++)
            g3[i][j] = (double) ((random () % (B + B)) - B) / B;
        normalize3 (g3[i]);
    }

    while (--i)
    {
        k = p[i];
        p[i] = p[j = random () % B];
        p[j] = k;
    }

    for (i = 0; i < B + 2; i++)
    {
        p[B + i] = p[i];
        g1[B + i] = g1[i];
        for (j = 0; j < 2; j++)
            g2[B + i][j] = g2[i][j];
        for (j = 0; j < 3; j++)
            g3[B + i][j] = g3[i][j];
    }
}




double CNoise::bias(double a, double b)
{
	return pow(a, log(b) / log(0.5));
}

double CNoise::gain(double a, double b)
{
	double p = log(1. - b) / log(0.5);

	if (a < .001)
		return 0.;
	else if (a > .999)
		return 1.;
	if (a < 0.5)
		return pow(2 * a, p) / 2;
	else
		return 1. - pow(2 * (1. - a), p) / 2;
}

/*double noise1(double arg);
double noise2(double vec[]);
double noise3(double vec[]);

double noise(double vec[], int len)
{
	switch (len) 
    {
	case 0:
		return 0.;
	case 1:
		return noise1(vec[0]);
	case 2:
		return noise2(vec);
	default:
		return noise3(vec);
	}
} */

double CNoise::turbulence3(double v[3], double freq)
{
	double t, vec[3];

	for (t = 0. ; freq >= 1. ; freq /= 2) {
		vec[0] = freq * v[0];
		vec[1] = freq * v[1];
		vec[2] = freq * v[2];
		t += fabs(noise3(vec)) / freq;
	}
	return t;
}


double CNoise::turbulence2(double v[2], double freq)
{
	double t, vec[3];

	for (t = 0. ; freq >= 1. ; freq /= 2) 
    {
		vec[0] = freq * v[0];
		vec[1] = freq * v[1];
		t += fabs(noise2(vec)) / freq;
	}
	return t;
}

