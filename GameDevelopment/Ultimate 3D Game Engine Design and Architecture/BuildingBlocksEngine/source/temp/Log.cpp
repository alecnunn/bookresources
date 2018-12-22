/*

*/


#include<string.h>
#include<Log.h>


DECLARE_ENGINE_NAMESPACE


bool Log::OpenLog(const char *filename)
{
   return (m_log.OpenFile(filename, BB_TEXT_FILE));
}


void Log::CloseLog()
{
   if(m_log.is_open() == true)
      m_log.close();
}


bool Log::Write(const char *message)
{
   if(!m_log.is_open())
      return false;

   m_log << message;

#ifdef _DEBUG
   m_log.flush();
#endif

   return true;
}


END_ENGINE_NAMESPACE