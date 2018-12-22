/*
   Endianness Demo - Chapter 2
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<File.h>
#include<Endian.h>


bool Write(char *file, BB_ENDIAN_TYPE endianType)
{
   bbe::FileOutputStream fileStream;
   bbe::Endian endian;
   int intVar1 = 436;
   int intVar2 = 6849;

   if(fileStream.OpenFile(file, BB_BINARY_FILE) == false)
      return false;

   std::cout << "The values written: " << intVar1 <<
                " and " << intVar2 << ".\n";

   endian.ResolveEndian((char*)&intVar1, sizeof(intVar1), endianType);
   endian.ResolveEndian((char*)&intVar2, sizeof(intVar2), endianType);

   std::cout << "After Endian Swap: " << intVar1 <<
                " and " << intVar2 << ".\n";

   fileStream.Write((char*)&intVar1, sizeof(intVar1));
   fileStream.Write((char*)&intVar2, sizeof(intVar2));

   fileStream.CloseFile();

   return true;
}


bool Read(char *file, BB_ENDIAN_TYPE endianType)
{
   bbe::FileInputStream fileStream;
   bbe::Endian endian;
   int intVar1 = 0;
   int intVar2 = 0;

   if(fileStream.OpenFile(file, BB_BINARY_FILE) == false)
      return false;   

   fileStream.Read((char*)&intVar1, sizeof(intVar1));
   fileStream.Read((char*)&intVar2, sizeof(intVar2));

   endian.ResolveEndian((char*)&intVar1, sizeof(intVar1), endianType);
   endian.ResolveEndian((char*)&intVar2, sizeof(intVar2), endianType);

   std::cout << "The values read: " << intVar1 <<
                " and " << intVar2 << ".\n";

   fileStream.CloseFile();

   return true;
}


int main(int args, char **argc)
{
   std::cout << "Files and Endianess Test...\n\n";


   std::cout << "READING THE FILE...\n";

   if(Read("file.txt", BB_ENDIAN_BIG) == false)
      std::cout << "FILE READ FAIL!\n";

   std::cout << "\n";


   std::cout << "WRITING THE FILE...\n";

   if(Write("file.txt", BB_ENDIAN_BIG) == false)
      std::cout << "FILE WRITE FAIL!\n";

   std::cout << "\n";


   std::cout << "READING THE FILE...\n";

   if(Read("file.txt", BB_ENDIAN_BIG) == false)
      std::cout << "FILE READ FAIL!\n";

   std::cout << "\n";

   return 0;
}