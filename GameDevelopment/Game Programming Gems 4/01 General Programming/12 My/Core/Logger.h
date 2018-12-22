//--------------------------------------------------------------------------------------------------------------------//
//                                                LOGGER CLASS & MACROS                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//                                                                                                                    //
//           this class writes to several log files according to the macros that are used                             //
//           an application can use many loggers at the same time                                                     //
//                                                                                                                    //
// macros  : _DLOG_     writes to [file].LOG                                                                          //
//           _DTRC_     writes to [file].LOG & TRACE.LOG                                                              //
//           _DWAR_     writes to [file].LOG & TRACE.LOG & WARNINGS.LOG                                               //
//           _DERR_     writes to [file].LOG & TRACE.LOG & ERRORS.LOG                                                 //
//           _DTRCINC_  same as _DTRC_ and indentation++                                                              //
//           _DTRCDEC_  same as _DTRC_ and indentation--                                                              //
//           _DTRCDEF_  same as _DTRCINC_+_DTRCDEC_                                                                   //
//           _DTRCIF_   same as _DTRC_ with a condition                                                               //
//           _DWARRET_  if(!Cond) _DWAR_ and return                                                                   //
//           _DTRCRET_  if(!Cond) _DWAR_ and _DTRCDEC_ and return                                                     //
//           _DTRCWND_  same as _DTRC_ with another message bit                                                       //
//           _DWND_     same as _DTRCWND_ but without file/line info (= raw text is sent)                             //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_LOGGER_H_
#define   _GPG4_LOGGER_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  <strstrea.h>                                      // ostrstream
#include  "StdString.h"

#include  "BasicTypes.h"
#include  "Defines.h"

//----------------------------------------------- DEFINES ------------------------------------------------------------//

#define   _TRACE_                                           // enable the macros
#define   _LOGWND_                                          // send to window

//----------------------------------------------- TYPEDEFS -----------------------------------------------------------//

