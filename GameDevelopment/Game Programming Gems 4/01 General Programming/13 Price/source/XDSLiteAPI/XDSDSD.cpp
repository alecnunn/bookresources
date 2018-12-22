// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include "string.h"
#include "XDSinternal.h"


bool xdsProcessDefinition(struct xdsHandle *hXds, u16 iRecType, CxdsBuffer *pDSD)
{
	const char *szTagName;
	u16  iTagId;

	switch(iRecType)
	{
	case XDS_RECORD_ENDOFDATA:	// End-of-Stream record
		return true;

//	case XDS_RECORD_ENCODING:	// String Encoding Definition record
//		(void)pDSD->ReadShort();	// skip record length
//		szTagName = pDSD->ReadTag();
//		iTagId = pDSD->ReadShort();
//		if(iTagId > hXds->iNextId)
//		{
//			XDS_ERROR("Out of order ID in data stream definition");
//			return false;
//		}
//		hXds->iNextId = iTagId;
//		xdsDefStringEncoding(hXds, szTagName);
//		break;

	case XDS_RECORD_STREAMNAME:	// Stream Name record
		(void)pDSD->ReadShort();
		hXds->szStreamName = pDSD->ReadTag();
		break;

	case XDS_RECORD_RECORDDEF:	// Record Definition record
	{
		(void)pDSD->ReadShort();
		szTagName = pDSD->ReadTag();
		iTagId    = pDSD->ReadShort();
		if(iTagId > hXds->iNextId)
		{
			XDS_ERROR("Out of order ID in data stream definition");
			return false;
		}
		hXds->iNextId = iTagId;

		u8 iLenSize = pDSD->ReadByte();
		xdsDefRecord(hXds, szTagName, iLenSize);
		XDS_FREE((void*)szTagName);
		break;
	}

	case XDS_RECORD_TYPEDEF:	// Type/Element Definition record
	{
		u16 iTypeId;

		(void)pDSD->ReadShort();
		szTagName = pDSD->ReadTag();
		iTagId    = pDSD->ReadShort();
		if(iTagId > hXds->iNextId)
		{
			XDS_ERROR("Out of order ID in data stream definition");
			return false;
		}
		hXds->iNextId = iTagId;

		iTypeId = pDSD->ReadShort();
		switch(iTypeId)
		{
		case XDS_META_TYPE_INTEGER:			// n-byte integral value
		{
			char cSign = pDSD->ReadByte();
			u8 iValueSize = pDSD->ReadByte();
			xdsDefIntegerType(hXds, szTagName, cSign, iValueSize);
			break;
		}

		case XDS_META_TYPE_ENUMERATION:		// enumerated n-byte integral type
		{
			char cSign = pDSD->ReadByte();
			u8 iValueSize = pDSD->ReadByte();
			unsigned short iEnumType = xdsDefEnumeratedType(hXds, szTagName, cSign, iValueSize);

			u16 iEnumCount = pDSD->ReadShort();
			u32 iValue;
			for(int ii = 0; ii < iEnumCount; ++ii)
			{
				char *szEnumName = pDSD->ReadTag();
				iValue = pDSD->ReadValue(iValueSize);
#ifndef XDS_SUPPORT_COMPACTDSD
				xdsDefEnumerand(hXds, iEnumType, szEnumName, iValue);
#endif
				XDS_FREE((void*)szEnumName);
			}
#ifndef XDS_SUPPORT_COMPACTDSD
			xdsDefEnumeratedDone(hXds, iEnumType);
#endif
			break;
		}

		case XDS_META_TYPE_STRING:			// string
		{
			char iLengthSize = pDSD->ReadByte();
			u16 iEncoding = pDSD->ReadShort();
			u32 iLength;
			if(iLengthSize < 0)
				iLength = pDSD->ReadValue(-iLengthSize);
			else
				iLength = 0;
			xdsDefStringType(hXds, szTagName, iEncoding, iLength, iLengthSize);
			break;
		}

		case XDS_META_TYPE_FIXEDPOINT:		// Fixed-point number
		{
			u8 iTypeLength = pDSD->ReadByte();
			u8 iFracLength = pDSD->ReadByte();
			xdsDefFixedpointType(hXds, szTagName, iTypeLength, iFracLength);
			break;
		}

		case XDS_META_TYPE_FLOATINGPOINT:	// IEEE 754-style floating point number
		{
			u8 iTypeLength = pDSD->ReadByte();
			u8 iExpLength  = pDSD->ReadByte();
			u8 iPadLength  = pDSD->ReadByte();
			u8 iCoeffLength = pDSD->ReadByte();
			xdsDefFloatType(hXds, szTagName, iTypeLength, iExpLength, iPadLength, iCoeffLength);
			break;
		}

//		case XDS_META_TYPE_BITFIELD:		// n-bit packed data field 
//			xdsDefBitfieldType(hXds, szTagName, pDSD->ReadLong());
//			break;

		case XDS_META_TYPE_ARRAY:			// Array type
		{
			char iLengthSize = pDSD->ReadByte();
			u16  iBaseType   = pDSD->ReadShort();
			u32 iDefaultSize;
			if(iLengthSize < 0)
				iDefaultSize = pDSD->ReadValue(-iLengthSize);
			else
				iDefaultSize = 0;
			xdsDefArrayType(hXds, szTagName, iBaseType, iDefaultSize, iLengthSize);
			break;
		}

		case XDS_META_TYPE_STRUCTURE:		// Structure type
		{
			unsigned short iStructType = xdsDefStructType(hXds, szTagName);
			u16 iFieldCount = pDSD->ReadShort();
			for(int ii = 0; ii < iFieldCount; ++ii)
			{
				char *szFieldName = pDSD->ReadTag();
				u16 iFieldType    = pDSD->ReadShort();
				u16 iArraySize    = pDSD->ReadShort();
				xdsDefStructField(hXds, iStructType, szFieldName, iFieldType, iArraySize);
				XDS_FREE((void*)szFieldName);
			}
			xdsDefStructDone(hXds, iStructType);
			break;
		}

//		case XDS_META_TYPE_GRAPH:			// Tree / Graph type
//		{
//			u16 iBaseType   = pDSD->ReadShort();
//			u8  iLengthSize = pDSD->ReadByte();
//			xdsDefGraphType(hXds, szTagName, iBaseType, iLengthSize);
//			break;
//		}

		case XDS_META_TYPE_POINTER:			// Pointer type
		{
			u16 iBaseType = pDSD->ReadShort();
			u8  iValueSize = pDSD->ReadByte();
			xdsDefPointerType(hXds, szTagName, iBaseType, iValueSize);
			break;
		}

		default:
			XDS_ERROR("Unsupported type in data stream definition");
			return false;
		}

		XDS_FREE((void*)szTagName);
		break;
	}
	case XDS_RECORD_ELEMDEF:	// Type/Element Definition record
	case XDS_RECORD_ATTRIBDEF:
	{
		u16 iTypeId, iRecordId;

		(void)pDSD->ReadShort();
		szTagName = pDSD->ReadTag();
		iTagId    = pDSD->ReadShort();
		if(iTagId > hXds->iNextId)
		{
			XDS_ERROR("Out of order ID in data stream definition");
			return false;
		}
		hXds->iNextId = iTagId;

		iTypeId = pDSD->ReadShort();
		iRecordId = pDSD->ReadShort();

		if(iRecType == XDS_RECORD_ELEMDEF)
			xdsDefElement(hXds, szTagName, iTypeId, iRecordId);
		else
			xdsDefAttribute(hXds, szTagName, iTypeId, iRecordId);
		break;
	}

	case XDS_RECORD_COMMENT:	// Comment record
	{
		u32 iLen = pDSD->ReadLong();
		pDSD->Skip(iLen);
		break;
	}

	// the following are currently unsupported in the XDS-lite API data stream definition
	case XDS_RECORD_SIGNATURE:	// don't make sense to have in DSD
	case XDS_RECORD_DATA:
	case XDS_ENCODING_UTF8:		// currently only supports UTF8 for tagnames
	case XDS_ENCODING_UTF16:
	default:
		XDS_ERROR("Unsupported record type in data stream definition");
		return false;
	}

	return true;
}

