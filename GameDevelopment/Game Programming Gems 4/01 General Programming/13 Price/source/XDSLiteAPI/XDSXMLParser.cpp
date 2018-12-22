// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include "XDSinternal.h"
#include <ctype.h>

#ifdef XDS_SUPPORT_XML

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Forward Declarations
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// ---------------------------------------------------------------------------
//  Helper classes
// ---------------------------------------------------------------------------

// - - - XML Tag stack - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define xdsTagStack_BUFSIZE 256		// default length of tag buffer
#define xdsTagStack_DEPTH 8			// default depth of tag stack

struct xdsXmlTag_t
{
	u16  iClass;	// one of XDS_RECORD_DOCNAME, XDS_RECORD_RECORDDEF or XDS_RECORD_ELEMDEF
	u16  iTypeId;
	int  iTagPos;
};

class CxdsXmlTagStack
{
	int m_iTagPos;			// top of tag stack
	int m_iTagSize;			// allocated size of tag stack
	int m_iNamePos;			// last used char of tag string
	int m_iNameSize;		// allocated size of tag string
	struct xdsXmlTag_t *m_pTags;
	char *m_szTagNames;

public:
	CxdsXmlTagStack::CxdsXmlTagStack();
	CxdsXmlTagStack::~CxdsXmlTagStack();

	bool Top(u16 &iClass, u16 &iTypeId, char *&szName);
	void Push(u16 iClass, u16 iTypeId, char *szName, int iLength);
	void Pop(void);

//	const char *TopName()  { (m_iTagPos > 0) ? m_szTagNames[m_pTags[m_iTagPos-1].iTagPos] : NULL; };
//	const u16   TopClass() { (m_iTagPos > 0) ? m_pTags[m_iTagPos-1].iClass : 0; };
//	const u16   TopType()  { (m_iTagPos > 0) ? m_pTags[m_iTagPos-1].iTypeId : 0; };

	int Depth() { return m_iTagPos; };
};

CxdsXmlTagStack::CxdsXmlTagStack()
: m_iTagPos(0), m_iTagSize(xdsTagStack_BUFSIZE), m_iNamePos(0), m_iNameSize(xdsTagStack_DEPTH)
{
	m_pTags      = (struct xdsXmlTag_t *)XDS_ALLOC(NULL, m_iTagSize, XDS_MEMTYPE_SCRATCH);
	m_szTagNames = (char *)XDS_ALLOC(NULL, m_iNameSize, XDS_MEMTYPE_SCRATCH);
}
CxdsXmlTagStack::~CxdsXmlTagStack()
{
	XDS_FREE(m_pTags);
	XDS_FREE(m_szTagNames);
}

bool CxdsXmlTagStack::Top(u16 &iClass, u16 &iTypeId, char *&szName)
{
	if(m_iTagPos <= 0)
		return false;

	iClass = m_pTags[m_iTagPos-1].iClass;
	iTypeId = m_pTags[m_iTagPos-1].iTypeId;
	szName = &m_szTagNames[m_pTags[m_iTagPos-1].iTagPos];

	return true;
}

void CxdsXmlTagStack::Push(u16 iClass, u16 iTypeId, char *szName, int iLength)
{
	// add entry to stack (points to uninitialized buffer)
	if(m_iTagSize <= m_iTagPos)
	{
		m_iTagSize <<= 1;
		m_pTags = (struct xdsXmlTag_t*)XDS_ALLOC(m_pTags, m_iTagSize*sizeof(struct xdsXmlTag_t), XDS_MEMTYPE_SCRATCH);
	}
	m_pTags[m_iTagPos].iClass  = iClass;
	m_pTags[m_iTagPos].iTypeId = iTypeId;
	m_pTags[m_iTagPos].iTagPos = m_iNamePos;
	++m_iTagPos;

	// add tag name to tag name buffer
	if(m_iNameSize < (m_iNamePos+iLength+1))
	{
		m_iNameSize <<= 1;
		m_szTagNames = (char *)XDS_ALLOC(m_szTagNames, m_iNameSize, XDS_MEMTYPE_SCRATCH);
	}
	memcpy(m_szTagNames+m_iNamePos, szName, iLength);
	m_iNamePos += iLength;
	m_szTagNames[m_iNamePos++] = '\0';
}