class     CDocument;
typedef   void      (*LogCallbackFct) (const char*);        // callback pointer
typedef   void      (*ReportCallback) (const char*,const DWORD,const char*,const char*,const DWORD,const DWORD);
                                                            // msg,ID,level,file,line,extra data

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CLogger                                                             //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CLogger
  {
  // public enums

  public:

    enum
      {
      _OUTPUT_NONE_ = 0,
      _OUTPUT_FILE_ = 1 << 0,
      _OUTPUT_WND_  = 1 << 1,
      _OUTPUT_ALL_  = DWORD(-1),
      }
    _OUTPUT_ENUM_;

    enum
      {
      _MSG_NONE_    = 0,
      _MSG_LOG_     = 1 << 0,
      _MSG_WAR_     = 1 << 1,
      _MSG_ERR_     = 1 << 2,
      _MSG_TRC_     = 1 << 3,
      _MSG_TRCINC_  = 1 << 4,
      _MSG_TRCDEC_  = 1 << 5,
      _MSG_TRCDEF_  = 1 << 6,
      _MSG_TRCWND_  = 1 << 7,
      _MSG_WND_     = 1 << 8,
      _MSG_ALL_     = DWORD(-1),
      }
    _MSG_ENUM_;

  // public methods

  public:
                    CLogger             (const char* pszDir=NULL);
                   ~CLogger             (void);

    // setup/cleanup

    void            DeleteFiles         (const char* pszDir);
    void            ResetCounters       (void);
    void            Reset               (const char* pszDir="LOGS");
    CStdString      GetResults          (void) const;

    // get/set

    u32             GetNextGlobalError  (void) const;
    u32             GetNextGlobalWarning(void) const;
    u32             GetNextGlobalMsg    (void) const;

    u32             GetNextError        (void) const;
    u32             GetNextWarning      (void) const;
    u32             GetNextMsg          (void) const;

    bool            IsMsgEnabled        (const u32 u32MsgBit) const;
    void            EnableMsg           (const u32 u32MsgBit, const bool boSet);
    u32             GetMsgs             (void) const;
    void            SetMsgs             (const u32 u32MsgBit);

    bool            IsOutputEnabled     (const u32 u32OutputBit) const;
    void            EnableOutput        (const u32 u32OutputBit, const bool boSet);
    u32             GetOutputs          (void) const;
    void            SetOutputs          (const u32 u32OutputBit);

    s32             GetPriority         (void) const;
    void            SetPriority         (const s32 s32Priority);
    void            IncPriority         (void);
    void            DecPriority         (void);

    void            SetIndentRange      (const u32 u32Min,const u32 u32Max);
    void            GetIndentRange      (u32* pu32Min,u32* pu32Max) const;
    void            IncIndent           (void);
    void            DecIndent           (void);

    void            SetRootPath         (const char* pszPath);
    CStdString      GetRootPath         (void) const;

    CStdString      GetLastWarning      (void) const;
    CStdString      GetLastError        (void) const;

    LogCallbackFct  GetCallback         (void) const;
    void            SetCallback         (LogCallbackFct pCallback);
    ReportCallback  GetReportCallback   (void) const;
    void            SetReportCallback   (ReportCallback pReportCallback,DWORD dwExtraData);

    // trace

    void            SysTrace            (const char* pszFile,const char* pszString);
    void            Trace               (const char* pszFile,const u32 u32Line,const char* pszString);
    void            Log                 (const char* pszFile,const u32 u32Line,const char* pszString);
    void            Warning             (const char* pszFile,const u32 u32Line,const char* pszString);
    void            Error               (const char* pszFile,const u32 u32Line,const char* pszString);
    void            StreamTrace         (const char* pszFile,const u32 u32Line,const s32   s32Indent);
    void            StreamLog           (const char* pszFile,const u32 u32Line,const char* pszType="log");
    void            StreamWarning       (const char* pszFile,const u32 u32Line);
    void            StreamError         (const char* pszFile,const u32 u32Line);
    
    void            SendCallback        (const char* pszString);
    void            SendReportCallback  (const char* pszString,const DWORD dwID,const char* pszLevel,
                                         const char* pszFile,  const DWORD dwLine);

  // public data

  public:

    ostrstream      m_Stream;

  // protected data

  protected:

    static u32      m_u32GlobalNextError;
    static u32      m_u32GlobalNextWarning;
    static u32      m_u32GlobalNextMsg;

    u32             m_u32NextError;
    u32             m_u32NextWarning;
    u32             m_u32NextMsg;

    u32             m_u32MsgMask;                           // bits field: messages that are enabled
    u32             m_u32OutputMask;                        // bits field: outputs  that are enabled

    s32             m_s32Priority;                          // can be ++/-- many times, messages are only logged when > 0
    u32             m_u32Indent;                            // indentation level
    u32             m_u32IndentMin,m_u32IndentMax;          // indentation limits, msgs only logged when min <= m_dwIndent <= max

    CStdString      m_strRootPath;                          // path that is removed from the full source dir
    u32             m_u32RootLen;                           // optim
    CStdString      m_strDir;                               // logs directory

    CStdString      m_strLastWarning;
    CStdString      m_strLastError;
    LogCallbackFct  m_pCallback;                            // target for the wnd msgs

    ReportCallback  m_pReportCallback;
    DWORD           m_dwExtraData;                          // used by the callback as it wants
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CLogHelper                                                          //
// used by _DTRCDEF_                                                                                                  //
// ~CLogHelper calls _DTRCDEC_ automatically                                                                          //
// -------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CLogHelper
  {
  // public methods

  public:

                    CLogHelper          (CLogger* pLogger);
                   ~CLogHelper          (void);


    void            Init                (const char* pszFile,const u32 u32Line,ostrstream Stream);

  // protected data

  protected:

    CLogger*        m_pLogger;
    char*           m_pszBuf;                               // function name
    char*           m_pszFile;                              // __FILE__
    u32             m_u32Line;                              // __LINE__
  };
}

//----------------------------------------------- MACROS -------------------------------------------------------------//

