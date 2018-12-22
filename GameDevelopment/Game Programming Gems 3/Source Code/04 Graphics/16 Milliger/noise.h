#include <GL/gl.h>

float clamp(float x, float a, float b);

class FilteredNoise {
public:
  // constructor - width, height, depth should be powers of two
  FilteredNoise(int w, int h, int d);

  float noise2D(int x, int y);
  float noise3D(int x, int y, int z);
  float bicubicNoise2D(float x, float y);
  float bicubicNoise3D(float x, float y, float z);

  // noise types
  enum NoiseMode {
	  NOISE = 0,  // filtered noise
	  ABSNOISE,   // absolute value noise
	  VEINS,      // scaled noise
	  RAW,        // raw (unfiltered) noise
	  DISTANCE,   // distance function (for Worley patterns)
      CHECKER,
      RAMP,
      RIDGED
  };

  GLuint createNoiseTexture2D(float scale, float amp, NoiseMode noisemode, bool mipmap);
  GLuint createNoiseTexture3D(float scale, float amp, NoiseMode noisemode, bool mipmap);

private:
  int m_w, m_h, m_d;
  float *m_noise;
};
