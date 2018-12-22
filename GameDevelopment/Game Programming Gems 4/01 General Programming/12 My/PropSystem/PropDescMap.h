//--------------------------------------------------------------------------------------------------------------------//
//                                                MAP OF LOADED PROPERTIES                                            //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_PROPDESCMAP_H_
#define   _GPG4_PROPDESCMAP_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Defines.h"
#include  "StdString.h"
#include  "StlIncludes.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CPropListLoad;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropDescMap                                                        //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropDescMap
  {
  // typedefs

  public:

    typedef std::map<CStdString,CPropListLoad*>   mapStr2ListLoad;
    typedef mapStr2ListLoad::iterator             iterStr2ListLoad;

  // public methods

  public:

    // constructors & destructor

                              CPropDescMap        (void);
    virtual                  ~CPropDescMap        (void);

    // operations

    void                      AddPropDesc         (const CStdString& strClass,CPropListLoad* pListLoad);
    CPropListLoad*            GetPropDesc         (const CStdString& strClass);

  // protected methods

  protected:

    static void               DeleteListLoad      (std::pair<CStdString,CPropListLoad*> Iter);

  // protected data

  protected:

    mapStr2ListLoad           m_map;
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "PropDescMap.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_PROPDESCMAP_H_
