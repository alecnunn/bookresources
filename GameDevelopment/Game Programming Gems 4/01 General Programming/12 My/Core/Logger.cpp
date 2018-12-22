//--------------------------------------------------------------------------------------------------------------------//
//                                                LOGGER CLASS & MACROS                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "Logger.h"
#ifdef    _DEBUG
#include  "Logger.inl"
#include  "LogHelper.inl"
#endif

#include  <time.h>
#include  "MyAssert.h"

#include  "DirC.h"
#include  "FileC.h"
#include  "FindFileC.h"

//----------------------------------------------- DEFINES ------------------------------------------------------------//

#define   _NUMLINE_                               "4"       // chrs for line number
#define   _NUMMSG_                                "4"       // chrs for msg  number
#define   _INDENTATION_                           "  "
#define   _FILECHRS_                              "30"      // chrs for file name
#define   _NBFILECHRS_                             30

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//


u32       CLogger::m_u32GlobalNextError           = 1;
u32       CLogger::m_u32GlobalNextWarning         = 1;
u32       CLogger::m_u32GlobalNextMsg             = 1;


//--------------------------------------------------------------------------------------------------------------------//
//                                                SETUP/CLEANUP                                                       //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- DeleteFiles --------------------------------------------------------//
// delete all .LOG files in given directory (which becomes the log dir)
// in : log directory
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::DeleteFiles(const char* pszDir)
  {
  if(pszDir) 
    m_strDir = pszDir;
  else
    m_strDir = "LOGS";
  char szFullPath[_MAX_PATH];
  Dir Dir;
  Dir.FullPath(szFullPath,m_strDir.c_str(),_MAX_PATH);
  m_strDir = szFullPath;                                    // save full path

  CStdString strFileName = m_strDir+"\\*.LOG";
  FindFile Finder;
  if(Finder.FindFirst(strFileName.c_str(),true))
    {
    do
      {                                                     // remove all .LOG files
      bool boRes = Dir.RemoveOneFile(Finder.GetFilePath().c_str());
      MY_ASSERT(boRes,"error removing file");
      }
    while(Finder.FindNext());
    Finder.FindClose();
    }

  Dir.MakeDir(m_strDir.c_str());                            // fails if the dir already exists, not a pb
  }

//----------------------------------------------- ResetCounters ------------------------------------------------------//
// reinit all counters
// in :
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::ResetCounters()
  {
  m_u32NextError = m_u32NextWarning = m_u32NextMsg = 1;
  m_s32Priority  = 1;
  m_u32Indent    = 0;
  }

//----------------------------------------------- Reset --------------------------------------------------------------//
// (re)init logger
// in :
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::Reset(const char* pszDir)
  {
  DeleteFiles(pszDir);
  ResetCounters();

  m_u32MsgMask     = _MSG_ALL_;
  m_u32OutputMask  = _OUTPUT_ALL_;
  m_u32IndentMin   = 0;
  m_u32IndentMax   = u32(-1);
  m_strRootPath    = "";
  m_u32RootLen     = m_strRootPath.size();
  m_strLastWarning = "";
  m_strLastError   = "";
  }

//----------------------------------------------- GetResults ---------------------------------------------------------//
// return some stats
// in :
// out: stats string
//--------------------------------------------------------------------------------------------------------------------//

