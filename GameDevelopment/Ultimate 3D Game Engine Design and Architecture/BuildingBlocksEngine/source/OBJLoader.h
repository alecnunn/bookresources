/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_


#include<TokenStream.h>
#include<File.h>
#include<ModelData.h>


struct stOBJMeshVertex
{
   float x, y, z;
   float tu1, tv1;
   float nx, ny, nz;
};

struct stObjModel
{
   float *vertices;
   float *normals;
   float *texCoords;
   int numFaces;
};


DECLARE_ENGINE_NAMESPACE


stObjModel *LoadOBJ(char *fileName);


inline bool CreateOBJMesh(char *fileName, bbe::ModelData *model)
{
   if(model == NULL)
      return false;

   stObjModel *loadedModel = LoadOBJ(fileName);

   if(loadedModel == NULL)
      return false;

   int totalVertices = loadedModel->numFaces * 3;
   stOBJMeshVertex *objMesh = new stOBJMeshVertex[totalVertices];

   if(objMesh == NULL)
      return false;

   int i = 0, v = 0, n = 0, t = 0;

   for(i = 0; i < totalVertices; i++)
      {
         objMesh[i].x = loadedModel->vertices[v++];
         objMesh[i].y = loadedModel->vertices[v++];
         objMesh[i].z = loadedModel->vertices[v++];

         objMesh[i].nx = loadedModel->normals[n++];
         objMesh[i].ny = loadedModel->normals[n++];
         objMesh[i].nz = loadedModel->normals[n++];

         objMesh[i].tu1 = loadedModel->texCoords[t++];
         objMesh[i].tv1 = loadedModel->texCoords[t++];
      }

   model->Clear();

   model->AddDescriptorElement(BB_ELEMENT_TYPE_VERTEX_3F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_TEX1_2F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_NORMAL_3F);

   if(model->SetVertices(BB_PRIMITIVE_TRI_LIST, totalVertices,
      sizeof(stOBJMeshVertex), (char*)objMesh) == false)
      {
         return false;
      }

   model->SetIndices(0, NULL);

   delete[] loadedModel;
   delete[] objMesh;

   return true;
}


