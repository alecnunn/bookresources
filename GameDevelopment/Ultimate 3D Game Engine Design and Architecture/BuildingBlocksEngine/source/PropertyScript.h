/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_PROPERTY_SCRIPT_H_
#define _BB_PROPERTY_SCRIPT_H_

#include<NameSpace.h>
#include<string>
#include<vector>


DECLARE_ENGINE_NAMESPACE


struct Property
{
   std::string m_propertyName;
   std::string m_value;
};


class PropertyScript
{
   public:
      PropertyScript() {}
      virtual ~PropertyScript() {}

      bool Load(char *fileName);
      bool GetPropertyValue(std::string property,
                            std::string *val);

   private:
      std::vector<Property> m_list;
};


END_ENGINE_NAMESPACE

#endif