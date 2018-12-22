//
// SmoothCD for ease-in / ease-out smoothing 
//

#ifndef _CRITICALDAMPING_H_
#define _CRITICALDAMPING_H_

//
// NOTE: This code depends on a global variable named timeDelta,
//       where it expects to find a value for delta-t.
//

template <typename T>
inline T SmoothCD(const T &from, const T &to, T &vel, float smoothTime)
{
  float omega = 2.f/smoothTime;
  float x = omega*timeDelta;
  float exp = 1.f/(1.f+x+0.48f*x*x+0.235f*x*x*x);
  T change = from - to;
  T temp = (vel+omega*change)*timeDelta;
  vel = (vel - omega*temp)*exp;  // Equation 5
  return to + (change+temp)*exp; // Equation 4
}

#endif
