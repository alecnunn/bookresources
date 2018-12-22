/*
   Property Script Demo - Chapter 8
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<string>
#include<PropertyScript.h>

using namespace std;


int main(int args, char **argc)
{
   bbe::PropertyScript script;
   string val;
   int age = 0;

   if(script.Load("properties.txt") == false)
      {
         cout << "Error loading properties.txt!\n\n";
         return 0;
      }

   if(script.GetPropertyValue("Name", &val) == true)
      {
         cout << "Name = " << val << "." << endl;
      }
   else
      {
         cout << "The property Name was not found!" << endl;
      }

   if(script.GetPropertyValue("Age", &val) == true)
      {
         age = atoi(val.c_str());
         cout << "Age = " << age << "." << endl;
      }
   else
      {
         cout << "The property Age was not found!" << endl;
      }

   if(script.GetPropertyValue("Date", &val) == true)
      {
         cout << "Date = " << val << "." << endl;
      }
   else
      {
         cout << "The property Date was not found!" << endl;
      }

   cout << endl << endl;

   return 1;
}