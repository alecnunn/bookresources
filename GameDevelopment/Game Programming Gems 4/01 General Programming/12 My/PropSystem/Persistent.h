//--------------------------------------------------------------------------------------------------------------------//
//                                                PERSISTENT OBJECT CLASS                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_PERSISTENT_H_
#define   _GPG4_PERSISTENT_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "ExtraProp.h"
#include  "RefCount.h"
#include  "SmartPtr.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CLevelFiles;
  class CPropListLoad;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPersistent                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPersistent : public CRefCount
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CPersistent,CExtraProp);

  // public methods

  public:

    // constructors & destructor

                              CPersistent         (void);
                              CPersistent         (const CPersistent& Persistent);
    virtual                  ~CPersistent         (void);

    // get/set

    virtual CStdString        GetDefaultName      (void) const;
    virtual CStdString        GetEditName         (void) const;

    virtual const CStdString& GetName             (void) const;
    virtual void              SetName             (const CStdString& strName);

    virtual const CRTTI*      GetWriteRTTI        (void) const;

    // streaming

    virtual bool              Write               (CLevelFiles& Level);

    virtual bool              ReadPropFile        (CLevelFiles& Level);
    virtual bool              ReadPropFileBin     (CLevelFiles& Level);
    virtual bool              ReadDataFile        (CLevelFiles& Level);
    virtual bool              ReadDataFileBin     (CLevelFiles& Level);

    virtual bool              LinkObjects         (CLevelFiles& Level);
    virtual bool              PostProcess         (CLevelFiles& Level);
    virtual bool              ReadValue           (CLevelFiles& Level,CStdString* pstrVal);
    virtual bool              PostRead            (void);

    // editing

    bool                      IsPropOfThis        (const CProperty* pProp) const;
    bool                      IsPropOfRTTI        (const CProperty* pProp,const CRTTI* pRTTI) const;
    virtual bool              ModifyProp          (CProperty* pProp,const CStdString& strNew);

    virtual bool              IsPropExposed       (CProperty* pProp);
    virtual bool              IsPropReadOnly      (CProperty* pProp);

    // helpers

    static bool               LookForTag          (IFile& File,char* pszLine,const u32 u32Size,const char* pszTag,  const char* pszStop="</xml>");
    static u32                LookForTags         (IFile& File,char* pszLine,const u32 u32Size,CStdString* pstrTags,const u32 u32NbTags);
    static CStdString         GetTagParam         (const char* pszLine,const char* pszParam);

    static  bool              WriteString2Bin     (IFile& File,const CStdString& strTxt);
    static  bool              ReadStringBin       (IFile& File,CStdString* pstrTxt);

  // protected methods

  protected:

    // streaming

    virtual bool              WritePropDesc       (CLevelFiles& Level);
    virtual bool              WriteBaseDesc       (CLevelFiles& Level,const CRTTI* pRTTI);
    virtual bool              WriteData           (CLevelFiles& Level);
    virtual bool              WriteBaseData       (CLevelFiles& Level,const CRTTI* pRTTI);

    virtual bool              ReadProperties      (CLevelFiles& Level,CPropListLoad* pListLoad);
    virtual bool              ReadPropertiesBin   (CLevelFiles& Level,CPropListLoad* pListLoad);
    virtual bool              MatchProperties     (CLevelFiles& Level,CPropListLoad* pListLoad);
    virtual void              RecursiveMatch      (CPropListLoad* pListFile,CPropListLoad* pListBase,const CRTTI* pExeRTTI);
    virtual bool              MatchProperty       (CPropListLoad* pDescFile,const CRTTI* pRTTI,  CProperty* pPropLoad);
    virtual bool              InsertProperty      (CPropListLoad* pDescFile,CProperty* pPropLoad,CProperty* pPropExe);

    virtual bool              ReadValues          (CLevelFiles& Level,CPropListLoad* pPropDesc,CPersistent* pObj);
    virtual bool              ReadValuesBin       (CLevelFiles& Level,CPropListLoad* pPropDesc,CPersistent* pObj);
    virtual bool              ReadUnmatched       (CLevelFiles& Level,CProperty* pProp,const CStdString& strVal);
    virtual bool              ReadUnmatchedBin    (CLevelFiles& Level,CProperty* pProp);

    // operators

    CPersistent&              operator =          (const CPersistent& Persistent);

  // protected data

  protected:

    CStdString                m_strName;
  };

  // smart ptr

  SMART_POINTER(CPersistent);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "Persistent.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_PERSISTENT_H_