bool xdsProcessDSD(struct xdsHandle *hXds, CxdsBuffer *pDSD)
{
	// validate & skip XDS header
	if(pDSD->ReadLong() != xdsFOURCC('X', 'D', 'S', '!'))
	{
		XDS_ERROR("Bad magic number in DSD header");
		return false;
	}

	if(pDSD->ReadLong() != xdsFOURCC('0', '3', '0', '0'))
	{
		XDS_ERROR("Bad version in DSD header");
		return false;
	}

	if(pDSD->ReadLong() != xdsFOURCC(XDS_CONFIG_BYTEORDER, XDS_CONFIG_ALIGN, XDS_CONFIG_TAGSIZE, 0))
	{
		XDS_ERROR("Bad configuration values in DSD header");
		return false;
	}

	// process type & record definitions
	u16 rectype;
	do {
		rectype = pDSD->ReadShort();
		if(!xdsProcessDefinition(hXds, rectype, pDSD))
			return false;
	} while(rectype != XDS_RECORD_ENDOFDATA);

	return true;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Type Definition
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static bool isSimpleType(struct xdsHandle *hXds, unsigned short iId)
{
	// search for root type
	for(;;)
	{
//#ifndef XDS_SUPPORT_COMPACTDSD
//		struct xdsElement_t *pItem = (struct xdsElement_t *)hXds->pTypeDb->Find((unsigned char *)&iId, sizeof(iId));
		struct xdsElement_t *pItem = (struct xdsElement_t *)xdsFindType(hXds, iId);
		if(!pItem)
		{
//#else
//			if(iId >= XDS_FIRST_DEFID && iId < hXds->iNextId)
//				return false;
//#endif
			// not defined, check to see if it's a simple predefined type
			if(iId == XDS_TYPE_BYTE || iId == XDS_TYPE_WORD || iId == XDS_TYPE_DWORD ||
				iId == XDS_TYPE_CHAR || iId == XDS_TYPE_SHORT || iId == XDS_TYPE_LONG ||
				iId == XDS_TYPE_BCSTRING || iId == XDS_TYPE_WCSTRING ||
				iId == XDS_TYPE_FLOAT || iId == XDS_TYPE_DOUBLE ||

				iId == XDS_META_TYPE_INTEGER || iId == XDS_META_TYPE_ENUMERATION ||
				iId == XDS_META_TYPE_STRING || iId == XDS_META_TYPE_FLOATINGPOINT ||
				iId == XDS_META_TYPE_FLOATINGPOINT || iId == XDS_META_TYPE_BITFIELD ||
				iId == XDS_META_TYPE_POINTER)
			{
				return true;
			}

			return false;
		}
		else if(pItem->iClass != XDS_RECORD_ELEMDEF && pItem->iClass != XDS_RECORD_ATTRIBDEF)
			return false;
		else
		{
			// recurse to base type
			iId = pItem->iTypeId;
		}
	}
}


static bool getTypeSize(struct xdsHandle *hXds, unsigned short iId, struct xdsElement_t *pNode)
{
//	struct xdsType_t *pType = (struct xdsType_t *)hXds->pTypeDb->Find((unsigned char *)&iId, sizeof(iId));
	struct xdsType_t *pType = xdsFindType(hXds, iId);
	if(!pType)
	{
		pNode->iNodeSizeLength = 0;
		pNode->iNodeSizeType   = XDS_NODESIZE_FIXED;

		switch(iId)
		{
		case XDS_TYPE_BYTE:
		case XDS_TYPE_CHAR:
			pNode->iNodeSize = 1;
			break;
		case XDS_TYPE_WORD:
		case XDS_TYPE_SHORT:
			pNode->iNodeSize = sizeof(u16);
			break;
		case XDS_TYPE_DWORD:
		case XDS_TYPE_LONG:
			pNode->iNodeSize = sizeof(u32);
			break;
		case XDS_TYPE_BCSTRING:
			pNode->iNodeSize       = 0;
			pNode->iNodeSizeType   = XDS_NODESIZE_INDIRECT;
			pNode->iNodeSizeLength = 1;
			break;
		case XDS_TYPE_WCSTRING:
			pNode->iNodeSize       = 0;
			pNode->iNodeSizeType   = XDS_NODESIZE_INDIRECT;
			pNode->iNodeSizeLength = sizeof(u16);
			break;
		case XDS_TYPE_FLOAT:
			pNode->iNodeSize = sizeof(float);
			break;
		case XDS_TYPE_DOUBLE:
			pNode->iNodeSize = sizeof(double);
			break;
		default:
			return false;
		}

		return true;
	}
	if(pType->iClass != XDS_RECORD_TYPEDEF &&
	   pType->iClass != XDS_RECORD_ELEMDEF && pType->iClass != XDS_RECORD_ATTRIBDEF)
		return false;

	pNode->iNodeSize       = pType->iNodeSize;
	pNode->iNodeSizeLength = pType->iNodeSizeLength;
	pNode->iNodeSizeType   = pType->iNodeSizeType;

	return true;
}
static bool isValidId(struct xdsHandle *hXds, unsigned short iId, unsigned short iClass)
{
//	struct xdsElement_t *pItem = (struct xdsElement_t *)hXds->pTypeDb->Find((unsigned char *)&iId, sizeof(iId));
	struct xdsElement_t *pItem = (struct xdsElement_t *)xdsFindType(hXds, iId);
	if(!pItem)
	{
		// not defined, check predefined records & types
		if(iClass == XDS_RECORD_RECORDDEF)
		{
			if(iId == XDS_RECORD_DATA)
				return true;
		}
		else if(iClass == XDS_RECORD_TYPEDEF)
		{
			if(iId == XDS_TYPE_BYTE || iId == XDS_TYPE_WORD || iId == XDS_TYPE_DWORD ||
				iId == XDS_TYPE_CHAR || iId == XDS_TYPE_SHORT || iId == XDS_TYPE_LONG ||
				iId == XDS_TYPE_BCSTRING || iId == XDS_TYPE_WCSTRING ||
				iId == XDS_TYPE_FLOAT || iId == XDS_TYPE_DOUBLE)
				return true;
		}
		else if(iClass == XDS_RECORD_ENCODING)
		{
			if(iId == XDS_ENCODING_UTF8 || iId == XDS_ENCODING_UTF16)
				return true;
		}
		return false;
	}
	if(pItem->iClass != iClass)
		return false;
	return true;
}

static struct xdsType_t *xdsAllocType(struct xdsHandle *hXds, const char *szTagName)
{
#ifdef XDS_SUPPORT_COMPACTDSD
	if(hXds->iNextId >= hXds->iLastAllocId + XDS_FIRST_DEFID)
	{
		hXds->iLastAllocId <<= 1;
		hXds->pTypeDb = (struct xdsType_t*)XDS_ALLOC(hXds->pTypeDb, sizeof(struct xdsType_t)*hXds->iLastAllocId, XDS_MEMTYPE_SCRATCH);
		if(!hXds->pTypeDb)
			return NULL;
	}
	struct xdsType_t *pType = &hXds->pTypeDb[hXds->iNextId - XDS_FIRST_DEFID];
#else
	struct xdsType_t *pType = (struct xdsType_t *)XDS_ALLOC(NULL, sizeof(struct xdsType_t), XDS_MEMTYPE_SCRATCH);
	if(!pType)
		return NULL;
#endif

	pType->iClass   = XDS_RECORD_TYPEDEF;
	pType->iTypeId  = hXds->iNextId++;
#ifndef XDS_SUPPORT_COMPACTDSD
	pType->szName   = xdsStringDup(szTagName);
	hXds->pTypeDb->Add((void*)pType);
#endif
#ifdef XDS_SUPPORT_XML
	hXds->pTypeDbByName->Add((void*)pType);
#endif

	return pType;
}

// returns the high-level class of the passed type, one of:
//   XDS_RECORD_RECORDDEF   for defined and built-in record IDs
//   XDS_RECORD_TYPEDEF     for defined and built-in type IDs
//   XDS_RECORD_ELEMDEF     for defined element IDs
//   XDS_RECORD_ATTRIBDEF   for defined attribute IDs
//   XDS_RECORD_COMMENT     for the comment record
//   XDS_RECORD_ENDOFDATA   for the end-of-data record
//   0xffff                 for all other IDs
unsigned short xdsNodeClass(struct xdsHandle *hXds, unsigned short iType)
{
	struct xdsType_t *pType = xdsFindType(hXds, iType);
	if(pType)
		return pType->iClass;

	switch(iType)
	{
	case XDS_RECORD_ENDOFDATA:
	case XDS_RECORD_COMMENT:
		return iType;

	case XDS_RECORD_DATA:
		return XDS_RECORD_RECORDDEF;

	case XDS_TYPE_CHAR:
	case XDS_TYPE_BYTE:
	case XDS_TYPE_SHORT:
	case XDS_TYPE_WORD:
	case XDS_TYPE_LONG:
	case XDS_TYPE_DWORD:
	case XDS_TYPE_BCSTRING:
	case XDS_TYPE_WCSTRING:
	case XDS_TYPE_FLOAT:
	case XDS_TYPE_DOUBLE:
		return XDS_RECORD_TYPEDEF;

	default:
		return 0xffff;
	}
}

// returns the base item size for a type -- must be a defined element or attribute!
// otherwise it returns 0
unsigned long xdsNodeSize(struct xdsHandle *hXds, unsigned short iType)
{
	struct xdsElement_t elem;
	if(iType == XDS_RECORD_COMMENT)
		return 1;
	else if(getTypeSize(hXds, iType, &elem))
		return elem.iNodeSize;
	else
		return 0;
}


//#ifdef XDS_SUPPORT_DEFTYPE
/*
unsigned short xdsDefStringEncoding(struct xdsHandle *hXds, const char *szEncoding)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	struct xdsEncoding_t *pEncoding = (struct xdsEncoding_t *)XDS_ALLOC(NULL, sizeof(struct xdsEncoding_t), XDS_MEMTYPE_SCRATCH);

	pEncoding->iClass      = XDS_RECORD_ENCODING;
	pEncoding->iEncodingId = hXds->iNextId++;
	pEncoding->szName      = xdsStringDup(szEncoding);

	hXds->pTypeDb->Add((void*)pEncoding);
#ifdef XDS_SUPPORT_XML
	hXds->pTypeDbByName->Add((void*)pEncoding);
#endif
	return pEncoding->iEncodingId;
}
*/
unsigned short xdsDefRecord(struct xdsHandle *hXds, const char *szTagName, unsigned char iLengthSize)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
#ifdef XDS_SUPPORT_COMPACTDSD
	struct xdsRecord_t *pRecord = (struct xdsRecord_t *)xdsAllocType(hXds, szTagName);
#else
	struct xdsRecord_t *pRecord = (struct xdsRecord_t *)XDS_ALLOC(NULL, sizeof(struct xdsRecord_t), XDS_MEMTYPE_SCRATCH);
	if(!pRecord)
		return 0;
	pRecord->szName      = xdsStringDup(szTagName);
	pRecord->iRecordId   = hXds->iNextId++;
#endif

	pRecord->iClass      = XDS_RECORD_RECORDDEF;

	pRecord->iNodeSize       = 0;
	pRecord->iNodeSizeLength = iLengthSize;
	pRecord->iNodeSizeType   = XDS_NODESIZE_INDIRECT;

#ifndef XDS_SUPPORT_COMPACTDSD
	hXds->pTypeDb->Add((void*)pRecord);
#endif
#ifdef XDS_SUPPORT_XML
	hXds->pTypeDbByName->Add((void*)pRecord);
#endif

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_RECORDDEF);
		hXds->xBuf.WriteShort(3 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE));
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pRecord->iRecordId);
		hXds->xBuf.WriteByte(iLengthSize);
	}
