//--------------------------------------------------------------------------------------------------------------------//
//                                                CLASS GROUPING THE FILES MAKING A LEVEL                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_LEVELFILES_H_
#define   _GPG4_LEVELFILES_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Defines.h"
#include  "FileC.h"
#include  "LinkLoad.h"
#include  "Persistent.h"
#include  "PropDescMap.h"
#include  "StdString.h"
#include  "TSet.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CLogger;
  class CRTTIMap;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CLevelFiles                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CLevelFiles
  {
  // enums

  public:

    typedef enum
      {
      TEXT        = 0,
      BINARY,
      } MODE;

  // typedefs

  public:

    typedef TSet< CStdString >                    setStr;
    typedef TSet< CPersistent* >                  setObjPtr;
    typedef std::queue< CPersistent* >            queueObjPtr;

    typedef std::map< CStdString,CPersistentSP >  mapStr2Obj;
    typedef mapStr2Obj::iterator                  iterStr2Obj;

    typedef std::map<CPersistent*,CPersistentSP>  mapAddr2Obj;
    typedef mapAddr2Obj::iterator                 iterAddr2Obj;

    typedef std::list< CLinkLoadSP >              listLinks;
    typedef listLinks::iterator                   iterLinks;

    typedef CPersistent* (*pfnCreate)  (const CStdString& strClass);

  // public methods

  public:

    // constructors & destructor

                    CLevelFiles         (CRTTIMap* pmapRTTI,pfnCreate CreateFct);
    virtual        ~CLevelFiles         (void);

    // get

    CStdString&     GetDir              (void);
    File&           GetMainFile         (void);
    File&           GetDescFile         (void);
    File&           GetDataFile         (void);

    CLogger*        GetLogger           (void) const;
    CRTTIMap*       GetRTTIMap          (void) const;

    CStdString&     GetEngineRoot       (void);
    CStdString&     GetEditorRoot       (void);
    CPersistent*    GetEngineRootAddr   (void) const;
    CPersistent*    GetEditorRootAddr   (void) const;

    bool            BinaryMode          (void) const;
    bool            TextMode            (void) const;

    // set

    void            SetDir              (const CStdString& strDir);
    void            SetLogger           (CLogger* pLogger);
    void            SetEngineRootID     (const CStdString& strRoot);
    void            SetEditorRootID     (const CStdString& strRoot);
    void            SetEngineRootAddr   (CPersistent* pRoot);
    void            SetEditorRootAddr   (CPersistent* pRoot);

    void            SetMode             (const MODE Mode);

    // containers

    void            AddSavedDesc        (const CStdString& strClass);
    bool            DescIsSaved         (const CStdString& strClass);

    void            AddSavedObj         (CPersistent* pObj);
    bool            ObjIsSaved          (CPersistent* pObj);

    void            AddObjToSave        (CPersistent* pObj);
    u32             ObjToSaveCount      (void);
    CPersistent*    PopObjToSave        (void);

    void            AddLink             (CLinkLoad* pLink);

    void            AddPropDesc         (const CStdString& strClass,CPropListLoad* pListLoad);
    CPropListLoad*  GetPropDesc         (const CStdString& strClass);

    void            AddCreatedObj       (const CStdString& strID,CPersistent* pObj);
    CPersistent*    GetCreatedObj       (const CStdString& strID);

    void            AddCreatedObj       (CPersistent* pID,CPersistent* pObj);
    CPersistent*    GetCreatedObj       (CPersistent* pID);

    // streaming

    bool            WriteMainFile       (void);
    bool            ReadMainFile        (void);

    CPersistent*    CreateObject        (const CStdString& strClass,const CStdString& strID);
    CPersistent*    CreateObject        (const CStdString& strClass,CPersistent* pID);
    bool            LinkObjects         (void);
    bool            PostProcess         (void);

  // protected data

  protected:

    // files

    File            m_MainFile;
    File            m_DescFile;
    File            m_DataFile;

    MODE            m_Mode;

    // main file data

    CStdString      m_strVersion;
    CStdString      m_strDir;                               // level directory
    CStdString      m_strCurrentCam;
    CStdString      m_strSceneSel;
    CStdString      m_strContextObj;

    // PropDesc

    CPropDescMap    m_mapPropDesc;                          // prop desc already loaded
    CRTTIMap*       m_pmapRTTI;                             // map[String->pRTTI]

    // <data> & <root>

    pfnCreate       m_pfnCreate;
    mapStr2Obj      m_mapCreated;
    mapAddr2Obj     m_mapCreatedBin;

    CStdString      m_strEngineRootID;                      // <engine_root...>
    CStdString      m_strEditorRootID;                      // <editor_root...>
    CPersistent*    m_pEngineRootAddr;
    CPersistent*    m_pEditorRootAddr;

    listLinks       m_listLinks;

    //

    CLogger*        m_pLogger;

    //

    setStr          m_SavedDesc;                            // props desc already saved
    setObjPtr       m_SavedObj;                             // objs       already saved
    queueObjPtr     m_ObjToSave;                            // pointed to objects to save later
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "LevelFiles.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_LEVELFILES_H_
