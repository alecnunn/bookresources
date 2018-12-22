/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_GAME_MONKEY_H_
#define _BB_GAME_MONKEY_H_


#include<string>
#include<gmThread.h>
#include<gmCall.h>
#include<NameSpace.h>


DECLARE_ENGINE_NAMESPACE

typedef int BB_GM_SCRIPT;


class GameMonkeyFunction
{
   public:
      GameMonkeyFunction(){}
      virtual ~GameMonkeyFunction(){}

      void AddIntParam(int param)
      {
         m_scriptCall.AddParamInt(param);
      }

      void AddFloatParam(float param)
      {
         m_scriptCall.AddParamFloat(param);
      }

      void AddStringParam(std::string param)
      {
         m_scriptCall.AddParamString(param.c_str());
      }

      void ExecuteCall()
      {
         m_scriptCall.End();
      }

      bool GetReturnValueAsInt(int &retVal)
      {
         if(m_scriptCall.GetReturnedInt(retVal))
            return true;

         return false;
      }

      float GetReturnValueAsFloat(float &retVal)
      {
         if(m_scriptCall.GetReturnedFloat(retVal))
            return true;

         return false;
      }

      bool GetReturnValueAsString(const char *retVal)
      {
         if(m_scriptCall.GetReturnedString(retVal))
            return true;

         return false;
      }

      gmCall *GetCallPtr()
      {
         return &m_scriptCall;
      }

   private:
      gmCall m_scriptCall;
};


class GameMonkey
{
   public:
      GameMonkey()
      {
      }

      virtual ~GameMonkey()
      {
      }

      bool LoadScript(std::string &script, BB_GM_SCRIPT *handle);

      void ExecuteScript(BB_GM_SCRIPT handle);

      bool GetScriptFunction(std::string name,
                             GameMonkeyFunction &call);

      void RegisterHostFunction(std::string &name, gmCFunction func);

      const char *GetError()
      {
         return m_errorBuffer.c_str();
      }

   private:
      gmMachine m_gvm;

      std::string m_errorBuffer;
};


END_ENGINE_NAMESPACE


#endif