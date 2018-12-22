/*
   Token Stream Demo - Chapter 8
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<TokenStream.h>
#include<File.h>

using namespace std;


int main(int args, char **argc)
{
   bbe::TokenStream tokenStream;
   bbe::FileInputStream fileInput;
   string token;
   int fileSize = 0;
   char *buffer = NULL;

   if(fileInput.OpenFile("tokens.txt", BB_TEXT_FILE) == false)
      {
         cout << "Error loading tokens.txt!\n\n";
         return 0;
      }

   fileSize = fileInput.GetFileSize();

   if(fileSize <= 0)
      {
         cout << "Error with file size!\n\n";
         return 0;
      }

   buffer = new char[fileSize];
   memset(buffer, 0, fileSize);
   fileInput.Read(buffer, fileSize);

   tokenStream.SetTokenStream(buffer);

   delete[] buffer;

   cout << "Token Stream Contents: ";

   while(tokenStream.GetNextToken(&token) == true)
      {
         cout << token << " ";
      }

   cout << endl << endl;

   return 1;
}