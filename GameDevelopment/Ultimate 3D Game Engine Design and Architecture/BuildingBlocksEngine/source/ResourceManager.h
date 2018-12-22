/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_RESOURCE_MANAGER_H_
#define _BB_RESOURCE_MANAGER_H_


#include<NameSpace.h>
#include<Defines.h>
#include<assert.h>
#include<HashTable.h>
#include<vector>
#include<string>


DECLARE_ENGINE_NAMESPACE


template<typename TAG>
class Handle
{
   public:
      Handle() { m_index = BB_NULL_HANDLE; }

      void Initialize(int index) { m_index = index; }
      void Nullify() { m_index = BB_NULL_HANDLE; }
      int GetIndex() { return m_index; }
      bool isNull() { return (m_index <= BB_NULL_HANDLE); }

      bool operator==(Handle<TAG> handle)
      {
         return m_index == handle.GetIndex();
      }

   private:
      int m_index;
};


class ResourceName
{
   public:
      ResourceName()
      {
         m_index = -1;
      }
      
      std::string GetName() { return m_name; }
      int GetIndex() { return m_index; }

      void SetName(std::string name) { m_name = name; }
      void SetIndex(int index) { m_index = index; }

      bool operator==(ResourceName resName)
      {
         return (m_name == resName.GetName() &&
                 m_index == resName.GetIndex());
      }

   private:
      std::string m_name;
      int m_index;
};


template<typename A, typename HANDLE>
class ResourceManager
{
   public:
      ResourceManager(int hashSize = 0)
      {
         m_resourceHash = NULL;

         if(hashSize > 0)
            m_resourceHash = new HashTable<ResourceName>(hashSize);
      }

      ~ResourceManager()
      {
         DeleteAllMemory();
      }

      bool Create(A *res, char *name, HANDLE *handle);
      void Release(HANDLE handle);

      int GetRegisteredIndex(char *name);

   private:
      void SetRegisteredName(char *name, int index);

      void DeleteAllMemory()
      {
         for(int i = 0; i < GetSize(); i++)
            {
               if(m_objects[i])
                  delete m_objects[i];

               m_objects[i] = NULL;
            }

         if(m_resourceHash != NULL)
            delete m_resourceHash;

         m_resourceHash = NULL;
      }

   public:
      A *GetResource(HANDLE handle)
      {
         int index = handle.GetIndex();

         if(index >= GetSize() || index <= BB_NULL_HANDLE)
            return NULL;

         return m_objects[handle.GetIndex()];
      }

      int GetSize()
      {
         return (int)m_objects.size();
      }

      std::vector<A*> *GetObjects()
      {
         return &m_objects;
      }

   private:
      std::vector<A*> m_objects;
      HashTable<ResourceName> *m_resourceHash;
};


template<typename A, typename HANDLE>
bool ResourceManager<A, HANDLE>::Create(A *res, char *name, HANDLE *handle)
{
   int index = -1;

   if(res == NULL)
      return false;

   if(handle)
      handle->Nullify();

   index = GetRegisteredIndex(name);

   if(index == -1)
      {
         m_objects.push_back(res);

         index = (int)m_objects.size() - 1;

         if(index < 0)
            return false;

         SetRegisteredName(name, index);
      }

   if(handle)
      handle->Initialize(index);

   return true;
}


template<typename A, typename HANDLE>
void ResourceManager<A, HANDLE>::Release(HANDLE handle)
{
   int index = handle.GetIndex();

   if(handle.isNull())
      return;

   handle.Nullify();
}


template<typename A, typename HANDLE>
int ResourceManager<A, HANDLE>::GetRegisteredIndex(char *name)
{
   HashItem<ResourceName> tempHashItem;
   ResourceName tempRes;
   int key = -1;

   if(name == NULL)
      return -1;

   std::string str(name);

   if(m_resourceHash != NULL)
      {
         key = m_resourceHash->HashFunction(str);
         tempHashItem = m_resourceHash->Find(key);

         if(tempHashItem.GetKey() != -1 &&
            tempHashItem.GetObject().GetName() == str)
            {
               return tempHashItem.GetObject().GetIndex();
            }
      }

   return -1;
}


template<typename A, typename HANDLE>
void ResourceManager<A, HANDLE>::SetRegisteredName(char *name, int index)
{
   if(name == NULL || m_resourceHash == NULL || index < 0)
      return;

   HashItem<ResourceName> hashItem;
   std::string str(name);

   ResourceName resName;
   resName.SetName(name);
   resName.SetIndex(index);

   hashItem.SetKey(m_resourceHash->HashFunction(str));
   hashItem.SetObj(resName);

   m_resourceHash->Insert(hashItem);
}


END_ENGINE_NAMESPACE

#endif