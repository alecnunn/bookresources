// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "WSceneBuilder.h"
#include "WSceneExport.h"
#include "Max6ToWm3.h"
#include "resource.h"

//----------------------------------------------------------------------------
WSceneExport::WSceneExport ()
{
}
//----------------------------------------------------------------------------
WSceneExport::~WSceneExport ()
{
}
//----------------------------------------------------------------------------
int WSceneExport::ExtCount ()
{
    // The number of extensions supported by the plugin.  Only one is
    // supported (*.wmof).
    return 1;
}
//----------------------------------------------------------------------------
const TCHAR* WSceneExport::Ext (int)
{       
    // Return the filename extension.  Since we support only one extension,
    // return it as a constant string.
    return _T("wmof");
}
//----------------------------------------------------------------------------
const TCHAR* WSceneExport::LongDesc ()
{
    // Long description of the file type to be exported.
    return Max6ToWm3::ThePlugin.GetResourceString(IDS_LONGDESCRIPTION);
}
//----------------------------------------------------------------------------
const TCHAR* WSceneExport::ShortDesc () 
{           
    // Short description of the file type to be exported.
    return Max6ToWm3::ThePlugin.GetResourceString(IDS_SHORTDESCRIPTION);
}
//----------------------------------------------------------------------------
const TCHAR* WSceneExport::AuthorName ()
{           
    // The name of the plugin's author.
    return Max6ToWm3::ThePlugin.GetResourceString(IDS_AUTHORNAME);
}
//----------------------------------------------------------------------------
const TCHAR* WSceneExport::CopyrightMessage () 
{   
    // Returns a copyright message.  (TO DO)
    return _T("Magic Software, Inc. Copyright (c) 2004. All Rights Reserved");
}
//----------------------------------------------------------------------------
const TCHAR* WSceneExport::OtherMessage1 () 
{       
    // Return additional information.  Empty string for now.
    return _T("");
}
//----------------------------------------------------------------------------
const TCHAR* WSceneExport::OtherMessage2 () 
{       
    // Return even more additional information.  Empty string for now.
    return _T("");
}
//----------------------------------------------------------------------------
unsigned int WSceneExport::Version ()
{               
    // Return the version number of the exporter.
    return WSCENE_VERSION;
}
//----------------------------------------------------------------------------
void WSceneExport::ShowAbout (HWND)
{           
    // Allows the plugin to display an About box.  (TO DO)
}
//----------------------------------------------------------------------------
BOOL WSceneExport::SupportsOptions (int, DWORD)
{
    // Max calls this method to determine which options are supported for a
    // given file extension.  All options are currently supported.
    return TRUE;
}
//----------------------------------------------------------------------------
int WSceneExport::DoExport (const TCHAR* acFilename, ExpInterface* pkExport,
    Interface* pkMax, BOOL bSuppressPrompts, DWORD dwOptions)
{
    // Restore the exporter's configuration from disk, display the user
    // interface, build and save the required Wild Magic scene graph, and
    // save the exporter's config to disk.
    //   acFilename:
    //       Filename the user has chosen for the Wild Magic scene graph.
    //   pkExport:
    //       Interface pointer to access MAX's exporter specific
    //       functionality.
    //   pkMax:
    //       Interface pointer to access general MAX functionality.
    //   bSuppressPrompts:
    //       Allow, or disallow user input.
    //   dwOptions:
    //       Determines whether to export entire scene, or selection only.

    m_pkMax = pkMax;
    INode* pkRootNode = NULL;
    bool bExportSelected;

    if ( dwOptions == SCENE_EXPORT_SELECTED )
    {
        // User has chosen "Export Selected" - check to see that one and
        // only one node (corresponding to the top level node of the
        // hierarchy to be exported) has been selected.
        if ( m_pkMax->GetSelNodeCount() != 1 )
        {
            assert( false );
            return 0;
        }

        pkRootNode = m_pkMax->GetSelNode(0);
        bExportSelected = true;
    }
    else
    {
        // export entire scene, starting from the root node in MAX
        pkRootNode = m_pkMax->GetRootNode();
        bExportSelected = false;
    }

    // get the path to the exporter plugin's config file
    m_kStrConfigFile = m_pkMax->GetDir(APP_PLUGCFG_DIR);
    m_kStrConfigFile += "\\Max6ToWm3.cfg";

    // restore the exporter's configuration state
    ReadConfiguration();

    // show the exporter dialog
    if ( !bSuppressPrompts )
    {
        INT_PTR piResult = DialogBoxParam(Max6ToWm3::ThePlugin.GetInstance(),
            MAKEINTRESOURCE(IDD_PANEL),GetActiveWindow(),DialogProcedure,
            (LPARAM)this);

        if ( !piResult )
            return 1;
    }

    // convert the Max scene to a Wild Magic scene
    WSceneBuilder(acFilename,bExportSelected,&m_kEffectiveSettings,pkExport,
        pkMax,pkRootNode);

    // save the exporter's configuration state
    WriteConfiguration();
    
    return 1;
}
//----------------------------------------------------------------------------
void WSceneExport::ReadConfiguration ()
{
    // Restore the exporter's configuration state from disk.  This
    // configuration will correspond to the state in which the user left the
    // user interface on the previous occasion the exporter was run.

    // If configuration file doesn't exist (e.g. is this is the first time
    // the exporter has been run), then default options will be used.
    FILE* pkConfig = fopen(m_kStrConfigFile,"rb");
    if ( pkConfig  == NULL ) 
        return;

    // read the configuration file version number
    int iVersion;
    if ( fread(&iVersion,sizeof(int),1,pkConfig) != 1 )
    {
        // corrupt configuration file
        assert( false );
        fclose(pkConfig);
        return;
    }

    // verify that the exporter and configuration file versions match
    if ( iVersion != WSCENE_VERSION )
    {
        // version mismatch
        assert( false );
        fclose(pkConfig);
        return;
    }

    // read the configuration settings
    WExportSettings kSettings;
    if ( fread(&kSettings,sizeof(WExportSettings),1,pkConfig) != 1 )
    {
        // corrupt configuration file
        assert( false );
        fclose(pkConfig);
        return;
    }
    
    fclose(pkConfig);

    // Only override the default settings if the configuration file seems to
    // be valid.
    memcpy(&m_kSettings,&kSettings,sizeof(WExportSettings));
}
//----------------------------------------------------------------------------
void WSceneExport::WriteConfiguration ()
{
    FILE* pkConfig = fopen(m_kStrConfigFile,"wb");
    if ( pkConfig == NULL )
    {
        // cannot create configuration file (write-protected directory?)
        assert( false );
        return;
    }

    // write version number
    int iVersion = WSCENE_VERSION;
    if ( fwrite(&iVersion,sizeof(int),1,pkConfig) != 1 )
    {
        // cannot write to configuration file
        assert( false );
        fclose(pkConfig);
        return;
    }

    // write exporter configuration
    if ( fwrite(&m_kSettings,sizeof(WExportSettings),1,pkConfig) != 1 )
    {
        // cannot write to configuration file
        assert( false );
        fclose(pkConfig);
        return;
    }
    
    fclose(pkConfig);
}
//----------------------------------------------------------------------------
BOOL CALLBACK WSceneExport::DialogProcedure (HWND hWnd, UINT uiMsg,
    WPARAM wParam, LPARAM lParam) 
{
    if ( uiMsg == WM_INITDIALOG )
    {
        // When the dialog is initialized, MAX passes a "this" pointer to
        // the current instance of the exporter in the message's LPARAM.
        // Save this for later calls to DialogProcedure.
        SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam);

        // Call the per-instance dialog initialization method
        return ((WSceneExport*)lParam)->OnInitDialog(hWnd);
    }

    WSceneExport* pkExporter = (WSceneExport*)GetWindowLongPtr(hWnd,
        GWLP_USERDATA); 

    switch ( uiMsg )
    {
    case WM_COMMAND:
        // The user has altered the state of a user interface element.
        // Determine what has changed and take the appropriate action.
        switch ( LOWORD(wParam) ) 
        {
        case IDC_CHECK_MESHES:
            return pkExporter->OnMeshChecked(hWnd,
                IsDlgButtonChecked(hWnd,IDC_CHECK_MESHES));
        case IDC_CHECK_OBJECTS:
            return pkExporter->OnObjectsChecked(hWnd,
                IsDlgButtonChecked(hWnd,IDC_CHECK_OBJECTS));
        case IDC_CHECK_MODIFIERS:
            return pkExporter->OnModifiersChecked(hWnd,
                IsDlgButtonChecked(hWnd,IDC_CHECK_MODIFIERS));
        case IDC_RADIO_ALLFRAMES:
        case IDC_RADIO_KEYFRAMES:
            return pkExporter->OnEnableFrameSpinners(hWnd,TRUE);
        case IDC_RADIO_CURRENTFRAME:
            return pkExporter->OnEnableFrameSpinners(hWnd,FALSE);
        case IDOK:
            pkExporter->OnOK(hWnd);
            EndDialog(hWnd,1);
            break;
        case IDCANCEL:
            EndDialog(hWnd,0);
            break;
        default:
            return FALSE;
        }
        break;

    case CC_SPINNER_CHANGE:
        // The user has changed the state of a spinner control.
        switch( LOWORD(wParam) )
        {
        case IDC_SPINNER_ENDFRAME:
            return pkExporter->OnEndFrameSpinnerChanged(hWnd);
        case IDC_SPINNER_STARTFRAME:
            return pkExporter->OnStartFrameSpinnerChanged(hWnd);
        }
        break;

    case WM_CLOSE:
        EndDialog(hWnd,0);
        return TRUE;
    }

    return FALSE;
}
//----------------------------------------------------------------------------
BOOL WSceneExport::OnEnableFrameSpinners (HWND hWnd, BOOL bEnabled)
{
    // Enable or disable the frame spinner controls.
    ISpinnerControl* pkSpinner = GetISpinner(GetDlgItem(hWnd,
        IDC_SPINNER_STARTFRAME)); 
    pkSpinner->Enable(bEnabled);
    ReleaseISpinner(pkSpinner); 
    pkSpinner = GetISpinner(GetDlgItem(hWnd,IDC_SPINNER_ENDFRAME)); 
    pkSpinner->Enable(bEnabled);
    return TRUE;
}
//----------------------------------------------------------------------------
BOOL WSceneExport::OnEndFrameSpinnerChanged (HWND hWnd)
{
    // Called when user modifies the spinner control corresponding to the
    // end frame of the animation to be exported.

    // get the start frame from the start frame spinner control
    ISpinnerControl* pkSpinner = GetISpinner(GetDlgItem(hWnd,
        IDC_SPINNER_STARTFRAME)); 
    int iStartFrame = pkSpinner->GetIVal();
    ReleaseISpinner(pkSpinner);

    // Ensure that the end frame spinner doesn't display a value less than
    // the start frame.
    pkSpinner = GetISpinner(GetDlgItem(hWnd,IDC_SPINNER_ENDFRAME)); 
    int iEndFrame = pkSpinner->GetIVal();
    if ( iStartFrame > iEndFrame )
        pkSpinner->SetValue(iStartFrame,FALSE);

    ReleaseISpinner(pkSpinner); 
    return TRUE;
}
//----------------------------------------------------------------------------
BOOL WSceneExport::OnInitDialog (HWND hWnd)
{
    // Perform initialization when the dialog gets a WM_INITDIALOG message.

    CenterWindow(hWnd,GetParent(hWnd)); 

    // Set the user interface state.  If a configuration file exists, then
    // the state will correspond to the settings the user chose, the last
    // time the exporter was used.  Otherwise, default settings will be used.
    CheckDlgButton(hWnd,IDC_CHECK_OBJECTS,m_kSettings.IncludeObjects);
    CheckDlgButton(hWnd,IDC_CHECK_LIGHTS,m_kSettings.IncludeLights);
    CheckDlgButton(hWnd,IDC_CHECK_MESHES,m_kSettings.IncludeMeshes);
    CheckDlgButton(hWnd,IDC_CHECK_CAMERAS,m_kSettings.IncludeCameras);
    CheckDlgButton(hWnd,IDC_CHECK_NORMALS,m_kSettings.IncludeNormals);       
    CheckDlgButton(hWnd,IDC_CHECK_MATERIALS,m_kSettings.IncludeMaterials);
    CheckDlgButton(hWnd,IDC_CHECK_VERTEXCOLS,m_kSettings.IncludeVertexColors);
    CheckDlgButton(hWnd,IDC_CHECK_TEXCOORDS,m_kSettings.IncludeTexCoords);
    CheckDlgButton(hWnd,IDC_CHECK_GENERATEMAPS,m_kSettings.GenerateMaps);
    CheckDlgButton(hWnd,IDC_CHECK_MODIFIERS,m_kSettings.IncludeModifiers);
    CheckDlgButton(hWnd,IDC_CHECK_SKINS,m_kSettings.IncludeSkins);
    CheckDlgButton(hWnd,IDC_RADIO_CURRENTFRAME,m_kSettings.IncludeCurrentFrame);
    CheckDlgButton(hWnd,IDC_RADIO_KEYFRAMES,m_kSettings.IncludeKeyFrames);
    CheckDlgButton(hWnd,IDC_RADIO_ALLFRAMES,m_kSettings.IncludeAllFrames);
    CheckDlgButton(hWnd,IDC_CHECK_USELOCALTIME,m_kSettings.UseLocalTime);

    if( !m_kSettings.IncludeObjects )
    {
        // disable sub-options if the "Objects" check box is cleared
        OnObjectsChecked(hWnd,FALSE);
    }
    else if ( !m_kSettings.IncludeMeshes )
    {
        // disable sub-options if the "Mesh" check box is cleared
        OnMeshChecked(hWnd,FALSE);
    }

    // disable sub-options if the "Modifiers" check box is cleared
    if ( !m_kSettings.IncludeModifiers )
        OnModifiersChecked(hWnd, FALSE);

    // get the number of animation frames in the MAX scene
    int iNumFrames = m_pkMax->GetAnimRange().Duration()/GetTicksPerFrame();

    // set up frame spinner controls
    ISpinnerControl* pkSpinner = GetISpinner(GetDlgItem(hWnd,
        IDC_SPINNER_STARTFRAME));
    pkSpinner->LinkToEdit(GetDlgItem(hWnd,IDC_EDIT_STARTFRAME),EDITTYPE_INT);
    pkSpinner->SetLimits(0,iNumFrames,TRUE);
    pkSpinner->SetScale(1.0f);
    pkSpinner->SetValue(0,FALSE);
    if ( m_kSettings.IncludeCurrentFrame )
    {
        pkSpinner->Enable(FALSE);
    }
    ReleaseISpinner(pkSpinner);

    pkSpinner = GetISpinner(GetDlgItem(hWnd,IDC_SPINNER_ENDFRAME));
    pkSpinner->LinkToEdit(GetDlgItem(hWnd,IDC_EDIT_ENDFRAME),EDITTYPE_INT);
    pkSpinner->SetLimits(0,iNumFrames,TRUE);
    pkSpinner->SetScale(1.0f);
    pkSpinner->SetValue(iNumFrames,FALSE);
    if ( m_kSettings.IncludeCurrentFrame )
    {
        pkSpinner->Enable(FALSE);
    }
    ReleaseISpinner(pkSpinner);

    return TRUE;
}
//----------------------------------------------------------------------------
BOOL WSceneExport::OnMeshChecked (HWND hWnd, BOOL bEnabled)
{
    // Called when the "Mesh" checkbox has been set or cleared.  Enable or
    // disable all "Mesh" suboptions.
    EnableWindow(GetDlgItem(hWnd,IDC_CHECK_NORMALS),bEnabled);
    EnableWindow(GetDlgItem(hWnd,IDC_CHECK_VERTEXCOLS),bEnabled);
    EnableWindow(GetDlgItem(hWnd,IDC_CHECK_TEXCOORDS),bEnabled);
    EnableWindow(GetDlgItem(hWnd,IDC_CHECK_MATERIALS),bEnabled);
    EnableWindow(GetDlgItem(hWnd,IDC_CHECK_GENERATEMAPS),bEnabled);
    return TRUE;
}
//----------------------------------------------------------------------------
BOOL WSceneExport::OnModifiersChecked (HWND hWnd, BOOL bEnabled)
{
    // Called when the "Modifiers" checkbox has been set or cleared.  Enable
    // or disable all "Modifiers" suboptions.
    EnableWindow(GetDlgItem(hWnd,IDC_CHECK_SKINS),bEnabled);
    return TRUE;
}
//----------------------------------------------------------------------------
BOOL WSceneExport::OnObjectsChecked (HWND hWnd, BOOL bEnabled)
{
    // Called when the "Objects" checkbox has been set or cleared.  Enable or
    // disable all suboptions
    EnableWindow(GetDlgItem(hWnd,IDC_CHECK_MESHES),bEnabled);
    EnableWindow(GetDlgItem(hWnd,IDC_CHECK_CAMERAS),bEnabled);
    EnableWindow(GetDlgItem(hWnd,IDC_CHECK_LIGHTS),bEnabled);   

    // The mesh check box and its suboptions are themselves suboptions of
    // the "Objects" group.  Propagate the change to them.
    OnMeshChecked(hWnd,bEnabled && IsDlgButtonChecked(hWnd,IDC_CHECK_MESHES));
    return TRUE;
}
//----------------------------------------------------------------------------
void WSceneExport::OnOK (HWND hWnd)
{
    // The user has clicked the dialog's OK button.  Get the exporter options
    // selected by the user prior to performing the export.

    m_kSettings.IncludeObjects =
        IsDlgButtonChecked(hWnd,IDC_CHECK_OBJECTS);
    m_kSettings.IncludeLights =
        IsDlgButtonChecked(hWnd,IDC_CHECK_LIGHTS);
    m_kSettings.IncludeCameras =
        IsDlgButtonChecked(hWnd,IDC_CHECK_CAMERAS);
    m_kSettings.IncludeMeshes =
        IsDlgButtonChecked(hWnd,IDC_CHECK_MESHES);
    m_kSettings.IncludeNormals =
        IsDlgButtonChecked(hWnd,IDC_CHECK_NORMALS);
    m_kSettings.IncludeVertexColors =
        IsDlgButtonChecked(hWnd,IDC_CHECK_VERTEXCOLS);
    m_kSettings.IncludeTexCoords =
        IsDlgButtonChecked(hWnd,IDC_CHECK_TEXCOORDS);
    m_kSettings.IncludeMaterials =
        IsDlgButtonChecked(hWnd,IDC_CHECK_MATERIALS);
    m_kSettings.GenerateMaps =
        IsDlgButtonChecked(hWnd,IDC_CHECK_GENERATEMAPS);
    m_kSettings.IncludeModifiers =
        IsDlgButtonChecked(hWnd,IDC_CHECK_MODIFIERS);
    m_kSettings.IncludeSkins =
        IsDlgButtonChecked(hWnd,IDC_CHECK_SKINS);
    m_kSettings.IncludeAllFrames =
        IsDlgButtonChecked(hWnd,IDC_RADIO_ALLFRAMES);
    m_kSettings.IncludeKeyFrames =
        IsDlgButtonChecked(hWnd,IDC_RADIO_KEYFRAMES);
    m_kSettings.IncludeCurrentFrame =
        IsDlgButtonChecked(hWnd,IDC_RADIO_CURRENTFRAME);
    m_kSettings.UseLocalTime =
        IsDlgButtonChecked(hWnd,IDC_CHECK_USELOCALTIME);

    // get start and end frames from the relevant spinner controls
    if ( m_kSettings.IncludeAllFrames || m_kSettings.IncludeKeyFrames )
    {
        ISpinnerControl* pkSpinner = GetISpinner(GetDlgItem(hWnd,
            IDC_SPINNER_STARTFRAME)); 
        m_kSettings.StartFrame = pkSpinner->GetIVal();
        ReleaseISpinner(pkSpinner);

        pkSpinner = GetISpinner(GetDlgItem(hWnd,IDC_SPINNER_ENDFRAME)); 
        m_kSettings.EndFrame = pkSpinner->GetIVal();
        ReleaseISpinner(pkSpinner);
    }

    // If the user has unchecked the Objects, Mesh, or Modifiers boxes, then
    // the suboptions of those categories will be disabled on the user
    // interface, but they may still be checked, which means that the
    // corresponding option in m_kSettings will be set to TRUE.  We need to
    // determine the list of "effective" settings, which takes into account
    // suboptions which have been disabled on the UI.
    m_kEffectiveSettings = m_kSettings;
    if ( !m_kSettings.IncludeObjects )
    {
        m_kEffectiveSettings.IncludeCameras = FALSE;
        m_kEffectiveSettings.IncludeLights = FALSE;
        m_kEffectiveSettings.IncludeMeshes = FALSE;
    }
    if ( !m_kEffectiveSettings.IncludeMeshes )
    {
        m_kEffectiveSettings.IncludeNormals = FALSE;
        m_kEffectiveSettings.IncludeMaterials = FALSE;
        m_kEffectiveSettings.IncludeVertexColors = FALSE;
        m_kEffectiveSettings.IncludeTexCoords = FALSE;
        m_kEffectiveSettings.GenerateMaps = FALSE;
    }
    if ( !m_kSettings.IncludeModifiers )
    {
        m_kSettings.IncludeSkins = FALSE;
    }
}
//----------------------------------------------------------------------------
BOOL WSceneExport::OnStartFrameSpinnerChanged (HWND hWnd)
{
    // Called when the user modifies the spinner control corresponding to the
    // starting frame of the animation to be exported.

    // get the start frame from the spinner control
    ISpinnerControl* pkSpinner = GetISpinner(GetDlgItem(hWnd,
        IDC_SPINNER_STARTFRAME)); 
    int iStartFrame = pkSpinner->GetIVal();
    ReleaseISpinner(pkSpinner);

    // Get the end frame from the end frame spinner control and ensure the
    // end frame is not less than the start frame.
    pkSpinner = GetISpinner(GetDlgItem(hWnd,IDC_SPINNER_ENDFRAME)); 
    int iEndFrame = pkSpinner->GetIVal();
    if ( iEndFrame < iStartFrame )
        pkSpinner->SetValue(iStartFrame,FALSE);
    ReleaseISpinner(pkSpinner); 

    return TRUE;
}
//----------------------------------------------------------------------------