void CxdsXmlTagStack::Pop(void)
{
	if(m_iTagPos > 0)
	{
		--m_iTagPos;
		m_iNamePos = m_pTags[m_iTagPos].iTagPos;
	}
}

// - - - Generic XML Node  - - - - - - - - - - - - - - - - - - - - - - - - - -

class CxdsXmlNode {
	char *m_szTagName;
	char *m_szText;

	CxdsXmlNode *m_pAttributes;
	CxdsXmlNode *m_pChildren;
	CxdsXmlNode *m_pParent;

	CxdsXmlNode *m_pNext;

public:
	CxdsXmlNode::CxdsXmlNode(char *szTagName);
	CxdsXmlNode::~CxdsXmlNode();

	const char *GetName(void) const { return m_szTagName; };
	const char *GetText(void) const { return m_szText;    };
	void SetName(char *szTagName);
	void SetText(char *szTagName);

	CxdsXmlNode *GetAttribute(unsigned int iAttribNum) const;
	CxdsXmlNode *GetChild(unsigned int iChildNum) const;
	CxdsXmlNode *GetAttribute(const char *szName) const;
	CxdsXmlNode *GetChild(const char *szName) const;

	CxdsXmlNode *GetParent(void) const { return m_pParent; };

	void AddChild(CxdsXmlNode *pChild);
	void AddAttribute(CxdsXmlNode *pAttrib);

	int NumChildren();
	int NumAttributes();
};

CxdsXmlNode::CxdsXmlNode(char *szTagName)
: m_szTagName(szTagName), m_szText(NULL), m_pAttributes(NULL), m_pChildren(NULL), m_pParent(NULL), m_pNext(NULL)
{
}

CxdsXmlNode::~CxdsXmlNode()
{
	XDS_FREE(m_szTagName);
	XDS_FREE(m_szText);
	delete m_pAttributes;
	delete m_pChildren;
	delete m_pNext;
}

void CxdsXmlNode::SetName(char *szTagName)
{
	XDS_FREE(m_szTagName);
	m_szTagName = szTagName;
}

void CxdsXmlNode::SetText(char *szText)
{
	XDS_FREE(m_szText);
	m_szText = (char *)XDS_ALLOC(NULL, strlen(szText)+1, XDS_MEMTYPE_SCRATCH);
	strcpy(m_szText, szText);
}

void CxdsXmlNode::AddChild(CxdsXmlNode *pChild)
{
	pChild->m_pParent = this;

	if(!m_pChildren)
		m_pChildren = pChild;
	else
	{
		CxdsXmlNode *pNode = m_pChildren;
		while(pNode->m_pNext != NULL)
			pNode = pNode->m_pNext;
		pNode->m_pNext = pChild;
	}
}

void CxdsXmlNode::AddAttribute(CxdsXmlNode *pAttrib)
{
	pAttrib->m_pParent = this;
	if(!m_pAttributes)
		m_pAttributes = pAttrib;
	else
	{
		CxdsXmlNode *pNode = m_pAttributes;
		while(pNode->m_pNext != NULL)
			pNode = pNode->m_pNext;
		pNode->m_pNext = pAttrib;
	}
}

