/* 
  Functions to create OpenGL textures containing pre-filtered noise patterns for procedural texturing.

  Pre-filtering the texture with a bicubic filter avoids some of the artifacts associated with linear filtering,
  and allows us to pre-compute the abs() function required for Perlin-style turbulence.

  sgreen@nvidia.com 7/2000
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include "glh_extensions.h"

#include "noise.h"

// clamp x to be between a and b
float clamp(float x, float a, float b)
{
    return (x < a ? a : (x > b ? b : x));
}

// a nice piecewise-cubic spline function, defined between 0.0<=x<=2.0
// approximates a windowed sinc function - negative lobes (sharpens slightly)
// a = contraint parameter -0.5<=a<=-1.0 recommended, -0.75 is good
float cubic(float x, float a)
{
  float w;

  if ((x >= 0.0) && (x < 1.0)) {
    // (a+2)x^3 - (a+3)x^2 + 1
    w = ((a + 2.0) * x - (a + 3.0)) * x * x + 1.0;
  } else if ((x >= 1.0) && (x <= 2.0)) {
    // ax^3 - 5ax^2 + 8ax - 4a
    w = ((a * x - 5.0 * a) * x + 8.0 * a) * x - 4.0 * a;
  } else {
    w = 0.0;
  }
  return w;
}


#define A	-0.75		// constraint parameter

// 1-dimensional cubic interpolation
// not very fast, but easy to understand
// 0<=x<=1
float cubicFilter4f(float x, float vm1, float v, float vp1, float vp2)
{
  return vm1 * cubic(1.0+x, A) +
		 v   * cubic(x, A) +
		 vp1 * cubic(1.0-x, A) +
		 vp2 * cubic(2.0-x, A);
}

float cubicFilter4fv(float x, float *v)
{
  return v[0] * cubic(1.0+x, A) +
		 v[1] * cubic(x, A) +
		 v[2] * cubic(1.0-x, A) +
		 v[3] * cubic(2.0-x, A);
}

/*
  1D cubic interpolator (this is just a faster version of the above)
  assumes x > 0.0
  constraint parameter = -1
*/
float cubicFilterFast4fv(float x, float *i)
{
  float x1, x2, x3;
  float h1, h2, h3, h4;
  float result;

  x1 = x + 1.0;
  x2 = x1 * x1;
  x3 = x2 * x1;
  h4 = -x3 + 5 * x2 - 8 * x1 + 4;
  result = h4 * i[0];

  x1 = x;
  x2 = x1 * x1;
  x3 = x2 * x1;
  h1 = x3 - 2 * x2 + 1;
  result += h1 * i[1];

  x1 = 1.0 - x;
  x2 = x1 * x1;
  x3 = x2 * x1;
  h2 = x3 - 2 * x2 + 1;
  result += h2 * i[2];

  x1 = 2.0 - x;
  x2 = x1 * x1;
  x3 = x2 * x1;
  h3 = -x3 + 5 * x2 - 8 * x1 + 4;
  result += h3 * i[3];

  return result;
}

// distance from a point
float distance(float x, float y, float z, float cx, float cy, float cz)
{
  float dx = x-cx;
  float dy = y-cy;
  float dz = z-cz;
  return sqrt(dx*dx + dy*dy + dz*dz);
}

// create a 3d table of random floating point values between -1.0 and 1.0
// width, height, depth must be powers of two
FilteredNoise::FilteredNoise(int w, int h, int d)
{
  m_w = w;
  m_h = h;
  m_d = d;

  m_noise = new float[w * h * d];

  float *ptr = m_noise;
  for(int i=0; i<w*h*d; i++) {
    *ptr++ = (2.0 * rand() / (float) RAND_MAX) - 1.0;
  }
}


// get a repeatable random value for a given integer position
// returns random float between -1.0 and 1.0
// nb - lots of other ways to do this (hash functions etc.)

float
FilteredNoise::noise2D(int x, int y)
{
  x = x & (m_w - 1);
  y = y & (m_h - 1);

  return m_noise[(y*m_w) + x];
}

