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
/* - Simple Wavefront OBJ file reader						*/
/*										*/
/* AUTHOR:	Gabor Nagy							*/
/* DATE:	2000-May-02 14:45:38						*/
/*  v1.0									*/
/*										*/
/* For Game Programming Graphics Gems						*/
/*==============================================================================*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <string.h>
#define MAXPATHLEN 255
#else
#include <strings.h>
#include <sys/param.h>
#endif

#include "obj.h"
#include "str.h"

typedef struct
{
 char*		Name;
 E3dMaterial*	Material;
 E3dPolygon*	Polygons;
 unsigned long	NumOfPolygons,NumOfPolygonsAllocated;
} E3dOBJPolyGroup;


/*======================================================*/
/* Add a polygon group to an E3dOBJPolyGroup array	*/
/*======================================================*/
E3dOBJPolyGroup* E3d_OBJPolyGroupAdd(E3dOBJPolyGroup* LOBJPolyGroups,unsigned int LNumOfPolyGroups,char* LGroupName)
{
 E3dOBJPolyGroup*	LOBJPolyGroupsT;

 if(LOBJPolyGroups==NULL)
 {
  LOBJPolyGroups=(E3dOBJPolyGroup*)EMalloc(sizeof(E3dOBJPolyGroup));
 }
 else
 {
  if((LOBJPolyGroupsT=(E3dOBJPolyGroup*)ERealloc(LOBJPolyGroups,sizeof(E3dOBJPolyGroup)*(LNumOfPolyGroups+1)))!=NULL)
  {
   LOBJPolyGroups=LOBJPolyGroupsT;
  }
 }
 if(LOBJPolyGroups!=NULL)
 {
  LOBJPolyGroups[LNumOfPolyGroups].Name=strdup(LGroupName);
  LOBJPolyGroups[LNumOfPolyGroups].Material=NULL;
  LOBJPolyGroups[LNumOfPolyGroups].Polygons=NULL;
  LOBJPolyGroups[LNumOfPolyGroups].NumOfPolygons=0;
  LOBJPolyGroups[LNumOfPolyGroups].NumOfPolygonsAllocated=0;
 }
 return(LOBJPolyGroups);
}


/*======================================*/
/* Read Material(s) from a ".mtl" file	*/
/*======================================*/
static E3dMaterial** E3d_MaterialReadMTL(char* LFileName, int* LNumOfMaterialsPtr)
{
 char		LBuffer[128];
 FILE*		LInFile;
 E3dMaterial*	LMaterial;
 E3dMaterial**	LMaterials;
 E3dMaterial**	LMaterialsT;
 int		LNumOfMaterials;
 float		LRF,LGF,LBF;

 if((LInFile = fopen(LFileName, "r"))!=NULL)
 {

// Count the number of materials in the file
//
  LNumOfMaterials = 0;
  LMaterials = NULL;
  LMaterial = NULL;

  while(fscanf(LInFile, "%s", LBuffer) != EOF)
  {
   switch(LBuffer[0])
   {
    case '#':						// Comment
     fgets(LBuffer, sizeof(LBuffer), LInFile);		// Skip the rest of the line
    break;

    case 'n':						// "newmtl"
     fgets(LBuffer, sizeof(LBuffer), LInFile);

     sscanf(LBuffer, "%s %s", LBuffer, LBuffer);

     if(LMaterials==NULL) LMaterials=(E3dMaterial**)EMalloc(sizeof(E3dMaterial*));
     else
     {
      if((LMaterialsT=(E3dMaterial**)ERealloc(LMaterials,sizeof(E3dMaterial*)*(LNumOfMaterials+1)))!=NULL)
      {
       LMaterials=LMaterialsT;
      }
      else { EFree(LMaterials);return(NULL); }
     }

     if((LMaterial=E3d_MaterialAllocate())!=NULL)
     {
      
      LMaterials[LNumOfMaterials]=LMaterial;
      LMaterial->Name=strdup(LBuffer);
      LMaterial->Type=E3dMAT_LAMBERT;
      LNumOfMaterials++;
     }
    break;

    case 'K':
     switch(LBuffer[1])
     {
      case 'a':
       fscanf(LInFile, "%f %f %f", &LRF, &LGF, &LBF);
       LMaterial->Ambient[0]=LRF;
       LMaterial->Ambient[1]=LGF;
       LMaterial->Ambient[2]=LBF;
      break;

      case 'd':
       fscanf(LInFile, "%f %f %f", &LRF, &LGF, &LBF);
       LMaterial->Diffuse[0]=LRF;
       LMaterial->Diffuse[1]=LGF;
       LMaterial->Diffuse[2]=LBF;
      break;

      case 's':
       fscanf(LInFile, "%f %f %f", &LRF, &LGF, &LBF);
       LMaterial->Specular[0]=LRF;
       LMaterial->Specular[1]=LGF;
       LMaterial->Specular[2]=LBF;
       LMaterial->Type=E3dMAT_PHONG;
      break;

      default:
       fgets(LBuffer, sizeof(LBuffer), LInFile);		// Skip the rest of the line
      break;
     }
    break;

    case 'N':
     if(LMaterial!=NULL)
     {
      fscanf(LInFile, "%f", &LRF);
// Wavefront shininess is from [0, 1000], so scale for OpenGL
//
      LRF /= 1000.0;LRF*= 128.0;
      LMaterial->Specularity = LRF;
      LMaterial->Type=E3dMAT_PHONG;
     }
    break;

    default:
     fgets(LBuffer, sizeof(LBuffer), LInFile);			// Skip the rest of the line
    break;
   }
  }

  if(LNumOfMaterialsPtr!=NULL) *LNumOfMaterialsPtr=LNumOfMaterials;
  fclose(LInFile);
  return(LMaterials);
 }
 return(NULL);
}


