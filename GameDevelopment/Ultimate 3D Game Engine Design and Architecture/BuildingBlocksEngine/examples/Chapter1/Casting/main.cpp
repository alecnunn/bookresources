/*
   C++ Casting Demo - Chapter 1
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>


using namespace std;


void Display(char *str)
{
   cout << str << endl;
}


int main(int args, char argc[])
{
   int intVar = 10;
   float floatVar = 5.85f;
   const char *constStr = "Some message";

   cout << "intVar = " << intVar << endl;
   cout << "floatVar = " << floatVar << endl;


   // static_cast example.
   cout << "floatVar cast to int = " << static_cast<int>(floatVar) << endl;


   // const_cast example.
   Display(const_cast<char*>(constStr));


   // reinterpret_cast example.
   int *intPtr = &intVar;
   float *floatPtr = reinterpret_cast<float*>(intPtr);


   // dynamic_cast example.
   class Base { };
   class Derived: public Base { };

   Base baseObj;
   Base* basePtr = 0;

   Derived derived;
   Derived* derivedPtr = 0;

   // If valid.
   basePtr = dynamic_cast<Base*>(&derived);

   // Is not valid.
   //derivedPtr = dynamic_cast<Derived*>(&baseObj);

   return 1;
}