CxdsXmlNode *CxdsXmlNode::GetAttribute(unsigned int iAttribNum) const
{
	CxdsXmlNode *pAttrib = m_pAttributes;
	while(iAttribNum-- && pAttrib)
	{
		pAttrib = pAttrib->m_pNext;
	}
	return pAttrib;
}
CxdsXmlNode *CxdsXmlNode::GetAttribute(const char *szName) const
{
	CxdsXmlNode *pAttrib = m_pAttributes;
	while(pAttrib)
	{
		if(!strcmp(pAttrib->m_szTagName, szName))
			return pAttrib;
		pAttrib = pAttrib->m_pNext;
	}
	return NULL;
}
int CxdsXmlNode::NumAttributes()
{
	CxdsXmlNode *pAttrib = m_pAttributes;
	int iCount;
	for(iCount = 0; pAttrib; pAttrib = pAttrib->m_pNext, ++iCount)
		;
	return iCount;
}

CxdsXmlNode *CxdsXmlNode::GetChild(unsigned int iChildNum) const
{
	CxdsXmlNode *pChild = m_pChildren;
	while(iChildNum-- && pChild)
	{
		pChild = pChild->m_pNext;
	}
	return pChild;
}
CxdsXmlNode *CxdsXmlNode::GetChild(const char *szName) const
{
	CxdsXmlNode *pChild = m_pChildren;
	while(pChild)
	{
		if(!strcmp(pChild->m_szTagName, szName))
			return pChild;
		pChild = pChild->m_pNext;
	}
	return NULL;
}
int CxdsXmlNode::NumChildren()
{
	CxdsXmlNode *pChild = m_pChildren;
	int iCount;
	for(iCount = 0; pChild; pChild = pChild->m_pNext, ++iCount)
		;
	return iCount;
}



// ---------------------------------------------------------------------------
//  Main XML Parser logic
// ---------------------------------------------------------------------------

// skip directive or comment
static bool xmlSkipDirective(struct xdsHandle *hXds)
{
	for(int iDist = 2; ; ++iDist)			// start with iDist=2 to skip opening "<?" or "<!"
	{
		int ch;
		if((ch = hXds->xBuf.Lookahead(iDist)) == '<')
		{
			hXds->xBuf.Skip(iDist-1);

			xmlSkipDirective(hXds);		// skip nested directives
			iDist = 0;
		}
		else if(ch == '>')					// match end of directive?
		{
			hXds->xBuf.Skip(iDist+1);
			return true;
		}
		else if(ch == -1)
			return false;
	}
}


char *xmlText(struct xdsHandle *hXds, char cTerm)
{
	char *szText;

	if(!cTerm && hXds->xBuf.MatchString("<![CDATA[", 9))
	{
		hXds->xBuf.Skip(9);
		int iDataLen = hXds->xBuf.LookaheadFind("]]>", 3);
		if(iDataLen < 0)
		{
			XDS_ERROR("Missing end of CDATA section");
			return NULL;
		}
		szText = xdsStringNDup((const char *)hXds->xBuf.Cursor(), iDataLen);
		hXds->xBuf.Skip(iDataLen+3);
	}
	else
	{
		if(!cTerm) cTerm = '<';
		hXds->xBuf.SkipWhite();
		int iDataLen = 0;
		while(hXds->xBuf.Lookahead(iDataLen) != cTerm)
			++iDataLen;
		szText = xdsStringNDup((const char *)hXds->xBuf.Cursor(), iDataLen);
		hXds->xBuf.Skip(iDataLen);
	}
	return szText;
}

#ifdef XDS_SUPPORT_XMLCOMMENTS
static bool xmlComment(struct xdsHandle *hXds)
{
	hXds->xBuf.Skip(4);	// skip over '<!--'
	int iCommentLen = hXds->xBuf.LookaheadFind("-->", 3);
	if(iCommentLen < 0)
	{
		XDS_ERROR("Missing end of XML comment");
		return false;
	}
	char *szComment = xdsStringNDup((const char *)hXds->xBuf.Cursor(), iCommentLen);
	hXds->xBuf.Skip(iCommentLen + 3);

	XDS_PROCESSNODE(XDS_RECORD_COMMENT, szComment, iCommentLen);
	XDS_FREE(szComment);

	return true;
}
#endif


