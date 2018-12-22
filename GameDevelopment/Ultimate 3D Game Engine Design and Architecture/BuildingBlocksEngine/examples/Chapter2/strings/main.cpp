/*
   Strings Demo - Chapter 2
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<string>

using namespace std;


int main(int args, char **argc)
{
   string str("Hello World");

   cout << "String contents: " << str << endl;

   str.clear();
   cout << "New string contents: " << str << endl;

   str = "Goodbye World";
   cout << "New string contents: " << str << endl;

   cout << endl;

   cout << "Enter in a string and press enter: ";
   getline(cin, str);

   cout << endl;

   cout << "You've entered: " << str << endl;

   cout << endl;

   return 1;
}