CStdString CLogger::GetResults() const
  {
  if(m_u32NextError+m_u32NextWarning == 2) return "";
  CStdString strInfo;
  strInfo.Format("%u error(s), %u warning(s)\n",m_u32NextError-1,m_u32NextWarning-1);
  return strInfo;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                TRACE                                                               //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- SysTrace -----------------------------------------------------------//
// put the string in [pszFile].LOG
// in : file name,string 
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::SysTrace(const char* pszFile,const char* pszString)
  {
  if(m_s32Priority <= 0) return;
  if(!IsOutputEnabled(_OUTPUT_FILE_)) return;

  char szDrive[_MAX_DRIVE];
  char szDir  [_MAX_DIR];
  char szName [_MAX_FNAME];
  char szExt  [_MAX_EXT];
  char szOpen [_MAX_PATH];

  _splitpath(pszFile,szDrive,szDir,szName,szExt);
  strncpy(szDir,m_strDir.c_str(),_MAX_DIR-1);
  _makepath(szOpen,"",m_strDir.c_str(),szName,".LOG");

  File File;
  if(File.Open(szOpen,IFile::APPEND_TEXT))
    { 
    // 24/11/01: time

    time_t  t = time(NULL);
    struct  tm *local = localtime(&t);
    char    szTime[256];
    sprintf(szTime,"<%02d/%02d/%02d %02d:%02d:%02d> ",local->tm_mday,local->tm_mon+1,local->tm_year%100,
                                                      local->tm_hour,local->tm_min,  local->tm_sec);
    File.PutString(szTime);

    //

    File.PutString(pszString);
    File.Close();
    }
  }

//----------------------------------------------- Trace --------------------------------------------------------------//
// put the string in TRACE.LOG & [pszFile].LOG
// in : file name,line number,string
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::Trace(const char* pszFile,const u32 u32Line,const char* pszString)
  {
  if(m_s32Priority <= 0) return;

  CStdString strBuf1,strBuf2,strBuf3;
  strBuf1.reserve(256); strBuf2.reserve(256); strBuf3.reserve(256);
  strBuf1  = pszString;
  strBuf1 +="\n";
  strBuf2.Format("[%" _NUMLINE_ "d] %" _NUMMSG_ "d: ",u32Line,m_u32GlobalNextMsg);
                                                            // message number
  for(ui uiI = 0; uiI < m_u32Indent; uiI++) strBuf2 += _INDENTATION_;
  if(u32Line) strBuf2 += strBuf1;
  else        strBuf2  = strBuf1;
  SysTrace(pszFile,strBuf2.c_str());                        // put message in [pszFile].LOG

  char   szFile[_MAX_PATH];
  strcpy(szFile,pszFile);
  strupr(szFile);
  char*  pcPos = strstr(szFile,m_strRootPath.c_str());
  strBuf2.Format("[%." _FILECHRS_ "s :",pcPos ? pcPos+m_u32RootLen : pszFile);
  u32 u32Len = strBuf2.size();
  while(u32Len < _NBFILECHRS_+3)
    {
    strBuf2 += " ";
    u32Len++;
    }

  strBuf3.Format("%s %" _NUMLINE_ "d] %" _NUMMSG_ "d: ",strBuf2.c_str(),u32Line,m_u32GlobalNextMsg++);
  m_u32NextMsg++;
  for(uiI = 0; uiI < m_u32Indent; uiI++) strBuf3 += _INDENTATION_;
  if(u32Line) strBuf3 += strBuf1;
  else        strBuf3  = strBuf1;
  SysTrace("TRACE.LOG",strBuf3.c_str());                    // put message in TRACE.LOG

  strBuf3.Delete(strBuf3.size()-1);                         // remove "\n"
  SendCallback(strBuf3.c_str());
  }

//----------------------------------------------- Log ----------------------------------------------------------------//
// put the string in [pszFile].LOG
// in : file name,line number,string
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::Log(const char* pszFile,const u32 u32Line,const char* pszString)
  {
  if(m_s32Priority <= 0) return;

  CStdString strBuf1,strBuf2;
  strBuf1.reserve(256); strBuf2.reserve(256);
  strBuf1  = pszString;
  strBuf1 += "\n";
  strBuf2.Format("[%" _NUMLINE_ "d] %" _NUMMSG_ "d: ",u32Line,m_u32GlobalNextMsg++);
  m_u32NextMsg++;
  for(ui uiI = 0; uiI < m_u32Indent; uiI++) strBuf2 += _INDENTATION_;
  strBuf2 += strBuf1;
  SysTrace(pszFile,strBuf2);                                // put message in [pszFile].LOG
  }

//----------------------------------------------- Warning ------------------------------------------------------------//
// put the string WARNING.LOG & [pszFile].LOG & TRACE.LOG
// in : file name,line number,string
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::Warning(const char* pszFile,const u32 u32Line,const char* pszString)
  {
  CStdString strBuf1,strBuf2,strBuf3;
  strBuf1.reserve(256); strBuf2.reserve(256); strBuf3.reserve(256);
  strBuf1 = m_strLastWarning = pszString;

  if(m_s32Priority <= 0) return;

  Trace(pszFile,u32Line,strBuf1.c_str());                   // put message in TRACE.LOG & [pszFile].LOG
  m_u32GlobalNextMsg--;
  m_u32NextMsg--;

  char   szFile[_MAX_PATH];
  strcpy(szFile,pszFile);
  strupr(szFile);
  char*  pcPos = strstr(szFile,m_strRootPath.c_str());
  strBuf2.Format("[%." _FILECHRS_ "s :",pcPos ? pcPos+m_u32RootLen : pszFile);
  DWORD u32Len = strBuf2.size();
  while(u32Len < _NBFILECHRS_+3)
    {
    strBuf2 += " ";
    u32Len++;
    }

  strBuf3.Format("%s %" _NUMLINE_ "d] %" _NUMMSG_ "d: ",strBuf2.c_str(),u32Line,m_u32GlobalNextMsg++);
  m_u32NextMsg++;
  strBuf3 += strBuf1+"\n";
  SysTrace("WARNINGS.LOG",strBuf3.c_str());                 // put message in WARNINGS.LOG
  m_u32GlobalNextWarning++;
  m_u32NextWarning++;
  }

//----------------------------------------------- Error --------------------------------------------------------------//
// put the string ERRORS.LOG & [pszFile].LOG & TRACE.LOG
// in : file name,line number,string
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::Error(const char* pszFile,const u32 u32Line,const char* pszString)
  {
  CStdString strBuf1,strBuf2,strBuf3;
  strBuf1.reserve(256); strBuf2.reserve(256); strBuf3.reserve(256);
  strBuf1 = m_strLastError = pszString;

  if(m_s32Priority <= 0) return;

  Trace(pszFile,u32Line,strBuf1.c_str());                   // put message in TRACE.LOG & [pszFile].LOG
  m_u32GlobalNextMsg--;
  m_u32NextMsg--;

  char   szFile[_MAX_PATH];
  strcpy(szFile,pszFile);
  strupr(szFile);
  char*  pcPos = strstr(szFile,m_strRootPath.c_str());
  strBuf2.Format("[%." _FILECHRS_ "s :",pcPos ? pcPos+m_u32RootLen : pszFile);
  DWORD u32Len = strBuf2.size();
  while(u32Len < _NBFILECHRS_+3)
    {
    strBuf2 += " ";
    u32Len++;
    }

  strBuf3.Format("%s %" _NUMLINE_ "d] %" _NUMMSG_ "d: ",strBuf2.c_str(),u32Line,m_u32GlobalNextMsg++);
  m_u32NextMsg++;
  strBuf3 += strBuf1+"\n";
  SysTrace("ERRORS.LOG",strBuf3.c_str());                   // put message in WARNINGS.LOG
  m_u32GlobalNextError++;
  m_u32NextError++;
  }

//----------------------------------------------- StreamTrace --------------------------------------------------------//
// put stream in TRACE.LOG & [pszFile].LOG
// in : file name,line number,indent increment
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::StreamTrace(const char* pszFile,const u32 u32Line,const s32 s32Indent)
  {
  char* lpcBuf;
  m_Stream << 'a';                                          // add dummy character
  lpcBuf = m_Stream.str();                                  // stream buffer address
  int iCount = m_Stream.pcount();                           // buffer length
  lpcBuf[iCount-1] = 0;                                     // ASCIIZ

  SendReportCallback(lpcBuf,m_u32GlobalNextMsg,"trace",pszFile,u32Line);

  if((s32Indent    < 0) && (m_u32Indent > 0)) m_u32Indent += s32Indent;
  if(m_s32Priority > 0) Trace(pszFile,u32Line,lpcBuf);      // put stream in files
  if( s32Indent    > 0) m_u32Indent += s32Indent;

  m_Stream.rdbuf()->freeze(0);                              // unlock buffer
  m_Stream.rdbuf()->seekoff(0,ios::beg,ios::in|ios::out);   // clear  buffer
  }

//----------------------------------------------- StreamLog ----------------------------------------------------------//
// put stream in [pszFile].LOG
// in : file name,line number,type
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::StreamLog(const char* pszFile,const u32 u32Line,const char* pszType/*="log"*/)
  {
  char* lpcBuf;
  m_Stream << 'a';                                          // add dummy character
  lpcBuf = m_Stream.str();                                  // stream buffer address
  int iCount = m_Stream.pcount();                           // buffer length
  lpcBuf[iCount-1] = 0;                                     // ASCIIZ

  SendReportCallback(lpcBuf,m_u32GlobalNextMsg,pszType,pszFile,u32Line);

  if(m_s32Priority > 0) Log(pszFile,u32Line,lpcBuf);        // put stream in files

  m_Stream.rdbuf()->freeze(0);                              // unlock buffer
  m_Stream.rdbuf()->seekoff(0,ios::beg,ios::in|ios::out);   // clear  buffer
  }

//----------------------------------------------- StreamWarning ------------------------------------------------------//
// put stream in WARNINGS.LOG & [pszFile].LOG & TRACE.LOG
// in : file name,line number
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::StreamWarning(const char* pszFile,const u32 u32Line)
  {
  char* lpcBuf;
  m_Stream << 'a';                                          // add dummy character
  lpcBuf = m_Stream.str();                                  // stream buffer address
  int iCount = m_Stream.pcount();                           // buffer length
  lpcBuf[iCount-1] = 0;                                     // ASCIIZ

  SendReportCallback(lpcBuf,m_u32GlobalNextMsg,"warning",pszFile,u32Line);

  if(m_s32Priority > 0) Warning(pszFile,u32Line,lpcBuf);    // put stream in files

  m_Stream.rdbuf()->freeze(0);                              // unlock buffer
  m_Stream.rdbuf()->seekoff(0,ios::beg,ios::in|ios::out);   // clear  buffer
  }

//----------------------------------------------- StreamError --------------------------------------------------------//
// put stream in ERRORS.LOG & [pszFile].LOG & TRACE.LOG
// in : file name,line number
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::StreamError(const char* pszFile,const u32 u32Line)
  {
  char* lpcBuf;
  m_Stream << 'a';                                          // add dummy character
  lpcBuf = m_Stream.str();                                  // stream buffer address
  int iCount = m_Stream.pcount();                           // buffer length
  lpcBuf[iCount-1] = 0;                                     // ASCIIZ

  SendReportCallback(lpcBuf,m_u32GlobalNextMsg,"error",pszFile,u32Line);

  if(m_s32Priority > 0) Error(pszFile,u32Line,lpcBuf);      // put stream in files

  m_Stream.rdbuf()->freeze(0);                              // unlock buffer
  m_Stream.rdbuf()->seekoff(0,ios::beg,ios::in|ios::out);   // clear  buffer
  }

//----------------------------------------------- SendCallback -------------------------------------------------------//
// send string to log view if any
// in : string
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::SendCallback(const char* pszString)
  {
  if(!m_pCallback) return;
  (*m_pCallback)(pszString);
  }

//----------------------------------------------- SendReportCallback -------------------------------------------------//
// send string to log report view if any
// in : string,ID,level,file,line
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogger::SendReportCallback(const char* pszString,const DWORD dwID,const char* pszLevel,
                                 const char* pszFile,  const DWORD dwLine)
  {
  if(!m_pReportCallback) return;

  const char* pcPos = pszFile;
  if(pszFile && !_strnicmp(pszFile,m_strRootPath.c_str(),m_u32RootLen))
    pcPos += m_u32RootLen;

  (*m_pReportCallback)(pszString,dwID,pszLevel,pcPos,dwLine,m_dwExtraData);
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                LOG HELPER                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Init ---------------------------------------------------------------//
// save the file,line,and msg
// in : __FILE__,__LINE__,msg
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CLogHelper::Init(const char* pszFile,const u32 u32Line,ostrstream Stream)
  { 
  int iCount = Stream.pcount();
  m_pszBuf   = new char[iCount+1];
  if(m_pszBuf)
    {
    memcpy(m_pszBuf,Stream.str(),iCount);
    m_pszBuf[iCount] = 0;
    Stream.rdbuf()->freeze(0); 
    }

  iCount = strlen(pszFile);
  m_pszFile = new char[iCount+1];
  if(m_pszFile)
    {
    memcpy(m_pszFile,pszFile,iCount);
    m_pszFile[iCount] = 0;
    }

  m_u32Line = u32Line;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
