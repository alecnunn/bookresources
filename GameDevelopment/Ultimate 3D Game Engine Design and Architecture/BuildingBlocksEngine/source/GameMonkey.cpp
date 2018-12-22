/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<GameMonkey.h>


DECLARE_ENGINE_NAMESPACE


bool GameMonkey::LoadScript(std::string &script, BB_GM_SCRIPT *handle)
{
   int result = 0;
   
   result = m_gvm.ExecuteString(script.c_str(), handle, false);

   if(result != 0)
      {
         gmLog &errorLog = m_gvm.GetLog();
         bool flag = true;
         const char *errorString = NULL;

         errorString = errorLog.GetEntry(flag);

         while(errorString)
            {
               m_errorBuffer += "Error: ";
               m_errorBuffer += errorString;
               m_errorBuffer += "\n";
               errorString = errorLog.GetEntry(flag);
            }

         return false;
      }

   return true;
}


void GameMonkey::ExecuteScript(BB_GM_SCRIPT handle)
{
   m_gvm.Execute(handle);
}


bool GameMonkey::GetScriptFunction(std::string name, GameMonkeyFunction &call)
{
   gmCall *func = call.GetCallPtr();

   if((*func).BeginGlobalFunction(&m_gvm, name.c_str()))
      return true;

   return false;
}


void GameMonkey::RegisterHostFunction(std::string &name, gmCFunction func)
{
   gmFunctionEntry hostFunc;
   hostFunc.m_name = name.c_str();
   hostFunc.m_function = func;

   m_gvm.RegisterLibrary(&hostFunc, 1);
}


END_ENGINE_NAMESPACE