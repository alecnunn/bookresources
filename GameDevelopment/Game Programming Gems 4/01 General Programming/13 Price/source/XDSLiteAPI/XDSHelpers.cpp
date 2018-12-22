// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include "XDSinternal.h"
#include <string.h>

// ****************************************************************************
// *** 
// *** Internal support functions
// *** 
// ****************************************************************************


char *xdsStringNDup(const char *szTagName, int iCount)
{
	char *psz = (char *)XDS_ALLOC(NULL, iCount+1, XDS_MEMTYPE_SCRATCH);
	strncpy(psz, szTagName, iCount);
	psz[iCount] = '\0';
	return psz;
}
char *xdsStringDup(const char *szTagName)
{
	if(szTagName)
		return xdsStringNDup(szTagName, strlen(szTagName));
	else
		return NULL;
}

xdsType_t *xdsFindType(struct xdsHandle *hXds, u16 iType)
{
#ifdef XDS_SUPPORT_COMPACTDSD
	if(iType < XDS_FIRST_DEFID || iType >= hXds->iNextId)
		return NULL;
	return &hXds->pTypeDb[iType - XDS_FIRST_DEFID];
#else
	return (struct xdsType_t *)hXds->pTypeDb->Find((unsigned char*)&iType, sizeof(iType));
#endif
}