inline stObjModel *LoadOBJ(char *fileName)
{
   FileInputStream fileInput;
   int fileSize = 0;

   if(fileInput.OpenFile(fileName, BB_TEXT_FILE) == false)
      return false;

   fileSize = fileInput.GetFileSize();

   if(fileSize <= 0)
      return false;

   char *buffer = NULL;

   buffer = new char[fileSize];

   if(buffer == NULL)
      return false;

   TokenStream tokenStream, tempStream;
   std::string tempLine, token;
   bool validFile = false;

   fileInput.Read(buffer, fileSize);
   tokenStream.SetTokenStream(buffer);

   delete[] buffer;

   // Look for the word Wavefront.
   while(tokenStream.GetNextToken(&token))
      {
         if(strcmp(token.c_str(), "Wavefront") == 0)
            {
               validFile = true;
               break;
            }
      }

   if(validFile == false)
      return NULL;

   tokenStream.ResetStream();

   int totalVertices = 0, totalNormals = 0,
       totalTexC = 0, totalFaces = 0;

   while(tokenStream.MoveToNextLine(&tempLine))
      {
         tempStream.SetTokenStream((char*)tempLine.c_str());
         tokenStream.GetNextToken(NULL);

         if(!tempStream.GetNextToken(&token))
            continue;

         if(strcmp(token.c_str(), "v") == 0) totalVertices++;
         else if(strcmp(token.c_str(), "vn") == 0) totalNormals++;
         else if(strcmp(token.c_str(), "vt") == 0) totalTexC++;
         else if(strcmp(token.c_str(), "f") == 0) totalFaces++;

         token[0] = '\0';
      }

   float *verts = new float[totalVertices * 3];
   float *norms = new float[totalNormals * 3];
   float *texC = new float[totalTexC * 2];
   int *faces = new int[totalFaces * 9];
   int vIndex = 0, nIndex = 0, tIndex = 0, fIndex = 0, index = 0;

   tokenStream.ResetStream();

   while(tokenStream.MoveToNextLine(&tempLine))
      {
         tempStream.SetTokenStream((char*)tempLine.c_str());
         tokenStream.GetNextToken(NULL);

         if(!tempStream.GetNextToken(&token))
            continue;

         if(strcmp(token.c_str(), "v") == 0)
            {
               tempStream.GetNextToken(&token);
               verts[vIndex] = (float)atof(token.c_str());
               vIndex++;

               tempStream.GetNextToken(&token);
               verts[vIndex] = (float)atof(token.c_str());
               vIndex++;

               tempStream.GetNextToken(&token);
               verts[vIndex] = (float)atof(token.c_str());
               vIndex++;
            }
         else if(strcmp(token.c_str(), "vn") == 0)
            {
               tempStream.GetNextToken(&token);
               norms[nIndex] = (float)atof(token.c_str());
               nIndex++;

               tempStream.GetNextToken(&token);
               norms[nIndex] = (float)atof(token.c_str());
               nIndex++;

               tempStream.GetNextToken(&token);
               norms[nIndex] = (float)atof(token.c_str());
               nIndex++;
            }
         else if(strcmp(token.c_str(), "vt") == 0)
            {
               tempStream.GetNextToken(&token);
               texC[tIndex] = (float)atof(token.c_str());
               tIndex++;

               tempStream.GetNextToken(&token);
               texC[tIndex] = (float)atof(token.c_str());
               tIndex++;
            }
         else if(strcmp(token.c_str(), "f") == 0)
            {
               for(int i = 0; i < 3; i++)
                  {
                     tempStream.GetNextToken(&token);
                     int len = strlen(token.c_str());

                     for(int s = 0; s < len + 1; s++)
                        {
                           char buff[64];

                           if(token[s] != '/' && s < len)
                              {
                                 buff[index] = token[s];
                                 index++;
                              }
                           else
                              {
                                 buff[index] = '\0';
                                 faces[fIndex] = (int)atoi(buff);
                                 fIndex++;
                                 index = 0;
                              }
                        }
                  }
            }

         token[0] = '\0';
      }

   stObjModel *model = new stObjModel;

   if(!model)
      return NULL;

   memset(model, 0, sizeof(stObjModel));

   model->numFaces = totalFaces;

   vIndex = 0, nIndex = 0, tIndex = 0, fIndex = 0, index = 0;

   model->vertices = new float[totalFaces * 3 * 3];
   if(totalNormals) model->normals = new float[totalFaces * 3 * 3];
   if(totalTexC) model->texCoords = new float[totalFaces * 3 * 2];

   for(int f = 0; f < totalFaces * 9; f+=3)
      {
         model->vertices[vIndex + 0] = verts[(faces[f + 0] - 1) * 3 + 0];
         model->vertices[vIndex + 1] = verts[(faces[f + 0] - 1) * 3 + 1];
         model->vertices[vIndex + 2] = verts[(faces[f + 0] - 1) * 3 + 2];
         vIndex += 3;

         if(model->texCoords)
            {
               model->texCoords[tIndex + 0] = texC[(faces[f + 1] - 1) * 2 + 0];
               model->texCoords[tIndex + 1] = texC[(faces[f + 1] - 1) * 2 + 1];
               tIndex += 2;
            }

         if(model->normals)
            {
               model->normals[nIndex + 0] = norms[(faces[f + 2] - 1) * 3 + 0];
               model->normals[nIndex + 1] = norms[(faces[f + 2] - 1) * 3 + 1];
               model->normals[nIndex + 2] = norms[(faces[f + 2] - 1) * 3 + 2];
               nIndex += 3;
            }
      }

   delete[] verts;
   delete[] norms;
   delete[] texC;
   delete[] faces;

   return model;
}


END_ENGINE_NAMESPACE


#endif