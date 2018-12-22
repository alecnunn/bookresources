//--------------------------------------------------------------------------------------------------------------------//
//                                                RTTI MAP CLASS                                                      //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_RTTIMAP_H_
#define   _GPG4_RTTIMAP_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"
#include  "StdString.h"
#include  "StlIncludes.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CRTTI;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CRTTIMap                                                            //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CRTTIMap
  {
  // typedefs

  public:

    typedef std::map< CStdString,const CRTTI* >   mapStr2RTTI;
    typedef mapStr2RTTI::iterator                 iterStr2RTTI;

  // public methods

  public:
                              CRTTIMap            (void);
                             ~CRTTIMap            (void);

    // get/set

    const CRTTI*              GetRTTI             (const CStdString& strClass);
    void                      AddRTTI             (const CStdString& strClass,const CRTTI* pRTTI);

  // protected data

  protected:

    mapStr2RTTI               m_map;
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#include  "RTTIMap.inl"

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_RTTIMAP_H_