// scan tag name from input (returns tag length)
static int xmlTagName(struct xdsHandle *hXds)
{
	int ch = hXds->xBuf.Lookahead(0);

	if(!isalpha(ch))
		XDS_ERROR("Non-alpha start character in XML tag");

	// scan for tag end
	for(int ii = 1; ch = hXds->xBuf.Lookahead(ii), (isalnum(ch) || ch == ':' || ch == '_' || ch == '-'); ++ii)
		;

	return ii;
}


static CxdsXmlNode *xmlAttribute(struct xdsHandle *hXds)
{
	hXds->xBuf.SkipWhite();

	int iTagLen = xmlTagName(hXds);
	char *szAttribName = xdsStringNDup((const char *)hXds->xBuf.Cursor(), iTagLen);
	hXds->xBuf.Skip(iTagLen);
	if(hXds->xBuf.ReadByte() != '=')
	{
		XDS_ERROR("Invalid attribute (no '=')");
		delete szAttribName;
		return NULL;
	}
	char cQuote = hXds->xBuf.ReadByte();
	if(cQuote != '"' && cQuote != '\'')
	{
		XDS_ERROR("Unsupported unquoted attribute");
		delete szAttribName;
		return NULL;
	}

	CxdsXmlNode *pAttrib = new CxdsXmlNode(szAttribName);
	pAttrib->SetText(xmlText(hXds, cQuote));
	hXds->xBuf.Skip(1); // skip quote

	return pAttrib;
}

static bool xmlReadDocname(struct xdsHandle *hXds)
{
	if(hXds->xBuf.ReadByte() != '<')
	{
		XDS_ERROR("Missing XML tag open ('<')");
		return false;
	}

	int iTagLen = xmlTagName(hXds);
	char *szDocName = xdsStringNDup((const char *)hXds->xBuf.Cursor(), iTagLen);
	hXds->xBuf.Skip(iTagLen);
	hXds->szStreamName = szDocName;

	hXds->pTagStack->Push(XDS_RECORD_STREAMNAME, 0, szDocName, iTagLen);

	// skip all attributes on document tag
	while(isspace(hXds->xBuf.Lookahead(0)))
	{
		CxdsXmlNode *pAttrib = xmlAttribute(hXds);
		if(!pAttrib)
		{
			return false;
		}
		delete pAttrib;
	}

	if(hXds->xBuf.ReadByte() != '>')
	{
		XDS_ERROR("Missing XML tag close ('>')");
		return false;
	}

	return true;
}

