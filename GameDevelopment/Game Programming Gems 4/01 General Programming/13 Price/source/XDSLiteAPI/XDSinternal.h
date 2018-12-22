// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#ifndef XDSINTERNAL_INCL
#define XDSINTERNAL_INCL

// ---------------------------------------------------------------------------
//  Configuration validation
// ---------------------------------------------------------------------------

// force compilation errors on incompatible options

#ifdef XDS_SUPPORT_COMPACTDSD

#ifdef XDS_SUPPORT_XML
typedef char COMPILE_TIME_ASSERTION_FAILED__XML_Support_requires_full_DSD[-1];
#endif

#ifdef XDS_SUPPORT_DEFTYPE
typedef char COMPILE_TIME_ASSERTION_FAILED__Cannot_Programmatically_Define_Types_With_Compact_DSD[-1];
#endif

#endif


// ****************************************************************************
// *** 
// *** Internal defines, data types & declarations
// *** 
// ****************************************************************************

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define XDS_NODESIZE_FIXED      0x01	// element is fixed size, implies !(XDS_NODESIZE_INDIRECT|XDS_NODESIZE_MULTIPLE|XDS_NODESIZE_BITFIELD)
#define XDS_NODESIZE_INDIRECT   0x02	// element is sized by input, implies !XDS_NODESIZE_FIXED
#define XDS_NODESIZE_MULTIPLE   0x04	// element is multiple of calculated size, implies !XDS_NODESIZE_FIXED
//#define XDS_NODESIZE_BITFIELD   0x08    // element is a bit-field, implies !XDS_NODESIZE_INDIRECT

#include "stdlib.h"
#include "string.h"
#include "XDSliteAPI.h"
#include "XDSBuffer.h"

// ---------------------------------------------------------------------------
//  External Declarations
// ---------------------------------------------------------------------------

// XDSCRC.cpp
#ifdef XDS_SUPPORT_SIGNATURE
extern unsigned long xdsCRC(unsigned char *buf, unsigned long buflen);
#endif

// XDSDSD.cpp
extern bool xdsProcessDSD(struct xdsHandle *hXds, CxdsBuffer *pDSD);
extern bool xdsProcessDefinition(struct xdsHandle *hXds, u16 iRecType, CxdsBuffer *pDSD);
//extern unsigned short    xdsDefStringEncoding(struct xdsHandle *hXds, const char *szEncoding);
extern unsigned short    xdsDefRecord(struct xdsHandle *hXds, const char *szTagName, unsigned char iLengthSize);
extern unsigned short    xdsDefElement(struct xdsHandle *hXds, const char *szTagName, unsigned short iBaseType, unsigned short iRecord);
extern unsigned short    xdsDefAttribute(struct xdsHandle *hXds, const char *szTagName, unsigned short iBaseType, unsigned short iRecord);
extern unsigned short    xdsDefIntegerType(struct xdsHandle *hXds, const char *szTagName, char signtype, unsigned char iValueSize);
extern unsigned short    xdsDefEnumeratedType(struct xdsHandle *hXds, const char *szTagName, char signtype, unsigned char iValueSize);
extern bool              xdsDefEnumerand(struct xdsHandle *hXds, unsigned short iEnumType, char *szEnumName, unsigned iValue);
extern bool              xdsDefEnumeratedDone(struct xdsHandle *hXds, unsigned short iEnumType);
extern unsigned short    xdsDefStringType(struct xdsHandle *hXds, const char *szTagName, unsigned short iEncoding,
										  unsigned iLength, char iLengthSize);
extern unsigned short    xdsDefFixedpointType(struct xdsHandle *hXds, const char *szTagName, unsigned char iTypeLength,
											  unsigned char iFracLength);
extern unsigned short    xdsDefFloatType(struct xdsHandle *hXds, const char *szTagName, unsigned char iTypeLength,
										 unsigned char iExpLength, unsigned char iPadLength, unsigned char iCoeffLength);
