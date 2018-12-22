/*
   Game Monkey Script Demo - Chapter 8
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod

   COMPILATION NOTE: Set the include directories to where
                     you've setup the Game Monkey Script SDK.
*/


#include<iostream>
#include<fstream>
#include<string>
#include<GameMonkey.h>

using namespace std;


int GM_CDECL gmHostMultiply(gmThread *a_thread);


int main(int args, char argc[])
{
   bbe::GameMonkey gvm;
   bbe::GameMonkeyFunction gmAddTest, gmDisplay;
   bbe::BB_GM_SCRIPT thread = -1;
   std::string hostName = "HostMultiply";

   gvm.RegisterHostFunction(hostName, gmHostMultiply);

   ifstream fStream("TestScript.gm");

   if(!fStream)
      {
         cout << "ERROR LOADING SCRIPT!" << endl;
         return 0;
      }

   string str = string(istreambuf_iterator<char>(fStream),
                       istreambuf_iterator<char>());
   fStream.close();


   if(gvm.LoadScript(str, &thread) == false)
      {
         cout << "Error:\n\n" <<
                 gvm.GetError();

         return 0;
      }

   gvm.ExecuteScript(thread);

   cout << endl;

   if(gvm.GetScriptFunction("AddTest", gmAddTest))
      {
         int retVal = 0;
         gmAddTest.AddIntParam(50);
         gmAddTest.AddIntParam(22);
         gmAddTest.ExecuteCall();
         
         if(gmAddTest.GetReturnValueAsInt(retVal))
            {
               cout << "Script function Add() returned: " <<
                       retVal << endl;
            }
         else
            {
               cout << "Add() return NOT INTEGER!" << endl;
            }
      }
   else
      {
         cout << "Script function Add() NOT FOUND!" << endl;
      }

   cout << endl;
   
   
   if(gvm.GetScriptFunction("Display", gmDisplay))
      {
         gmDisplay.ExecuteCall();
      }
   else
      {
         cout << "Script function Display() NOT FOUND!" << endl;
      }

   cout << endl;

	return 0;
}


int GM_CDECL gmHostMultiply(gmThread *a_thread)
{
   GM_CHECK_NUM_PARAMS(2);
   GM_CHECK_INT_PARAM(lVal, 0);
   GM_CHECK_INT_PARAM(rVal, 1);

   int retVal = lVal * rVal;

   a_thread->PushInt(retVal);

   return GM_OK;
}