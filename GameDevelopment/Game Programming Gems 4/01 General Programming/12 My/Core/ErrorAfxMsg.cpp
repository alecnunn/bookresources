//--------------------------------------------------------------------------------------------------------------------//
//                                                SIMPLE ERROR OBJECT                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "ErrorAfxMsg.h"

#include  "Logger.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CErrorAfxMsg::CErrorAfxMsg(const CStdString& strMsg,CLogger* pLog) : m_strMsg(strMsg), m_pLog(pLog)
  {
  m_boOK = false;
  }

CErrorAfxMsg::~CErrorAfxMsg()
  {
  if(!m_boOK)
    {
    if(m_pLog) _DWAR_((*m_pLog),m_strMsg.c_str());
    MessageBox(/*AfxGetMainWnd()->m_hWnd*/NULL,m_strMsg.c_str(),"ERROR",MB_OK|MB_ICONEXCLAMATION);
    }
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
