/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"PortalSystem.h"


bool Sector::LoadSectorFromFile(char *modelFile)
{
   bbe::FileInputStream fileInput;

   if(fileInput.OpenFile(modelFile, BB_TEXT_FILE) == false)
      return false;

   int fileSize = fileInput.GetFileSize();
   if(fileSize <= 0) return false;

   bbe::TokenStream tokenStream;
   char *buffer = new char[fileSize];

   if(buffer == NULL)
      return false;

   fileInput.Read(buffer, fileSize);
   tokenStream.SetTokenStream(buffer);

   delete[] buffer;


   std::string token;

   // Get texture name.  Loop allows empty lines before filename.
   do
   {
      tokenStream.GetNextToken(&m_mesh.m_decalFile);
   }while(m_mesh.m_decalFile == "");

   // Get total faces.
   do
   {
      tokenStream.GetNextToken(&token);
   }while(token == "");

   int totalFaces = atoi(token.c_str());

   // Allocate based on total vertices.
   stSectorVertex *verts = new stSectorVertex[totalFaces * 3];

   if(verts == NULL)
      return false;

   bbe::Vector3D min, max;

   // Read each vertex position and texture coordinate.
   for(int i = 0; i < totalFaces * 3; i++)
      {
         // Allow empty lines between vertices.
         do
         {
            tokenStream.GetNextToken(&token);
         }while(token == "");

         verts[i].x = (float)atof(token.c_str());

         tokenStream.GetNextToken(&token);
         verts[i].y = (float)atof(token.c_str());

         tokenStream.GetNextToken(&token);
         verts[i].z = (float)atof(token.c_str());

         tokenStream.GetNextToken(&token);
         verts[i].tu = (float)atof(token.c_str());

         tokenStream.GetNextToken(&token);
         verts[i].tv = (float)atof(token.c_str());


         // Update bounding box.
         if(verts[i].x < m_aabb.m_min.x)
            m_aabb.m_min.x = verts[i].x;
         if(verts[i].x > m_aabb.m_max.x)
            m_aabb.m_max.x = verts[i].x;

         if(verts[i].y < m_aabb.m_min.y)
            m_aabb.m_min.y = verts[i].y;
         if(verts[i].y > m_aabb.m_max.y)
            m_aabb.m_max.y = verts[i].y;

         if(verts[i].z < m_aabb.m_min.z)
            m_aabb.m_min.z = verts[i].z;
         if(verts[i].z > m_aabb.m_max.z)
            m_aabb.m_max.z = verts[i].z;
      }

   m_aabb.m_center = (m_aabb.m_min + m_aabb.m_max) * 0.5f;


   // Get total portals.
   do
   {
      tokenStream.GetNextToken(&token);
   }while(token == "");

   int totalPortals = atoi(token.c_str());
   bbe::Vector3D tempCenter;
   Portal tempPortal;

   // Read portals.
   for(int i = 0; i < totalPortals; i++)
      {
         // Get portal name.
         do
         {
            tokenStream.GetNextToken(&token);
         }while(token == "");

         tempPortal.SetName(token);

         tokenStream.GetNextToken(&token);
         tempCenter.x = (float)atof(token.c_str());

         tokenStream.GetNextToken(&token);
         tempCenter.y = (float)atof(token.c_str());

         tokenStream.GetNextToken(&token);
         tempCenter.z = (float)atof(token.c_str());

         tempPortal.SetCenter(tempCenter);

         tokenStream.GetNextToken(&token);
         tempPortal.SetSize((float)atof(token.c_str()));

         m_portals.push_back(tempPortal);
      }


   // Set information.
   m_mesh.m_geometry.Clear();

   m_mesh.m_geometry.AddDescriptorElement(BB_ELEMENT_TYPE_VERTEX_3F);
   m_mesh.m_geometry.AddDescriptorElement(BB_ELEMENT_TYPE_TEX1_2F);

   if(m_mesh.m_geometry.SetVertices(BB_PRIMITIVE_TRI_LIST,
      totalFaces * 3, sizeof(stSectorVertex),
      (char*)verts) == false)
      {
         return false;
      }

   delete[] verts;

   return true;
}