#endif

	return pRecord->iRecordId;
}

unsigned short xdsDefElement(struct xdsHandle *hXds, const char *szTagName, unsigned short iBaseType,
							 unsigned short iRecord)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	struct xdsElement_t *pElem;

	if(iRecord != 0 && !isValidId(hXds, iRecord, XDS_RECORD_RECORDDEF))
	{
		XDS_ERROR("Invalid record ID in element definition");
		return 0;
	}

#ifdef XDS_SUPPORT_COMPACTDSD
	pElem = (struct xdsElement_t *)xdsAllocType(hXds, szTagName);
#else
	pElem = (struct xdsElement_t *)XDS_ALLOC(NULL, sizeof(struct xdsElement_t), XDS_MEMTYPE_SCRATCH);
#endif
	if(!pElem)
		return 0;

	if(!getTypeSize(hXds, iBaseType, pElem))
	{
		XDS_ERROR("Invalid base type in element definition");
		XDS_FREE(pElem);
		return 0;
	}

	pElem->iClass   = XDS_RECORD_ELEMDEF;
	pElem->iType    = iBaseType;
	pElem->iRecord  = iRecord;
#ifndef XDS_SUPPORT_COMPACTDSD
	pElem->iTypeId  = hXds->iNextId++;
	pElem->szName   = xdsStringDup(szTagName);

	hXds->pTypeDb->Add((void*)pElem);
