/* Copyright (C) Kevin Kaiser, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Kevin Kaiser, 2000"
 */
/* ************************************************************************ *
 * Filename: 3dcol_demo.c     Description: 3dcol_demo.c contains functions  *
 *   Author: Kevin Kaiser                  that implement mathematically    *
 *  Version: 1.62                          precise collision detection      *
 *  Updated: April 28, 2000                between two 3-dimensional        *
 *  Created: December 27, 1999             triangulated polygons.           *
 *                                                                          *
 * Copyright (C) 1999,2000 Kevin Kaiser                                     *
 * For use in 'Game Programming Gems'                               4-28-00 *
 * ************************************************************************ *
 *                                 Keys                                     *
 *                                 ----                                     *
 *                        ESC - quit program                                *
 *                        Q/E - rotate camera (q-left, e-right)             *
 *                        1/2 - X-minus,X-plus                              *
 *                        3/4 - Y-minux,Y-Plus                              *
 *                        5/6 - Z-minus,Z-Plus                              *
 *                        7/8 - Toggle Bounding Spheres (7-on, 8-off)       *
 *                        +/- - move camera out/in                          *
 * ************************************************************************ * 
 * Requirements:  GLUT : http://www.opengl.org                              *
 *               glPNG : http://www.wyatt100.freeserve.co.uk/download.htm   *
 * ************************************************************************ *
 *                            Contact Kevin Kaiser at velderon@warp3000.com *
 * ************************************************************************ */

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "glpng.h"

#define DIRECTION(a,b) (a=b)
#define UP 0
#define DOWN 1
#ifndef _WIN32
#define TRUE 1
#define FALSE 0
#endif

// Displays debug information
// --------------------------
//#define DEBUG

// Toggles bounding spheres
int DRAW_BS;

// Determines what kind of triangle you want
#define NORMAL 0
//#define FLAT 1
//#define FLAT_RIGHT 2
//#define VERTICAL 3
//#define VERTICAL_RIGHT 4
//#define CUSTOM 5

typedef struct vert_typ
	{
	float x,y,z;
	}*vertex_ptr,vertex;
typedef struct two_d_vert_typ
	{
	float x,y;
	}*two_d_vertex_ptr,vertex2d;
typedef struct tri_typ
	{
	vertex a,b,c;
	float pA,pB,pC,pD; // Plane normals
	float radius;
	vertex center;
	}*triangle_ptr,triangle;

// Global Variables
triangle tri1,tri2;
float cam_x,cam_z,cam_angle;
float height;
pngInfo info1,info2,info3;
GLuint id1,id2,id3; // collide.png/not_collide.png/maybe_colliding.png
const char *window_header =
      "3D Collision Detection v1.62 - Game Programming Gems [Kevin Kaiser]";

/*
****************************************************************
* FUNCTION: find_polygon_radius()
*  PURPOSE: Determine the center point and radius of the polygon.
**************************************************************** */
void find_polygon_radius(triangle *tri)
{
  float minx,miny,minz,maxx,maxy,maxz;
  float temp_radius;

  minx = maxx = tri->a.x;
  miny = maxy = tri->a.y;
  minz = maxz = tri->a.z;

  if (tri->b.x < minx)
    minx = tri->b.x;
  if (tri->c.x < minx)
    minx = tri->c.x;
  if (tri->b.y < miny)
    miny = tri->b.y;
  if (tri->c.y < miny)
    miny = tri->c.y;
  if (tri->b.z < minz)
    minz = tri->b.z;
  if (tri->c.z < minz)
    minz = tri->c.z;
    
  if (tri->b.x > maxx)
    maxx = tri->b.x;
  if (tri->c.x > maxx)
    maxx = tri->c.x;
  if (tri->b.y > maxy)
    maxy = tri->b.y;
  if (tri->c.y > maxy)
    maxy = tri->c.y;
  if (tri->b.z > maxz)
    maxz = tri->b.z;
  if (tri->c.z > maxz)
    maxz = tri->c.z;

  tri->center.x = ((minx+maxx)/2);
  tri->center.y = ((miny+maxy)/2);
  tri->center.z = ((minz+maxz)/2);

  temp_radius = sqrt(((tri->center.x-tri->a.x)*(tri->center.x-tri->a.x))+
                     ((tri->center.y-tri->a.y)*(tri->center.y-tri->a.y))+
                     ((tri->center.z-tri->a.z)*(tri->center.z-tri->a.z)));

  if ((sqrt(((tri->center.x-tri->b.x)*(tri->center.x-tri->b.x))+
            ((tri->center.y-tri->b.y)*(tri->center.y-tri->b.y))+
            ((tri->center.z-tri->b.z)*(tri->center.z-tri->b.z))))>temp_radius)
    
    temp_radius = sqrt(((tri->center.x-tri->b.x)*(tri->center.x-tri->b.x))+
                       ((tri->center.y-tri->b.y)*(tri->center.y-tri->b.y))+
                       ((tri->center.z-tri->b.z)*(tri->center.z-tri->b.z)));

  if ((sqrt(((tri->center.x-tri->c.x)*(tri->center.x-tri->c.x))+
            ((tri->center.y-tri->c.y)*(tri->center.y-tri->c.y))+
            ((tri->center.z-tri->c.z)*(tri->center.z-tri->c.z)))>temp_radius));
    
    temp_radius = sqrt(((tri->center.x-tri->c.x)*(tri->center.x-tri->c.x))+
                       ((tri->center.y-tri->c.y)*(tri->center.y-tri->c.y))+
                       ((tri->center.z-tri->c.z)*(tri->center.z-tri->c.z)));
  
  tri->radius = temp_radius;

  return;
}

