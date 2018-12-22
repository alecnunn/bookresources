/* Copyright (C) Gabor Nagy, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gabor Nagy, 2000"
 */
/*==============================================================================*/
/* Real-time cast shadows							*/
/*										*/
/* - Geometry library include file						*/
/*										*/
/* AUTHOR:	Gabor Nagy							*/
/* DATE:	2000-Apr-24 18:43:05						*/
/*  v1.0									*/
/*										*/
/* For Game Programming Graphics Gems						*/
/*==============================================================================*/
#include <math.h>
#include <stdlib.h>


#if defined (__cplusplus) && (!(defined (__CPLUSLIBS)))
extern "C" {
#endif

// Define boolean flags
//
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif



// For matrix indexing
//
enum { M00=0,M01,M02,M03, M10,M11,M12,M13, M20,M21,M22,M23, M30,M31,M32,M33 };

typedef float	E3dType;
typedef float	E3dCoordType;
typedef float	E3dAngleType;
typedef float	E3dColorType;
typedef float	E3dMatrix[16];

typedef char	EBool;

typedef struct
{
 float		X, Y, Z;
} E3d3DPosition;

typedef struct
{
 float		X, Y, Z;
} E3dVector;

typedef struct
{
 float		X, Y, Z;
} E3dRotation;

typedef struct
{
 float		S, T;
} E3dST;

typedef struct
{
 float		R, G, B;
} E3dFColor;


typedef enum
{
 E3dLIGHT_POINT=1,
 E3dLIGHT_SPOT,
 E3dLIGHT_INFINITE,
 E3dLIGHT_SUN
} E3dLightType;

typedef struct E3dLight
{
 char*			Name;			// Light name
 char*			Prefix;			// Light prefix
 struct E3dModel*	Model;			// Pointer to the model struct
 E3dLightType		Type;			// Light source type
 E3d3DPosition		Position;		// Light source position
 E3d3DPosition		CamTransPosition;	// Light source position transformed with the camera transformation matrix
 E3dFColor		Color;			// Color of the light source
 float			ConstAttenuation,
			LinAttenuation,
			SquareAttenuation;
} E3dLight;


#define	E3dRADIANFACTOR	57.295779513082320877
#define E3dRADIANDIV	0.017453292519943295769



/*----------------------------------------------------------------------*/
/* Material types							*/
/*----------------------------------------------------------------------*/
enum
{
 E3dMAT_CONSTANT=0,
 E3dMAT_PHONG,
 E3dMAT_LAMBERT
};


typedef struct E3dMaterial
{
 char*		Name;
 unsigned long	RefCnt;				// Reference count
 char		Type;

 float		Ambient[4];
 float		Diffuse[4];
 float		Specular[4];
 float		Emission[4];
 float		Shininess[1];
 float		Specularity;
/*
 unsigned short	NumOf2DTextures;
 E3d2DTexture**	Textures2D;
*/
} E3dMaterial;


/*==============================================================================*/
/* Geometries									*/
/*==============================================================================*/
// General geometry structure (these are the first fields are in all geometry structures)
//
typedef struct
{
 short		GeoType;		// Geometry type
 unsigned long	RefCnt;			// Reference count for freeing (how many structures reference this mesh)
} E3dGeometry;











#define E3dVTXALLOC_INCREMENT		32
#define E3dPOLYALLOC_INCREMENT		32
#define E3dPOLYGROUPALLOC_INCREMENT	4


#define E3dNONE				0
#define E3dUSE_VERTEX_NORMALS		1
#define E3dPOLY_NORMALS_UPTODATE	(1<<1)
#define E3dVERTEX_NORMALS_UPTODATE	(1<<2)
#define E3dUPTODATE			(E3dPOLY_NORMAL_UPTODATE|E3dVERTEX_NORMALS_UPTODATE)
#define E3dALL				0xFF

typedef struct
{
 E3dType	X,Y,Z;
 short		Flags;
} E3dVertex;


typedef struct
{
 long		VertexID;
 E3dVector	Normal;

 float		GLVertex[3];
 float		GLNormal[3];
 float		ST[2];
} E3dVertexNode;


typedef struct E3dPolygon
{
 E3dVector		Normal;

 unsigned long		NumOfVertices;
 E3dVertexNode*		VertexNodes;
} E3dPolygon;


typedef struct
{
 unsigned long		RefCnt;			// Reference count for freeing (how many structures reference this PolyGroup)

 char*			Name;

 E3dMaterial*		Material;		// The E3dPolyGroup's own material

 unsigned long		NumOfPolygons;		// Number of polygons in the group
 E3dPolygon*		Polygons;		// Array of polygons

 int			VertexNormalType;	// E3dPolyGroup is FACETED (no per-vertex normals on polygons), polygon vertex normals
						// were computed in the geometry creation PROCESS, are ALL AVERAGED from the
						// polygons using a given vertex or computed from the DISCONTINUITY ANGLE
 int			NormalFlags;		// Shows whether the polygon normals and/or the vertex normals are up-to-date
 EBool			ShadowPass;		// Shows if this PolyGroup is a shadow-pass on a receiver object

} E3dPolyGroup;


typedef struct
{
 short		GeoType;		// Geometry type
 unsigned long	RefCnt;			// Reference count for freeing (how many structures reference this mesh)

 unsigned long	NumOfVertices;
 E3dVertex*	Vertices;

 unsigned long	NumOfPolyGroups;	// Polygon groups
 E3dPolyGroup**	PolyGroups;
} E3dMesh;












typedef struct E3dModel
{
 char*			Name;

 unsigned int		NumOfGeometries;
 E3dGeometry*		Geometry;			// Model's geometry (E3dMesh, E3dSpline ...)
 E3dMaterial*		Material;			// Main material of the Model
 E3d3DPosition		Translation;
 E3d3DPosition		Scaling;
 E3dRotation		Rotation;
 E3dMatrix		LocalToWorldMatrix;		// Local->World matrix for faster display if only the camera moves
 E3dMatrix		NormalLocalToWorldMatrix;	// Local->World matrix for normals (no scaling)
 E3dMatrix		ShadowMapUVMatrix;		// Local->Shadow-map UV matrix

#ifdef USEOpenGL
 unsigned int		GLDisplayListID;		// OpenGL display list Id
 EBool			GLDisplayListUptodate;
#endif // USEOpenGL

 E3d3DPosition		BBoxMin,BBoxMax;		// Bounding box

} E3dModel;



/*======================================*/
/* Cross product of two vectors		*/
/*======================================*/
#define E3dM_VectorCrossProduct(mAx,mAy,mAz,mBx,mBy,mBz,mCx,mCy,mCz) mCx = mAy*mBz - mAz*mBy,mCy = mAz*mBx - mAx*mBz,mCz = mAx*mBy - mAy*mBx


/*==============================================================*/
/* Transform a vector with a given matrix's upper-left 3x3 part	*/
/*==============================================================*/
#define E3dM_MatrixTransformVector3x3(mtx,xo,yo,zo)\
 (xo)=Mx*(mtx)[M00]+My*(mtx)[M10]+Mz*(mtx)[M20],\
 (yo)=Mx*(mtx)[M01]+My*(mtx)[M11]+Mz*(mtx)[M21],\
 (zo)=Mx*(mtx)[M02]+My*(mtx)[M12]+Mz*(mtx)[M22]


/*==============================================*/
/* Transform a position with a given matrix	*/
/*==============================================*/
#define E3dM_MatrixTransform3x4(mtx,xo,yo,zo)\
 (xo)=Mx*(mtx)[M00]+My*(mtx)[M10]+Mz*(mtx)[M20]+(mtx)[M30],\
 (yo)=Mx*(mtx)[M01]+My*(mtx)[M11]+Mz*(mtx)[M21]+(mtx)[M31],\
 (zo)=Mx*(mtx)[M02]+My*(mtx)[M12]+Mz*(mtx)[M22]+(mtx)[M32]


/*======================================================================*/
/* Transform a position as a homogeneous position with a given matrix	*/
/*======================================================================*/
#define E3dM_MatrixTransformHPosition(matrix)\
 LXF=Mx*matrix[M00]+My*matrix[M10]+Mz*matrix[M20]+matrix[M30],\
 LYF=Mx*matrix[M01]+My*matrix[M11]+Mz*matrix[M21]+matrix[M31],\
 LZF=Mx*matrix[M02]+My*matrix[M12]+Mz*matrix[M22]+matrix[M32],\
 LWF=Mx*matrix[M03]+My*matrix[M13]+Mz*matrix[M23]+matrix[M33]




extern void		E3d_DrawModel(E3dModel* LModel, EBool LTextureOn, E3dLight* LLightForShadow, EBool LRemoveBackfaceShadows);

extern void		E3d_LightDefault(E3dLight* LLight);
extern void		E3d_GLUpdateLight(int LGLLight, E3dLight* LLight);

extern void		E3d_MaterialDefault(E3dMaterial* LMaterial);
extern E3dMaterial E3d_DefaultMaterial;

extern E3dMaterial*	E3d_MaterialAllocate();


extern void		E3d_PrintMatrix(E3dMatrix LMatrix);
extern void		E3d_MatrixCopy(E3dMatrix LSrcMtx, E3dMatrix LDestMtx);
extern void		E3d_MatrixRotate(E3dMatrix LMatrix, int LAxis, double LAngle);
extern void		E3d_MatrixMult(E3dMatrix LSrcMtx,E3dMatrix LDestMtx);

extern E3dModel*	E3d_ModelAllocate(char* LName);
extern void		E3d_ModelFree(E3dModel* LFModel);
extern void		E3d_ModelRefreshMatrices(E3dModel* LModel);

extern E3dVertexNode*	E3d_PolygonVertexNodeAllocate(E3dPolygon* LPolygon, int LNumOfVertexNodes);
extern E3dPolygon*	E3d_PolygonAddOneToArray(E3dPolygon* LPolygons, unsigned long LNumOfPolys, unsigned long* LNumOfPolysAllocatedPtr);

extern void		E3d_PolyGroupRefreshPolygonNormals(E3dMesh* LMesh, E3dPolyGroup* LPolyGroup);


extern E3dMesh*		E3d_MeshAllocate();
extern E3dPolyGroup*	E3d_MeshAddPolyGroup(E3dMesh* LMesh);
extern void		E3d_MeshRefreshGLPolyVertices(E3dMesh* LMesh, EBool LDoNormals);









#if defined (__cplusplus) && (!(defined (__CPLUSLIBS)))
}	// Close scope of 'extern "C"' declaration which encloses file.
#endif
