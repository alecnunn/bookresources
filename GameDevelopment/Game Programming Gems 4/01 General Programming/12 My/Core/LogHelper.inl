//--------------------------------------------------------------------------------------------------------------------//
//                                                LOGGER CLASS & MACROS                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_LOGHELPER_INL_
#define   _GPG4_LOGHELPER_INL_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "MyAssert.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CLogHelper::CLogHelper(CLogger* pLogger)
  {
  m_u32Line = 0;
  m_pLogger = pLogger;
  MY_ASSERT(pLogger,"logger can't be NULL");
  }

//

INLINE CLogHelper::~CLogHelper()
  {
  if(m_u32Line)
    {                                                       // trace mode was off when the object has been created
    m_pLogger->m_Stream << m_pszBuf << " ends"; 
    m_pLogger->StreamTrace(m_pszFile,m_u32Line,-1);
    delete m_pszBuf; 
    delete m_pszFile;
    }
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_LOGHELPER_INL_
