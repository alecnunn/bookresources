// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#ifndef XDSCONFIG_INCL
#define XDSCONFIG_INCL

// ****************************************************************************
// *** 
// *** Configuration Options
// *** 
// ****************************************************************************

// The following configuration #defines must match the host on which the
// XDS-lite API is to be run.
//
// Note: The only TAGSIZE that is supported by the current XDS-lite library
//       implementation is 1 (up to 255 character tag names only)

#define XDS_CONFIG_BYTEORDER    'l' // Byte order ('l' for Little-Endian or 'b' for Big-Endian)
#define XDS_CONFIG_ALIGN        4   // Structure alignment size
#define XDS_CONFIG_TAGSIZE      1   // number of bytes in tag length (1=up to 255 character tag names)


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// API features
//
// Un-comment to activate feature.
//
#define XDS_SUPPORT_DEFTYPE         // allow record/node types to be defined via the API
#define XDS_SUPPORT_XML             // handle reading of XML files
#define XDS_SUPPORT_WRITING         // handle writing XDS data files
#define XDS_SUPPORT_SIGNATURE       // handle read/write of XDS signature records

#define XDS_SUPPORT_XMLCOMMENTS     // pass comments in XML files to XDS_PROCESSNODE (otherwise just skipped)
//#define XDS_SUPPORT_COMPACTDSD      // use abbreviated in-memory DSD (doesn't work with XML, DEFTYPE)

#define XDS_SUPPORT_ALLOCNODES      // allocate memory for node data -- if this is not defined,
                                    // the data passed to XDS_PROCESSNODE is temporary and must
                                    // be copied before the call returns
//#define XDS_SUPPORT_READBYRECORD    // read entire records into memory -- this is more efficient
                                    // than reading node by node, but it does require a larger buffer

#define XDS_CONFIG_XMLALLSTRUCTFIELDS // all structure fields must be present when reading XML
// if this is not set, missing fields will be set to all zeros

#define XDS_CONFIG_NUMIDS       128 // default number of user IDs to handle -- defining the exact
// number that you are using is optimal
// (only used if XDS_SUPPORT_COMPACTDSD is defined)



// The below features will be added in future versions of this library
//#define XDS_SUPPORT_RECORDNESTING   // handle nested records
//#define XDS_SUPPORT_GRAPH           // handle read/write of XDS graph type data (not yet supported)
//#define XDS_SUPPORT_BITFIELDS       // handle bit-field structure members (not yet supported)
//#define XDS_SUPPORT_XMLENTITIES     // support use of entities (e.g. "&amp;" in XML text)



// ****************************************************************************
// *** 
// *** Host Integration
// *** 
// ****************************************************************************

// Integration for xdsMakeSchema
// -- uses expandable buffer when writing data
// -- doesn't support reading
// -- uses malloc/free
#include "XDSBuffer.h"

inline int XDS_READ(void *hFile, void *buf, int iSize)
{
	return 0;
}

inline int XDS_WRITE(void *hFile, void *buf, int iSize)
{
	if(!((CxdsBuffer*)hFile)->Write(buf, iSize))
		return 0;
	return iSize;
}

inline void *XDS_ALLOC(void *buf, int iSize, int iMemType)
{
	return realloc(buf, iSize);
}

inline void XDS_FREE(void *buf)
{
	free(buf);
}


inline void XDS_PROCESSNODE(unsigned short nodeType, void *nodeData, int nodeSize)
{
}

extern void XDS_ERROR(const char *errText);

#endif // XDSCONFIG_INCL