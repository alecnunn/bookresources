// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include "XDSinternal.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Stream I/O
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


#ifdef XDS_SUPPORT_WRITING

#ifdef XDS_SUPPORT_SIGNATURE

bool xdsWriteSignature(struct xdsHandle *hXds)
{
	if(hXds->bReading)
		return false;

	if(!hXds->lDSDLength)
		return false;

	u32 sigCRC = xdsCRC((unsigned char *)&hXds->lDSDLength, 2*sizeof(unsigned long));

	if(hXds->xBuf.WriteShort(XDS_RECORD_SIGNATURE))
		if(hXds->xBuf.WriteShort(12))
			if(hXds->xBuf.WriteLong(hXds->lDSDLength))
				if(hXds->xBuf.WriteLong(hXds->lDSDCRC))
					if(hXds->xBuf.WriteLong(sigCRC))
						return true;

	return false;
}

#endif // XDS_SUPPORT_SIGNATURE

bool xdsWriteComment(struct xdsHandle *hXds, char *szComment)
{
	if(hXds->bReading)
		return false;

	if(hXds->xBuf.WriteShort(XDS_RECORD_COMMENT))
	{
		u32 len = strlen(szComment) + 1;
		if(hXds->xBuf.WriteLong(len))
			if(hXds->xBuf.Write(szComment, len))
			{
				if(!hXds->iRecNesting)
					hXds->xBuf.Flush();
				return true;
			}
	}

	return false;
}

bool xdsStartRecord(struct xdsHandle *hXds, unsigned short iType)
{
	if(hXds->iRecNesting >= 1)
		return false;

	struct xdsRecord_t *pRec = NULL;
	if(iType != XDS_RECORD_DATA)
	{
//		pRec = (struct xdsRecord_t *)hXds->pTypeDb->Find((unsigned char *)&iType, sizeof(iType));
		pRec = (struct xdsRecord_t *)xdsFindType(hXds, iType);
		if(!pRec)
			return false;
	}

	hXds->xBuf.WriteShort(iType);

	hXds->recEntry.iRecordType = iType;
	hXds->recEntry.lOffset     = hXds->xBuf.Cursor() - hXds->xBuf.Data();
	if(iType == XDS_RECORD_DATA)
		hXds->recEntry.iRecordSize = 2;
	else
		hXds->recEntry.iRecordSize = pRec->iNodeSizeLength;

	hXds->xBuf.WriteValue(0, (u8)hXds->recEntry.iRecordSize);

	++hXds->iRecNesting;
	return true;
}

bool xdsWriteNode(struct xdsHandle *hXds, unsigned short iType, void *data, int iCount)
{
	if(hXds->bEndOfStream)
		return false;

//	struct xdsElement_t *pElem = (struct xdsElement_t *)hXds->pTypeDb->Find((unsigned char*)&iType, sizeof(iType));
	struct xdsElement_t *pElem = (struct xdsElement_t *)xdsFindType(hXds, iType);
	if(!pElem)
		return false;
	if(pElem->iClass != XDS_RECORD_ELEMDEF && pElem->iClass != XDS_RECORD_ATTRIBDEF)
		return false;

	if(!hXds->xBuf.WriteShort(iType))
		return false;

	if(pElem->iNodeSizeType & XDS_NODESIZE_FIXED)
        return hXds->xBuf.Write(data, pElem->iNodeSize) != 0;
	else if(pElem->iNodeSizeType & XDS_NODESIZE_INDIRECT)
	{
		if(hXds->xBuf.WriteValue(iCount, pElem->iNodeSizeLength))
		{
			if(pElem->iNodeSizeType & XDS_NODESIZE_MULTIPLE)
				return hXds->xBuf.Write(data, pElem->iNodeSize * iCount) != 0;
			else
				return hXds->xBuf.Write(data, iCount) != 0;
		}
	}

	return false;
}

bool xdsEndRecord(struct xdsHandle *hXds)
{
	if(!hXds->iRecNesting)
		return false;
	--hXds->iRecNesting;

	// write record length to output at start of record
	unsigned char *pData = hXds->xBuf.Data() + hXds->recEntry.lOffset;

	hXds->xBuf.WriteValueAt(hXds->recEntry.lOffset,
		(hXds->xBuf.Cursor() - hXds->xBuf.Data()) - hXds->recEntry.lOffset - hXds->recEntry.iRecordSize,
		(u8)hXds->recEntry.iRecordSize);
	return true;
}


#endif // XDS_SUPPORT_WRITING
