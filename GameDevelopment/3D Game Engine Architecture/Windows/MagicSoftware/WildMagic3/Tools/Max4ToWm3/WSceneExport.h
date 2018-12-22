// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WSCENEEXPORT_H
#define WSCENEEXPORT_H

#include "max.h"
#include "WExportSettings.h"

class WSceneExport : public SceneExport 
{
public:
    // construction and destruction
    WSceneExport ();
    virtual ~WSceneExport ();       

    // overrides from SceneExport
    virtual int ExtCount ();                    
    virtual const TCHAR* Ext (int iExt);    
    virtual const TCHAR* LongDesc ();           
    virtual const TCHAR* ShortDesc ();          
    virtual const TCHAR* AuthorName ();         
    virtual const TCHAR* CopyrightMessage ();   
    virtual const TCHAR* OtherMessage1 ();      
    virtual const TCHAR* OtherMessage2 ();      
    virtual unsigned int Version ();                
    virtual void ShowAbout (HWND hWnd);         
    virtual int DoExport (const TCHAR* acFilename, ExpInterface* pkExport,
        Interface* pkMax, BOOL bSuppressPrompts = FALSE, DWORD dwOptions = 0);
    virtual BOOL SupportsOptions (int iExt, DWORD dwOptions);

    // user interface dialog's window procedure
    static BOOL CALLBACK DialogProcedure (HWND hWnd, UINT uiMsg,
        WPARAM wParam, LPARAM lParam);

    // user interface dialog message handlers   
    BOOL OnEnableFrameSpinners (HWND hWnd, BOOL bEnabled);
    BOOL OnEndFrameSpinnerChanged (HWND hWnd);
    BOOL OnInitDialog (HWND hWnd);
    BOOL OnMeshChecked (HWND hWnd, BOOL bEnabled);
    BOOL OnModifiersChecked (HWND hWnd, BOOL bEnabled);
    BOOL OnObjectsChecked (HWND hWnd, BOOL bEnabled);
    void OnOK (HWND hWnd);
    BOOL OnStartFrameSpinnerChanged (HWND hWnd);

private:
    enum { WSCENE_VERSION = 100 };

    // exporter configuration file methods
    void ReadConfiguration ();
    void WriteConfiguration ();
    
    Interface* m_pkMax;
    WExportSettings m_kSettings, m_kEffectiveSettings;
    TSTR m_kStrConfigFile;
};

#endif
