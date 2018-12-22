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

// These functions should be replaced with code that references your file
// handling interface.  For the purposes of the sample, they are using the
// standard POSIX file interface.

#include "stdlib.h"
#include "stdio.h"

// function: read a block of data from a file
//
// hFile - the file handle as given to the xdsInit function
// buf        - a pointer to the buffer to be filled with data
// iSize      - the number of bytes requested
//
// returns    - the number of bytes read or -1 if end-of-file is reached
//
inline int XDS_READ(void *hFile, void *buf, int iSize)
{
	int nbytes = fread(buf, 1, iSize, (FILE *)hFile);
	if(!nbytes && (feof((FILE*)hFile) || ferror((FILE*)hFile)))
		return -1;
	return nbytes;
}

// function: write a block of data to a file
//
// hFile - the file handle as given to the xdsInit function
// buf        - a pointer to the buffer to be written
// iSize      - the number of bytes to be written
//
// returns    - iSize if the buffer was successfully written, otherwise
//              any number from -1 to iSize-1 (actual value is unused)
//
inline int XDS_WRITE(void *hFile, void *buf, int iSize)
{
	return (int)fwrite(buf, 1, iSize, (FILE*)hFile);
}


// These functions should be replaced with code that references your memory
// allocation interface.  For the purposes of the sample, they are using the
// standard malloc/realloc/free interface.

// function: allocate/reallocate a block of data
//
// buf        - a pointer to the existing block of data (if any)
// iSize      - the size of the requested buffer, in bytes
//
// returns    - pointer to allocated buffer (or NULL on failure)
//
inline void *XDS_ALLOC(void *buf, int iSize, int iMemType)
{
	return realloc(buf, iSize);
}

// function: release a block of data
//
// buf        - a pointer to the block of data to be free'd
//
inline void XDS_FREE(void *buf)
{
	free(buf);
}


// The following are callback functions that are called when data is read
// from an XML/XDS file.

// function: do any necessary processing for a piece of data from a file
//
// nodeType   - the type ID of the node to be processed (may be comment, (sub)record, element, or attribute)
// nodeData   - a pointer the data for the node
// nodeSize   - the number of bytes in the nodeData
//
// For records and sub-records, and end-of-data notifications the nodeData value will be NULL
//
// If XDS_SUPPORT_ALLOCNODES is defined, the user is responsible for releasing the nodeData buffer.
// XDS_RECORD_COMMENT records are never allocated and should not be released!
//
// The nodeSize is always the total number of bytes in the nodeData buffer.  Thus, for structures
// the size is the size of the structure; for arrays the size is the size of an array entry times the
// number of entries.
//
extern void XDS_PROCESSNODE(unsigned short nodeType, void *nodeData, int nodeSize);

// function: report error in data being read from an XML/XDS file
//
// errText    - short description of the error
//
// Note: The behavior of the XDS-lite API library is unpredictable if you
//       return back to it.  Since the data is assumed to be completely under
//       your control any error in when reading the data is catastrophic.
//
extern void XDS_ERROR(const char *errText);

#endif // XDSCONFIG_INCL