float
FilteredNoise::noise3D(int x, int y, int z)
{
  x = x & (m_w - 1);
  y = y & (m_h - 1);
  z = z & (m_d - 1);

  return m_noise[(z*m_w*m_h) + (y*m_w) + x];
}

#define FLOOR(x) ((int)(x) - ((x) < 0 && (x) != (int)(x)))

// 2D bicubic-filtered noise
// filters 16 points
// returns float between -1.0 and 1.0
float
FilteredNoise::bicubicNoise2D(float x, float y)
{
  int ix = FLOOR(x);
  float fx = x - ix;
  int iy = FLOOR(y);
  float fy = y - iy;

  float r = cubicFilter4f(fy,
            cubicFilter4f(fx, noise2D(ix-1, iy-1), noise2D(ix, iy-1), noise2D(ix+1, iy-1), noise2D(ix+2, iy-1)),
            cubicFilter4f(fx, noise2D(ix-1, iy),   noise2D(ix, iy),   noise2D(ix+1, iy),   noise2D(ix+2, iy)),
            cubicFilter4f(fx, noise2D(ix-1, iy+1), noise2D(ix, iy+1), noise2D(ix+1, iy+1), noise2D(ix+2, iy+1)),
            cubicFilter4f(fx, noise2D(ix-1, iy+2), noise2D(ix, iy+2), noise2D(ix+1, iy+2), noise2D(ix+2, iy+2)) );

  return clamp(r, -1.0, 1.0);
}


// 3D bicubic-filtered noise
// filters between 64 points
float
FilteredNoise::bicubicNoise3D(float x, float y, float z)
{
  int ix = FLOOR(x);
  float fx = x - ix;
  int iy = FLOOR(y);
  float fy = y - iy;
  int iz = FLOOR(z);
  float fz = z - iz;

  float xknots[4], yknots[4], zknots[4];

  for (int k = -1; k <= 2; k++) {
    for (int j = -1; j <= 2; j++) {
      for (int i = -1; i <= 2; i++) {
        xknots[i+1] = noise3D(ix+i, iy+j, iz+k);
	  }
      yknots[j+1] = cubicFilterFast4fv(fx, xknots);
    }
    zknots[k+1] = cubicFilterFast4fv(fy, yknots);
  }
  float r = cubicFilterFast4fv(fz, zknots);

  return clamp(r, -1.0, 1.0);
}


// assumes cubical volume (w=h=d=size)
int getVoxel(GLubyte *data, int x, int y, int z, int size)
{
  return data[((z*size+y)*size)+x];
}

// downsample volume using simple box filter - averages 8 neighbours
GLubyte *downSampleVolume(GLubyte *data, int size)
{
  int newsize = size/2;
  GLubyte *newdata = new GLubyte [newsize*newsize*newsize];
  GLubyte *ptr = newdata;

  for(int z=0; z<newsize; z++) {
	for(int y=0; y<newsize; y++) {
	  for(int x=0; x<newsize; x++) {
        int ox = x*2;
        int oy = y*2;
        int oz = z*2;
        *ptr++ = (getVoxel(data, ox, oy, oz, size) + 
                  getVoxel(data, ox+1,   oy, oz,   size) +
                  getVoxel(data, ox,   oy+1, oz,   size) +
                  getVoxel(data, ox+1, oy+1, oz,   size) +
                  getVoxel(data, ox,     oy, oz+1, size) + 
                  getVoxel(data, ox+1,   oy, oz+1, size) +
                  getVoxel(data, ox,   oy+1, oz+1, size) +
                  getVoxel(data, ox+1, oy+1, oz+1, size)) / 8;
      }
    }
  }
  return newdata;
}


float sfrand()
{
  return (2.0 * rand() / (float) RAND_MAX) - 1.0;
}

