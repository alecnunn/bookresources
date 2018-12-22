//--------------------------------------------------------------------------------------------------------------------//
//                                                LIST OF LOADED PROPERTIES                                           //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_PROPLISTLOAD_H_
#define   _GPG4_PROPLISTLOAD_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "PropList.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropListLoad                                                       //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropListLoad : public CPropList
  {
  // public methods

  public:

    // constructors & destructor

                              CPropListLoad       (const CStdString& strName,CPropListLoad* pBase);
    virtual                  ~CPropListLoad       (void);

    // get

    CPropListLoad*            GetBase             (void) const;
    const CStdString&         GetName             (void) const;
    CPropList&                GetFlatMatch        (void);

  // protected data

  protected:

    CPropListLoad*            m_pBase;                      // CPropList base
    CStdString                m_strName;                    // class name

    CPropList                 m_FlatMatch;                  // match with exe layout, containing all inherited properties
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "PropListLoad.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_PROPLISTLOAD_H_
