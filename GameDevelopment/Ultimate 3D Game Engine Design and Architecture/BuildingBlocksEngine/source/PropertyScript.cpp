/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<PropertyScript.h>
#include<TokenStream.h>
#include<File.h>


DECLARE_ENGINE_NAMESPACE


bool PropertyScript::Load(char *fileName)
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
   memset(buffer, 0, fileSize);

   if(buffer == NULL)
      return false;

   TokenStream tokenStream;
   Property prop;
   bool result = true;

   fileInput.Read(buffer, fileSize);
   tokenStream.SetTokenStream(buffer);

   delete[] buffer;

   while(result == true)
      {
         result = tokenStream.GetNextToken(&prop.m_propertyName);

         if(result == true)
            result = tokenStream.GetNextToken(&prop.m_value);

         m_list.push_back(prop);
      }

   return true;
}


bool PropertyScript::GetPropertyValue(std::string property,
                                      std::string *val)
{
   if(val == NULL)
      return false;

   for(int i = 0; i < (int)m_list.size(); i++)
      {
         if(m_list[i].m_propertyName == property)
            {
               *val = m_list[i].m_value;
               return true;
            }
      }

   return false;
}


END_ENGINE_NAMESPACE