#endif
#ifdef XDS_SUPPORT_XML
	hXds->pTypeDbByName->Add((void*)pElem);
#endif

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_ELEMDEF);
		hXds->xBuf.WriteShort(6 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE));
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pElem->iTypeId);
		hXds->xBuf.WriteShort(iBaseType);
		hXds->xBuf.WriteShort(iRecord);
	}
#endif
	return pElem->iTypeId;
}

unsigned short xdsDefAttribute(struct xdsHandle *hXds, const char *szTagName, unsigned short iBaseType,
							   unsigned short iRecord)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	struct xdsElement_t *pAttrib;

	if(!isSimpleType(hXds, iBaseType))
	{
		XDS_ERROR("Non-simple base type in attribute definition");
		return 0;
	}
	if(!isValidId(hXds, iRecord, XDS_RECORD_RECORDDEF))
	{
		XDS_ERROR("Invalid record ID in attribute definition");
		return 0;
	}

#ifdef XDS_SUPPORT_COMPACTDSD
	pAttrib = (struct xdsElement_t *)xdsAllocType(hXds, szTagName);
#else
	pAttrib = (struct xdsElement_t *)XDS_ALLOC(NULL, sizeof(struct xdsElement_t), XDS_MEMTYPE_SCRATCH);
#endif
	if(!pAttrib)
		return 0;

	if(!getTypeSize(hXds, iBaseType, pAttrib))
	{
		XDS_ERROR("Invalid base type in attribute definition");
		XDS_FREE(pAttrib);
		return 0;
	}

	pAttrib->iClass   = XDS_RECORD_ATTRIBDEF;
	pAttrib->iType    = iBaseType;
	pAttrib->iRecord  = iRecord;
