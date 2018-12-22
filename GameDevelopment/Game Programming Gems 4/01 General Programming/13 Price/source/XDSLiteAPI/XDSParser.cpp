// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include "string.h"
#include "XDSinternal.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Forward Declarations
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static bool xdsDoHeader(struct xdsHandle *hXds);


// ****************************************************************************
// *** 
// *** Public Interface
// *** 
// ****************************************************************************

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Stream Initialization & shutdown
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct testst_t { u8 test; u32 test1;};
struct xdsHandle *xdsInit(void *hFile, const char *szStreamName, const unsigned char *pDSD, char mode)
{
	// validate compiler/host specific compilation parameters
	u32 testbo = 1;
	if((XDS_CONFIG_BYTEORDER == 'l' && testbo != *(u8*)&testbo) || (XDS_CONFIG_BYTEORDER == 'b' && testbo == *(u8*)&testbo))
		XDS_ERROR("Badly configured byte order");
	struct testst_t testst;
	if(XDS_CONFIG_ALIGN != ((u8*)&testst.test1 - (u8*)&testst))
		XDS_ERROR("Badly configured structure alignment size");
	if(XDS_CONFIG_TAGSIZE != 1)
		XDS_ERROR("Configured tag size is not supported");

#ifdef XDS_SUPPORT_WRITING
	if(mode != 'r' && mode != 'w')
#else
	if(mode != 'r')
#endif
		XDS_ERROR("Invalid read/write mode");


	struct xdsHandle *hXds = new struct xdsHandle;
	hXds->szStreamName  = xdsStringDup(szStreamName);
	hXds->iNextId       = XDS_FIRST_DEFID;
	hXds->bEndOfStream  = false;

	hXds->xBuf.SetFile(hFile);

#ifdef XDS_SUPPORT_COMPACTDSD
	hXds->pTypeDb = (struct xdsType_t *)XDS_ALLOC(NULL, sizeof(struct xdsType_t)*XDS_CONFIG_NUMIDS, XDS_MEMTYPE_SCRATCH);
	hXds->iLastAllocId = XDS_CONFIG_NUMIDS;
#else
	hXds->pTypeDb       = new CIdTrie;
#endif
	if(!hXds->pTypeDb)
	{
		delete hXds;
		return NULL;
	}

#ifdef XDS_SUPPORT_SIGNATURE
	hXds->lDSDLength = hXds->lDSDCRC = 0;
#endif

#ifdef XDS_SUPPORT_XML
	hXds->bNative       = true;    // will be reset in xdsDoHeader()
	hXds->pTypeDbByName = new CNameTrie;
#endif

	if(pDSD)
	{
#ifdef XDS_SUPPORT_WRITING
		hXds->bReading      = true;	// prevent DSD from being written to new stream
#endif
		CxdsBuffer dsdBuf(pDSD);
		if(!xdsProcessDSD(hXds, &dsdBuf))
		{
			xdsFini(hXds);
			return NULL;
		}
#ifdef XDS_SUPPORT_SIGNATURE
		// calculate signature
		hXds->lDSDLength = dsdBuf.Cursor() - dsdBuf.Data();
		hXds->lDSDCRC    = xdsCRC(dsdBuf.Data(), hXds->lDSDLength);
#endif
	}

#ifdef XDS_SUPPORT_WRITING
	hXds->bReading   = (bool)(mode == 'r');
	hXds->iRecNesting   = 0;
#endif

#ifdef XDS_MAKESCHEMA
	if(!hXds->bReading)	// prevent attempt to parse non-existent file in xdsMakeSchema
#endif

	if(!xdsDoHeader(hXds))
	{
		xdsFini(hXds);
		return NULL;
	}

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading && szStreamName && *szStreamName)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_STREAMNAME);
		hXds->xBuf.WriteShort(strlen(szStreamName) + XDS_CONFIG_TAGSIZE);
		hXds->xBuf.WriteTag(szStreamName, strlen(szStreamName));
	}
#endif
	return hXds;
};

#ifdef XDS_SUPPORT_WRITING
bool xdsEndStream(struct xdsHandle *hXds)
{
	if(!hXds->bEndOfStream && !hXds->bReading)
	{
		(void)xdsEndRecord(hXds);
		hXds->xBuf.WriteShort(XDS_RECORD_ENDOFDATA);
		hXds->xBuf.Flush();

		hXds->bEndOfStream = true;
		return true;
	}
	else
		return false;
}
#endif

bool xdsFini(struct xdsHandle *hXds)
{
#ifdef XDS_SUPPORT_WRITING
	(void)xdsEndStream(hXds);
#endif
	XDS_FREE((void*)hXds->szStreamName);
#ifndef XDS_SUPPORT_COMPACTDSD
	delete hXds->pTypeDb;
#else
	XDS_FREE(hXds->pTypeDb);
#endif
#ifdef XDS_SUPPORT_XML
	delete hXds->pTypeDbByName;
#endif
	XDS_FREE(hXds);
	return true;
}

const char *xdsGetStreamName(struct xdsHandle *hXds)
{
	return hXds->szStreamName;
}

#ifndef XDS_MAKESCHEMA
bool xdsRestart(struct xdsHandle *hXds, void *hFile, bool bReading)
{
#ifdef XDS_SUPPORT_WRITING
	(void)xdsEndStream(hXds);
	hXds->bReading = bReading;
	hXds->iRecNesting   = 0;
#endif
	hXds->bEndOfStream  = false;

	hXds->xBuf.SetFile(hFile);
	hXds->xBuf.Erase();

	if(!xdsDoHeader(hXds))
		return false;

	return true;
}
#endif


// ****************************************************************************
// *** 
// *** Internal support functions
// *** 
// ****************************************************************************

static bool xdsDoHeader(struct xdsHandle *hXds)
{
#ifdef XDS_SUPPORT_WRITING
	if(hXds->bReading)
	{
#endif
#ifndef XDS_MAKESCHEMA
		// validate & skip header
		if(hXds->xBuf.MatchString("XDS!0300", 8))
		{
			hXds->xBuf.Skip(8);

#ifdef XDS_SUPPORT_XML
			hXds->bNative = true;
#endif

			if(hXds->xBuf.ReadLong() != xdsFOURCC(XDS_CONFIG_BYTEORDER, XDS_CONFIG_ALIGN, XDS_CONFIG_TAGSIZE, 0))
				return false;
		}
#ifdef XDS_SUPPORT_XML
		else
		{
			hXds->xBuf.SkipWhite();
			if(!hXds->xBuf.MatchString("<?xml", 5))
				return false;

			hXds->bNative = false;

			if(!xdsXmlReadHeader(hXds))
				return false;
		}
#else
		else
		{
			XDS_ERROR("Bad magic number in XDS data");
			return false;
		}
#endif
#endif
#ifdef XDS_SUPPORT_WRITING
	}
	else
	{
		// write XDS header
		char bufHeader[12];
		*(u32 *)&bufHeader[0] = xdsFOURCC('X', 'D', 'S', '!');
		*(u32 *)&bufHeader[4] = xdsFOURCC('0', '3', '0', '0');
		*(u32 *)&bufHeader[8] = xdsFOURCC(XDS_CONFIG_BYTEORDER, XDS_CONFIG_ALIGN, XDS_CONFIG_TAGSIZE, 0);
		hXds->xBuf.Write(&bufHeader[0], 3*sizeof(u32));
	}
#endif

	return true;
}
