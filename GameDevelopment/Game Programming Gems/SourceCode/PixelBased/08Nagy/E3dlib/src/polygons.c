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
/* Simple 3D library for reading/displaying geometry				*/
/*										*/
/* - Polygon-related functions							*/
/*										*/
/* AUTHOR:	Gabor Nagy							*/
/* DATE:	2000-May-02 14:46:31						*/
/*  v1.0									*/
/*										*/
/* For Game Programming Graphics Gems						*/
/*==============================================================================*/
#ifdef _WIN32
#include <windows.h>
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <GL/gl.h>

#include "geo.h"
#include "str.h"

/*======================================*/
/* Allocate vertex nodes for a polygon	*/
/*======================================*/
E3dVertexNode* E3d_PolygonVertexNodeAllocate(E3dPolygon* LPolygon,int LNumOfVertexNodes)
{
 E3dVertexNode*	LVertexNodes;

 if(LNumOfVertexNodes>0)
 {
  LVertexNodes=(E3dVertexNode*)EMalloc(sizeof(E3dVertexNode)*LNumOfVertexNodes);
  if(LVertexNodes!=NULL)
  {
   LPolygon->NumOfVertices=LNumOfVertexNodes;
  }
  LPolygon->VertexNodes=LVertexNodes;
 }
 return(LVertexNodes);
}




/*======================================*/
/* Default a Polygon			*/
/*======================================*/
void E3d_PolygonDefault(E3dPolygon* LPolygon)
{
 LPolygon->NumOfVertices=0;
 LPolygon->VertexNodes=NULL;
}


/*======================================*/
/* Allocate polygons array		*/
/*======================================*/
E3dPolygon* E3d_PolygonsAllocate(unsigned long LNumOfPolys)
{
 E3dPolygon*	LAPolygon;
 E3dPolygon*	LIPolygon;
 unsigned long	LC;

 if(LNumOfPolys>0) LAPolygon=(E3dPolygon*)EMalloc(sizeof(E3dPolygon)*LNumOfPolys);

 if(LAPolygon!=NULL)
 {
  LIPolygon=LAPolygon;
  for(LC=0;LC<LNumOfPolys;LC++,LIPolygon++)
  {
   E3d_PolygonDefault(LIPolygon);
  }
 }
 return(LAPolygon);
}


/*======================================*/
/* Add a polygon to Polygon array	*/
/*======================================*/
E3dPolygon* E3d_PolygonAddOneToArray(E3dPolygon* LPolygons,unsigned long LNumOfPolys,unsigned long* LNumOfPolysAllocatedPtr)
{
 unsigned long	LNumOfPolysAllocated;
 E3dPolygon*	LPolygonsT;

 if(LNumOfPolysAllocatedPtr!=NULL) LNumOfPolysAllocated=*LNumOfPolysAllocatedPtr;
 else LNumOfPolysAllocated=0;

 if(LPolygons==NULL)
 {
  if((LPolygons=E3d_PolygonsAllocate(E3dPOLYALLOC_INCREMENT))!=NULL)
  {
   if(LNumOfPolysAllocatedPtr!=NULL) *LNumOfPolysAllocatedPtr=E3dPOLYALLOC_INCREMENT;
  }
  else return(NULL);
 }
 else
 {
  if(LNumOfPolys>=LNumOfPolysAllocated)
  {
   if((LPolygonsT=(E3dPolygon*)ERealloc(LPolygons,sizeof(E3dPolygon)*(LNumOfPolys+E3dPOLYALLOC_INCREMENT)))!=NULL)
   {
    LPolygons=LPolygonsT;
    if(LNumOfPolysAllocatedPtr!=NULL) *LNumOfPolysAllocatedPtr=LNumOfPolysAllocated+E3dPOLYALLOC_INCREMENT;
   }
   else return(NULL);
  }
 }

 if(LPolygons!=NULL) E3d_PolygonDefault(LPolygons+LNumOfPolys);

 return(LPolygons);
}