#ifndef XDS_SUPPORT_COMPACTDSD
	pAttrib->iTypeId  = hXds->iNextId++;
	pAttrib->szName   = xdsStringDup(szTagName);

	hXds->pTypeDb->Add((void*)pAttrib);
#endif
#ifdef XDS_SUPPORT_XML
	hXds->pTypeDbByName->Add((void*)pAttrib);
#endif

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_ATTRIBDEF);
		hXds->xBuf.WriteShort(6 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE));
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pAttrib->iTypeId);
		hXds->xBuf.WriteShort(iBaseType);
		hXds->xBuf.WriteShort(iRecord);
	}
#endif
	return pAttrib->iTypeId;
}

unsigned short xdsDefIntegerType(struct xdsHandle *hXds, const char *szTagName, char signtype, unsigned char iValueSize)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	if(signtype != 's' && signtype != 'u')
		XDS_ERROR("Invalid sign type in xdsDefIntegerType");

	struct xdsType_t *pType = xdsAllocType(hXds, szTagName);

	pType->iNodeSize = iValueSize;
	pType->iNodeSizeLength = 0;
	pType->iNodeSizeType = XDS_NODESIZE_FIXED;

	pType->iType = XDS_META_TYPE_INTEGER;
#ifndef XDS_SUPPORT_COMPACTDSD
	pType->var.int_t.bSigned = (bool)(signtype == 's');
	pType->var.int_t.iValueSize = iValueSize;
#endif

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_TYPEDEF);
		hXds->xBuf.WriteShort(6 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE));
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pType->iTypeId);
		hXds->xBuf.WriteShort(XDS_META_TYPE_INTEGER);
		hXds->xBuf.WriteByte(signtype);
		hXds->xBuf.WriteByte(iValueSize);
	}
#endif
	return pType->iTypeId;
}

unsigned short xdsDefEnumeratedType(struct xdsHandle *hXds, const char *szTagName, char signtype, unsigned char iValueSize)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	if(signtype != 's' && signtype != 'u')
		XDS_ERROR("Invalid sign type in xdsDefEnumeratedType");

	struct xdsType_t *pType = xdsAllocType(hXds, szTagName);

	pType->iNodeSize = iValueSize;
	pType->iNodeSizeLength = 0;
	pType->iNodeSizeType = XDS_NODESIZE_FIXED;

	pType->iType = XDS_META_TYPE_ENUMERATION;
#ifndef XDS_SUPPORT_COMPACTDSD
	pType->var.enum_t.iEnumCount = 0;
	pType->var.enum_t.bSigned = (bool)(signtype == 's');
	pType->var.enum_t.iValueSize = iValueSize;
	pType->var.enum_t.pEnums = NULL;
#endif

	return pType->iTypeId;
}

#ifndef XDS_SUPPORT_COMPACTDSD
bool xdsDefEnumerand(struct xdsHandle *hXds, unsigned short iEnumType, char *szEnumName, unsigned iValue)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return false;
#endif
//	struct xdsType_t *pType = (struct xdsType_t*)hXds->pTypeDb->Find((unsigned char *)&iEnumType, sizeof(iEnumType));
	struct xdsType_t *pType = xdsFindType(hXds, iEnumType);
	if(!pType)
		return false;

	struct xdsEnum_t *pEnum = (struct xdsEnum_t*)XDS_ALLOC(pType->var.enum_t.pEnums,
		sizeof(struct xdsEnum_t)*(pType->var.enum_t.iEnumCount+1), XDS_MEMTYPE_SCRATCH);
	if(!pEnum)
		return false;

	pType->var.enum_t.iEnumCount++;
	pType->var.enum_t.pEnums = pEnum;

	pEnum += pType->var.enum_t.iEnumCount-1;
	pEnum->iValue = iValue;
	pEnum->szName = xdsStringDup(szEnumName);
	if(!pEnum->szName)
		return false;

	return true;
}

