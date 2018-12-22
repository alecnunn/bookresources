/*
   Command Script Demo - Chapter 8
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<string>
#include<TokenStream.h>
#include<File.h>

using namespace std;


void Print(string &str)
{
   if(str != "\"\"")
      cout << str;
}


void PrintLine(string &str)
{
   Print(str);
   cout << endl;
}


void DisplayDemoInfo()
{
   cout << "Command Scripting Demo!" << endl;
   cout << "Ultimate Game Engine Design & Architecture" << endl;
   cout << "Chapter 8" << endl << endl;
}


int main(int args, char **argc)
{
   bbe::TokenStream tokenStream;
   bbe::FileInputStream fileInput;
   string command, value;
   int fileSize = 0;
   char *buffer = NULL;

   if(fileInput.OpenFile("commands.txt", BB_TEXT_FILE) == false)
      {
         cout << "Error reading commands.txt!" << endl << endl;
         return 0;
      }

   fileSize = fileInput.GetFileSize();

   if(fileSize <= 0)
      {
         cout << "Error reading commands.txt!" << endl << endl;
         return 0;
      }

   buffer = new char[fileSize];
   memset(buffer, 0, fileSize);
   

   fileInput.Read(buffer, fileSize);
   tokenStream.SetTokenStream(buffer);

   delete[] buffer;

   while(tokenStream.GetNextToken(&command))
      {
         if(command == "Print")
            {
               tokenStream.GetNextToken(&value);
               Print(value);
            }
         else if(command == "PrintLine")
            {
               tokenStream.GetNextToken(&value);
               PrintLine(value);
            }
         else if(command == "DisplayDemoInfo")
            {
               DisplayDemoInfo();
            }
      }

   cout << endl << endl;

   return 1;
}