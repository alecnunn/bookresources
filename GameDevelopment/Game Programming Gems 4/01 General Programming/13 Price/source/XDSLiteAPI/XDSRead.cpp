// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include "XDSinternal.h"

// ****************************************************************************
// *** 
// *** Record processing
// *** 
// ****************************************************************************

static bool xdsProcessRecord(struct xdsHandle *hXds, int iRecLength)
{
	for(; iRecLength > 0; )
	{
		u16 nodetype = (u16)hXds->xBuf.ReadShort();
		u32 iNodeLength;
//		struct xdsElement_t *pItem = (struct xdsElement_t *)hXds->pTypeDb->Find((unsigned char *)&nodetype, sizeof(nodetype));
		struct xdsElement_t *pItem = (struct xdsElement_t *)xdsFindType(hXds, nodetype);
		if(nodetype != XDS_RECORD_DATA && !pItem)
		{
			XDS_ERROR("Bad node type in input stream");
			return false;
		}
		if(nodetype == XDS_RECORD_DATA || pItem->iClass == XDS_RECORD_RECORDDEF)
		{
			// process subrecord...
			XDS_PROCESSNODE(nodetype, NULL, 0);

			u16 iLengthSize = sizeof(u16);

			if(pItem)
				iLengthSize = ((struct xdsRecord_t*)pItem)->iNodeSizeLength;
			iNodeLength = hXds->xBuf.ReadValue((u8)iLengthSize);
			if(!xdsProcessRecord(hXds, iNodeLength))
				return false;
			iNodeLength += iLengthSize;
		}
		else if(pItem->iClass == XDS_RECORD_ELEMDEF || pItem->iClass == XDS_RECORD_ATTRIBDEF)
		{
			// allocate space for element data
			if(pItem->iNodeSizeType & XDS_NODESIZE_INDIRECT)
			{
				iNodeLength = hXds->xBuf.ReadValue(pItem->iNodeSizeLength);
				if(pItem->iNodeSizeType & XDS_NODESIZE_MULTIPLE)
					iNodeLength *= pItem->iNodeSize;
			}
			else if(pItem->iNodeSizeType & XDS_NODESIZE_FIXED)
				iNodeLength = pItem->iNodeSize;

#ifdef XDS_SUPPORT_ALLOCNODES
			unsigned char *pElement = (unsigned char *)XDS_ALLOC(NULL, iNodeLength, XDS_MEMTYPE_NODE);
			hXds->xBuf.Read((unsigned char *)pElement, iNodeLength);
#else
#ifndef XDS_SUPPORT_READBYRECORD
			hXds->xBuf.Fill(iNodeLength);
#endif
			unsigned char *pElement = (unsigned char *)hXds->xBuf.Cursor();
#endif

			XDS_PROCESSNODE(nodetype, pElement, iNodeLength);
#ifndef XDS_SUPPORT_ALLOCNODES
			hXds->xBuf.Skip(iNodeLength);
#endif
			iNodeLength += sizeof(u16) + pItem->iNodeSizeLength;
		}
		else
		{
			XDS_ERROR("Bad node type in input stream");
			return false;
		}

		iRecLength -= iNodeLength;
	}

	return iRecLength == 0;
}

bool xdsReadRecord(struct xdsHandle *hXds)
{
	if(hXds->bEndOfStream)
		return false;
#ifdef XDS_SUPPORT_WRITING
	if(!hXds->bReading)
		return false;
#endif
#ifdef XDS_SUPPORT_XML
	if(!hXds->bNative)
		return xdsXmlReadRecord(hXds);
#endif

	for(;;)
	{
		int rectype = hXds->xBuf.ReadShort();
		if(rectype < 0)
			return false;

		u32 iLength;
		u16 iLengthSize = sizeof(u16);

		switch(rectype)
		{
		case XDS_RECORD_ENDOFDATA:	// End-of-Stream record
			hXds->bEndOfStream = true;
			XDS_PROCESSNODE(XDS_RECORD_ENDOFDATA, NULL, 0);
			return false;

		case XDS_RECORD_ENCODING:	// String Encoding Definition record
		case XDS_RECORD_STREAMNAME:	// Stream Name record
		case XDS_RECORD_RECORDDEF:	// Record Definition record
		case XDS_RECORD_TYPEDEF:	// Type/Element Definition record
		case XDS_RECORD_ELEMDEF:	// Type/Element Definition record
		case XDS_RECORD_ATTRIBDEF:
			iLength = hXds->xBuf.LookaheadShort();
			hXds->xBuf.Fill(iLength+2);
			xdsProcessDefinition(hXds, rectype, &hXds->xBuf);
			break;

		case XDS_RECORD_COMMENT:	// Comment record
			iLength = hXds->xBuf.ReadLong();
			hXds->xBuf.Fill(iLength);
			XDS_PROCESSNODE(XDS_RECORD_COMMENT, (void*)hXds->xBuf.Cursor(), iLength);
			hXds->xBuf.Skip(iLength);
			break;

		default:
		{
			// verify that it is a user defined data record...
//			struct xdsRecord_t *pItem = (struct xdsRecord_t *)hXds->pTypeDb->Find((unsigned char *)&rectype, sizeof(rectype));
			struct xdsRecord_t *pItem = (struct xdsRecord_t *)xdsFindType(hXds, rectype);
			if(!pItem)
			{
				XDS_ERROR("Bad record type in input stream");
				return false;
			}
			if(pItem->iClass != XDS_RECORD_RECORDDEF)
			{
				XDS_ERROR("Invalid data in input stream (expected record ID)");
				return false;
			}
			iLengthSize = pItem->iNodeSizeLength;
		}
		// fall through...
		case XDS_RECORD_DATA:
			// force entire record into memory
			iLength = hXds->xBuf.ReadValue((u8)iLengthSize);
#ifdef XDS_SUPPORT_READBYRECORD
			hXds->xBuf.Fill(iLength);
#endif
			XDS_PROCESSNODE(rectype, NULL, 0);
			return xdsProcessRecord(hXds, iLength);

		case XDS_RECORD_SIGNATURE:	// DSD signature (make sure it matches)
#ifdef XDS_SUPPORT_SIGNATURE
			if(hXds->lDSDLength)
			{
				(void)hXds->xBuf.Skip(2);
				u32 iSigDSDLen = hXds->xBuf.ReadLong();
				u32 iSigDSDCRC = hXds->xBuf.ReadLong();
				(void)hXds->xBuf.Skip(sizeof(u32));

				if(hXds->lDSDLength != iSigDSDLen || hXds->lDSDCRC != iSigDSDCRC)
					XDS_ERROR("Stream signature does not match DSD");
			}
			else
#else
			hXds->xBuf.Skip(2 * 3*sizeof(u32));		// skip length + payload
#endif
			break;

		// The following record types are currently ignored by XDS-lite

		case XDS_ENCODING_UTF8:		// currently only supports UTF8 for tagnames
		case XDS_ENCODING_UTF16:
			break;
		}
	}
}