bool xdsDefEnumeratedDone(struct xdsHandle *hXds, unsigned short iEnumType)
{
	if(!hXds->bReading)
	{
//		struct xdsType_t *pType = (struct xdsType_t*)hXds->pTypeDb->Find((unsigned char*)&iEnumType, sizeof(iEnumType));
		struct xdsType_t *pType = xdsFindType(hXds, iEnumType);
		if(!pType)
			return false;

		unsigned short iLen = 8 + (strlen(pType->szName) + XDS_CONFIG_TAGSIZE);
		for(int ii = 0; ii < pType->var.enum_t.iEnumCount; ++ii)
		{
			struct xdsEnum_t *pEnum = &pType->var.enum_t.pEnums[ii];
			iLen += pType->var.enum_t.iValueSize + (strlen(pEnum->szName) + XDS_CONFIG_TAGSIZE);
		}

		hXds->xBuf.WriteShort(XDS_RECORD_TYPEDEF);
		hXds->xBuf.WriteShort(iLen);
		hXds->xBuf.WriteTag(pType->szName, strlen(pType->szName));
		hXds->xBuf.WriteShort(iEnumType);
		hXds->xBuf.WriteShort(XDS_META_TYPE_ENUMERATION);
		hXds->xBuf.WriteShort(pType->var.enum_t.iEnumCount);
		for(int ii = 0; ii < pType->var.enum_t.iEnumCount; ++ii)
		{
			struct xdsEnum_t *pEnum = &pType->var.enum_t.pEnums[ii];
			hXds->xBuf.WriteTag(pEnum->szName, strlen(pEnum->szName));
			hXds->xBuf.WriteValue(pEnum->iValue, (u8)pType->var.enum_t.iValueSize);
		}
	}
	return true;
}
#endif

unsigned short xdsDefStringType(struct xdsHandle *hXds, const char *szTagName, unsigned short iEncoding,
								unsigned iLength, char iLengthSize)
{
	if(iEncoding != XDS_ENCODING_UTF8)
	{
		XDS_ERROR("Unsupported string encoding");
		return false;
	}

#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	struct xdsType_t *pType = xdsAllocType(hXds, szTagName);

	if(iLengthSize > 0)
	{
		pType->iNodeSize       = iLengthSize;
		pType->iNodeSizeLength = 0;
		pType->iNodeSizeType   = XDS_NODESIZE_FIXED;
	}
	else
	{
		pType->iNodeSize       = 0;
		pType->iNodeSizeLength = iLengthSize;
		pType->iNodeSizeType   = XDS_NODESIZE_INDIRECT;
	}

	pType->iType = XDS_META_TYPE_STRING;
#ifndef XDS_SUPPORT_COMPACTDSD
	pType->var.string_t.iLengthSize = iLengthSize;
	pType->var.string_t.iEncoding   = iEncoding;
	pType->var.string_t.iLength     = iLength;
#endif

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_TYPEDEF);
		hXds->xBuf.WriteShort(7 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE) + (iLength > 0 ? 2 : 0));
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pType->iTypeId);
		hXds->xBuf.WriteShort(XDS_META_TYPE_STRING);
		hXds->xBuf.WriteByte(iLength > 0 ? iLengthSize : -iLengthSize);
		hXds->xBuf.WriteShort(iEncoding);
		if(iLength > 0)
			hXds->xBuf.WriteValue(iLength, iLengthSize);
	}
#endif
	return pType->iTypeId;
}

unsigned short xdsDefFixedpointType(struct xdsHandle *hXds, const char *szTagName, unsigned char iTypeLength,
									unsigned char iFracLength)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	struct xdsType_t *pType = xdsAllocType(hXds, szTagName);

	pType->iNodeSize = iTypeLength;
	pType->iNodeSizeLength = 0;
	pType->iNodeSizeType = XDS_NODESIZE_FIXED;

	pType->iType = XDS_META_TYPE_FIXEDPOINT;
#ifndef XDS_SUPPORT_COMPACTDSD
	pType->var.fixed_t.iTypeLen = iTypeLength;
	pType->var.fixed_t.iFracLen = iFracLength;
#endif

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_TYPEDEF);
		hXds->xBuf.WriteShort(6 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE));
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pType->iTypeId);
		hXds->xBuf.WriteShort(XDS_META_TYPE_FIXEDPOINT);
		hXds->xBuf.WriteByte(iTypeLength);
		hXds->xBuf.WriteByte(iFracLength);
	}
#endif
	return pType->iTypeId;
}

unsigned short xdsDefFloatType(struct xdsHandle *hXds, const char *szTagName, unsigned char iTypeLength,
							   unsigned char iExpLength, unsigned char iPadLength, unsigned char iCoeffLength)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	struct xdsType_t *pType = xdsAllocType(hXds, szTagName);

	pType->iNodeSize       = iTypeLength;
	pType->iNodeSizeLength = 0;
	pType->iNodeSizeType   = XDS_NODESIZE_FIXED;

	pType->iType = XDS_META_TYPE_FLOATINGPOINT;
#ifndef XDS_SUPPORT_COMPACTDSD
	pType->var.float_t.iTypeLen  = iTypeLength;
	pType->var.float_t.iExpLen   = iExpLength;
	pType->var.float_t.iPadLen   = iPadLength;
	pType->var.float_t.iCoeffLen = iCoeffLength;
