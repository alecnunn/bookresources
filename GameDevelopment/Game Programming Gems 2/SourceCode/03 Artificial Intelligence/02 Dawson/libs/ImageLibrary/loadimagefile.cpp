#include "priv_precompiled.h"

/*
Copyright 2001 Bruce Dawson, Cygnus Software
For more information, visit http://www.cygnus-software.com/papers/
or e-mail: comments@cygnus-software.com

This code may be redistributed as long as this notice is kept intact.
*/

#include "csbitmap.h"

bool LoadImageFile(const char* FileName, CSBitmap* DestBitmap)
{
	// Yes, I know this is really crude. I should at least check the
	// extensions or something. Oh well...
	if (LoadBMPFile(FileName, DestBitmap))
		return true;

	//if (LoadTargaFile(FileName, DestBitmap))
	//	return true;

	if (LoadPCXFile(FileName, DestBitmap))
		return true;

	return false;
}