// this routine parses a single record out of the open XML stream
// it assumes that the prologs and document name have already been
// read from the stream
static CxdsXmlNode *xmlParseRecord(struct xdsHandle *hXds)
{
	CxdsXmlNode *pTopNode = NULL;
	CxdsXmlNode *pCurNode = NULL;

	for(;;)
	{
		hXds->xBuf.SkipWhite();
		if(hXds->xBuf.MatchString("<!--", 4))
		{
#ifdef XDS_SUPPORT_XMLCOMMENTS
			if(!xmlComment(hXds))
#else
			if(!xmlSkipDirective(hXds))
#endif
			{
				delete pTopNode;
				return NULL;
			}
		}
		else if(hXds->xBuf.MatchString("</", 2))
		{
			hXds->xBuf.Skip(2);
			int iTagLen = xmlTagName(hXds);
			if(hXds->xBuf.Lookahead(iTagLen) != '>')
			{
				XDS_ERROR("Missing end of close tag");
				delete pTopNode;
				return NULL;
			}

			char *szTagName = xdsStringNDup((const char *)hXds->xBuf.Cursor(), iTagLen);
			hXds->xBuf.Skip(iTagLen+1);

			if(pCurNode)
			{
				if(strcmp(pCurNode->GetName(), szTagName))
				{
					XDS_ERROR("Mismatched close tag");
					XDS_FREE(szTagName);
					delete pTopNode;
					return NULL;
				}
			}
			else
			{
				if(strcmp(hXds->szStreamName, szTagName))
				{
					XDS_ERROR("Mismatched document close tag");
					XDS_FREE(szTagName);
					delete pTopNode;
					return NULL;
				}
				// reached end of document
				XDS_FREE(szTagName);
				hXds->bEndOfStream = true;
				XDS_PROCESSNODE(XDS_RECORD_ENDOFDATA, NULL, 0);
				return NULL;
			}
			XDS_FREE(szTagName);

			// pop the open tag
			pCurNode = pCurNode->GetParent();
			if(!pCurNode)
			{
				// reached end of record
				return pTopNode;
			}
		} 
		else if(hXds->xBuf.MatchString("<", 1))
		{
			hXds->xBuf.Skip(1);
			int iTagLen = xmlTagName(hXds);
			char *szTagName = xdsStringNDup((const char *)hXds->xBuf.Cursor(), iTagLen);
			hXds->xBuf.Skip(iTagLen);

			CxdsXmlNode *pNode = new CxdsXmlNode(szTagName);

			// parse attributes (if any)
			while(isspace(hXds->xBuf.Lookahead(0)))
			{
				CxdsXmlNode *pAttrib = xmlAttribute(hXds);
				if(!pAttrib)
				{
					delete pTopNode;
					return false;
				}

				pNode->AddAttribute(pAttrib);
			}

			// match end of tag
			if(hXds->xBuf.MatchString(">", 1))
			{
				hXds->xBuf.Skip(1);
				pNode->SetText(xmlText(hXds, 0));
			}
			else if(hXds->xBuf.MatchString("/>", 2))
			{
				hXds->xBuf.Skip(2);
			}

			if(pTopNode == NULL)
			{
				pTopNode = pCurNode = pNode;
			}
			else
			{
				pCurNode->AddChild(pNode);
				pCurNode = pNode;
			}
		}
	}
}


// - - - XML Decoding  - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static u16 getBaseType(struct xdsHandle *hXds, unsigned short iId)
{
	// search for root type
	for(;;)
	{
//		struct xdsType_t *pItem = (struct xdsType_t *)hXds->pTypeDb->Find((unsigned char *)&iId, sizeof(iId));
		struct xdsType_t *pItem = xdsFindType(hXds, iId);
		if(!pItem)
		{
			return iId;
		}
		else
		{
			// recurse to base type
			iId = pItem->iType;
		}
	}
}

