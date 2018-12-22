/**********************************************************************
 	MODULE: MRCexport
	
	DESCRIPTION: This module is the plugin for MAX 3 to export in MRC format
	
	Version:		1.22			

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#pragma once

#include "std.h"

//====================================================================================
#define MRC_EXPORT_VERSION			122				//version 1.22
//====================================================================================
#define MRCEXPORT_CLASS_ID			Class_ID(0x8b845a2, 0x3c290572)
//====================================================================================

//////////////////////////////////////////////////////////////////////////////////////
extern HINSTANCE hInstance;
//////////////////////////////////////////////////////////////////////////////////////



//====================================================================================
inline TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}
//====================================================================================



//====================================================================================
//====================================================================================
class MRCexport : public SceneExport {

	Interface *i;
	ExpInterface *ei;
	ULONG sampleDelta;

public:
	int				ExtCount()	{return 1;}			// Number of extensions supported
	const TCHAR *	Ext(int n)	{return _T("MRC");}	// Extension("MRC")
	const TCHAR *	LongDesc()						// Long ASCII description ("MaRCo Tombesi File format")
						{ return GetString(IDS_LIBDESCRIPTION); }
	const TCHAR *	ShortDesc()	 					// Short ASCII description ("MRC Export")
						{ return GetString(IDS_CLASS_NAME); }
	const TCHAR *	AuthorName()					// ASCII Author name TOMBESI MARCO
						{ return _T("MARCO TOMBESI"); }
	const TCHAR *	CopyrightMessage()				// ASCII Copyright message
						{ return GetString(IDS_COPYRIGHT); }
	const TCHAR *	OtherMessage1()					// Other message #1
						{ return GetString(IDS_CONTACT1); }
	const TCHAR *	OtherMessage2()					// Other message #2
						{ return GetString(IDS_CONTACT2); }
	unsigned int	Version()						// Version number * 100 
						{ return MRC_EXPORT_VERSION; }
	
	// Show DLL's "About..." box
	void			ShowAbout(HWND hWnd);			
	
	// Export to an MRC file
	int				DoExport(const TCHAR *name,
						ExpInterface *ei,
						Interface *i, 
						BOOL suppressPrompts=FALSE, 
						DWORD options=0);
	
	void			setSampleDelta (ULONG delta) 
						{ sampleDelta=delta; }

	// Main write function
	bool			Write( FILE *out);
	//Constructor/Destructor
	MRCexport();
	~MRCexport();		
	void			DeleteThis()					// Deallocation
						{ delete this; }
};
//====================================================================================
//====================================================================================



//====================================================================================
class MRCexportClassDesc : public ClassDesc2 
{
public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new MRCexport();}
	const TCHAR *	ClassName() {return GetString(IDS_CLASS_NAME);}
	SClass_ID		SuperClassID() {return SCENE_EXPORT_CLASS_ID;}
	Class_ID		ClassID() {return MRCEXPORT_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(IDS_CATEGORY);}
	const TCHAR*	InternalName() { return _T("MRCexport"); }		// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};
//====================================================================================
