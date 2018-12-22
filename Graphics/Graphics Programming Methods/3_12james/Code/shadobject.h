// Rob James 2003

#include <stdio.h>


// Basic 3D data structure

// Point
struct vert_s
{
   double x, y, z;
   double nx,ny, nz;
};

// Plane
struct planeeq_s
{
   double a, b, c, d;
};

// Face
struct face_s{
   unsigned int		verts[3]; // Three vertices
   vert_s			norms[3]; // Three Normals
   planeeq_s		planeeq; // Plane Equation for fast light-facing test
   unsigned char    visible; // Vis flag
   unsigned int		neigh[3]; // Index to 3 neighbhouring faces
   unsigned int		smoothing;   // smoothing group for normal calc 
};

#define MAXPOINTS 50000
#define MAXFACES  50000
// Model
struct model_s{
   unsigned int  vcount;
   unsigned int  fcount;
   vert_s  verts [MAXPOINTS];
   face_s  faces [MAXFACES];
};



extern int	 LoadAscObject(char *mfile, model_s *model);
extern void	 drawModel(model_s *model);
extern void  drawModelShadow(model_s *model, double *lp, bool showshadowvol);
extern void  drawModelShadowCR(model_s *model, float *lp, bool showshadowvol, double pscale, bool final);
extern void  drawModelShadowVolume(model_s *model, float *lp, double pscale);
extern void  drawShadowQuad();