static int xdsXmlDecodeInt(CxdsXmlNode *pXmlNode, u16 iType, struct xdsType_t *pType, u8 *pElemData)
{
	unsigned long lval = strtol(pXmlNode->GetText(), NULL, 0);
	int iValSize;
	switch(iType)
	{
	case XDS_TYPE_CHAR:
	case XDS_TYPE_BYTE:
		iValSize = sizeof(u8);
		break;

	case XDS_TYPE_SHORT:
	case XDS_TYPE_WORD:
		iValSize = sizeof(u16);
		break;

	case XDS_TYPE_LONG:
	case XDS_TYPE_DWORD:
		iValSize = sizeof(u32);
		break;

	case XDS_META_TYPE_INTEGER:
		iValSize = pType->var.int_t.iValueSize;
		break;
	case XDS_META_TYPE_ENUMERATION:
		iValSize = pType->var.enum_t.iValueSize;
		break;
	}

	if(iValSize == sizeof(u16))
	{
		*(u16*)pElemData = (u16)lval;
		return sizeof(u16);
	}
	else if(iValSize == sizeof(u32))
	{
		*(u32*)pElemData = (u32)lval;
		return sizeof(u32);
	}
	else if(iValSize == sizeof(u8))
	{
		*(u8*)pElemData = (u8)lval;
		return sizeof(u8);
	}
	else
	{
		XDS_ERROR("Unsupported integer value size with XML");
		return -1;
	}
}
static int xdsXmlDecodeString(CxdsXmlNode *pXmlNode, u16 iType, struct xdsType_t *pType, u8 *pElemData)
{
	int iLen = strlen(pXmlNode->GetText());
	strcpy((char*)pElemData, pXmlNode->GetText());
	return iLen;
}
static int xdsXmlDecodeFloat(CxdsXmlNode *pXmlNode, u16 iType, struct xdsType_t *pType, u8 *pElemData)
{
	double dval = strtod(pXmlNode->GetText(), NULL);
	switch(iType)
	{
	case XDS_TYPE_FLOAT:
		*(float*)pElemData = (float)dval;
		return sizeof(float);
	case XDS_TYPE_DOUBLE:
		*(double*)pElemData = dval;
		return sizeof(double);

	case XDS_META_TYPE_FIXEDPOINT:
	case XDS_META_TYPE_FLOATINGPOINT:
	default:
		XDS_ERROR("Unsupported floating point type with XML");
		return -1;
	}
}
// this routine is recursive
static int xdsXmlDecodeElement(struct xdsHandle *hXds, CxdsXmlNode *pXmlNode, u16 iType, struct xdsType_t *pType, u8 *pElemData)
{
	switch(iType)
	{
	case XDS_META_TYPE_INTEGER:
	case XDS_META_TYPE_ENUMERATION:
	case XDS_TYPE_CHAR:
	case XDS_TYPE_BYTE:
	case XDS_TYPE_SHORT:
	case XDS_TYPE_WORD:
	case XDS_TYPE_LONG:
	case XDS_TYPE_DWORD:
		return xdsXmlDecodeInt(pXmlNode, iType, pType, pElemData);

	case XDS_META_TYPE_STRING:
	case XDS_TYPE_BCSTRING:
	case XDS_TYPE_WCSTRING:
		return xdsXmlDecodeString(pXmlNode, iType, pType, pElemData);

	case XDS_META_TYPE_FIXEDPOINT:
	case XDS_META_TYPE_FLOATINGPOINT:
	case XDS_TYPE_FLOAT:
	case XDS_TYPE_DOUBLE:
		return xdsXmlDecodeFloat(pXmlNode, iType, pType, pElemData);

	case XDS_META_TYPE_POINTER:
		*(u32*)pElemData = 0;
		return sizeof(u32);

	case XDS_META_TYPE_ARRAY:
	{
//		struct xdsType_t *pBaseType = (struct xdsType_t *)hXds->pTypeDb->Find((unsigned char*)&pType->var.array_t.iBaseType, sizeof(pType->var.array_t.iBaseType)); 
		struct xdsType_t *pBaseType = xdsFindType(hXds, pType->var.array_t.iBaseType);
		for(int ii = 0; ii < pXmlNode->NumChildren(); ++ii)
		{
			if(!xdsXmlDecodeElement(hXds, pXmlNode->GetChild(ii), pType->var.array_t.iBaseType, pBaseType, pElemData))
				return -1;
			pElemData += pType->iNodeSize;
		}
		return ii*pType->iNodeSize;
	}
	case XDS_META_TYPE_STRUCTURE:
	{
		// convert each of the structure fields
		struct xdsField_t *pField = pType->var.struct_t.pFields;
		for(int ii = 0; ii < pType->var.struct_t.iFieldCount; ++ii, ++pField)
		{
			// find appropriate child for field
			CxdsXmlNode *pXmlField = pXmlNode->GetChild(pField->szName);
			if(!pXmlField)
			{
#ifdef XDS_CONFIG_XMLALLSTRUCTFIELDS
				XDS_ERROR("Missing structure field");
				return -1;
#else
				continue;
#endif
			}

//            struct xdsType_t *pBaseType = (struct xdsType_t *)hXds->pTypeDb->Find((unsigned char *)&pField->iType,
//				sizeof(pField->iType));
			struct xdsType_t *pBaseType = xdsFindType(hXds, pField->iType);
			// handle simple arrays
			if(pField->iArraySize > 1)
			{
				if(pField->iType == XDS_TYPE_CHAR)	// treat simple arrays of char as strings
				{
					if(xdsXmlDecodeElement(hXds, pXmlField, XDS_TYPE_WCSTRING, NULL, pElemData+pField->iOffset) < 0)
						return -1;
				}
				else
				{
					int iOffset = pField->iOffset;
					for(int ii = 0; ii < pField->iArraySize; ++ii)
					{
						int iSize = xdsXmlDecodeElement(hXds, pXmlNode->GetChild(ii), pField->iType, pBaseType, pElemData+iOffset);
						if(iSize < 0)
							return -1;

						iOffset += iSize;

					}
					return iOffset;
				}
			}
			else
			{
				if(xdsXmlDecodeElement(hXds, pXmlField, pField->iType, pBaseType, pElemData+pField->iOffset) < 0)
					return -1;
			}
		}
		return pType->iNodeSize;
	}

//	case XDS_META_TYPE_BITFIELD:
//	case XDS_META_TYPE_GRAPH:
	default:
	{
//		struct xdsType_t *pBaseType = (struct xdsType_t *)hXds->pTypeDb->Find((unsigned char *)&iType, sizeof(iType));
		struct xdsType_t *pBaseType = xdsFindType(hXds, iType);
		if(pBaseType)
		{
			return xdsXmlDecodeElement(hXds, pXmlNode, pBaseType->iType, pBaseType, pElemData);
		}
		else
		{
			XDS_ERROR("Unsupported type in XML conversion");
			return -1;
		}
	}
	}
}