#ifdef _TRACE_

  #define _DTRC_(Logger,Stream)         { if(Logger.IsMsgEnabled(GPG4::CLogger::_MSG_TRC_))         { Logger.m_Stream << Stream;   \
                                                                                                      Logger.StreamTrace  (__FILE__,__LINE__,0);  } }
  #define _DLOG_(Logger,Stream)         { if(Logger.IsMsgEnabled(GPG4::CLogger::_MSG_LOG_))         { Logger.m_Stream << Stream;   \
                                                                                                      Logger.StreamLog    (__FILE__,__LINE__);    } }
  #define _DLOG2_(Logger,Stream,Type)   { if(Logger.IsMsgEnabled(GPG4::CLogger::_MSG_LOG_))         { Logger.m_Stream << Stream;   \
                                                                                                      Logger.StreamLog    (__FILE__,__LINE__,Type); } }
  #define _DWAR_(Logger,Stream)         { if(Logger.IsMsgEnabled(GPG4::CLogger::_MSG_WAR_))         { Logger.m_Stream << Stream;   \
                                                                                                      Logger.StreamWarning(__FILE__,__LINE__);    } }
  #define _DERR_(Logger,Stream)         { if(Logger.IsMsgEnabled(GPG4::CLogger::_MSG_ERR_))         { Logger.m_Stream << Stream;   \
                                                                                                      Logger.StreamError  (__FILE__,__LINE__);    } }

  #define _DTRCINC_(Logger,Stream)      { if(Logger.IsMsgEnabled(GPG4::CLogger::_MSG_TRCINC_))      { Logger.m_Stream << Stream;                      \
                                                                                                      Logger.StreamTrace  (__FILE__,__LINE__,+1); }   \
                                          else                                                      { Logger.IncIndent(); }                       }
  #define _DTRCDEC_(Logger,Stream)      { if(Logger.IsMsgEnabled(GPG4::CLogger::_MSG_TRCDEC_))      { Logger.m_Stream << Stream;                      \
                                                                                                      Logger.StreamTrace  (__FILE__,__LINE__,-1); }   \
                                          else                                                      { Logger.DecIndent(); }                       }
  #define _DTRCDEF_(Logger,Stream)        CLogHelper LogHelper(&Logger);                                                                              \
                                          if(Logger.IsMsgEnabled(GPG4::CLogger::_MSG_TRCDEF_))      { Logger.m_Stream << Stream;                                \
                                                                                                      LogHelper.Init      (__FILE__,__LINE__,Logger.m_Stream);  \
                                                                                                      Logger.StreamTrace  (__FILE__,__LINE__,+1); }
  #define _DTRCIF_(Logger,Cond,Stream)  { if(Cond) _DTRC_(Logger,Stream); }

  //

  #define _RETSKEL_(Logger,Cond,Stream,Msgs)                                                                    \
                                          if(!(Cond))                                                           \
                                            {                                                                   \
                                            if(Logger.IsMsgEnabled(Msgs))                                       \
                                              {                                                                 \
                                              Logger.m_Stream << Stream;                                        \
                                              if(Logger.m_Stream.pcount())                                      \
                                                {                                                               \
                                                Logger.m_Stream.rdbuf()->freeze(0);                             \
                                                Logger.m_Stream.rdbuf()->seekoff(0,ios::beg,ios::in|ios::out);  \
                                                _DWAR_(Logger,Stream);                                          \
                                                }                                                               \
                                              Logger.m_Stream.rdbuf()->freeze(0);
                                      
  #define _DWARRET_(Logger,Cond,StreamWar,Return)                                                      \
                                        { _RETSKEL_(Logger,Cond,StreamWar,GPG4::CLogger::_MSG_WAR_)    \
                                              }                                                        \
                                            return Return;                                             \
                                            }                                                          \
                                        }

  #define _DTRCRET_(Logger,Cond,StreamWar,StreamDec,Return)                                                                          \
                                        { _RETSKEL_(Logger,Cond,StreamWar,GPG4::CLogger::_MSG_WAR_|GPG4::CLogger::_MSG_TRCDEC_)      \
                                              _DTRCDEC_(Logger,StreamDec);                                                           \
                                              }                                                                                      \
                                            return Return;                                                                           \
                                            }                                                                                        \
                                        }

  //

  #define _DTRCWND_(Logger,Stream)      { if(Logger.IsMsgEnabled(GPG4::CLogger::_MSG_TRCWND_))      { Logger.m_Stream << Stream;   \
                                                                                                      Logger.StreamTrace  (__FILE__,__LINE__,0);  } }
  #define _DWND_(Logger,Stream)         { if(Logger.IsMsgEnabled(GPG4::CLogger::_MSG_WND_))         { Logger.m_Stream << Stream;   \
                                                                                                      Logger.StreamTrace  ("",0,0);               } }

#else

  #define _DTRC_(Logger,Stream)         {}
  #define _DLOG_(Logger,Stream)         {}
  #define _DLOG2_(Logger,Stream,Type)   {}
  #define _DWAR_(Logger,Stream)         {}
  #define _DERR_(Logger,Stream)         {}

  #define _DTRCINC_(Logger,Stream)      {}
  #define _DTRCDEC_(Logger,Stream)      {}
  #define _DTRCDEF_(Logger,Stream)      {}
  #define _DTRCIF_ (Logger,Cond,Stream) {}

  #define _DWARRET_(Logger,Cond,StreamWar,Return)           { if(!(Cond)) return Return; }
  #define _DTRCRET_(Logger,Cond,StreamWar,StreamDec,Return) { if(!(Cond)) return Return; }

  #define _DTRCWND_(Logger,Stream)      {}
  #define _DWND_(Logger,Stream)         {}

#endif

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "Logger.inl"
#include  "LogHelper.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_LOGGER_H_
