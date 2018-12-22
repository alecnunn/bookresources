//--------------------------------------------------------------------------------------------------------------------//
//                                                CLASS-ID CLASS                                                      //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//                                                                                                                    //
//           the goal of this class is to have simple class IDs, primarily based on the name of the class (15 chrs)   //
//           (but it can be anything else in fact)                                                                    //
//      rem: sort with < is NOT in alphabetical order                                                                 //
//           use CClassIDCompareNoCase for that, as a predicate in a map for example :                                //
//           typedef std::map< const GPG4::CClassID, u32, GPG4::CClassIDCompareNoCase > mapID2Number;                 //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_CLASSID_H_
#define   _GPG4_CLASSID_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"
#include  "Defines.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CClassID                                                            //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CClassID
  {
  // public methods

  public:
                    CClassID            (void);
                    CClassID            (const CClassID& ID);
                    CClassID            (const char*  pszID);
                    CClassID            (const u32 u32A,const u32 u32B=0,const u32 u32C=0,const u32 u32D=0);
                   ~CClassID            (void);

    // operators

    CClassID&       operator =          (const CClassID& ID);
    CClassID&       operator =          (const char*  pszID);
    CClassID&       operator =          (const u32*  pu32ID);

    bool            operator ==         (const CClassID& ID) const;
    bool            operator !=         (const CClassID& ID) const;
    bool            operator ==         (const char*  pszID) const;
    bool            operator !=         (const char*  pszID) const;
    bool            operator ==         (const u32*  pu32ID) const;
    bool            operator !=         (const u32*  pu32ID) const;

    bool            operator <          (const CClassID& ID) const;   // to sort the ClassIDs;
    bool            operator >          (const CClassID& ID) const;   // ! be careful, this is NOT the alphabetical order of m_szID

    static bool     CompareNoCase       (const CClassID& ID1,const CClassID& ID2);
                                                                      // this IS the alphabetical order
    // get/set

    const char*     GetIDsz             (void) const;
    const u32*      GetIDu32            (void) const;
    void            GetID               (u32* pu32A,u32* pu32B,u32* pu32C,u32* pu32D) const;

    void            Set                 (const char* pszID);
    void            Set                 (const u32 u32A,const u32 u32B=0,const u32 u32C=0,const u32 u32D=0);

  // protected data

  protected:

    union
      {
      char          m_szID[16];                             // zero terminated string, padded with zeros
      u32           m_u32ID[4];
      };
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CClassIDCompareNocase                                               //
// predicate for STL containers                                                                                       //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CClassIDCompareNoCase
  {
  // public methods

  public:

    // operators

    bool            operator ()         (const CClassID& ID1,const CClassID& ID2);
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "ClassID.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_CLASSID_H_
