/*
   Archive Creator
   Created by Allen Sherrod
   Ultimate Game Engine Design and Architecture
*/


#include<iostream>
#include<Archive.h>

using namespace std;


int main(int args, char **arg)
{
   cout << "Archive Creator." << endl;
   cout << "Created by Allen Sherrod." << endl;
   cout << "Ultimate Game Engine Design and Architecture." << endl << endl;

   if(args < 3)
      {
         cout << "Not enough arguments defined." << endl;
         return 0;
      }

   // Arg 1 is total files, 2 is archive file,
   // 3+ are all file names to go into archive.

   if(arg[1] == NULL)
      {
         cout << "No total files defined." << endl;
         return 0;
      }

   int totalFiles = atoi(arg[1]);

   for(int i = 2; i < totalFiles; i++)
      {
         if(arg[i] == NULL)
            {
               cout << "Argument " << i << " is NULL!" << endl;
               return 0;
            }
      }

   bbe::ArchiveFileHeader *headers = new bbe::ArchiveFileHeader[totalFiles];
   bbe::Archive archiveFile;

   for(int i = 0; i < totalFiles; i++)
      {
         headers[i].SetFileName(arg[i + 3]);
      }


   if(archiveFile.WriteArchiveFile(arg[2], headers, totalFiles))
      {
         cout << "Archive " << "NAME" << " created!\n";
      }
   else
      {
         cout << "Archive " << "NAME" << " NOT created!\n";
      }

   archiveFile.CloseArchive();

   delete[] headers;

   return 1;
}