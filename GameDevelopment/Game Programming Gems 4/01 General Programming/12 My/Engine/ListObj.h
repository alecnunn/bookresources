//--------------------------------------------------------------------------------------------------------------------//
//                                                OBJECTS LIST CLASS                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_LISTOBJ_H_
#define   _GPG4_LISTOBJ_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"
#include  "Defines.h"
#include  "StlIncludes.h"
#include  "SmartPtr.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CEngineObj;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CListObjs                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  SMART_POINTER(CEngineObj);

  class CListObj
  {
  // typedefs

  public:

    typedef std::list< CEngineObjSP >             listObj;
    typedef listObj::iterator                     iterObj;

  // public methods

  public:

    // constructors & destructor

                              CListObj            (void);
    virtual                  ~CListObj            (void);

    // Objs

    u32                       GetNbObj            (void) const;
    CEngineObj*               GetFirstObj         (void);
    CEngineObj*               GetNextObj          (CEngineObj* pCurrent=NULL);

    bool                      AddObj              (CEngineObj* pObj);
    bool                      RemoveObj           (CEngineObj* pObj);
    void                      RemoveAll           (void);

  // protected data

  protected:

    // Objs

    listObj                   m_Objs;
    iterObj                   m_CurrObj;
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "ListObj.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_LISTOBJ_H_
