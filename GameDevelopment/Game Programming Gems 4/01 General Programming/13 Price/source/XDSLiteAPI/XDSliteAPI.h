// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#ifndef XDSLITEAPI_INCL
#define XDSLITEAPI_INCL

#include "XDSconfig.h"

struct xdsHandle;

// ****************************************************************************
// *** 
// *** Argument #defines
// *** 
// ****************************************************************************

// XDS_ALLOC() iMemType values
#define XDS_MEMTYPE_NODE            0       // memory for node data
#define XDS_MEMTYPE_BUFFER          1       // memory for I/O buffer
#define XDS_MEMTYPE_SCRATCH         2       // scratch memory

// I/O mode values
#define XDS_IOMODE_READ             'r'     // init/restart for reading
#define XDS_IOMODE_WRITE            'w'     // init/restart for writing


// ****************************************************************************
// *** 
// *** Predefined Types
// *** 
// ****************************************************************************


#define XDS_RECORD_ENDOFDATA        0x0000  // End-of-Stream record
#define XDS_RECORD_ENCODING         0x0001  // String Encoding Definition record
#define XDS_RECORD_STREAMNAME       0x0002  // Stream Name record
#define XDS_RECORD_RECORDDEF        0x0003  // Record Definition record
#define XDS_RECORD_TYPEDEF          0x0004  // Type Definition record
#define XDS_RECORD_ELEMDEF          0x0005  // Element Definition record
#define XDS_RECORD_ATTRIBDEF        0x0006  // Attribute Definition record
#define XDS_RECORD_SIGNATURE        0x0007  // Stream Signature record
#define XDS_RECORD_COMMENT          0x0008  // Comment record
#define XDS_RECORD_DATA             0x000F  // Generic Data record

#define XDS_META_TYPE_INTEGER       0x0010  // n-byte integral value
#define XDS_META_TYPE_ENUMERATION   0x0011  // enumerated n-byte integral type
#define XDS_META_TYPE_STRING        0x0012  // string
#define XDS_META_TYPE_FIXEDPOINT    0x0013  // Fixed-point number
#define XDS_META_TYPE_FLOATINGPOINT 0x0014  // IEEE 754-style floating point number
#define XDS_META_TYPE_BITFIELD      0x0015  // n-bit packed data field 
#define XDS_META_TYPE_ARRAY         0x0016  // Array type
#define XDS_META_TYPE_STRUCTURE     0x0017  // Structure type
#define XDS_META_TYPE_GRAPH         0x0018  // Tree / Graph type
#define XDS_META_TYPE_POINTER       0x0019  // Pointer type

#define XDS_TYPE_CHAR               0x0020  // single byte signed integral value
#define XDS_TYPE_BYTE               0x0021  // single byte unsigned integral value
#define XDS_TYPE_SHORT              0x0022  // 2 byte signed integral value
#define XDS_TYPE_WORD               0x0023  // 2 byte unsigned integral value
#define XDS_TYPE_LONG               0x0024  // 4 byte signed integral value
#define XDS_TYPE_DWORD              0x0025  // 4 byte unsigned integral value
#define XDS_TYPE_BCSTRING           0x0026  // byte-length specified string
#define XDS_TYPE_WCSTRING           0x0027  // word-length specified string
#define XDS_TYPE_FLOAT              0x0028  // IEEE 754 32-bit floating-point number
#define XDS_TYPE_DOUBLE             0x0029  // IEEE 754 64-bit floating-point number

#define XDS_ENCODING_UTF8           0x00F0	// UTF-8 string encoding
#define XDS_ENCODING_UTF16          0x00F1  // UTF-16 string encoding

#define XDS_FIRST_DEFID             0x0100  // first available ID for user-defined types


// ****************************************************************************
// *** 
// *** Basic API
// *** 
// ****************************************************************************


extern struct xdsHandle *xdsInit(void *hFile, const char *szStreamName, const unsigned char *pDSD, char mode='r');
extern bool              xdsFini(struct xdsHandle *hXds);          // release all resources for stream

extern const char       *xdsGetStreamName(struct xdsHandle *hXds);
extern bool              xdsRestart(struct xdsHandle *hXds, void *hFile, char mode='r'); 

extern bool              xdsReadRecord(struct xdsHandle *hXds);


#ifdef XDS_SUPPORT_WRITING

#ifdef XDS_SUPPORT_SIGNATURE
extern bool              xdsWriteSignature(struct xdsHandle *hXds);
#endif

extern bool              xdsWriteComment(struct xdsHandle *hXds, char *szComment);
extern bool              xdsStartRecord(struct xdsHandle *hXds, unsigned short iType);
extern bool              xdsWriteNode(struct xdsHandle *hXds, unsigned short iType, void *data, int iCount);
extern bool              xdsEndRecord(struct xdsHandle *hXds);
extern bool              xdsEndStream(struct xdsHandle *hXds);

#endif // XDS_SUPPORT_WRITING


#ifdef XDS_SUPPORT_DEFTYPE

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

#endif // XDS_SUPPORT_DEFTYPE

extern unsigned short    xdsNodeClass(struct xdsHandle *hXds, unsigned short iType);
extern unsigned long     xdsNodeSize(struct xdsHandle *hXds, unsigned short iType);

#endif // XDSLITEAPI_INCL
