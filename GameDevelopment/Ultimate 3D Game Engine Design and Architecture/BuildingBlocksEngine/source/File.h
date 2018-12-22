/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_FILE_H_
#define _BB_FILE_H_


#include<fstream>
#include<stdarg.h>
#include<NameSpace.h>
#include<Defines.h>


DECLARE_ENGINE_NAMESPACE


class FileStream
{
   public:
      FileStream() : m_fileSize(0) {}
      virtual ~FileStream() {}

      virtual bool OpenFile(const char *fileName,
                            BB_FILE_TYPE fileType) = 0;
      virtual void CloseFile() = 0;

      virtual void SeekStreamToStart() = 0;
      virtual void SeekStreamToEnd() = 0;
      virtual void SeekPast(int offset) = 0;
      virtual int GetSeekPosition() = 0;

      virtual bool Read(char *buffer, int bytesToRead) = 0;
      virtual bool Write(char *buffer, int bytesToWrite) = 0;

      int GetFileSize() { return m_fileSize; }
      virtual bool isOpen() = 0;

   protected:
      int m_fileSize;
};


class FileInputStream : public FileStream
{
   public:
      FileInputStream() {}

      FileInputStream(const char *fileName, BB_FILE_TYPE fileType)
      {
         OpenFile(fileName, fileType);
      }

      ~FileInputStream()
      {
         CloseFile();
      }

      bool OpenFile(const char *fileName, BB_FILE_TYPE fileType);
      void CloseFile();

      void SeekStreamToStart();
      void SeekStreamToEnd();
      void SeekPast(int offset);
      int GetSeekPosition();

      bool Read(char *buffer, int bytesToRead);
      bool Write(char *buffer, int bytesToWrite);

      bool isOpen();

   private:
      std::ifstream m_fileStream;
};


class FileOutputStream : public FileStream
{
   public:
      FileOutputStream() {}

      FileOutputStream(const char *fileName, BB_FILE_TYPE fileType)
      {
         OpenFile(fileName, fileType);
      }

      ~FileOutputStream()
      {
         CloseFile();
      }

      bool OpenFile(const char *fileName, BB_FILE_TYPE fileType);
      void CloseFile();

      void SeekStreamToStart();
      void SeekStreamToEnd();
      void SeekPast(int offset);
      int GetSeekPosition();

      bool Read(char *buffer, int bytesToRead);
      bool Write(char *buffer, int bytesToWrite);

      bool isOpen();

   private:
      std::ofstream m_fileStream;
};

END_ENGINE_NAMESPACE

#endif