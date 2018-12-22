/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<string.h>
#include<TokenStream.h>


DECLARE_ENGINE_NAMESPACE


bool isValidIdentifier(char c)
{
   // Ascii from ! to ~.
   if((int)c > 32 && (int)c < 127)
      return true;
      
   return false;
}


TokenStream::TokenStream()
{
   ResetStream();
}


TokenStream::~TokenStream()
{

}


void TokenStream::ResetStream()
{
   m_startIndex = m_endIndex = 0;
}


void TokenStream::SetTokenStream(char *data)
{
   ResetStream();
   m_data = data;
}


bool TokenStream::GetNextToken(std::string *buffer)
{
   bool inString = false;
   m_startIndex = m_endIndex;
   int length = m_data.length();

   if(m_startIndex >= length)
      return false;

   while(m_startIndex < length &&
         isValidIdentifier(m_data[m_startIndex]) == false)
      {
         m_startIndex++;
      }

   m_endIndex = m_startIndex + 1;

   if(m_data[m_startIndex] == '"')
      inString = !inString;

   if(m_startIndex < length)
      {
         while(m_endIndex < length &&
               (isValidIdentifier(m_data[m_endIndex]) ||
               inString == true))
            {
               if(m_data[m_endIndex] == '"')
                  inString = !inString;

               m_endIndex++;
            }

         if(buffer != NULL)
            {
               int size = (m_endIndex - m_startIndex);
               int index = m_startIndex;

               buffer->reserve(size + 1);
               buffer->clear();

               for(int i = 0; i < size; i++)
                  {
                     buffer->push_back(m_data[index++]);
                  }
            }

         return true;
      }

   return false;
}


bool TokenStream::GetNextToken(std::string *token, std::string *buffer)
{
   std::string tok;

   if(token == NULL)
      return false;

   while(GetNextToken(&tok))
      {
         if(strcmp(tok.c_str(), token->c_str()) == 0)
            return GetNextToken(buffer);
      }

	return false;
}


bool TokenStream::MoveToNextLine(std::string *buffer)
{
   int length = m_data.length();

   if(m_startIndex < length && m_endIndex < length)
      {
         m_endIndex = m_startIndex;

         while(m_endIndex < length &&
               (isValidIdentifier(m_data[m_endIndex]) ||
               m_data[m_endIndex] == ' '))
            {
               m_endIndex++;
            }

         if((m_endIndex - m_startIndex) == 0)
            return false;

         if(m_endIndex - m_startIndex >= length)
            return false;

         if(buffer != NULL)
            {
               int size = (m_endIndex - m_startIndex);
               int index = m_startIndex;

               buffer->reserve(size + 1);
               buffer->clear();

               for(int i = 0; i < size; i++)
                  {
                     buffer->push_back(m_data[index++]);
                  }
            }
      }
   else
      {
         return false;
      }

   m_endIndex++;
   m_startIndex = m_endIndex + 1;

   return true;
}


END_ENGINE_NAMESPACE