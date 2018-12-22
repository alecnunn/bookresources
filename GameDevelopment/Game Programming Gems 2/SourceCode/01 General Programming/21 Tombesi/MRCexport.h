/* Copyright (C) Marco Tombesi, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Marco Tombesi, 2001"
 */
class MRCexport : public SceneExport {
public:
	// Number of extensions supported
	int ExtCount()  {return 1;}

	// Extension("MRC")
	const TCHAR * Ext(int n)  {return _T("MRC");}
	...

	// Export to an MRC file
	int DoExport(const TCHAR *name,
	             ExpInterface *ei,
	             Interface *i, 
	             BOOL suppressPrompts=FALSE, 
	             DWORD options=0);		

	//Constructor/Destructor
	MRCexport();
	~MRCexport();		
	...
};
