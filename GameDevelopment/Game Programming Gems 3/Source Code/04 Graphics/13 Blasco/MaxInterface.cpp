/*============================================================================
	Title: MaxInterface.cpp
	Author: Oscar Blasco
	Description: Normal Map Generator For Max4
	Changes:
============================================================================*/

#include "NormalMapGen.h"
#include "resource.h"

/*----------------------------------------------------------------------------
	Definitions
----------------------------------------------------------------------------*/

#define TINORMALMAPGEN_CLASS_ID	Class_ID(0x57624e6e, 0x63a268ad)
#define IDS_RB_PRIMITIVES               30264


/*----------------------------------------------------------------------------
	Globals
----------------------------------------------------------------------------*/

extern HINSTANCE hInstance;
extern int controlsInit;

/** The Creator Plugin */
static TiNormalMapGen TheUtility;

Interface* TiNormalMapGen::ip = NULL;
IParamMap *TiNormalMapGen::pmapCreate = NULL;

/*----------------------------------------------------------------------------
	Classes
----------------------------------------------------------------------------*/

/**
 * Class descriptor
 */
class TiNormalMapGenClassDesc : public ClassDesc2{
	public:
	int 			IsPublic(){ return TRUE; }
	void *			Create(BOOL loading = FALSE){ return &TheUtility; }
	const TCHAR *	ClassName(){ return _T("NormalMap Generator"); }
	SClass_ID		SuperClassID(){ return UTILITY_CLASS_ID; }
	Class_ID		ClassID(){ return TINORMALMAPGEN_CLASS_ID; }
	const TCHAR* 	Category(){ return _T(""); }

	const TCHAR*	InternalName(){ return _T("NormalMapGen"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance(){ return hInstance; }				// returns owning module handle
	char*			GetRsrcString(long v){ return _T("NormalMapGen"); }

};

/* The static descriptor for the creator plugin */
static TiNormalMapGenClassDesc TiNormalMapGenDesc;

/** Returns the Creator Plugin descriptor */
ClassDesc2 * GetTiNormalMapGenrDesc(){ return &TiNormalMapGenDesc; }

DWORD WINAPI ThreadFunc( LPVOID lpParam ) 
{ 
    TheUtility.ComputeDisplacementMap();

    return 0; 
} 


/**
 * Class for the Pick Mode
 */
class PickControlNode : public PickModeCallback, public PickNodeCallback{
public:				
	TiNormalMapGen *mod;
	PickControlNode() {mod=NULL; pickinglow = false; }
	BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);		
	BOOL Pick(IObjParam *ip,ViewExp *vpt);		
	void EnterMode(IObjParam *ip);
	void ExitMode(IObjParam *ip);		
	BOOL Filter(INode *node);
	PickNodeCallback *GetFilter() {return this;}
	BOOL RightClick(IObjParam *ip,ViewExp *vpt) {return TRUE;}

	bool pickinglow;
};

/** The Pick Mode object */
static PickControlNode thePickMode;


BOOL PickControlNode::Filter(INode *node)
{
	return TRUE;
}


BOOL PickControlNode::HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
{	
	if(ip->PickNode(hWnd,m,this)){
		return TRUE;
	} else {
		return FALSE;
	}
}


class NullView : public View{
public:
	Point2 ViewToScreen(Point3 p){ return Point2(p.x,p.y); }

	NullView() {

		worldToView.IdentityMatrix();
		screenW=640.0f; screenH = 480.0f;
	}

};

extern bool CancelProgress;

/** 
 * This is the dialog message handling proc for the info window that appears while
 * the map is being generated
 */ 
BOOL CALLBACK ProgressDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message) { 
		case WM_INITDIALOG: 	
		 	break;
		case WM_DESTROY: 
			{
				TheUtility.hwnd_progress = NULL;
				
			break;
			}
		case WM_CLOSE:
			TheUtility.hwnd_progress = NULL;
			EndDialog(hDlg, 0);
			break;

		case WM_COMMAND: 
			{switch(LOWORD(wParam)){
					case IDC_CANCELBUTTON:
						CancelProgress = true;
						break;
					case IDC_LOWPRIORITYBUTTON:
						if(TheUtility.LowPriorityOn){
							TheUtility.LowPriorityOn = false;
							SetThreadPriority(TheUtility.hThread, THREAD_PRIORITY_HIGHEST);
						} else{
							TheUtility.LowPriorityOn = true;
							SetThreadPriority(TheUtility.hThread, THREAD_PRIORITY_LOWEST);
						}
						break;
					default:	 
						break; 
	
				}
			 break;
			}
		case WM_NOTIFY: // Others this way...
			{
				break;
			}
	
		default:
			return FALSE;
	}

	return TRUE;
}