/*======================================*/
/* Allocate PolyGroup			*/
/*======================================*/
E3dPolyGroup* E3d_PolyGroupAllocate()
{
 E3dPolyGroup*	LPolyGroup;

 if((LPolyGroup=(E3dPolyGroup*)EMalloc(sizeof(E3dPolyGroup)))!=NULL)
 {
  LPolyGroup->RefCnt=0;

  LPolyGroup->Name=NULL;

  LPolyGroup->Material=NULL;

  LPolyGroup->NormalFlags=E3dNONE;
  LPolyGroup->NumOfPolygons=0;
  LPolyGroup->Polygons=NULL;

  LPolyGroup->ShadowPass=FALSE;
 }
 return(LPolyGroup);
}


/*======================================*/
/* Allocate a polygon mesh		*/
/*======================================*/
E3dMesh* E3d_MeshAllocate()
{
 E3dMesh*	LMesh;

 if((LMesh=(E3dMesh*)EMalloc(sizeof(E3dMesh)))!=NULL)
 {
  LMesh->RefCnt=0;
  LMesh->NumOfVertices=0;
  LMesh->Vertices=NULL;
  LMesh->NumOfPolyGroups=0;
  LMesh->PolyGroups=NULL;

  return(LMesh);
 }
 return(NULL);
}


/*======================================*/
/* Add a polygon group to a Mesh	*/
/*======================================*/
E3dPolyGroup* E3d_MeshAddPolyGroup(E3dMesh* LMesh)
{
 E3dPolyGroup*	LPolyGroup;
 E3dPolyGroup**	LPolyGroups;

 if(LMesh->PolyGroups==NULL)
 {
  if((LPolyGroups=(E3dPolyGroup**)EMalloc(sizeof(E3dPolyGroup*)))!=NULL) LMesh->PolyGroups=LPolyGroups;
 }
 else
 {
  if((LPolyGroups=(E3dPolyGroup**)ERealloc(LMesh->PolyGroups,sizeof(E3dPolyGroup*)*(LMesh->NumOfPolyGroups+1)))!=NULL)
  {
   LMesh->PolyGroups=LPolyGroups;
  }
 }
 if(LPolyGroups!=NULL)
 {
  if((LPolyGroup=E3d_PolyGroupAllocate())!=NULL)
  {
   LPolyGroups[LMesh->NumOfPolyGroups]=LPolyGroup;

   LPolyGroup->RefCnt=1;

   LMesh->NumOfPolyGroups+=1;
  }
 }
 return(LPolyGroup);
}


/*==============================================================================*/
/* Refresh GL vertices and GL polygon and vertex normals of the polygons a mesh	*/
/*==============================================================================*/
void E3d_MeshRefreshGLPolyVertices(E3dMesh* LMesh,EBool LDoNormals)
{
 register unsigned long		LGCnt,LGNum,LVNum,LVCnt,LPCnt,LPNum;
 register long			LVertexID;
 register E3dVertex*		LVertices;
 E3dVertex*			LVertex;
 register E3dVertexNode*	LVertexNode;
 E3dPolyGroup*			LPolyGroup;
 E3dPolyGroup**			LPolyGroups;
 register E3dPolygon*		LPolygon;
 float				LCX,LCY,LCZ,LX,LY,LZ;

 LVertices=LMesh->Vertices;

 LGNum=LMesh->NumOfPolyGroups;LPolyGroups=LMesh->PolyGroups;

 for(LGCnt=0;LGCnt<LGNum;LGCnt++)
 {
  LPolyGroup=LPolyGroups[LGCnt];

  LPNum=LPolyGroup->NumOfPolygons;LPolygon=LPolyGroup->Polygons;
  for(LPCnt=0;LPCnt<LPNum;LPCnt++,LPolygon++)
  {
   LVNum=LPolygon->NumOfVertices;
   LVertexNode=LPolygon->VertexNodes;
   LCX=LCY=LCZ=0.0;

   for(LVCnt=0;LVCnt<LVNum;LVCnt++,LVertexNode++)
   {
    if((LVertexID=LVertexNode->VertexID)>=0)
    {
     LVertexID=LVertexNode->VertexID;LVertex=LVertices+LVertexID;
     
     LX=LVertex->X;LY=LVertex->Y;LZ=LVertex->Z;
     LVertexNode->GLVertex[0]=LX;
     LVertexNode->GLVertex[1]=LY;
     LVertexNode->GLVertex[2]=LZ;

     LCX+=LX;LCY+=LY;LCZ+=LZ;
    }
   }
  }
 }

 if(LDoNormals)
 {
  for(LGCnt=0;LGCnt<LGNum;LGCnt++)
  {
   LPolyGroup=LPolyGroups[LGCnt];
   LPNum=LPolyGroup->NumOfPolygons;LPolygon=LPolyGroup->Polygons;
   for(LPCnt=0;LPCnt<LPNum;LPCnt++,LPolygon++)
   {
    LVNum=LPolygon->NumOfVertices;
    LVertexNode=LPolygon->VertexNodes;
    for(LVCnt=0;LVCnt<LVNum;LVCnt++,LVertexNode++)
    {
     if(LVertexNode->VertexID!=-1)
     {
      LVertexNode->GLNormal[0]=LVertexNode->Normal.X;
      LVertexNode->GLNormal[1]=LVertexNode->Normal.Y;
      LVertexNode->GLNormal[2]=LVertexNode->Normal.Z;
     }
    }
   }
  }
 }
}


