/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_ARCHIVE_H_
#define _BB_ARCHIVE_H_


#include<File.h>
#include<NameSpace.h>
#include<Defines.h>


DECLARE_ENGINE_NAMESPACE

#define ARCHIVE_ID      "BBE1"
#define ARCHIVE_MAJOR   1
#define ARCHIVE_MINOR   0


class ArchiveHeader
{
   public:
      ArchiveHeader()
      {
         m_id[0] = '\0';
         m_majorVersion = 0;
         m_minorVersion = 0;
         m_totalFiles = 0;
      }


      char *GetID() { return m_id; }
      int GetMajorVersion() { return m_majorVersion; }
      int GetMinorVersion() { return m_minorVersion; }
      int GetTotalFiles() { return m_totalFiles; }


      void SetID(char *id)
      {
         if(id == NULL)
            return;

         memcpy(m_id, id, 4);
         m_id[4] = '\0';
      }

      void SetMajorVersion(int major) { m_majorVersion = major; }
      void SetMinorVersion(int minor) { m_minorVersion = minor; }
      void SetTotalFiles(int total) { m_totalFiles = total; }


   private:
	   char m_id[5];
	   int m_majorVersion;
	   int m_minorVersion;
	   int m_totalFiles;
};


class ArchiveFileHeader
{
   public:
      ArchiveFileHeader()
      {
         m_fileName[0] = '\0';
         m_size = 0;
         m_offset = 0;
      }


      char *GetFileName() { return m_fileName; }
      int GetSize() { return m_size; }
      int GetOffset() { return m_offset; }


      void SetFileName(char *fileName)
      {
         int len;
         m_fileName[0] = '\0';

         if(fileName == NULL)
            return;

         len = strlen(fileName);

         if(len > 255)
            return;

         memcpy(m_fileName, fileName, len);
         m_fileName[len] = '\0';
      }

      void SetSize(int size) { m_size = size; }
      void SetOffset(int offset) { m_offset = offset; }


      void operator=(ArchiveFileHeader &header)
      {
         m_size = header.GetSize();
         SetFileName(header.GetFileName());
         m_offset = header.GetOffset();
      }


   private:
	   char m_fileName[256];
	   int m_size;
	   int m_offset;
};


class Archive
{
   public:
      Archive();
      ~Archive();

      bool ReadArchiveFile(char *fileName);
      bool WriteArchiveFile(char *fileName,
                            ArchiveFileHeader *headers,
                            int totalHeaders);
      void CloseArchive();

      bool Extract(int index, char *location);      
      bool Extract(char *fileName, char *location);

      int GetFileIndex(char *fileName);
      bool GetFileData(int index, char *buffer, int bytesToRead);
      bool GetFileData(char *fileName, char *buffer, int bytesToRead);
      bool GetFileHeaderInfoByIndex(int index, ArchiveFileHeader *fh);

      int GetTotalHeaders() { return m_totalHeaders; }

      bool isArchiveOpen()
      {
         return (m_fileStream.isOpen() == true);
      }


   private:
      ArchiveFileHeader *m_headers;
      int m_totalHeaders;

      FileInputStream m_fileStream;
};

END_ENGINE_NAMESPACE

#endif