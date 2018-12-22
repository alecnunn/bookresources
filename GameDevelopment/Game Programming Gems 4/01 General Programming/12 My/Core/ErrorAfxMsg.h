//--------------------------------------------------------------------------------------------------------------------//
//                                                SIMPLE ERROR OBJECT                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ERRORAFXMSG_H_
#define   _GPG4_ERRORAFXMSG_H_
#pragma    once

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "StdString.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CLogger;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CErrorAfxMsg                                                        //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CErrorAfxMsg
  {
  // public methods

  public:

    // constructors & destructor

                    CErrorAfxMsg        (const CStdString& strMsg,CLogger* pLog=NULL);
    virtual        ~CErrorAfxMsg        (void);

  // public data

  public:

    CStdString      m_strMsg;
    bool            m_boOK;
    CLogger*        m_pLog;
  };
}

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_ERRORAFXMSG_H_
