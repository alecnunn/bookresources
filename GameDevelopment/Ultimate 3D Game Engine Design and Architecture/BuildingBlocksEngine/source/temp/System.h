/*

*/


#ifndef _BB_SYSTEM_H_
#define _BB_SYSTEM_H_


#include<Singleton.h>
#include<fstream>
#include<stdarg.h>
#include<NameSpace.h>
#include<Defines.h>


DECLARE_ENGINE_NAMESPACE

class CSystem : public CSingleton<CSystem>
{
   // OS Input
   public:
      bool isButtonDown(BB_INPUT_BUTTON btn);
      void GetMousePosition(int *mx, int *my);

   // Timer
   public:
      void SetTimerBeginPeriod(int ms);
      void SetTimerEndPeriod(int ms);
      float GetTimeMs();
      float GetTimeSeconds();
};

#define System CSystem::GetSingleton()

END_ENGINE_NAMESPACE

#endif