//extern unsigned short    xdsDefBitfieldType(struct xdsHandle *hXds, const char *szTagName, unsigned long iNumbits);
extern unsigned short    xdsDefArrayType(struct xdsHandle *hXds, const char *szTagName, unsigned short iBaseType, 
										 unsigned iLength, unsigned char iLengthSize);
extern unsigned short    xdsDefStructType(struct xdsHandle *hXds, const char *szTagName);
extern bool              xdsDefStructField(struct xdsHandle *hXds, unsigned short iStructType, char *szFieldName,
										   unsigned short iBaseType, unsigned iCount);
extern bool              xdsDefStructDone(struct xdsHandle *hXds, unsigned short iStructType);
extern unsigned short    xdsDefGraphType(struct xdsHandle *hXds, const char *szTagName, unsigned short iBaseType,
										 unsigned char iLengthSize);
extern unsigned short    xdsDefPointerType(struct xdsHandle *hXds, const char *szTagName, unsigned short iBaseType,
										   unsigned char iTypeLength);

// - - - XML Parsing - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// in XDSXMLParser.cpp
extern bool xdsXmlReadHeader(struct xdsHandle *hXds);
extern bool xdsXmlReadRecord(struct xdsHandle *hXds);

// - - - Utility - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// in XDSHelpers.cpp
extern char *xdsStringDup(const char *sz);
extern char *xdsStringNDup(const char *sz, int iCount);
extern struct xdsType_t *xdsFindType(struct xdsHandle *hXds, u16 iType);


// ---------------------------------------------------------------------------
//  Structure Definitions
// ---------------------------------------------------------------------------

// - - - Data Stream Definition  - - - - - - - - - - - - - - - - - - - - - - -
/*)
// encoding type
struct xdsEncoding_t {
	short           iClass;             // == XDS_RECORD_ENCODING
	unsigned short  iEncodingId;
	const char     *szName;
};
*/

// record type
struct xdsRecord_t {
	short           iClass;             // == XDS_RECORD_RECORDDEF
	unsigned short  iRecordId;
#ifndef XDS_SUPPORT_COMPACTDSD
	const char     *szName;
#endif

	unsigned long   iNodeSize;	        // base size (may for fixed or multiple types)
	unsigned char   iNodeSizeLength;
	unsigned char   iNodeSizeType;	    // bit-mask of XDS_NODESIZE_*
};

// element, attribute type
struct xdsElement_t {
	short           iClass;		        // == XDS_RECORD_ELEMDEF, XDS_RECORD_ATTRIBDEF
	unsigned short  iTypeId;
#ifndef XDS_SUPPORT_COMPACTDSD
	const char     *szName;
#endif

	unsigned long   iNodeSize;	        // base size (may for fixed or multiple types)
	unsigned char   iNodeSizeLength;
	unsigned char   iNodeSizeType;	    // bit-mask of XDS_NODESIZE_*

	unsigned short  iType;
	unsigned short  iRecord;
};


// data type
struct xdsType_t {
	unsigned short  iClass;             // == XDS_RECORD_TYPEDEF
	unsigned short  iTypeId;
#ifndef XDS_SUPPORT_COMPACTDSD
	const char     *szName;
#endif

	unsigned long   iNodeSize;	        // base size (may for fixed or multiple types)
	unsigned char   iNodeSizeLength;
	unsigned char   iNodeSizeType;	    // bit-mask of XDS_NODESIZE_*

	unsigned short  iType;

#ifndef XDS_SUPPORT_COMPACTDSD

	union {
		struct {
			unsigned short    iValueSize;
			bool              bSigned;
		} int_t;
		struct {
			unsigned short    iValueSize;
			unsigned short    iEnumCount;
			bool              bSigned;
			struct xdsEnum_t *pEnums;
		} enum_t;
		struct {
			short             iLengthSize;
			unsigned short    iEncoding;
			unsigned long     iLength;
		} string_t;
		struct {
			unsigned short    iTypeLen;
			unsigned short    iFracLen;
		} fixed_t;
		struct {
			unsigned char    iTypeLen;
			unsigned char    iExpLen;
			unsigned char    iPadLen;
			unsigned char    iCoeffLen;
		} float_t;
		struct {
			unsigned long     iBitCount;
		} bitfield_t;
		struct {
			short             iLengthSize;
			unsigned short    iBaseType;
			unsigned long     iDefaultSize;
		} array_t;
		struct {
			unsigned short    iFieldCount;
			struct xdsField_t *pFields;
		} struct_t;
		struct {
			unsigned short     iBaseType;
			unsigned short     iLengthSize;
		} graph_t;
		struct {
			unsigned short     iBaseType;
			unsigned short     iTypeLength;
		} pointer_t;
	} var;
#endif // !XDS_SUPPORT_COMPACTDSD
};

