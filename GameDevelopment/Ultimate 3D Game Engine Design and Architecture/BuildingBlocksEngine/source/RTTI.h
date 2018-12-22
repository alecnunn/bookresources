/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_RTTI_H_
#define _BB_RTTI_H_

#include<string.h>
#include"NameSpace.h"


DECLARE_ENGINE_NAMESPACE


class RTTI
{
   public:
      RTTI() : m_name(0), m_parent(0) {}
      RTTI(char *name, RTTI *parent) { SetName(name); SetParent(parent); }
      ~RTTI() { m_parent = 0; if(m_name) delete[] m_name; m_name = 0; }

      void SetName(char *name)
      {
         if(name == 0) return;

         int len = strlen(name);

         if(m_name) delete m_name;
         m_name = new char[len + 1];

         memcpy(m_name, name, len);
         m_name[len] = '\0';
      }

      void SetParent(RTTI *parent) { m_parent = parent; }
      const char *GetName() { return m_name; }
      RTTI *GetParent() { return m_parent; }

      bool IsOfType(RTTI *type)
      {
         RTTI *tempType = this;

         while(tempType)
            {
               if(tempType == type) return true;
               else tempType = tempType->GetParent();

            }

         return false;
      }


   private:
      char *m_name;
      RTTI *m_parent;
};


inline bool CanCast(RTTI *objType, RTTI *castType)
{
   if(objType && castType)
	   if(objType->IsOfType(castType)) return true;
	return false;
}


END_ENGINE_NAMESPACE


#define BB_RTTI_EXPOSE_TYPE public: static RTTI m_RTTIType
#define BB_RTTI_DEFINE_SUPER(SuperClass) public: typedef SuperClass m_super
#define BB_RTTI_SAFE_CAST(obj, cast) (obj ? (bbe::CanCast(&obj->m_RTTIType, &cast::m_RTTIType) ? (cast*)obj : 0) : 0)

#endif