//
// Tunnel3D demo for OpenPTC 1.0 C++ API
//
// Realtime raytraced tunnel
// Copyright (c) 1998 Christian Nentwich (brn@eleet.mcb.at)
// This source code is licensed under the GNU LGPL
//
// And do not just blatantly cut&paste this into your demo :)
//

#ifndef __TUNNEL_H__
#define __TUNNEL_H__

#include "ptc.h"

/* DEFINES */
#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define VERSION_CALC 0x130

/* TYPEDEFS */
typedef float Vector[3];                // X,Y,Z
typedef float Matrix[4][4];             // FIRST  = COLUMN
                                        // SECOND = ROW      

                                        // [0][0]  [1][0]  [2][0]
                                        // [0][1]  [1][1]  [2][1]
                                        // [0][2]  [1][2]  [2][2]
// (I know the matrices are the wrong way round, so what, the code is quite
//  old :)

void vector_normalize(Vector v);
void vector_times_matrix(Vector v,Matrix m,Vector res);
void matrix_idle(Matrix m);
void matrix_rotate_x(Matrix m,int angle,float *sintab,float *costab);
void matrix_rotate_y(Matrix m,int angle,float *sintab,float *costab);
void matrix_rotate_z(Matrix m,int angle,float *sintab,float *costab);
void matrix_times_matrix(Matrix m1,Matrix m2,Matrix res);


class RayTunnel {
  char8 *tunneltex;                      // Texture
  char8 *pal;                            // Original palette
  int32 *lookup;                         // Lookup table for lighting

  float *sintab,*costab;                 // Take a guess
  int *atantab;

  int *u_array,*v_array,*l_array;        // Raytraced coordinates and light
  Vector *norms;
  
  float radius,radius_sqr;
  Matrix rot;

  Vector pos,light;                      // Position in the tunnel, pos of
  int xa,ya,za;                          // lightsource, angles

  char lightstatus;                      // Following the viewer ?
  
public:
  RayTunnel(float rad);                  // Constructor takes the radius
  ~RayTunnel();

  void load_texture();

  void tilt(int,int,int);                 // Rotate relative
  void tilt(int,int,int,char abs);        // Absolute

  void move(float,float,float);           // Relative move
  void move(float,float,float,char abs);  // Absolute

  void movelight(float,float,float);
  void movelight(float,float,float,char abs);

  void locklight(char lock);              // Make the light follow the viewer

  void interpolate();                     // Raytracing

  void draw(int32 *dest);                 // Draw the finished tunnel
};

#endif
