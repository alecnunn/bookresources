#ifndef GPG5_HAMLAOUI_Main
#define GPG5_HAMLAOUI_Main

#include <windows.h>
#include <math.h>
#include <stdio.h>

#include "fmod.h"
#include "glut.h"

#pragma comment (lib, "fmodvc.lib")

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define PIdiv180 (M_PI / 180.0f)

inline float clampf (float x, float min, float max)
{
         if (x < min) return min;
    else if (x > max) return max;
    else              return x;
}

inline float radf (const float x)
{
    return x * PIdiv180;
}

#include "Primitives.h"
#include "Emitters.h"

#endif  // GPG5_HAMLAOUI_Main