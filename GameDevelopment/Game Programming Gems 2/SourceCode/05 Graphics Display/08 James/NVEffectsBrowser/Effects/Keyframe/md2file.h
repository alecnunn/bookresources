#ifndef __MD2FILE_H__
#define __MD2FILE_H__

/* Copyright NVIDIA Corporation, 2000. */

/* $Id: //sw/main/apps/OpenGL/mjk/md2shader/md2file.h#9 $ */

#include <stdlib.h>

#define MD2_MAX_TRIANGLES		4096
#define MD2_MAX_VERTICES		2048
#define MD2_MAX_TEXCOORDS		2048
#define MD2_MAX_FRAMES			512
#define MD2_MAX_SKINS			32
#define MD2_MAX_FRAMESIZE		(MD2_MAX_VERTICES * 4 + 128)

typedef struct { 
  int magic; 
  int version; 
  int skinWidth; 
  int skinHeight; 
  int frameSize; 
  int numSkins; 
  int numVertices; 
  int numTexCoords; 
  int numTriangles; 
  int numGlCommands; 
  int numFrames; 
  int offsetSkins; 
  int offsetTexCoords; 
  int offsetTriangles; 
  int offsetFrames; 
  int offsetGlCommands; 
  int offsetEnd; 
} Md2Header;  /* File format structure! */

typedef struct {
  unsigned char vertex[3];
  unsigned char lightNormalIndex;
} Md2AliasTriangleVertex;

typedef struct {
  float vertex[3];
  float normal[3];
} Md2TriangleVertex;


typedef struct {
  short vertexIndex;
  short triangle;
  short edge;
  short prev;
  short next[2];
  int active;
  float maxSqArea;
} Md2Boundary;

typedef struct {
  short vertexIndices[3];
  short textureIndices[3];
} Md2Triangle;

typedef struct {
  float p[4];  /* Plane equation the triangle in object space. */
} Md2FrameTrianglePlane;

typedef struct {
  short adjacentTriangle[3];
  char open;
  char openEdgeMask;
} Md2TriangleEdgeInfo;

typedef struct {
  short s, t;
} Md2TextureCoordinate;

typedef struct {
  float scale[3];
  float translate[3];
  char name[16];
  Md2AliasTriangleVertex alias_vertices[1];
} Md2AliasFrame;

typedef struct {
  char name[16];
  Md2TriangleVertex *vertices;
} Md2Frame;

#if 0
typedef char Md2Skin[64];
#endif

typedef struct {
  float s, t;
  int vertexIndex;
} Md2CommandVertex;

typedef struct {
  Md2Header header;
#if 0
  Md2Skin *skins;
#endif
  Md2TextureCoordinate *texCoords;
  Md2Triangle *triangles;
  Md2Frame *frames;
  Md2TriangleEdgeInfo *edgeInfo;
  Md2FrameTrianglePlane *framePlane;
  char *filename;
} Md2Model;

/*typedef struct {
  Md2Model *model;
  int smooth;
  int vertexLighting;
  int specular;
  int shadowVolumes;
  int numLights;
  size_t vertexInfoSize;
  void *vertexInfo;
  unsigned char *facingness;
  unsigned char *possibleSilhouetteEdgeMask;
  Vector *objectSpaceLightPosition;
} Md2ModelInstant;
*/
#define NUMVERTEXNORMALS 162

extern float md2VertexNormals[NUMVERTEXNORMALS][3];
extern void md2FreeModel(Md2Model *model);
extern Md2Model *md2ReadModel(const char *filename);

#endif /* __MD2FILE_H__ */