#endif

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_TYPEDEF);
		hXds->xBuf.WriteShort(8 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE));
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pType->iTypeId);
		hXds->xBuf.WriteShort(XDS_META_TYPE_FLOATINGPOINT);
		hXds->xBuf.WriteByte(iTypeLength);
		hXds->xBuf.WriteByte(iExpLength);
		hXds->xBuf.WriteByte(iPadLength);
		hXds->xBuf.WriteByte(iCoeffLength);
	}
#endif
	return pType->iTypeId;
}

#if 0
unsigned short xdsDefBitfieldType(struct xdsHandle *hXds, const char *szTagName, unsigned long iNumbits)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	struct xdsType_t *pType = xdsAllocType(hXds, szTagName);

	pType->iNodeSize       = iNumbits;
	pType->iNodeSizeLength = 0;
	pType->iNodeSizeType   = XDS_NODESIZE_BITFIELD | XDS_NODESIZE_FIXED;

	pType->iType = XDS_META_TYPE_BITFIELD;
	pType->var.bitfield_t.iBitCount = iNumbits;


#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_TYPEDEF);
		hXds->xBuf.WriteShort(8 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE));
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pType->iTypeId);
		hXds->xBuf.WriteShort(XDS_META_TYPE_BITFIELD);
		hXds->xBuf.WriteLong(iNumbits);
	}
#endif
	return pType->iTypeId;
}
#endif

unsigned short xdsDefArrayType(struct xdsHandle *hXds, const char *szTagName, unsigned short iBaseType, 
							   unsigned iLength, unsigned char iLengthSize)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	if(!iLengthSize)
	{
		XDS_ERROR("Missing length size in array definition");
		return 0;
	}
	struct xdsType_t *pType = xdsAllocType(hXds, szTagName);

	if(!getTypeSize(hXds, iBaseType, (struct xdsElement_t*)pType))
	{
		XDS_ERROR("Invalid base type in array definition");
		XDS_FREE(pType);
		return 0;
	}
	if(pType->iNodeSizeType != XDS_NODESIZE_FIXED)
	{
		XDS_ERROR("Attempted to use unsupported feature: variable size array elements");
		XDS_FREE(pType);
	}
	if(iLength)
	{
		// pType->iNodeSizeType = XDS_NODESIZE_FIXED;
		pType->iNodeSize *= iLength;
	}
	else
	{
		pType->iNodeSizeType = XDS_NODESIZE_MULTIPLE | XDS_NODESIZE_INDIRECT;
		pType->iNodeSizeLength = iLengthSize;
	}

	pType->iType = XDS_META_TYPE_ARRAY;
#ifndef XDS_SUPPORT_COMPACTDSD
	pType->var.array_t.iLengthSize = iLengthSize;
	pType->var.array_t.iBaseType = iBaseType;
	pType->var.array_t.iDefaultSize = iLength;
#endif

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		// write definition to output stream
		unsigned short iLen = 7 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE) + (iLength != 0 ? iLengthSize : 0);
		hXds->xBuf.WriteShort(XDS_RECORD_TYPEDEF);
		hXds->xBuf.WriteShort(iLen);
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pType->iTypeId);
		hXds->xBuf.WriteShort(XDS_META_TYPE_ARRAY);
		hXds->xBuf.WriteByte(iLength ? -iLengthSize : iLengthSize);
		hXds->xBuf.WriteShort(iBaseType);
		if(iLength)
			hXds->xBuf.WriteValue(iLength, iLengthSize);
	}
#endif

	return pType->iTypeId;
}

unsigned short xdsDefStructType(struct xdsHandle *hXds, const char *szTagName)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	struct xdsType_t *pType = xdsAllocType(hXds, szTagName);

	pType->iNodeSizeType   = XDS_NODESIZE_FIXED;
	pType->iNodeSizeLength = 0;
	pType->iNodeSize       = 0;

	pType->iType = XDS_META_TYPE_STRUCTURE;
#ifndef XDS_SUPPORT_COMPACTDSD
	pType->var.struct_t.iFieldCount = 0;
	pType->var.struct_t.pFields = NULL;
#endif
	return pType->iTypeId;
}

static unsigned int alignOffset(unsigned int iStructSize, unsigned int iFieldSize)
{
	unsigned int iPad;

	// use minimal alignment value
	if(iFieldSize > XDS_CONFIG_ALIGN)
		iFieldSize = XDS_CONFIG_ALIGN;

	// calculate pad required to move to multiple
	if(iPad = iStructSize % iFieldSize)
	{
		// no, move to multiple
		iPad = XDS_CONFIG_ALIGN - iPad;
	}
	return iStructSize + iPad;
}

bool xdsDefStructField(struct xdsHandle *hXds, unsigned short iStructType, char *szFieldName,
					   unsigned short iFieldType, unsigned iArraySize)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return false;
#endif
	if(!iArraySize)
		iArraySize = 1;
//	struct xdsType_t *pType = (struct xdsType_t*)hXds->pTypeDb->Find((unsigned char*)&iStructType, sizeof(iStructType));
	struct xdsType_t *pType = xdsFindType(hXds, iStructType);
	if(!pType)
		return false;

	// get field size to update structure size
	struct xdsElement_t elem;
	if(!getTypeSize(hXds, iFieldType, &elem))
	{
		XDS_ERROR("Invalid field type in structure definition");
		return false;
	}
	if(elem.iNodeSizeType != XDS_NODESIZE_FIXED)
	{
		XDS_ERROR("Invalid variable size field in structure definition");
	}

