/**********************************************************************
 	MODULE: MRCexport
	
	DESCRIPTION: This module is the plugin for MAX 3 to export in MRC format
	
	Version:		1.22			

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#include "MRCexport.h"
#include "MRCformat2.h"
#include "MRChelper.h"


//====================================================================================
BOOL CALLBACK MRCexportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MRCAboutDialogFunc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
static DWORD WINAPI fn(LPVOID arg) { return(0); }
//====================================================================================


//====================================================================================
//--- MRCexport -------------------------------------------------------
MRCexport::MRCexport()
{
	i=NULL;
	ei=NULL;
	sampleDelta=100;
}

MRCexport::~MRCexport() 
{

}

void MRCexport::ShowAbout(HWND hWnd)
{			
	DialogBox(hInstance,  
		MAKEINTRESOURCE(IDD_ABOUT),
		hWnd, 
		MRCAboutDialogFunc
		);

}

int MRCexport::DoExport(const TCHAR *name,
						ExpInterface *ei,
						Interface *i,
						BOOL suppressPrompts, DWORD options)
{
	//===================================================
	//					START DIALOG				   //
	if	(!DialogBoxParam
			(
			hInstance, 
			MAKEINTRESOURCE(IDD_PANEL), 
			GetActiveWindow(), 
			MRCexportOptionsDlgProc, 
			(LPARAM)this
			)
		)	return FALSE;
	//===================================================

	FILE *out = fopen(name, "wb");
	if(!out)
	{
		MessageBox(NULL, "Couldn't open file for writing.", NULL, MB_OK);
		return 0;
	}
	
	this->ei = ei;
	this->i = i;

	i->ProgressStart(_T("Exporting to MRC File: ..."), TRUE, fn, NULL);
	
	
	Write(out);	
	

	// Implement the actual file export here and 
	// return TRUE If the file is imported properly

	
	i->ProgressEnd();

	if (out) fclose(out);

	return TRUE;
}

// ============================================================================
bool MRCexport::Write(FILE *out) {
	rewind (out);
	//write format version info
	fwrite(&mrc_version, sizeof(mrc_version), 1, out);

	bool ok=false;
	long startPos = ftell(out);
	fseek(out, sizeof(MRCobject_hdr), SEEK_CUR);

	MRCobject_hdr oHdr;
	memset(&oHdr, 0, sizeof(MRCobject_hdr));

	if (!i->GetSelNodeCount()) {
		MessageBox(NULL,"You must select a mesh to export",NULL,MB_OK);
		return false;
	}
	INode* pNode = i->GetSelNode(0);
	INode* const pRoot = i->GetRootNode();	

	i->ProgressUpdate(0,FALSE,_T("..writing MESH data..."));

	// writing Mesh data to file
		ok = ExportMesh(pNode, out);
	
	oHdr.boneCnt = CountBones(pRoot);
	// Not only a mesh
	if (oHdr.boneCnt) 
	{	
		//Bone data
		i->ProgressUpdate(15,FALSE,_T("..Processing BONE data..."));
////
			oHdr.boneOfs = ftell(out);		
////
		BoneData_t* boneData = new BoneData_t [ oHdr.boneCnt ];
		
			ok = ProcessBoneWeights (pNode,pRoot, boneData);
			
		i->ProgressUpdate(20,FALSE,_T(".....Skel struct..."));
		
			ProcessBoneStruct (pRoot, pRoot, -1, boneData);
	
		i->ProgressUpdate(30,FALSE,_T(".....ANIM data..."));
		
			Interval range = i->GetAnimRange();
			oHdr.animLast  = TicksToMilliSec(range.Duration());
			oHdr.animFPS = GetFrameRate();

			oHdr.keyCnt = ProcessBoneAnim (pRoot, range, sampleDelta, boneData);
		
		i->ProgressUpdate(50,FALSE,_T("..writing BONE data..."));

		// writing Bones to file
			ok = ExportBoneData(boneData, oHdr.boneCnt, oHdr.keyCnt, out);

		// bone data deallocation
		if (boneData) delete [] boneData;
	}

	long endPos = ftell(out);
	fseek(out, startPos, SEEK_SET);
	fwrite(&oHdr, sizeof(MRCobject_hdr), 1, out);
	fseek(out, endPos, SEEK_SET);
	
	return ok;
}
// ============================================================================
//--- MRCexport -------------------------------------------------------
//====================================================================================	


//====================================================================================	
//==============================DIALOGUES=============================================	
//====================================================================================	
BOOL CALLBACK MRCexportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static MRCexport *exp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			exp = (MRCexport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return FALSE;

		case WM_CLOSE:
			delete exp;
			EndDialog(hWnd, 0);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) 
			{
				case IDC_BUTTON_ABOUT:
					exp->ShowAbout(hWnd);
					break;
				
				case IDC_BUTTON_OK:
					/// set export sample rate
					{
						BOOL ok;
						UINT val = GetDlgItemInt(hWnd, IDC_EDIT, &ok, false);
						if (ok)
						{
							exp->setSampleDelta(val);
						}
					}

					EndDialog(hWnd, 1);
					break;
					
				case IDC_BUTTON_CANCEL:
					EndDialog(hWnd, 0);
					break;
			}
		
		default:		//message not catched
			return FALSE;
	}
	return TRUE;		//message catched
}

//====================================================================================	
BOOL CALLBACK MRCAboutDialogFunc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message) {
		case WM_INITDIALOG:
			SetDlgItemText(hWnd,IDC_ABOUT_TITLE ,
				GetString(IDS_TITLE));
			SetDlgItemText(hWnd,IDC_ABOUT_COPYRIGHT,
				GetString(IDS_COPYRIGHT));
			SetDlgItemText(hWnd,IDC_ABOUT_CONTACT1 ,
				GetString(IDS_CONTACT1));
			SetDlgItemText(hWnd,IDC_ABOUT_CONTACT2 ,
				GetString(IDS_CONTACT2));
			return FALSE;

		case WM_LBUTTONDOWN:
			EndDialog(hWnd, 0);
			return TRUE;
	}

	return FALSE;
}