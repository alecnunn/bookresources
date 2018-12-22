/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_HASH_H_
#define _BB_HASH_H_

#include<vector>
#include<string>
#include<NameSpace.h>


DECLARE_ENGINE_NAMESPACE


template <typename A>
class HashItem
{
   public:
      HashItem() : m_key(0) {}
      ~HashItem() {}
      
      int GetKey()
      {
         return m_key;
      }

      void SetKey(int k)
      {
         m_key = k;
      }

      A GetObject()
      {
         return m_obj;
      }

      void SetObj(A obj)
      {
         m_obj = obj;
      }

      bool operator==(HashItem &item)
      {
         if(m_key == item.GetKey())
            return true;
         
         return false;
      }

      void operator=(HashItem item)
      {
         m_key = item.GetKey();
         m_obj = item.GetObject();
      }

   private:
      int m_key;
      A m_obj;
};


template <typename A>
class HashTable
{
   public:
      HashTable(int size) : m_size(0)
      {
         if(size > 0)
            {
               m_size = size;
               m_table = new std::vector<HashItem<A> >[m_size];
            }
      }

      ~HashTable()
      {
         m_size = 0;

         if(m_table != NULL)
            {
               delete[] m_table;
               m_table = NULL;
            }
      }

      void Insert(HashItem<A> &obj)
      {
         int hash = HashFunction(obj.GetKey());
         m_table[hash].push_back(obj);
      }
      
      void Delete(int key)
      {
         HashItem<A> item;
         item.SetKey(key);

         int hash = HashFunction(key);

         std::vector<HashItem<A> > *ptr = &m_table[hash];
         typename std::vector<HashItem<A> >::iterator it;

         for(it = ptr->begin(); it != ptr->end(); it++)
            {
               if(*it == item)
                  {
                     ptr->erase(it);
                     break;
                  }
            }
      }

      HashItem<A> Find(int key)
      {
         HashItem<A> item, temp;
         item.SetKey(key);

         int hash = HashFunction(key);
         int i = 0;

         std::vector<HashItem<A> > *ptr = &m_table[hash];

         for(i = 0; i < (int)ptr->size(); i++)
            {
               temp = (*ptr)[i];

               if(temp == item)
                  return temp;
            }

         item.SetKey(-1);
         return item;
      }

      int HashFunction(int key)
      {
         return key % m_size;
      }
      
      int HashFunction(std::string &str)
      {
         int hash = 0;
         int i = 0;
         
         for(i = 0; i < (int)str.size(); i++)
            {
               int val = (int)str[i];
               hash = (hash * 256 + val) % m_size;
            }

         return hash;
      }

      int GetSize()
      {
         return m_size;
      }

   private:
      std::vector<HashItem<A> > *m_table;
      int m_size;
};


END_ENGINE_NAMESPACE

#endif