/*==============================================*/
/* Get information about the geometry		*/
/*==============================================*/
static int E3d_OBJReadMesh(FILE* LFile, char* LFileName,E3dMesh* LMesh)
{
 char			LGroupName[256];
 char			LMaterialName[256];
 char			LBuffer[256];
 char			LPathName[MAXPATHLEN+1];
 char			LMtlFileName[MAXPATHLEN+1];

 E3dVertexNode*		LVertexNode;
 E3dPolygon*		LPolygon;

 E3dVertex*		LVertex;
 E3dVertex*		LVertices;
 E3dVertex*		LVerticesT;

 E3dVector*		LNormal;
 E3dVector*		LNormals;
 E3dVector*		LNormalsT;

 E3dST*			LST;
 E3dST*			LSTs;
 E3dST*			LSTsT;

 E3dPolyGroup*		LPolyGroup;

 E3dOBJPolyGroup*	LOBJPolyGroup;
 E3dOBJPolyGroup*	LOBJPolyGroups;

 unsigned long		LNumOfVertices, LNumOfVerticesAllocated,
			LNumOfNormals, LNumOfNormalsAllocated,
			LNumOfPolyGroups, LNumOfPolyGroupsAllocated,
			LNumOfSTs, LNumOfSTsAllocated, LC, LN;

 unsigned long		LFilePos;

 E3dMaterial*		LMaterial;
 E3dMaterial**		LMaterials;
 int			LVtxId, LNormalId, LSTId, LI, LNumOfMaterials;
 float			LFX, LFY, LFZ;


 LOBJPolyGroup=NULL;
 LGroupName[0]='\0';

 LNumOfMaterials=0;
 LMaterial=NULL;
 LMaterials=NULL;

 LNumOfVertices = LNumOfNormals = LNumOfSTs = 0;

 LNumOfPolyGroups = LNumOfPolyGroupsAllocated = 0;
 LOBJPolyGroups=NULL;

 LPathName[0]='\0';
 EStr_GetPathName(LFileName,LPathName,MAXPATHLEN);

// Allocate first chunk of vertex array
//
 if((LVertices=(E3dVertex*)EMalloc(sizeof(E3dVertex)*E3dVTXALLOC_INCREMENT))!=NULL)
 {
  LNumOfVerticesAllocated=E3dVTXALLOC_INCREMENT;
  LVertex=LVertices;
 }
 else return(0);

// Allocate first chunk of normal array
//
 if((LNormals=(E3dVector*)EMalloc(sizeof(E3dVector)*E3dVTXALLOC_INCREMENT))!=NULL)
 {
  LNumOfNormalsAllocated=E3dVTXALLOC_INCREMENT;
  LNormal=LNormals;
 }
 else { EFree(LVertices);return(0); }

// Initialize ST array
//
 LSTs = NULL;
 LNumOfSTsAllocated = 0;

 while(fscanf(LFile, "%s", LBuffer) != EOF)
 {
  switch(LBuffer[0])
  {
   case '#':	fgets(LBuffer, sizeof(LBuffer), LFile);break;		// Comment, skip the rest of the line

   case 'v':								// v, vn or vt
    switch(LBuffer[1])
    {
     case '\0':								// Vertex array
      if(LNumOfVertices>=LNumOfVerticesAllocated)
      {
       if((LVerticesT=(E3dVertex*)ERealloc(LVertices, sizeof(E3dVertex)*(LNumOfVertices+E3dVTXALLOC_INCREMENT)))!=NULL)
       {
        LVertices=LVerticesT;
        LNumOfVerticesAllocated+=E3dVTXALLOC_INCREMENT;
        LVertex=LVertices+LNumOfVertices;
       }
       else
       {
	EFree(LVertices);EFree(LNormals);
	if(LSTs!=NULL) EFree(LSTs);
	return(0);
       }
      }

      fscanf(LFile, "%f %f %f", &LFX, &LFY, &LFZ);

      LVertex->X=LFX;
      LVertex->Y=LFY;
      LVertex->Z=LFZ;
      LVertex++;
      LNumOfVertices++;
     break;

     case 'n':								// Normal array
      if(LNumOfNormals>=LNumOfNormalsAllocated)
      {
       if((LNormalsT=(E3dVector*)ERealloc(LNormals, sizeof(E3dVector)*(LNumOfNormals+E3dVTXALLOC_INCREMENT)))!=NULL)
       {
        LNormals=LNormalsT;
        LNumOfNormalsAllocated+=E3dVTXALLOC_INCREMENT;
        LNormal=LNormals+LNumOfNormals;
       }
       else
       {
	EFree(LNormals);EFree(LNormals);
	if(LSTs!=NULL) EFree(LSTs);
	return(0);
       }
      }

      fscanf(LFile, "%f %f %f", &LFX, &LFY, &LFZ);

      LNormal->X=LFX;
      LNormal->Y=LFY;
      LNormal->Z=LFZ;
      LNormal++;
      LNumOfNormals++;
     break;

     case 't':								// Texture UVs
      if(LNumOfSTs>=LNumOfSTsAllocated)
      {
       if((LSTsT=(E3dST*)ERealloc(LSTs, sizeof(E3dST)*(LNumOfSTs+E3dVTXALLOC_INCREMENT)))!=NULL)
       {
        LSTs=LSTsT;
        LNumOfSTsAllocated+=E3dVTXALLOC_INCREMENT;
        LST=LSTs+LNumOfSTs;
       }
       else
       {
	EFree(LNormals);EFree(LNormals);
	if(LSTs!=NULL) EFree(LSTs);
	return(0);
       }
      }

      fscanf(LFile, "%f %f", &LFX, &LFY);

      LST->S=LFX;
      LST->T=LFY;
      LST++;
      LNumOfSTs++;
     break;

     default:
      printf("OBJ reader: Unknown token: [%s]\n", LBuffer);
      exit(1);
     break;
    }
   break;

   case 'm':								// Material reference
    fgets(LBuffer, sizeof(LBuffer), LFile);				// Skip the rest of the line
    sscanf(LBuffer, "%s %s", LBuffer, LBuffer);

    sprintf(LMtlFileName,"%s%s",LPathName,LBuffer);
    LMaterials=E3d_MaterialReadMTL(LMtlFileName, &LNumOfMaterials);
   break;

   case 'u':
    if(EStr_StringsEqual(LBuffer,"usemtl"))
    {
     fgets(LBuffer, sizeof(LBuffer), LFile);				// Skip the rest of the line
     sscanf(LBuffer, "%s", LMaterialName);

// Find Material with that name
//
     for(LC=0;LC<LNumOfMaterials;LC++)
     {
      if(EStr_StringsEqual(LMaterials[LC]->Name,LMaterialName)) { LMaterial=LMaterials[LC];break; }
     }

     if(LOBJPolyGroup!=NULL) LOBJPolyGroup->Material=LMaterial;
    }
   break;

   case 'g':								// Group
    fgets(LBuffer, sizeof(LBuffer), LFile);				// Skip the rest of the line
    sscanf(LBuffer, "%s", LGroupName);

    if(LNumOfPolyGroups>0)
    {
     LOBJPolyGroup=LOBJPolyGroups;

     for(LC=0;LC<LNumOfPolyGroups;LC++)
     {
      if(LOBJPolyGroup->Name!=NULL)
      {
       if(EStr_StringsEqual(LOBJPolyGroup->Name,LGroupName)) break;
      }
      LOBJPolyGroup++;
     }

// PolyGroup with that name was not found
//
     if(LC>=LNumOfPolyGroups)
     {
      if((LOBJPolyGroups=E3d_OBJPolyGroupAdd(LOBJPolyGroups,LNumOfPolyGroups,LGroupName))!=NULL)
      {
       LOBJPolyGroup=LOBJPolyGroups+LNumOfPolyGroups;
       LNumOfPolyGroups++;
      }
     }
    }
    else
    {
     if((LOBJPolyGroups=E3d_OBJPolyGroupAdd(LOBJPolyGroups,LNumOfPolyGroups,LGroupName))!=NULL)
     {
      LOBJPolyGroup=LOBJPolyGroups+LNumOfPolyGroups;
      LNumOfPolyGroups++;
     }
    }

    LOBJPolyGroup->Material=LMaterial;
   break;

   case 'f':								// Polygons ("Faces")
    if(LNumOfPolyGroups==0)						// If there is no PolyGroup yet, create one
    {
     if((LOBJPolyGroups=E3d_OBJPolyGroupAdd(LOBJPolyGroups,LNumOfPolyGroups,LGroupName))!=NULL)
     {
      LOBJPolyGroup=LOBJPolyGroups+LNumOfPolyGroups;
      LOBJPolyGroup->Material=LMaterial;
      LNumOfPolyGroups++;
     }
    }

    LVtxId = LNormalId = LSTId = 0;
    fscanf(LFile, "%s", LBuffer);

// Can be %d, %d//%d, %d/%d or %d/%d/%d %d//%d
//
    if(strstr(LBuffer, "//"))						// "v//n"
    {
     LN=1;
     sscanf(LBuffer, "%d//%d", &LVtxId, &LNormalId);

     LFilePos=ftell(LFile);
     while(fscanf(LFile, "%d//%d", &LI, &LI) > 0) LN++;
     fseek(LFile,LFilePos,SEEK_SET);

     if(LN>2)
     {
      if((LOBJPolyGroup->Polygons=E3d_PolygonAddOneToArray(LOBJPolyGroup->Polygons,LOBJPolyGroup->NumOfPolygons,&(LOBJPolyGroup->NumOfPolygonsAllocated)))!=NULL)
      {
       LPolygon=LOBJPolyGroup->Polygons+LOBJPolyGroup->NumOfPolygons;
       LOBJPolyGroup->NumOfPolygons+=1;
      }

      LVertexNode=E3d_PolygonVertexNodeAllocate(LPolygon,LN);

      LVertexNode->VertexID=LVtxId-1;
// Set vertex normal
//
      if(LNormalId<=LNumOfNormals)
      {
       LNormal=LNormals+LNormalId-1;
       LVertexNode->Normal.X=LNormal->X;LVertexNode->Normal.Y=LNormal->Y;LVertexNode->Normal.Z=LNormal->Z;
      }

      LVertexNode++;

      for(LC=1;LC<LN;LC++)
      {
       fscanf(LFile, "%d//%d", &LVtxId, &LNormalId);

       LVertexNode->VertexID=LVtxId-1;
// Set vertex normal
//
       if(LNormalId<=LNumOfNormals)
       {
	LNormal=LNormals+LNormalId-1;
	LVertexNode->Normal.X=LNormal->X;LVertexNode->Normal.Y=LNormal->Y;LVertexNode->Normal.Z=LNormal->Z;
       }

       LVertexNode++;
      }

      LPolygon->NumOfVertices=LN;
     }
    }
    else
    if(sscanf(LBuffer, "%d/%d/%d", &LVtxId, &LSTId, &LNormalId) == 3)	// "v/t/n"
    {
     LN=1;

     LFilePos=ftell(LFile);
     while(fscanf(LFile, "%d/%d/%d", &LI, &LI, &LI) > 0) LN++;
     fseek(LFile, LFilePos, SEEK_SET);

     if(LN>2)
     {
      if((LOBJPolyGroup->Polygons=E3d_PolygonAddOneToArray(LOBJPolyGroup->Polygons,LOBJPolyGroup->NumOfPolygons,&(LOBJPolyGroup->NumOfPolygonsAllocated)))!=NULL)
      {
       LPolygon=LOBJPolyGroup->Polygons+LOBJPolyGroup->NumOfPolygons;
       LOBJPolyGroup->NumOfPolygons+=1;
      }

      LVertexNode=E3d_PolygonVertexNodeAllocate(LPolygon, LN);

      LVertexNode->VertexID=LVtxId-1;
// Set vertex normal
//
      if(LNormalId<=LNumOfNormals)
      {
       LNormal=LNormals+LNormalId-1;
       LVertexNode->Normal.X=LNormal->X;LVertexNode->Normal.Y=LNormal->Y;LVertexNode->Normal.Z=LNormal->Z;
      }

// Set vertex ST
//
      if(LSTId<=LNumOfSTs)
      {
       LST=LSTs+LSTId-1;
       LVertexNode->ST[0]=LST->S;LVertexNode->ST[1]=LST->T;
      }

      LVertexNode++;

      for(LC=1;LC<LN;LC++)
      {
       fscanf(LFile, "%d/%d/%d", &LVtxId, &LSTId, &LNormalId);

       LVertexNode->VertexID=LVtxId-1;
// Set vertex normal
//
       if(LNormalId<=LNumOfNormals)
       {
	LNormal=LNormals+LNormalId-1;
	LVertexNode->Normal.X=LNormal->X;LVertexNode->Normal.Y=LNormal->Y;LVertexNode->Normal.Z=LNormal->Z;
       }

// Set vertex ST
//
       if(LSTId<=LNumOfSTs)
       {
	LST=LSTs+LSTId-1;
	LVertexNode->ST[0]=LST->S;LVertexNode->ST[1]=LST->T;
       }
       LVertexNode++;
      }

      LPolygon->NumOfVertices=LN;
     }
    }
    else
    if(sscanf(LBuffer, "%d/%d/", &LVtxId, &LSTId) == 2)			// "v/t"
    {
     LN=1;

     LFilePos=ftell(LFile);
     while(fscanf(LFile, "%d/%d/", &LI, &LI) > 0) LN++;
     fseek(LFile,LFilePos,SEEK_SET);

     if(LN>2)
     {
      if((LOBJPolyGroup->Polygons=E3d_PolygonAddOneToArray(LOBJPolyGroup->Polygons,LOBJPolyGroup->NumOfPolygons,&(LOBJPolyGroup->NumOfPolygonsAllocated)))!=NULL)
      {
       LPolygon=LOBJPolyGroup->Polygons+LOBJPolyGroup->NumOfPolygons;
       LOBJPolyGroup->NumOfPolygons+=1;
      }

      LVertexNode=E3d_PolygonVertexNodeAllocate(LPolygon,LN);

      LVertexNode->VertexID=LVtxId-1;

      LVertexNode++;

      for(LC=1;LC<LN;LC++)
      {
       fscanf(LFile, "%d/%d/", &LVtxId, &LSTId);

       LVertexNode->VertexID=LVtxId-1;

       LVertexNode++;
      }

      LPolygon->NumOfVertices=LN;
     }
    }
    else								// "v"
    {
     LN=1;
     sscanf(LBuffer, "%d", &LVtxId);

     LFilePos=ftell(LFile);
     while(fscanf(LFile, "%d", &LI) > 0) LN++;
     fseek(LFile,LFilePos,SEEK_SET);

     if(LN>2)
     {
      if((LOBJPolyGroup->Polygons=E3d_PolygonAddOneToArray(LOBJPolyGroup->Polygons,LOBJPolyGroup->NumOfPolygons,&(LOBJPolyGroup->NumOfPolygonsAllocated)))!=NULL)
      {
       LPolygon=LOBJPolyGroup->Polygons+LOBJPolyGroup->NumOfPolygons;
       LOBJPolyGroup->NumOfPolygons+=1;
      }

      LVertexNode=E3d_PolygonVertexNodeAllocate(LPolygon,LN);

      LVertexNode->VertexID=LVtxId-1;
      LVertexNode++;

      for(LC=1;LC<LN;LC++)
      {
       fscanf(LFile, "%dd", &LVtxId);

       LVertexNode->VertexID=LVtxId-1;

       LVertexNode++;
      }

      LPolygon->NumOfVertices=LN;
     }
    }
   break;

   default:	fgets(LBuffer, sizeof(LBuffer), LFile);break;		// Skip the rest of the line
  }
 }


// Set the stats in the E3dMesh structure
//
 LMesh->NumOfVertices  = LNumOfVertices;

 if(LNumOfVertices>0)
 {
// Free unnecesserarily allocated Mesh vertices
//
  if(LNumOfVerticesAllocated>LNumOfVertices)
  {
   LMesh->Vertices=(E3dVertex*)ERealloc(LVertices,sizeof(E3dVertex)*LNumOfVertices);
  }
  else LMesh->Vertices=LVertices;

  LMesh->NumOfVertices=LNumOfVertices;
 }
 else EFree(LVertices);

// Convert PolyGroups
//
 if((LNumOfPolyGroups>0)&&(LOBJPolyGroups!=NULL))
 {
  LOBJPolyGroup = LOBJPolyGroups;
  for(LC = 0; LC < LNumOfPolyGroups; LC++, LOBJPolyGroup++)
  {
   if((LPolyGroup=E3d_MeshAddPolyGroup(LMesh))!=NULL)
   {
    LPolyGroup->Name=LOBJPolyGroup->Name;

// Free unnecesserarily allocated Polygons
//
    if(LOBJPolyGroup->NumOfPolygonsAllocated>LOBJPolyGroup->NumOfPolygons)
    {
     LPolyGroup->Polygons=(E3dPolygon*)ERealloc(LOBJPolyGroup->Polygons,sizeof(E3dPolygon)*LOBJPolyGroup->NumOfPolygons);
    }
    else LPolyGroup->Polygons=LOBJPolyGroup->Polygons;

    LPolyGroup->Material=LOBJPolyGroup->Material;
    if(LOBJPolyGroup->Material!=NULL) LOBJPolyGroup->Material->RefCnt+=1;
    LPolyGroup->NumOfPolygons=LOBJPolyGroup->NumOfPolygons;

// Compute polygon normals for this PolyGroup
//
    E3d_PolyGroupRefreshPolygonNormals(LMesh,LPolyGroup);

    if(LNumOfNormals>0) LPolyGroup->NormalFlags|=E3dUSE_VERTEX_NORMALS;
   }
  }

  EFree(LOBJPolyGroups);
 }


 if(LMaterials!=NULL) EFree(LMaterials);
 EFree(LNormals);
 if(LSTs!=NULL) EFree(LSTs);

 return(LNumOfPolyGroups);
}


/*======================================*/
/* Read an OBJ file			*/
/*======================================*/
E3dModel* E3d_ReadOBJFile(char* LFileName)
{
 E3dModel*	LModel;
 E3dMesh*	LMesh;
 FILE* 		LInFile;

 if((LInFile=fopen(LFileName,"r"))!=NULL)
 {
  LModel=NULL;
  if((LMesh=E3d_MeshAllocate())!=NULL)
  {
   E3d_OBJReadMesh(LInFile, LFileName, LMesh);

   if(LMesh->NumOfPolyGroups>0)
   {
    E3d_MeshRefreshGLPolyVertices(LMesh, TRUE);

    if((LModel=E3d_ModelAllocate(LFileName))!=NULL)
    {
     LModel->Geometry=(E3dGeometry*)LMesh;

     fclose(LInFile);return(LModel);
    }

   }
  }
  fclose(LInFile);
 }
 return(NULL);
}
