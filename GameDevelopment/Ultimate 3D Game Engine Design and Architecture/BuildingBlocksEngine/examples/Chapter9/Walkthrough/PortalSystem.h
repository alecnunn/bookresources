/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _PORTAL_SYSTEM_H_
#define _PORTAL_SYSTEM_H_


#include<ModelData.h>
#include<TokenStream.h>
#include<File.h>
#include<BoundingBox.h>
#include<ResourceManager.h>
#include<RenderInterface.h>
#include<vector>


class Sector;

struct stModel { };
typedef bbe::Handle<stModel> BB_MODEL;


struct stSectorVertex
{
   float x, y, z;
   float tu, tv;
};


class SectorMesh
{
   public:
      SectorMesh() { }

      bbe::ModelData m_geometry;
      bbe::BB_TEXTURE m_decal;
      std::string m_decalFile;
};


class Portal
{
   public:
      Portal()
      {
         m_connectedArea = -1;
         m_size = 0;
      }

      ~Portal()
      {

      }

      void operator=(const Portal &p)
      {
         m_connectedArea = p.GetConnectedArea();
         m_center = p.GetCenter();
         m_size = p.GetSize();
         m_name = p.GetName();
      }

      void SetConnectedArea(int area)
      {
         m_connectedArea = area;
      }

      int GetConnectedArea() const
      {
         return m_connectedArea;
      }

      void SetCenter(bbe::Vector3D &pos)
      {
         m_center = pos;
      }

      bbe::Vector3D GetCenter() const
      {
         return m_center;
      }

      void SetSize(float size)
      {
         m_size = size;
      }

      float GetSize() const
      {
         return m_size;
      }

      void SetName(std::string &name)
      {
         m_name = name;
      }

      std::string GetName() const
      {
         return m_name;
      }


   private:
      int m_connectedArea;

      bbe::Vector3D m_center;
      float m_size;

      std::string m_name;
};


class Sector
{
   public:
      Sector() { }
      ~Sector() { }

      bool LoadSectorFromFile(char *modelFile);

      std::string GetDecalFile()
      {
         return m_mesh.m_decalFile;
      }

      void SetDecal(bbe::BB_TEXTURE decal)
      {
         m_mesh.m_decal.Initialize(decal.GetIndex());
      }

      void SetMesh(const SectorMesh &mesh)
      {
         m_mesh = mesh;
      }

      SectorMesh *GetMesh()
      {
         return &m_mesh;
      }

      void SetPortal(const Portal &p)
      {
         m_portals.push_back(p);
      }

      std::vector<Portal> *GetPortals()
      {
         return &m_portals;
      }

      bbe::BoundingBox GetBoundingBox()
      {
         return m_aabb;
      }


   private:
      SectorMesh m_mesh;
      std::vector<Portal> m_portals;

      bbe::BoundingBox m_aabb;
};


#endif