#ifndef XDS_SUPPORT_COMPACTDSD

struct xdsEnum_t			// enumerand
{
	const char     *szName;
	unsigned long   iValue;
};

struct xdsField_t {			// field of a structure
	const char     *szName;
	unsigned short  iType;
	unsigned short  iArraySize;
	unsigned int    iOffset;
};

#endif // !XDS_SUPPORT_COMPACTDSD


// - - - reTRIEval databases - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef XDS_SUPPORT_COMPACTDSD

#include "Trie.h"

class CNameTrie : public CTrie
{
	virtual void GetKey(void *data, unsigned char **key, int *keylen)
	{
		*key = (unsigned char*)((struct xdsRecord_t*)data)->szName;
		*keylen = strlen((const char*)*key);
	}
};

class CIdTrie : public CTrie
{
	virtual void GetKey(void *data, unsigned char **key, int *keylen)
	{
		*key = (unsigned char*)&((struct xdsRecord_t*)data)->iRecordId;
		*keylen = sizeof(u16);
	}
};

#endif

// - - - XML structures  - - - - - - - - - - - - - - - - - - - - - - - - - - -

class CxdsXmlTagStack;

struct recEntry_t {
	unsigned long lOffset;	    // offset of start of record in output buffer
	unsigned short iRecordType; // type of record
	unsigned short iRecordSize;
};

// - - - Stream Handle - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct xdsHandle {
	const char *szStreamName;
	bool        bEndOfStream;	// end of stream read/written

	u16        iNextId;
#ifdef XDS_SUPPORT_COMPACTDSD
	u16        iLastAllocId;    // number of allocated IDs plus XDS_FIRST_DEFID
	struct xdsType_t *pTypeDb;
#else
	CIdTrie   *pTypeDb;
#endif

	// buffered I/O support
	CxdsBuffer xBuf;

#ifdef XDS_SUPPORT_WRITING
	bool bReading;
	unsigned short iRecNesting;
	struct recEntry_t recEntry;
#endif

#ifdef XDS_SUPPORT_SIGNATURE
	u32 lDSDLength;				// These two fields must appear consecutively and
	u32 lDSDCRC;				//   in this order!
#endif

	// XML support
#ifdef XDS_SUPPORT_XML
	bool bNative;				// true==XDS false==XML
	CNameTrie *pTypeDbByName;
	CxdsXmlTagStack *pTagStack;
#endif
};

// ****************************************************************************
// *** 
// *** Inline helper functions
// *** 
// ****************************************************************************

inline u32 xdsFOURCC(u8 ch0, u8 ch1, u8 ch2, u8 ch3)
{
	if(XDS_CONFIG_BYTEORDER == 'l')
		return (u32)(u8)(ch0) | ((u32)(u8)(ch1) << 8) | ((u32)(u8)(ch2) << 16) | ((u32)(u8)(ch3) << 24);
	else // must be 'b'
		return ((u32)(u8)(ch0) << 24) | ((u32)(u8)(ch1) << 16) | ((u32)(u8)(ch2) << 8) | (u32)(u8)(ch3);
}

inline u16 xdsTWOCC(u8 ch0, u8 ch1)
{
	if(XDS_CONFIG_BYTEORDER == 'l')
		return (u16)(u8)(ch0) | ((u16)(u8)(ch1) << 8);
	else // must be 'b'
		return ((u16)(u8)(ch0) << 8) | ((u16)(u8)(ch1));
}


#endif // XDSINTERNAL_INCL