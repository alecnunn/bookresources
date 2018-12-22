/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_TOKEN_STREAM_H_
#define _BB_TOKEN_STREAM_H_

#include<NameSpace.h>
#include<string>


DECLARE_ENGINE_NAMESPACE


class TokenStream
{
   public:
      TokenStream();
      ~TokenStream();

      void ResetStream();

      void SetTokenStream(char *data);
      bool GetNextToken(std::string *buffer);
      bool GetNextToken(std::string *token, std::string *buffer);

      bool MoveToNextLine(std::string *buffer);

   private:
      int m_startIndex, m_endIndex;
      std::string m_data;
};


END_ENGINE_NAMESPACE

#endif