/*======================================================*/
/* Recalculate polygon normals of a polygon group	*/
/*======================================================*/
void E3d_PolyGroupRefreshPolygonNormals(E3dMesh* LMesh,E3dPolyGroup* LPolyGroup)
{
 unsigned long		LMeshVertNum,LPCnt,LNumOfPolys,LVertNum,LV0,LV1,LV2;
 E3dVertex*		LMeshVertices;
 E3dPolygon*		LPolygon;
 E3dVertexNode*		LActPolyVertNode;
 E3dCoordType		LX0,LY0,LZ0,LX1,LY1,LZ1,LX2,LY2,LZ2,LNormalX,LNormalY,LNormalZ;
 E3dCoordType		r;


 LNumOfPolys=LPolyGroup->NumOfPolygons;
 LMeshVertNum=LMesh->NumOfVertices;
 LMeshVertices=LMesh->Vertices;

 LPolygon=LPolyGroup->Polygons;
 for(LPCnt=0;LPCnt<LNumOfPolys;LPCnt++,LPolygon++)
 {
  if((LVertNum=LPolygon->NumOfVertices)>2)
  {
   LActPolyVertNode=LPolygon->VertexNodes;
   if(LVertNum==3)
   {
    LV0=LActPolyVertNode[0].VertexID;
    LV1=LActPolyVertNode[1].VertexID;
    LV2=LActPolyVertNode[2].VertexID;
    LX0=LMeshVertices[LV0].X;LY0=LMeshVertices[LV0].Y;LZ0=LMeshVertices[LV0].Z;
    LX1=LMeshVertices[LV1].X;LY1=LMeshVertices[LV1].Y;LZ1=LMeshVertices[LV1].Z;
    LX2=LMeshVertices[LV2].X;LY2=LMeshVertices[LV2].Y;LZ2=LMeshVertices[LV2].Z;

    LNormalX=-((LY2-LY1)*(LZ2-LZ0)-(LZ2-LZ1)*(LY2-LY0));
    LNormalY=(LX2-LX1)*(LZ2-LZ0)-(LZ2-LZ1)*(LX2-LX0);
    LNormalZ=-((LX2-LX1)*(LY2-LY0)-(LY2-LY1)*(LX2-LX0));
   }

   r=(E3dCoordType)sqrt(LNormalX*LNormalX+LNormalY*LNormalY+LNormalZ*LNormalZ);
   if(r>0.0) { r=1.0f/r;LNormalX*=r;LNormalY*=r;LNormalZ*=r; }
   LPolygon->Normal.X=LNormalX;LPolygon->Normal.Y=LNormalY;LPolygon->Normal.Z=LNormalZ;
  }
  else
  {
   LPolygon->Normal.X=0.0;LPolygon->Normal.Y=0.0;LPolygon->Normal.Z=0.0;
  }
 }
 LPolyGroup->NormalFlags|=E3dPOLY_NORMALS_UPTODATE;
}