#ifndef XDS_SUPPORT_COMPACTDSD
	struct xdsField_t *pField = (struct xdsField_t*)XDS_ALLOC(pType->var.struct_t.pFields,
		sizeof(struct xdsField_t)*(pType->var.struct_t.iFieldCount+1), XDS_MEMTYPE_SCRATCH);
	if(!pField)
		return false;

	pType->var.struct_t.iFieldCount++;
	pType->var.struct_t.pFields = pField;

	pField += pType->var.struct_t.iFieldCount-1;
	pField->iType = iFieldType;
	pField->iArraySize = iArraySize;
	pField->szName = xdsStringDup(szFieldName);
	if(!pField->szName)
		return false;

	// update structure size
	pField->iOffset =
#endif
		pType->iNodeSize = alignOffset(pType->iNodeSize, elem.iNodeSize);
	pType->iNodeSize += elem.iNodeSize * iArraySize;

	return true;

}
bool xdsDefStructDone(struct xdsHandle *hXds, unsigned short iStructType)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return false;
#endif
//	struct xdsType_t *pType = (struct xdsType_t*)hXds->pTypeDb->Find((unsigned char*)&iStructType, sizeof(iStructType));
	struct xdsType_t *pType = xdsFindType(hXds, iStructType);
	if(!pType)
		return false;

	// set structure size to multiple of alignment size
	pType->iNodeSize = alignOffset(pType->iNodeSize, XDS_CONFIG_ALIGN);

#ifndef XDS_SUPPORT_COMPACTDSD
	if(!hXds->bReading)
	{
		unsigned short iLen = 6 + (strlen(pType->szName) + XDS_CONFIG_TAGSIZE);
		for(int ii = 0; ii < pType->var.struct_t.iFieldCount; ++ii)
		{
			struct xdsField_t *pField = &pType->var.struct_t.pFields[ii];
			iLen += 2*sizeof(u16) + (strlen(pField->szName) + XDS_CONFIG_TAGSIZE);
		}

		hXds->xBuf.WriteShort(XDS_RECORD_TYPEDEF);
		hXds->xBuf.WriteShort(iLen);
		hXds->xBuf.WriteTag(pType->szName, strlen(pType->szName));
		hXds->xBuf.WriteShort(iStructType);
		hXds->xBuf.WriteShort(XDS_META_TYPE_STRUCTURE);
		hXds->xBuf.WriteShort(pType->var.struct_t.iFieldCount);
		for(int ii = 0; ii < pType->var.struct_t.iFieldCount; ++ii)
		{
			struct xdsField_t *pField = &pType->var.struct_t.pFields[ii];
			hXds->xBuf.WriteTag(pField->szName, strlen(pField->szName));
			hXds->xBuf.WriteShort(pField->iType);
			hXds->xBuf.WriteShort(pField->iArraySize);
		}
	}
#endif
	return true;
}

#if 0
unsigned short xdsDefGraphType(struct xdsHandle *hXds, const char *szTagName, unsigned short iBaseType,
							   unsigned char iLengthSize)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	struct xdsType_t *pType = xdsAllocType(hXds, szTagName);

	pType->iNodeSize = 0;
	pType->iNodeSizeLength = iLengthSize;
	pType->iNodeSizeType = XDS_NODESIZE_INDIRECT;

	pType->iType = XDS_META_TYPE_GRAPH;
	pType->var.graph_t.iBaseType = iBaseType;
	pType->var.graph_t.iLengthSize = iLengthSize;

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_TYPEDEF);
		hXds->xBuf.WriteShort(7 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE));
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pType->iTypeId);
		hXds->xBuf.WriteShort(XDS_META_TYPE_GRAPH);
		hXds->xBuf.WriteShort(iBaseType);
		hXds->xBuf.WriteByte(iLengthSize);
	}
#endif
	return pType->iTypeId;
}
#endif

unsigned short xdsDefPointerType(struct xdsHandle *hXds, const char *szTagName, unsigned short iBaseType,
								 unsigned char iTypeLength)
{
#ifdef XDS_SUPPORT_SIGNATURE
	if(hXds->lDSDLength > 0)
		return 0;
#endif
	struct xdsType_t *pType = xdsAllocType(hXds, szTagName);
	
	pType->iNodeSize = iTypeLength;
	pType->iNodeSizeLength = 0;
	pType->iNodeSizeType = XDS_NODESIZE_FIXED;

	pType->iType = XDS_META_TYPE_POINTER;
#ifndef XDS_SUPPORT_COMPACTDSD
	pType->var.pointer_t.iBaseType = iBaseType;
	pType->var.pointer_t.iTypeLength = iTypeLength;
#endif

#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
	{
		hXds->xBuf.WriteShort(XDS_RECORD_TYPEDEF);
		hXds->xBuf.WriteShort(7 + (strlen(szTagName) + XDS_CONFIG_TAGSIZE));
		hXds->xBuf.WriteTag(szTagName, strlen(szTagName));
		hXds->xBuf.WriteShort(pType->iTypeId);
		hXds->xBuf.WriteShort(XDS_META_TYPE_POINTER);
		hXds->xBuf.WriteShort(iBaseType);
		hXds->xBuf.WriteByte(iTypeLength);
	}
#endif
	return pType->iTypeId;
}

//#endif // XDS_SUPPORT_DEFTYPE