static bool xdsXmlProcElement(struct xdsHandle *hXds, CxdsXmlNode *pXmlNode, struct xdsElement_t *pElement)
{
	// allocate space for node...
	int iElemSize, iDataSize;
	int iBaseType = getBaseType(hXds, pElement->iType);

	if(pElement->iNodeSizeType & XDS_NODESIZE_FIXED)
	{
		iElemSize = iDataSize = pElement->iNodeSize;
	}
	else
	{
		// Must calculate size.  First find out number of entries.
		// Currently, we only have to worry about Strings (indirect) and Arrays (multiple, indirect)
		switch(iBaseType)
		{
		case XDS_META_TYPE_ARRAY:
			// size is number of children * array element size
			iDataSize = iElemSize = pXmlNode->NumChildren() * pElement->iNodeSize;
			break;

		case XDS_META_TYPE_STRING:
		case XDS_TYPE_BCSTRING:
		case XDS_TYPE_WCSTRING:
			// size is length of string
			iElemSize = strlen(pXmlNode->GetText());
			iDataSize = iElemSize + 1;

//		case XDS_META_TYPE_GRAPH:
		default:
			XDS_ERROR("Unrecoverable error: bogus or unsupported base type found in xdsXmlProcElement");
			return false;
		}
	}

	unsigned char *pElemData;
	pElemData = (unsigned char*)XDS_ALLOC(NULL, iDataSize, XDS_MEMTYPE_NODE);
	memset(pElemData, 0, iDataSize);

	// now convert data into buffer
	if(xdsXmlDecodeElement(hXds, pXmlNode, pElement->iType, (struct xdsType_t*)pElement, pElemData) < 0)
	{
		XDS_FREE(pElemData);
		return false;
	}

	XDS_PROCESSNODE(pElement->iTypeId, pElemData, iElemSize);
#ifndef XDS_SUPPORT_ALLOCNODES
	XDS_FREE(pElemData);
#endif
	return true;
}


