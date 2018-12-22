/*
   Archive Files Demo - Chapter 2
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<string>
#include<Archive.h>

using namespace std;


#define TOTAL_FILES  4
#define ARCHIVE_NAME "archive.ac1"


void WriteTest();
void ReadTest();
void ExtractTest(char *location);


int main(int args, char **argc)
{
   WriteTest();
   cout << endl;

   ReadTest();
   cout << endl;

   ExtractTest("extracted");
   cout << endl;

   return 0;
}


void WriteTest()
{
   bbe::ArchiveFileHeader headers[TOTAL_FILES];
   bbe::Archive archiveFile;
   
   headers[0].SetFileName("TestFiles/test1.txt");
   headers[1].SetFileName("TestFiles/test2.txt");
   headers[2].SetFileName("TestFiles/test3.txt");
   headers[3].SetFileName("TestFiles/test4.txt");

   if(archiveFile.WriteArchiveFile(ARCHIVE_NAME,
      headers, TOTAL_FILES))
      {
         cout << "Archive " << ARCHIVE_NAME << " created!\n";
      }
   else
      {
         cout << "Archive " << ARCHIVE_NAME << " NOT created!\n";
      }

   archiveFile.CloseArchive();
}


void ReadTest()
{
   bbe::Archive archiveFile;
   bbe::ArchiveFileHeader fileHeader;

   if(archiveFile.ReadArchiveFile(ARCHIVE_NAME) != true)
      {
         cout << "Archive " << ARCHIVE_NAME << " NOT loaded!\n\n";
         return;
      }

   cout << "ID: BBE1\n";
   cout << "Major Version: " << ARCHIVE_MAJOR << ".\n";
   cout << "Minor Version: " << ARCHIVE_MINOR << ".\n";
   cout << "Total files: " << archiveFile.GetTotalHeaders() << ".\n";
   cout << "\n";

   for(int i = 0; i < archiveFile.GetTotalHeaders(); i++)
      {
         archiveFile.GetFileHeaderInfoByIndex(i, &fileHeader);
         cout << "File Name: " << fileHeader.GetFileName() << ".\n";
         cout << "File Size: " << fileHeader.GetSize() << ".\n";
         cout << "\n";
      }

   archiveFile.CloseArchive();
}


void ExtractTest(char *location)
{
   bbe::Archive archiveFile;

   if(archiveFile.ReadArchiveFile(ARCHIVE_NAME) != true)
      {
         cout << "Archive " << ARCHIVE_NAME << " NOT loaded!\n\n";
         return;
      }

   if(archiveFile.Extract(0, location) == true)
      cout << "File at index 0 extracted!\n";
   else
      cout << "File at index 0 not extracted!\n";

   if(archiveFile.Extract(1, location) == true)
      cout << "File at index 1 extracted!\n";
   else
      cout << "File at index 1 not extracted!\n";

   if(archiveFile.Extract(2, location) == true)
      cout << "File at index 2 extracted!\n";
   else
      cout << "File at index 2 not extracted!\n";

   if(archiveFile.Extract("TestFiles/test4.txt", location) == true)
      cout << "test4.txt extracted!\n";
   else
      cout << "test4.txt not extracted!\n";

   archiveFile.CloseArchive();
}