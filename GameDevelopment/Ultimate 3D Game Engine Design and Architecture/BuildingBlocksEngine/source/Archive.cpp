/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<string.h>
#include<Archive.h>
#include<File.h>
#include<Endian.h>


DECLARE_ENGINE_NAMESPACE


Archive::Archive()
{
   m_headers = NULL;
   m_totalHeaders = 0;
}


Archive::~Archive()
{
   CloseArchive();
}


void Archive::CloseArchive()
{
   if(m_headers != NULL)
      delete[] m_headers;

   m_headers = NULL;

   m_fileStream.CloseFile();
}


bool Archive::ReadArchiveFile(char *fileName)
{
   if(fileName == NULL)
      return false;

   if(m_fileStream.OpenFile(fileName, BB_BINARY_FILE) == false)
      return false;

   ArchiveHeader archiveHeader;

   m_fileStream.Read((char*)&archiveHeader,
                     sizeof(ArchiveHeader));

   if(strcmp(archiveHeader.GetID(), ARCHIVE_ID) != 0)
      return false;

   Endian endianSwap;
   int temp = 0, temp2 = 0, offset = 0, size = 0;

   temp = archiveHeader.GetMajorVersion();
   endianSwap.ResolveEndian((char*)&temp, 4, BB_ENDIAN_LITTLE);

   temp2 = archiveHeader.GetMinorVersion();
   endianSwap.ResolveEndian((char*)&temp, 4, BB_ENDIAN_LITTLE);

   if(temp != ARCHIVE_MAJOR && temp2 != ARCHIVE_MINOR)
      return false;

   m_totalHeaders = archiveHeader.GetTotalFiles();
   endianSwap.ResolveEndian((char*)&m_totalHeaders, 4,
                            BB_ENDIAN_LITTLE);

   if(m_totalHeaders <= 0)
      return false;

   m_headers = new ArchiveFileHeader[m_totalHeaders];

   if(m_headers == NULL)
      return false;

   for(int i = 0; i < m_totalHeaders; i++)
      {
         m_fileStream.Read((char*)&m_headers[i],
                           sizeof(ArchiveFileHeader));

         offset = m_headers[i].GetOffset();
         endianSwap.ResolveEndian((char*)&offset, 4,
                                  BB_ENDIAN_LITTLE);
         
         size = m_headers[i].GetSize();
         endianSwap.ResolveEndian((char*)&size, 4,
                                  BB_ENDIAN_LITTLE);
         
         m_headers[i].SetOffset(offset);
         m_headers[i].SetSize(size);
         
         m_fileStream.SeekPast(size);
      }

   return true;
}


bool Archive::WriteArchiveFile(char *fileName, ArchiveFileHeader *headers, int totalHeaders)
{
   FileOutputStream fileOut;

   if(totalHeaders <= 0 || fileName == NULL)
      return false;

   if(fileOut.OpenFile(fileName, BB_BINARY_FILE) == false)
      return false;

   ArchiveHeader archiveHeader;
   Endian endianSwap;
   int temp = 0, currentOffset = 0;

   archiveHeader.SetID(ARCHIVE_ID);

   temp = ARCHIVE_MAJOR;
   endianSwap.ResolveEndian((char*)&temp, 4, BB_ENDIAN_LITTLE);
   archiveHeader.SetMajorVersion(temp);

   temp = ARCHIVE_MINOR;
   endianSwap.ResolveEndian((char*)&temp, 4, BB_ENDIAN_LITTLE);
   archiveHeader.SetMinorVersion(temp);

   temp = totalHeaders;
   endianSwap.ResolveEndian((char*)&temp, 4, BB_ENDIAN_LITTLE);
   archiveHeader.SetTotalFiles(temp);

   fileOut.Write((char*)&archiveHeader, sizeof(ArchiveHeader));
   currentOffset += sizeof(ArchiveHeader);

   char *fileData = NULL;
   int fileSize = 0;


   for(int i = 0; i < totalHeaders; i++)
      {
         FileInputStream fileIn;

         fileIn.OpenFile(headers[i].GetFileName(), BB_BINARY_FILE);

         fileSize = fileIn.GetFileSize();

         if(fileSize > 0)
            {
               fileData = new char[fileSize];
               fileIn.Read(fileData, fileSize);
               fileIn.CloseFile();
            }

         if(fileData == NULL)
            fileSize = 0;

         if(fileSize != 0)
            fileSize--;

         temp = fileSize;
         endianSwap.ResolveEndian((char*)&temp, 4, BB_ENDIAN_LITTLE);
         headers[i].SetSize(temp);

         currentOffset += sizeof(ArchiveFileHeader);

         temp = currentOffset;
         endianSwap.ResolveEndian((char*)&temp, 4, BB_ENDIAN_LITTLE);
         headers[i].SetOffset(temp);

         fileOut.Write((char*)&headers[i], sizeof(ArchiveFileHeader));

         if((fileSize > 0) && (fileData != NULL))
            {
               fileOut.Write(fileData, fileSize);
               currentOffset += fileSize;
            }
         
         if(fileData != NULL)
            {
               delete[] fileData;
               fileData = NULL;
            }
      }

   fileOut.CloseFile();

   return true;
}


bool Archive::Extract(int index, char *location)
{
   if(isArchiveOpen() == false || index < 0 ||
      index >= m_totalHeaders || location == NULL)
      {
         return false;
      }

   std::string str;
   char *buffer = NULL;
   int size = 0;

   size = m_headers[index].GetSize();

   if(size > 0)
      {
         buffer = new char[size];

         if(buffer == NULL)
            return false;
      }
   else
      {
         return false;
      }

   GetFileData(index, buffer, size);

   str = location;
   str += "/";
   str += m_headers[index].GetFileName();

   FileOutputStream fileOutput;

   fileOutput.OpenFile(str.c_str(), BB_TEXT_FILE);
   fileOutput.Write(buffer, size);
   fileOutput.CloseFile();

   delete[] buffer;

   return true;
}


bool Archive::Extract(char *fileName, char *location)
{
   int index = GetFileIndex(fileName);

   return (Extract(index, location));
}


int Archive::GetFileIndex(char *fileName)
{
   for(int i = 0; i < m_totalHeaders; i++)
      {
         if(strcmp(fileName, m_headers[i].GetFileName()) == 0)
            return i;
      }

   return -1;
}


bool Archive::GetFileData(int index, char *buffer, int bytesToRead)
{
   if(index < 0 || index >= m_totalHeaders)
      return false;

   m_fileStream.SeekPast(m_headers[index].GetOffset() -
                           m_fileStream.GetSeekPosition());

   return m_fileStream.Read(buffer, bytesToRead);
}


bool Archive::GetFileData(char *fileName, char *buffer, int bytesToRead)
{
   int index = GetFileIndex(fileName);

   return (GetFileData(index, buffer, bytesToRead));
}


bool Archive::GetFileHeaderInfoByIndex(int index, ArchiveFileHeader *header)
{
   if(index < 0 || index >= m_totalHeaders || header == NULL)
      return false;

   *header = m_headers[index];

   return true;
}


END_ENGINE_NAMESPACE