/*
****************************************************************
* FUNCTION: check_for_bounding_collision()
*  PURPOSE: Determine whether two bounding spheres are colliding
**************************************************************** */
int check_for_bounding_collision(triangle *tri1, triangle *tri2)
{
  float temp_dist;

  temp_dist = tri1->radius + tri2->radius;

  if ((sqrt(((tri2->center.x-tri1->center.x)*(tri2->center.x-tri1->center.x))+
            ((tri2->center.y-tri1->center.y)*(tri2->center.y-tri1->center.y))+
            ((tri2->center.z-tri1->center.z)*(tri2->center.z-tri1->center.z))
            ))<temp_dist)
    return TRUE;
  else
    return FALSE;
}

/*
*********************************************************
* FUNCTION: drawcross()
*  PURPOSE: Draw a cross of given 'size' at point (x,y,z)
********************************************************* */
void drawcross(float x, float y, float z,int size)
{
  glBegin(GL_LINES);
    glColor3f(1,1,1);
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(x+size,y    ,z    );
    glVertex3f(x-size,y    ,z    );
    glVertex3f(x    ,y+size,z    );
    glVertex3f(x    ,y-size,z    );
    glVertex3f(x    ,y    ,z+size);
    glVertex3f(x    ,y    ,z-size);
  glEnd();
}

/*
********************************************************
* FUNCTION: drawline()
*  PURPOSE: Draw a 3D line from (x1,y1,z1) to (x2,y2,z2)
******************************************************** */
void drawline(float x1, float y1, float z1,float x2,float y2,float z2)
{
  glBegin(GL_LINES);
    glColor4f(.3,.3,.3,1.0);
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(x1,y1,z1);
    glVertex3f(x2,y2,z2);
  glEnd();
}

/*
********************************************************
* FUNCTION: line_plane_collision()
*  PURPOSE: Use parametrics to see where on the plane of
*           tri1 the line made by a->b intersect
******************************************************** */
vertex line_plane_collision(vertex *a, vertex *b,triangle *tri1)
{
  float final_x,final_y,final_z,final_t;
  float t,i;
  vertex temp;

  t=(float)0; i=(float)0;
  i+=(float)(tri1->pA*b->x)+(tri1->pB*b->y)+(tri1->pC*b->z)+(tri1->pD);
  t+=(float)(tri1->pA*(b->x*-1))+(tri1->pB*(b->y*-1))+(tri1->pC*(b->z*-1));
  t+=(float)(tri1->pA*a->x)+(tri1->pB*a->y)+(tri1->pC*a->z);

  // Be wary of possible divide-by-zeros here (i.e. if t==0)
  final_t = (-i)/t;

  // Vertical Line Segment
  if ((a->x == b->x)&&(a->z == b->z)) { // vertical line segment

    temp.x = a->x;
    temp.y = (-((tri1->pA*a->x)+(tri1->pC*a->z)+(tri1->pD)))/(tri1->pB);
    temp.z = a->z;

    return(temp);
  }

  final_x = (((a->x)*(final_t))+((b->x)*(1-final_t)));
  final_y = (((a->y)*(final_t))+((b->y)*(1-final_t)));
  final_z = (((a->z)*(final_t))+((b->z)*(1-final_t)));

  temp.x = final_x;
  temp.y = final_y;
  temp.z = final_z;

  return(temp);
}