/** 
 * Pick function, used to pick the two models in max 
 * Notice that the objects are not rotated nor scaled
 * (we don't multiply the vertices by the object's matrix)
 */
BOOL PickControlNode::Pick(IObjParam *ip,ViewExp *vpt)
{
	INode *node = vpt->GetClosestHit();
	if(node){
		NullView nullView;
		float time = 0.f;

		ObjectState os = node->EvalWorldState(ip->GetTime());
	
		int deleteIt;
		TriObject *triObject = TiNormalMapGen::GetTriObjectFromNode(node, deleteIt);

		if(!triObject) return FALSE;

		if(pickinglow){
			TheUtility.LowMesh = triObject->GetRenderMesh(ip->GetTime(), node, nullView, deleteIt);
			TheUtility.LowMesh->DeleteIsoMapVerts();
			TheUtility.LowMesh->InvalidateGeomCache();
		}
		else{
			TheUtility.HighMesh = triObject->GetRenderMesh(ip->GetTime(), node, nullView, deleteIt);
			TheUtility.HighMesh->DeleteIsoMapVerts();
			TheUtility.HighMesh->InvalidateGeomCache();
		}

		TCHAR *name = node->GetName();
		if(pickinglow)
			SetWindowText(GetDlgItem(mod->hParams, IDC_PICKLOWBUTTON), name);
		else SetWindowText(GetDlgItem(mod->hParams, IDC_PICKHIGHBUTTON), name);
	}

	ICustButton *iBut;
	if(pickinglow)
		iBut = GetICustButton(GetDlgItem(mod->hParams,IDC_PICKLOWBUTTON));
	else iBut = GetICustButton(GetDlgItem(mod->hParams,IDC_PICKHIGHBUTTON));
	if (iBut) iBut->SetCheck(FALSE);
	ReleaseICustButton(iBut);

	ip->PopCommandMode();

	// And if both models were picked start the process. To allow the user
	// to use Max while the normal map is being generated the process is
	// done in a thread. Take care because if max is closed the thread will be
	// stoped. Also it's not very recommended to use the max while the normal map
	// is generated.
	if(TheUtility.HighMesh && TheUtility.LowMesh && !TheUtility.hwnd_progress){
		TheUtility.hwnd_progress = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_PROGRESSWIN), 
							GetActiveWindow(), ProgressDlg);

		ShowWindow(TheUtility.hwnd_progress, 1);

		DWORD dwThreadId, dwThrdParam = 1;

		TheUtility.hThread = CreateThread( 
			NULL,                        // no security attributes 
			0,                           // use default stack size  
			ThreadFunc,                  // thread function 
			&dwThrdParam,                // argument to thread function 
			0,                           // use default creation flags 
			&dwThreadId);                // returns the thread identifier 

		SetThreadPriority(TheUtility.hThread, THREAD_PRIORITY_HIGHEST);

	}

	return FALSE;
}


void PickControlNode::EnterMode(IObjParam *ip)
{
	ICustButton *iBut;
	if(pickinglow)
		iBut = GetICustButton(GetDlgItem(mod->hParams,IDC_PICKLOWBUTTON));
	else iBut = GetICustButton(GetDlgItem(mod->hParams,IDC_PICKHIGHBUTTON));
	if (iBut) iBut->SetCheck(TRUE);
	ReleaseICustButton(iBut);
}


void PickControlNode::ExitMode(IObjParam *ip)
{
	
}


/**
 * For win messages in the plugin rollup page  
 */