// Create an OpenGL 2D texture containing filtered noise
GLuint
FilteredNoise::createNoiseTexture2D(float scale, float amp, NoiseMode noiseMode, bool mipmap)
{
  int w = m_w * scale;
  int h = m_h * scale;

  GLubyte *img = new GLubyte[w * h * 3];

  GLubyte *p = img;
  for(int y=0; y<h; y++) {
    for(int x=0; x<w; x++) {
		float fx = (x * m_w / (float) w) - 0.5;
		float fy = (y * m_h / (float) h) - 0.5;

        float n;
		switch(noiseMode) {
		case RAW:
		  // raw unfiltered noise
		  n = (noise2D(x, y) + 1.0) * 0.5;
		  break;

        case NOISE:
		  // filtered noise
		  n = (bicubicNoise2D(fx, fy) + 1.0) * 0.5;
		  break;

		case ABSNOISE:
		  // absolute value of filtered noise (for turbulence)
	 	  n = (fabs(bicubicNoise2D(fx, fy)));
		  break;
        }

		n = clamp(n, 0.0, 1.0);
		*p++ = (GLubyte) (n * amp * 0xff);        
    }
  }

  GLuint texid;
  glGenTextures(1, &texid);
  glBindTexture(GL_TEXTURE_2D, texid);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if (mipmap) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img);

  delete [] img;
  return texid;
}

// Create an OpenGL 3D texture containing filtered noise
GLuint
FilteredNoise::createNoiseTexture3D(float scale, float amp, NoiseMode noiseMode, bool mipmap)
{
  int w = m_w * scale;
  int h = m_h * scale;
  int d = m_d * scale;

  const int checksize = 16;

  GLubyte *img = new GLubyte[w * h * d];

  GLubyte *p = img;
  for(int z=0; z<d; z++) {
	for(int y=0; y<h; y++) {
	  for(int x=0; x<w; x++) {

		float fx = (x * m_w / (float) w) - 0.5;
		float fy = (y * m_h / (float) h) - 0.5;
		float fz = (z * m_d / (float) d) - 0.5;

        float n;

		switch(noiseMode) {
		case RAW:
		  // raw unfiltered noise
		  n = (noise3D(x, y, z) + 1.0) * 0.5;
		  break;

        case NOISE:
		  // filtered noise
		  n = (bicubicNoise3D(fx, fy, fz) + 1.0) * 0.5;
		  break;

		case ABSNOISE:
		  // absolute value of filtered noise (for turbulence)
	 	  n = (fabs(bicubicNoise3D(fx, fy, fz)));
		  break;

		case VEINS:
		  // scaled filtered noise - gives cheesy "Star Trek" type effect
		  n = 1.0 - 2.0 * (fabs(bicubicNoise3D(fx, fy, fz)));
		  break;

		case DISTANCE:
		  // distance from centre of texture for Voronoi pattern
		  n = distance(x / (float) w, y / (float) h, z / (float) d, 0.5, 0.5, 0.5);
		  break;

		case CHECKER:
          // checkerboard pattern
          n = ((x%checksize>(checksize/2)-1) ^ 
               (y%checksize>(checksize/2)-1) ^ 
               (z%checksize>(checksize/2)-1)) ? 0.0 : 1.0;
          break;

        case RAMP:
          // linear ramp
          n = x / (float) d;
          break;

        case RIDGED:
          n = 1 - fabs(bicubicNoise3D(fx, fy, fz));
          n *= n;
          break;

        default:
          fprintf(stderr, "Noise mode not supported\n");
          return 0;
		}

		n = clamp(n, 0.0, 1.0);
		*p++ = (GLubyte) (n * amp * 0xff);
	  }
	}
  }

  GLuint texid;
  glGenTextures(1, &texid);
  glBindTexture(GL_TEXTURE_3D, texid);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if (mipmap) {
    // create mipmaps
    int size = w;
    int level = 0;
    while(size>0) {
      glTexImage3D(GL_TEXTURE_3D, level, GL_LUMINANCE, size, size, size, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img);
      GLubyte *newimg = downSampleVolume(img, size);
      delete [] img;
      img = newimg;
      level++;
      size>>=1;
    }

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  } else {
    glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, w, h, d, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }

  delete [] img;

  return texid;
}