static bool xdsXmlProcRecord(struct xdsHandle *hXds, CxdsXmlNode *pXmlRecord)
{
	// validate that root of fragment is a valid record type
	struct xdsRecord_t *pRecord = (struct xdsRecord_t *)hXds->pTypeDbByName->Find((unsigned char*)pXmlRecord->GetName(), strlen(pXmlRecord->GetName()));
	if(!pRecord || pRecord->iClass != XDS_RECORD_RECORDDEF)
	{
		XDS_ERROR("Invalid Record name in XML text");
		return false;
	}
	XDS_PROCESSNODE(pRecord->iRecordId, NULL, 0);	// notify start of record

	int ii;
	CxdsXmlNode *pXmlNode;

	// process all attributes
	for(ii = 0; pXmlNode = pXmlRecord->GetAttribute(ii); ++ii)
	{
		struct xdsElement_t *pAttrib = (struct xdsElement_t *)hXds->pTypeDbByName->Find((unsigned char*)pXmlNode->GetName(), strlen(pXmlNode->GetName()));
		if(!pAttrib || pAttrib->iClass != XDS_RECORD_ATTRIBDEF)
		{
			XDS_ERROR("Invalid Attribute name in XML text");
			return false;
		}

		if(!xdsXmlProcElement(hXds, pXmlNode, pAttrib))
			return false;
	}

	// process all elements & sub-records
	for(ii = 0; pXmlNode = pXmlRecord->GetChild(ii); ++ii)
	{
		struct xdsElement_t *pElem = (struct xdsElement_t *)hXds->pTypeDbByName->Find((unsigned char*)pXmlNode->GetName(), strlen(pXmlNode->GetName()));
		if(!pElem || (pElem->iClass != XDS_RECORD_ELEMDEF && pElem->iClass != XDS_RECORD_RECORDDEF))
		{
			XDS_ERROR("Invalid Element name in XML text");
			return false;
		}

		if(pElem->iClass == XDS_RECORD_RECORDDEF)
		{
			if(!xdsXmlProcRecord(hXds, pXmlNode))
				return false;
		}
		else // if(pElem->iClass == XDS_RECORD_ELEMDEF)
		{
			if(!xdsXmlProcElement(hXds, pXmlNode, pElem))
				return false;
		}
	}

	return true;
}


// - - - Public API  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool xdsXmlReadHeader(struct xdsHandle *hXds)
{
	// initialize XML-specific values as long as we're here
	hXds->pTagStack = new CxdsXmlTagStack;

	for(;;)
	{
		hXds->xBuf.SkipWhite();

#ifdef XDS_SUPPORT_XMLCOMMENTS
		if(hXds->xBuf.MatchString("<!--", 4))
		{
			if(!xmlComment(hXds))
				return false;
		}
		else
#endif
			if(hXds->xBuf.MatchString("<?", 2) || hXds->xBuf.MatchString("<!", 2))
		{
			if(!xmlSkipDirective(hXds))
				return false;
		}
		else
			break;
	}
	return xmlReadDocname(hXds);
}

/*
void PrintXML(CxdsXmlNode *pNode, int iDepth)
{
	if(!iDepth)
		printf("\nXML DATA\n");

	printf("%*s%s", iDepth, "", pNode->GetName());
	if(pNode->GetText() && pNode->GetText()[0])
		printf("='%s'", pNode->GetText());

	int ii;
	CxdsXmlNode *pChild;

	for(ii = 0; pChild = pNode->GetAttribute(ii); ++ii)
	{
		printf("  (%s='%s')", pChild->GetName(), pChild->GetText());
	}
	printf("\n");

	for(ii = 0; pChild = pNode->GetChild(ii); ++ii)
	{
		PrintXML(pChild, iDepth+2);
	}
}
*/

bool xdsXmlReadRecord(struct xdsHandle *hXds)
{
	CxdsXmlNode *pNode = xmlParseRecord(hXds);
	if(!pNode)
		return false;

//PrintXML(pNode, 0);

	// convert & send notifications
	return xdsXmlProcRecord(hXds, pNode);
}

#endif // XDS_SUPPORT_XML