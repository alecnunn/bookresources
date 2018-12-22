/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_LOG_H_
#define _BB_LOG_H_

#include<NameSpace.h>
#include<File.h>


DECLARE_ENGINE_NAMESPACE

class Log
{
   public:
      Log() { }
      ~Log() { CloseLog(); }

      bool OpenLog(const char *filename)
      {
         return (m_log.OpenFile(filename, BB_TEXT_FILE));
      }

      void CloseLog()
      {
         m_log.CloseFile();
      }

      bool Write(char *message)
      {
         return (m_log.Write(message, strlen(message)));
      }

   protected:
      FileOutputStream m_log;
};

END_ENGINE_NAMESPACE

#endif