BOOL CALLBACK NormalMapGenDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TiNormalMapGen *u = (TiNormalMapGen *)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	if (!u && message != WM_INITDIALOG ) return FALSE;

	switch (message) { // Respond to the message 
		case WM_INITDIALOG: // Initialize the Controls
			{
			u = (TiNormalMapGen *)lParam;
			SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG)u);
			u->OnInitDlg(hDlg);
		 	break;
			}
		case WM_DESTROY: // Release the Controls 
			{
			ReleaseICustButton(u->ccButtonPickLow);
			ReleaseICustButton(u->ccButtonPickHigh);
			ReleaseICustButton(u->ccButtonOnSelectedFaces);
			ReleaseICustButton(u->ccButtonNonContinousNormals);
			ReleaseICustButton(u->ccButtonTextureSpace);
			
			ReleaseICustEdit(u->ccEditMapSize);

			u->ip->SetStdCommandMode(CID_OBJMOVE);
			break;
			}
		case WM_COMMAND: // Various messages come in this way.
			{switch(LOWORD(wParam)){
				case IDC_ONSELECTEDFACESBUTTON: 
				{
					if(u->hwnd_progress){
						if(u->ccButtonOnSelectedFaces->IsChecked())
							u->ccButtonOnSelectedFaces->SetCheck(false);
						break;
					}
					if(u->OnSelectedFaces)
						u->OnSelectedFaces = false;
					else u->OnSelectedFaces = true;
				break; 
				}
				case IDC_NONCONTNORMALSBUTTON:
				{
					if(u->hwnd_progress){
						if(u->ccButtonNonContinousNormals->IsChecked())
							u->ccButtonNonContinousNormals->SetCheck(false);
						break;
					}
					if(!u->NonContinousNormals){
						u->NonContinousNormals = true;
					} else u->NonContinousNormals = false;
				break;
				}
				case IDC_TEXTURESPACEBUTTON:
				{
					if(u->hwnd_progress){
						if(u->ccButtonTextureSpace->IsChecked())
							u->ccButtonTextureSpace->SetCheck(false);
						break;
					}
					if(!u->InTextureSpace){
						u->InTextureSpace = true;
					} else u->InTextureSpace = false;
				break;
				}
				case IDC_PICKLOWBUTTON:
				{
					if(u->hwnd_progress){
						if(u->ccButtonPickLow->IsChecked())
							u->ccButtonPickLow->SetCheck(false);
						break;
					}
					if(u->ip->GetCommandMode() == (CommandMode *) &thePickMode){
						u->ip->SetStdCommandMode(CID_OBJMOVE);
					} else{
						thePickMode.mod  = u;			
						thePickMode.pickinglow = true;
						u->ip->SetPickMode(&thePickMode);
					} 
				break; 
				}
				case IDC_PICKHIGHBUTTON:
				{
					if(u->hwnd_progress){
						if(u->ccButtonPickHigh->IsChecked())
							u->ccButtonPickHigh->SetCheck(false);
						break;
					}
					if(u->ip->GetCommandMode() == (CommandMode *) &thePickMode){
						u->ip->SetStdCommandMode(CID_OBJMOVE);
					} else{
						thePickMode.mod  = u;		
						thePickMode.pickinglow = false;
						u->ip->SetPickMode(&thePickMode);
					} 
				break; 
				}

			}
			break; }
		case WM_NOTIFY:
			{
				int i = 0;
				break;
			}
		default:
			return FALSE;
	}

	return TRUE;
}


/** Create all controls for the rollup page */
void TiNormalMapGen::OnInitDlg(HWND hWnd){

	// The pick object button fo the low level detail model
	ccButtonPickLow = GetICustButton(GetDlgItem(hWnd, IDC_PICKLOWBUTTON));
	ccButtonPickLow->SetType(CBT_CHECK);

	// The pick object button fo the detailed model
	ccButtonPickHigh = GetICustButton(GetDlgItem(hWnd, IDC_PICKHIGHBUTTON));
	ccButtonPickHigh->SetType(CBT_CHECK);

	// Button to only use the selected faces of the low model
	ccButtonOnSelectedFaces = GetICustButton(GetDlgItem(hWnd, IDC_ONSELECTEDFACESBUTTON));
	ccButtonOnSelectedFaces->SetType(CBT_CHECK);

	NonContinousNormals = true;
	OnSelectedFaces = false;
	InTextureSpace = true;

	// If not called, the normals of the vertices with two or more texture coordinates will
	// not share the same normal
	ccButtonNonContinousNormals = GetICustButton(GetDlgItem(hWnd, IDC_FILTERMAPBUTTON));
	ccButtonNonContinousNormals->SetType(CBT_CHECK);

	// Output the normal map in object space or in texture space
	ccButtonTextureSpace = GetICustButton(GetDlgItem(hWnd, IDC_TEXTURESPACEBUTTON));
	ccButtonTextureSpace->SetType(CBT_CHECK);
	ccButtonTextureSpace->SetCheck(true);
	
	// The edit control for the scale
	ccEditMapSize = GetICustEdit(GetDlgItem(hWnd, IDC_MAPSIZEEDIT));
	ccEditMapSize->SetText(_T("128"));
}


/** Called when we enter into the creator object rollup page */
void TiNormalMapGen::BeginEditParams(Interface *ip,IUtil *iu)
{
	this->ip = ip;
	
	// Add the rollup page
	hParams = ip->AddRollupPage(
		hInstance, // DLL instance handle.
		MAKEINTRESOURCE(IDD_NORMALMAPROLLUP), // ID of the dialog box.
		NormalMapGenDialogProc, // Dialog procedure to process user input.
		"Normal Map Generator", // Rollup title text.
		(LPARAM)this); // Saves the this ptr of the item.

}
		

/** Called when the user quits the rollup page */
void TiNormalMapGen::EndEditParams(Interface *ip,IUtil *iu)
{		
	ip->DeleteRollupPage(hParams);
	hParams = NULL;

	this->ip = NULL;	
}