/*
********************************************************   
* FUNCTION: point_inbetween_vertices()
*  PURPOSE: Using parametrics, it is easy to determine   
*           whether a point lies between two vertices:
*           as long as t lies between 0 and 1, the point
*           lies between the vertices.
******************************************************** */
int point_inbetween_vertices(vertex *a, vertex *b,triangle *tri1)
{
  float t,i,final_t;

  t=(float)0; i=(float)0;
  i+=(float)(tri1->pA*b->x)+(tri1->pB*b->y)+(tri1->pC*b->z)+(tri1->pD);
  t+=(float)(tri1->pA*(b->x*-1))+(tri1->pB*(b->y*-1))+(tri1->pC*(b->z*-1));
  t+=(float)(tri1->pA*a->x)+(tri1->pB*a->y)+(tri1->pC*a->z);

  // Be wary of possible divide-by-zeros here (i.e. if t==0)
  final_t = (-i)/t;

  if ((final_t>=0)&&(final_t<=1))
    return TRUE;
  else
    return FALSE;
}

/*
*********************************************************
* FUNCTION: point_inside_triangle()
*  PURPOSE: Determine whether the given point 'vert' lies
*           inside the triangle 'tri' when both are
*           "flattened" into 2D
********************************************************* */
int point_inside_triangle(triangle *tri,vertex *vert,int x,int y,int z)
{
  float a1,b1,a2,b2,a3,b3,a4,b4;
  float center_x,center_y;
  float m1,m2,m3;
  float bb1,bb2,bb3;
  int inside,direction;
  int AB_vert,BC_vert,CA_vert;

  // First make sure only one axis was "dropped"
  if (((x==FALSE)&&(y==FALSE))||((x==FALSE)&&(z==FALSE))
    ||((y==FALSE)&&(z==FALSE))||((x==FALSE)&&(y==FALSE)&&(z==FALSE))) {

    printf("point_inside_triangle():more than one axis dropped, exiting\n");
    return FALSE;
  }

  if (x==FALSE) { // dropping x coordinate
    a1 = tri->a.y;
    b1 = tri->a.z;
    a2 = tri->b.y;
    b2 = tri->b.z;
    a3 = tri->c.y;
    b3 = tri->c.z;
    a4 = vert->y;
    b4 = vert->z;
    inside = 0;
  }
  else if (y==FALSE) { // dropping y coordinate
    a1 = tri->a.x;
    b1 = tri->a.z;
    a2 = tri->b.x;
    b2 = tri->b.z;
    a3 = tri->c.x;
    b3 = tri->c.z;
    a4 = vert->x;
    b4 = vert->z;
    inside = 0;
  }
  else if (z==FALSE) { // dropping z coordinate
    a1 = tri->a.x;
    b1 = tri->a.y;
    a2 = tri->b.x;
    b2 = tri->b.y;
    a3 = tri->c.x;
    b3 = tri->c.y;
    a4 = vert->x;
    b4 = vert->y;
    inside = 0;
  }

#ifdef DEBUG
  if (x==FALSE) {
    drawline(0,a1,b1,0,a2,b2);
    drawline(0,a2,b2,0,a3,b3);
    drawline(0,a3,b3,0,a1,b1);
  }
  else if (y==FALSE) {
    drawline(a1,0,b1,a2,0,b2);
    drawline(a2,0,b2,a3,0,b3);
    drawline(a3,0,b3,a1,0,b1);
  }
  else if (z==FALSE) {
    drawline(a1,b1,0,a2,b2,0);
    drawline(a2,b2,0,a3,b3,0);
    drawline(a3,b3,0,a1,b1,0);
  }
#endif

  AB_vert = BC_vert = CA_vert = FALSE;

  // y=mx+b for outer 3 lines
  if ((a2-a1)!=0) {
    m1 = (b2-b1)/(a2-a1); // a->b
    bb1 = (b1)-(m1*a1);    // y/(mx) using vertex a
  } else if ((a2-a1)==0) {
   AB_vert = TRUE;
  }

  if ((a3-a2)!=0) {
    m2 = (b3-b2)/(a3-a2); // b->c
    bb2 = (b2)-(m2*a2);    // y/(mx) using vertex b
  } else if ((a3-a2)==0) {
   BC_vert = TRUE;
  }

  if ((a1-a3)!=0) {
    m3 = (b1-b3)/(a1-a3); // c->a
    bb3 = (b3)-(m3*a3);    // y/(mx) using vertex c
  } else if ((a1-a3)==0) {
   CA_vert = TRUE;
  }

  // find average point of triangle (point is guaranteed
  center_x = (a1+a2+a3)/3;        // to lie inside the triangle)
  center_y = (b1+b2+b3)/3;

#ifdef DEBUG
  if (x==FALSE) {
    drawcross(0,center_x,center_y,2);
    drawcross(0,vert->y,vert->z,1);
  }
  else if (y==FALSE) {
    drawcross(center_x,0,center_y,2);
    drawcross(vert->x,0,vert->z,1);
  }
  else if (z==FALSE) {
    drawcross(center_x,center_y,0,2);
    drawcross(vert->x,vert->y,0,1);
  }
  drawcross(vert->x,vert->y,vert->z,1);
#endif

  
  // See whether (center_x,center_y) is above or below the line,
  // then set direction to UP if the point is above or DOWN if the
  // point is below the line

  // a->b
  if (((m1*center_x)+bb1) >= center_y)
    DIRECTION(direction,UP);
  else
    DIRECTION(direction,DOWN);
  if (AB_vert==TRUE) {
    if ((a1<a4)&&(a1<center_x)) // vert projected line
      inside++;
    else if ((a1>a4)&&(a1>center_x)) // vert projected line
      inside++;
  } else {
    if (direction==UP) {
      if (b4 <= ((m1*a4)+bb1)) // b4 less than y to be inside
        inside++;              // (line is above point)
    } else if (direction==DOWN) {
      if (b4 >= ((m1*a4)+bb1)) // b4 greater than y to be inside
        inside++;              // (line is below point)
    }
  }

  // b->c
  if (((m2*center_x)+bb2) >= center_y)
    DIRECTION(direction,UP);
  else
    DIRECTION(direction,DOWN);
  if (BC_vert==TRUE) {
    if ((a2<a4)&&(a2<center_x)) // vert projected line
      inside++;
    else if ((a2>a4)&&(a2>center_x)) // vert projected line
      inside++;
  } else {
    if (direction==UP) {
      if (b4 <= ((m2*a4)+bb2)) // b4 less than y to be inside 
        inside++;              // (line is above point)
    } else if (direction==DOWN) {
      if (b4 >= ((m2*a4)+bb2)) // b4 greater than y to be inside
        inside++;              // (line is below point)
    }
  }
  // c->a
  if (((m3*center_x)+bb3) >= center_y)
    DIRECTION(direction,UP);
  else
    DIRECTION(direction,DOWN);
  if (CA_vert==TRUE) {
    if ((a3<a4)&&(a3<center_x)) // vert projected line
      inside++;
    else if ((a3>a4)&&(a3>center_x)) // vert projected line
      inside++;
  } else {
    if (direction==UP) {
      if (b4 <= ((m3*a4)+bb3)) // b4 less than y to be inside 
        inside++;              // (line is above point)
    } else if (direction==DOWN) {
      if (b4 >= ((m3*a4)+bb3)) // b4 greater than y to be inside 
        inside++;              // (line is below point)
    }
  }
  if (inside==3) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*
**********************************************************
* FUNCTION: triangles_colliding()
*  PURPOSE: Determine whether triangle 'tri1' is colliding
*           with triangle 'tri2'
********************************************************** */
int triangles_colliding(triangle tri1, triangle tri2)
{
  int temp;
  vertex v1,v2,cross_v1xv2,p;

  v1.x = tri1.b.x - tri1.a.x;
  v1.y = tri1.b.y - tri1.a.y;
  v1.z = tri1.b.z - tri1.a.z;
  v2.x = tri1.c.x - tri1.a.x;
  v2.y = tri1.c.y - tri1.a.y;
  v2.z = tri1.c.z - tri1.a.z;

  cross_v1xv2.x =  (v2.y*v1.z - v2.z*v1.y);
  cross_v1xv2.y = -(v2.x*v1.z - v2.z*v1.x);
  cross_v1xv2.z =  (v2.x*v1.y - v2.y*v1.x);

  tri1.pA = cross_v1xv2.x;
  tri1.pB = cross_v1xv2.y;
  tri1.pC = cross_v1xv2.z;
  tri1.pD += (-(tri1.a.x))*(cross_v1xv2.x);
  tri1.pD += (-(tri1.a.y))*(cross_v1xv2.y);
  tri1.pD += (-(tri1.a.z))*(cross_v1xv2.z);

  temp = FALSE; // default assignment
  
  // Scroll thru 3 line segments of the other triangle
  // First iteration  (a,b)
  p=line_plane_collision((vertex_ptr)&tri2.a,(vertex_ptr)&tri2.b,
                                             (triangle_ptr)&tri1);

  // Determine which axis to project to
  // X is greatest
  if ((abs(tri1.pA)>=abs(tri1.pB))&&(abs(tri1.pA)>=abs(tri1.pC)))
    temp = point_inside_triangle((triangle_ptr)&tri1,(vertex_ptr)&p,
                                                     FALSE,TRUE,TRUE);
  // Y is greatest
  else if ((abs(tri1.pB)>=abs(tri1.pA))&&(abs(tri1.pB)>=abs(tri1.pC)))
    temp = point_inside_triangle((triangle_ptr)&tri1,(vertex_ptr)&p,
                                                     TRUE,FALSE,TRUE);
  // Z is greatest
  else if ((abs(tri1.pC)>=abs(tri1.pA))&&(abs(tri1.pC)>=abs(tri1.pB)))
    temp = point_inside_triangle((triangle_ptr)&tri1,(vertex_ptr)&p,
                                                     TRUE,TRUE,FALSE);

  if (temp==TRUE) {
    // Point needs to be checked to see if it lies between the two vertices
    // First check for the special case of vertical line segments
    if ((tri2.a.x == tri2.b.x)&&(tri2.a.z == tri2.b.z)) {
      if (((tri2.a.y <= p.y)&&(p.y <= tri2.b.y))||
          ((tri2.b.y <= p.y)&&(p.y <= tri2.a.y)))
        return TRUE;
    }
    // End vertical line segment check

    // Now check for point on line segment
    if (point_inbetween_vertices((vertex_ptr)&tri2.a,(vertex_ptr)&tri2.b,
                                              (triangle_ptr)&tri1)==TRUE)
      return TRUE;
    else
      return FALSE;
  }

  // Second iteration (b,c)
  p=line_plane_collision((vertex_ptr)&tri2.b,(vertex_ptr)&tri2.c,
                                             (triangle_ptr)&tri1);

  // Determine which axis to project to
  // X is greatest
  if ((abs(tri1.pA)>=abs(tri1.pB))&&(abs(tri1.pA)>=abs(tri1.pC)))
    temp = point_inside_triangle((triangle_ptr)&tri1,(vertex_ptr)&p,
                                                     FALSE,TRUE,TRUE);
  // Y is greatest
  else if ((abs(tri1.pB)>=abs(tri1.pA))&&(abs(tri1.pB)>=abs(tri1.pC)))
    temp = point_inside_triangle((triangle_ptr)&tri1,(vertex_ptr)&p,
                                                     TRUE,FALSE,TRUE);
  // Z is greatest
  else if ((abs(tri1.pC)>=abs(tri1.pA))&&(abs(tri1.pC)>=abs(tri1.pB)))
    temp = point_inside_triangle((triangle_ptr)&tri1,(vertex_ptr)&p,
                                                     TRUE,TRUE,FALSE);

  if (temp==TRUE) {
    // Point needs to be checked to see if it lies between the two vertices
    // First check for the special case of vertical line segments
    if ((tri2.b.x == tri2.c.x)&&(tri2.b.z == tri2.c.z)) {
      if (((tri2.b.y <= p.y)&&(p.y <= tri2.c.y))||
          ((tri2.c.y <= p.y)&&(p.y <= tri2.b.y)))
        return TRUE;
    }

    // Now check for point on line segment
    if (point_inbetween_vertices((vertex_ptr)&tri2.b,(vertex_ptr)&tri2.c,
                                              (triangle_ptr)&tri1)==TRUE)
      return TRUE;
    else
      return FALSE;
  }

  // Third iteration  (c,a)
  p=line_plane_collision((vertex_ptr)&tri2.c,(vertex_ptr)&tri2.a,
                                             (triangle_ptr)&tri1);

  // Determine which axis to project to
  // X is greatest
  if ((abs(tri1.pA)>=abs(tri1.pB))&&(abs(tri1.pA)>=abs(tri1.pC)))
    temp = point_inside_triangle((triangle_ptr)&tri1,(vertex_ptr)&p,
                                                     FALSE,TRUE,TRUE);
  // Y is greatest
  else if ((abs(tri1.pB)>=abs(tri1.pA))&&(abs(tri1.pB)>=abs(tri1.pC)))
    temp = point_inside_triangle((triangle_ptr)&tri1,(vertex_ptr)&p,
                                                     TRUE,FALSE,TRUE);
  // Z is greatest
  else if ((abs(tri1.pC)>=abs(tri1.pA))&&(abs(tri1.pC)>=abs(tri1.pB)))
    temp = point_inside_triangle((triangle_ptr)&tri1,(vertex_ptr)&p,
                                                     TRUE,TRUE,FALSE);

  if (temp==TRUE) {
    // Point needs to be checked to see if it lies between the two vertices
    // First check for the special case of vertical line segments
    if ((tri2.c.x == tri2.a.x)&&(tri2.c.z == tri2.a.z)) {
      if (((tri2.c.y <= p.y)&&(p.y <= tri2.a.y))||
          ((tri2.a.y <= p.y)&&(p.y <= tri2.c.y)))
        return TRUE;
    }

    // Now check for point on line segment
    if (point_inbetween_vertices((vertex_ptr)&tri2.c,(vertex_ptr)&tri2.a,
                                              (triangle_ptr)&tri1)==TRUE)
      return TRUE; // Intersection point is inside the triangle and on
    else           // the line segment
      return FALSE;
  }
  return FALSE; // Default value/no collision
}

/*
***********************************************************
* FUNCTION: init()
*  PURPOSE: Set the triangle coordinates, intialize OpenGL,
*           and load the textures
*********************************************************** */
void init(void)
{
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel (GL_FLAT);
  // Set initial triangle coordinates
  // Triangle 1
#ifdef NORMAL
  tri1.a.x = (float) 1.0;
  tri1.a.y = (float) 1.0;
  tri1.a.z = (float) 3.0;
  tri1.b.x = (float) 5.0;
  tri1.b.y = (float) 6.0;
  tri1.b.z = (float) 2.0;
  tri1.c.x = (float) 7.0;
  tri1.c.y = (float) 2.0;
  tri1.c.z = (float) 2.0;
  tri1.pA  = (float) 0.0;
  tri1.pB  = (float) 0.0;
  tri1.pC  = (float) 0.0;
  tri1.pD  = (float) 0.0;
  find_polygon_radius((triangle_ptr)&tri1);
#endif

#ifdef FLAT
  tri1.a.x = (float) 0.0;
  tri1.a.y = (float) 0.0;
  tri1.a.z = (float) 0.0;
  tri1.b.x = (float) 5.0;
  tri1.b.y = (float) 0.0;
  tri1.b.z = (float) 5.0;
  tri1.c.x = (float)10.0;
  tri1.c.y = (float) 0.0;
  tri1.c.z = (float) 0.0;
  tri1.pA  = (float) 0.0;
  tri1.pB  = (float) 0.0;
  tri1.pC  = (float) 0.0;
  tri1.pD  = (float) 0.0;
  find_polygon_radius((triangle_ptr)&tri1);
#endif

#ifdef FLAT_RIGHT
  tri1.a.x = (float) 0.0;
  tri1.a.y = (float) 0.0;
  tri1.a.z = (float) 0.0;
  tri1.b.x = (float) 0.0;
  tri1.b.y = (float) 0.0;
  tri1.b.z = (float) 5.0;
  tri1.c.x = (float) 5.0;
  tri1.c.y = (float) 0.0;
  tri1.c.z = (float) 0.0;
  tri1.pA  = (float) 0.0;
  tri1.pB  = (float) 0.0;
  tri1.pC  = (float) 0.0;
  tri1.pD  = (float) 0.0;
  find_polygon_radius((triangle_ptr)&tri1);
#endif

#ifdef VERTICAL
  tri1.a.x = (float) 0.0;
  tri1.a.y = (float) 0.0;
  tri1.a.z = (float) 0.0;
  tri1.b.x = (float) 2.5;
  tri1.b.y = (float) 5.0;
  tri1.b.z = (float) 0.0;
  tri1.c.x = (float) 5.0;
  tri1.c.y = (float) 0.0;
  tri1.c.z = (float) 0.0;
  tri1.pA  = (float) 0.0;
  tri1.pB  = (float) 0.0;
  tri1.pC  = (float) 0.0;
  tri1.pD  = (float) 0.0;
  find_polygon_radius((triangle_ptr)&tri1);
#endif

#ifdef VERTICAL_RIGHT
  tri1.a.x = (float) 0.0;
  tri1.a.y = (float) 0.0;
  tri1.a.z = (float) 0.0;
  tri1.b.x = (float) 0.0;
  tri1.b.y = (float) 5.0;
  tri1.b.z = (float) 0.0;
  tri1.c.x = (float) 5.0;
  tri1.c.y = (float) 0.0;
  tri1.c.z = (float) 0.0;
  tri1.pA  = (float) 0.0;
  tri1.pB  = (float) 0.0;
  tri1.pC  = (float) 0.0;
  tri1.pD  = (float) 0.0;
  find_polygon_radius((triangle_ptr)&tri1);
#endif

#ifdef CUSTOM
  tri1.a.x = (float) 1.0;
  tri1.a.y = (float) 1.0;
  tri1.a.z = (float) 3.0;
  tri1.b.x = (float) 5.0;
  tri1.b.y = (float) 6.0;
  tri1.b.z = (float) 2.0;
  tri1.c.x = (float) 7.0;
  tri1.c.y = (float) 2.0;
  tri1.c.z = (float) 2.0;
  tri1.pA  = (float) 0.0;
  tri1.pB  = (float) 0.0;
  tri1.pC  = (float) 0.0;
  tri1.pD  = (float) 0.0;
  find_polygon_radius((triangle_ptr)&tri1);
#endif

  // Triangle 2
  tri2.a.x = (float) 2.0;
  tri2.a.y = (float) 3.0;
  tri2.a.z = (float) 4.0;
  tri2.b.x = (float) 7.0;
  tri2.b.y = (float) 1.0;
  tri2.b.z = (float) 1.0;
  tri2.c.x = (float) 8.0;
  tri2.c.y = (float) 2.0;
  tri2.c.z = (float) 3.0;
  tri2.pA  = (float) 0.0;
  tri2.pB  = (float) 0.0;
  tri2.pC  = (float) 0.0;
  tri2.pD  = (float) 0.0;
  find_polygon_radius((triangle_ptr)&tri2);

  // Camera (radius = 30);
  cam_angle+=(3.14159265359/60);
  cam_x = (float)(15*(cos(cam_angle)));
  cam_z = (float)(15*(sin(cam_angle)));
  height=15.0;

  DRAW_BS = FALSE;

  glGenTextures(1, &id1);
  glBindTexture(GL_TEXTURE_2D, id1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  if (pngLoad("colliding.png", PNG_NOMIPMAP, PNG_SOLID, &info1)) {
    puts("Loaded colliding.png with resounding success.");
    printf("Size=%i,%i Depth=%i Alpha=%i\n", info1.Width, info1.Height,
                                             info1.Depth, info1.Alpha);
  }
  else {
    puts("Can't load collide.png.\n");
    exit(1);
  }

  glGenTextures(1, &id2);
  glBindTexture(GL_TEXTURE_2D, id2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  if (pngLoad("not_colliding.png", PNG_NOMIPMAP, PNG_SOLID, &info2)) {
    puts("Loaded not_colliding.png with resounding success.");
    printf("Size=%i,%i Depth=%i Alpha=%i\n", info2.Width, info2.Height,
                                             info2.Depth, info2.Alpha);
  }
  else {
    puts("Can't load not_colliding.png");
    exit(1);
  }

  glGenTextures(1, &id3);
  glBindTexture(GL_TEXTURE_2D, id3);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  if (pngLoad("maybe_colliding.png", PNG_NOMIPMAP, PNG_SOLID, &info3)) {
    puts("Loaded maybe_colliding.png with resounding success.");
    printf("Size=%i,%i Depth=%i Alpha=%i\n", info3.Width, info3.Height,
                                             info3.Depth, info3.Alpha);
  }
  else {
    puts("Can't load maybe_colliding.png");
    exit(1);
  }
}

/*
****************************************
* FUNCTION: display()
*  PURPOSE: Draw everything using OpenGL
**************************************** */
void display(void)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable (GL_DEPTH_TEST);
  glLoadIdentity ();
  gluLookAt (cam_x, height, cam_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  // Draw axis
  drawcross(0,0,0,25);

  // Geometry here
  // Triangle 1
  glColor3f (1.0, 0.5, 1.0);
  glPushMatrix();
  glBegin(GL_POLYGON);
  glVertex3f(tri1.a.x, tri1.a.y, tri1.a.z);
  glVertex3f(tri1.b.x, tri1.b.y, tri1.b.z);
  glVertex3f(tri1.c.x, tri1.c.y, tri1.c.z);
  glEnd();
  glPopMatrix();

  if (DRAW_BS==TRUE) {
    glPushMatrix();
      glTranslatef (tri1.center.x,tri1.center.y,tri1.center.z);
      glutWireSphere(tri1.radius,64,64);
    glPopMatrix();
  }

  // Triangle 2
  glColor3f (0.5, 1.0, 0.5);
  glPushMatrix();
  glBegin(GL_POLYGON);
  glVertex3f(tri2.a.x, tri2.a.y, tri2.a.z);
  glVertex3f(tri2.b.x, tri2.b.y, tri2.b.z);
  glVertex3f(tri2.c.x, tri2.c.y, tri2.c.z);
  glEnd();
  glPopMatrix();

  if (DRAW_BS==TRUE) {
    glPushMatrix();
      glTranslatef (tri2.center.x,tri2.center.y,tri2.center.z);
      glutWireSphere(tri2.radius,64,64);
    glPopMatrix();
  }

  // Floor
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,id2);

  // Collision detection goes here 
  if (check_for_bounding_collision((triangle_ptr)&tri1,
                                   (triangle_ptr)&tri2)==TRUE) {
    glBindTexture(GL_TEXTURE_2D,id3);
    if (triangles_colliding(tri1,tri2)==TRUE)
      glBindTexture(GL_TEXTURE_2D,id1);
    else {
      if (triangles_colliding(tri2,tri1)==TRUE)
        glBindTexture(GL_TEXTURE_2D,id1);
      else
        glBindTexture(GL_TEXTURE_2D,id3);
    }
  }
  else
    glBindTexture(GL_TEXTURE_2D,id2);

  glColor3f (0.5, 0.5, 0.5);
  glPushMatrix();
  glBegin(GL_POLYGON);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(-10,-10,5);

  glTexCoord2f(0.0, 0.0);
  glVertex3f(-10,-10,-5);

  glTexCoord2f(1.0, 0.0);
  glVertex3f(10,-10,-5);

  glTexCoord2f(1.0, 1.0);
  glVertex3f(10,-10,5);
  glEnd();
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);

  glutSwapBuffers();
}

/*
************************************
* FUNCTION: keyboard()
*  PURPOSE: Define keyboard behavior
************************************ */
void keyboard(unsigned char key, int x, int y)		// Handle 'ESC'
{
   switch (key) {
          case 27:  // Handle 'esc'
            { 
            exit(0); 
            }
            break;
	  case '1':
            tri1.a.x-=.25;
            tri1.b.x-=.25;
            tri1.c.x-=.25;
            tri1.center.x-=.25;
            glutPostRedisplay();
            break;
	  case '2':
            tri1.a.x+=.25;
            tri1.b.x+=.25;
            tri1.c.x+=.25;
            tri1.center.x+=.25;
            glutPostRedisplay();
            break;
	  case '3':
            tri1.a.y-=.25;
            tri1.b.y-=.25;
            tri1.c.y-=.25;
            tri1.center.y-=.25;
            glutPostRedisplay();
            break;
	  case '4':
            tri1.a.y+=.25;
            tri1.b.y+=.25;
            tri1.c.y+=.25;
            tri1.center.y+=.25;
            glutPostRedisplay();
            break;
	  case '5':
            tri1.a.z-=.25;
            tri1.b.z-=.25;
            tri1.c.z-=.25;
            tri1.center.z-=.25;
            glutPostRedisplay();
            break;
	  case '6':
            tri1.a.z+=.25;
            tri1.b.z+=.25;
            tri1.c.z+=.25;
            tri1.center.z+=.25;
            glutPostRedisplay();
            break;
	  case '7':
            if (DRAW_BS==FALSE)
              DRAW_BS=TRUE;
            glutPostRedisplay();
            break;
	  case '8':
            if (DRAW_BS==TRUE)
              DRAW_BS=FALSE;
            glutPostRedisplay();
            break;
	  case '+':
            height+=.5;
            glutPostRedisplay();
            break;
	  case '-':
            height-=.5;
            glutPostRedisplay();
            break;
	  case 'q':
            cam_angle-=(3.14159265359/60);
            cam_x = (float)(15*(cos(cam_angle)));
            cam_z = (float)(15*(sin(cam_angle)));
            glutPostRedisplay();
            break;
	  case 'e':
            cam_angle+=(3.14159265359/60);
            cam_x = (float)(15*(cos(cam_angle)));
            cam_z = (float)(15*(sin(cam_angle)));
            glutPostRedisplay();
            break;
   }
}

/*
*************************************************
* FUNCTION: reshape()
*  PURPOSE: Tell OpenGL what to do upon a reshape
************************************************* */
void reshape(int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective(40.0, 1.333333333, .1, 200);
  glMatrixMode (GL_MODELVIEW);
}

/*
*********************************************************
* FUNCTION: main()
*  PURPOSE: The bulk of the program
********************************************************* */
int main(int argc, char** argv)
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (640,480);
  glutInitWindowPosition (0, 0);
  glutCreateWindow (window_header);
  init ();
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}

