//--------------------------------------------------------------------------------------------------------------------//
//                                                LOGGER CLASS & MACROS                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_LOGGER_INL_
#define   _GPG4_LOGGER_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CLogger::CLogger(const char* pszDir)
  {
  ResetCounters();
  m_u32RootLen = 0;
  m_pCallback  = NULL;
  m_pReportCallback = NULL;

  Reset(pszDir);
  }

INLINE CLogger::~CLogger()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE u32 CLogger::GetNextGlobalError() const
  {
  return m_u32GlobalNextError;
  }

INLINE u32 CLogger::GetNextGlobalWarning() const
  {
  return m_u32GlobalNextWarning;
  }

INLINE u32 CLogger::GetNextGlobalMsg() const
  {
  return m_u32GlobalNextMsg;
  }

INLINE u32 CLogger::GetNextError() const
  {
  return m_u32NextError;
  }

INLINE u32 CLogger::GetNextWarning() const
  {
  return m_u32NextWarning;
  }

INLINE u32 CLogger::GetNextMsg() const
  {
  return m_u32NextMsg;
  }

// msg

INLINE bool CLogger::IsMsgEnabled(const u32 u32MsgBit) const
  {
  return((m_s32Priority > 0) && (u32MsgBit & m_u32MsgMask) && (m_u32Indent <= m_u32IndentMax) && (m_u32Indent >= m_u32IndentMin));
  }

INLINE void CLogger::EnableMsg(const u32 u32MsgBit,const bool boSet)
  {
  if(boSet) m_u32MsgMask |=  u32MsgBit;
  else      m_u32MsgMask &= ~u32MsgBit;
  }

INLINE u32 CLogger::GetMsgs() const
  {
  return m_u32MsgMask;
  }

INLINE void CLogger::SetMsgs(const u32 u32MsgBit)
  {
  m_u32MsgMask = u32MsgBit;
  }

// output

INLINE bool CLogger::IsOutputEnabled(const u32 u32OutputBit) const
  {
  return((u32OutputBit & m_u32OutputMask) != 0);
  }

INLINE void CLogger::EnableOutput(const u32 u32OutputBit,const bool boSet)
  {
  if(boSet) m_u32OutputMask |=  u32OutputBit;
  else      m_u32OutputMask &= ~u32OutputBit;
  }

INLINE u32 CLogger::GetOutputs() const
  {
  return m_u32OutputMask;
  }

INLINE void CLogger::SetOutputs(const u32 u32OutputBit)
  {
  m_u32OutputMask = u32OutputBit;
  }

// priority

INLINE s32 CLogger::GetPriority() const
  {
  return m_s32Priority;
  }

INLINE void CLogger::SetPriority(const s32 s32Priority)
  {
  m_s32Priority = s32Priority;
  }

INLINE void CLogger::IncPriority()
  {
  m_s32Priority++;
  }

INLINE void CLogger::DecPriority()
  {
  m_s32Priority--;
  }

// indentation

INLINE void CLogger::SetIndentRange(const u32 u32Min,const u32 u32Max)
  {
  m_u32IndentMin = u32Min;
  m_u32IndentMax = u32Max;
  }

INLINE void CLogger::GetIndentRange(u32* pu32Min,u32* pu32Max) const
  {
  *pu32Min = m_u32IndentMin;
  *pu32Max = m_u32IndentMax;
  }

INLINE void CLogger::IncIndent()
  {
  m_u32Indent++;
  }

INLINE void CLogger::DecIndent()
  {
  if(m_u32Indent) m_u32Indent--;
  }

// root path

INLINE void CLogger::SetRootPath(const char* pszPath)
  {
  if(!pszPath) return;
  m_strRootPath = pszPath;
  m_strRootPath.MakeUpper();
  m_u32RootLen  = m_strRootPath.size();
  }

INLINE CStdString CLogger::GetRootPath() const
  {
  return m_strRootPath;
  }

// last "pb"

INLINE CStdString CLogger::GetLastWarning() const
  {
  return m_strLastWarning;
  }

INLINE CStdString CLogger::GetLastError() const
  {
  return m_strLastError;
  }

// callback

INLINE LogCallbackFct CLogger::GetCallback() const
  {
  return m_pCallback;
  }

INLINE void CLogger::SetCallback(LogCallbackFct pCallback)
  {
  m_pCallback = pCallback;
  }

INLINE ReportCallback CLogger::GetReportCallback() const
  {
  return m_pReportCallback;
  }

INLINE void CLogger::SetReportCallback(ReportCallback pReportCallback,DWORD dwExtraData)
  {
  m_pReportCallback = pReportCallback;
  m_dwExtraData     = dwExtraData;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_LOGGER_INL_
