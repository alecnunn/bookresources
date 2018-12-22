#include "types.h"
#include "CParameterNode.h"
#include "Globals.h"
#include "CObjectManager.h"
#include "CVector.h"

CParameterNode		*CParameterNode::mCurrentChild = NULL;
EParameterResult	CParameterNode::mLastResult = EPR_OK;
CParameterNode		*CParameterNode::mBasePointer = NULL;

//****************************************************************
// Type defaults
int32				defaultInt = 0;
float				defaultFloat = 0.f;
EComponentTypeId	defaultComponentType = CID_INVALID;
CHash				defaultHash;
bool				defaultBool = false;
char				*defaultPointerString = NULL;
CVector				defaultVector;

//****************************************************************
// Type hashes
CHash	typeHashObject("OBJECT");
CHash	typeHashComponentType("COMPONENT");
CHash	typeHashInt("INT");
CHash	typeHashFloat("FLOAT");
CHash	typeHashBool("BOOL");
CHash	typeHashString("STRING");
CHash	typeHashHash("HASH");
CHash	typeHashVector("VECTOR");

CParameterNode *CParameterNode::GetFirstChild() const
{
	if (mFirstChildOffset == NULL_OFFSET)
	{
		return NULL;
	}
	return (CParameterNode*)((char *)mBasePointer + mFirstChildOffset);
}

CParameterNode *CParameterNode::GetNext() const
{
	if (mNextOffset == NULL_OFFSET)
	{
		return NULL;
	}
	return (CParameterNode*)((char *)mBasePointer + mNextOffset);
}

CParameterNode *CParameterNode::GetParent() const
{
	if (mParentOffset == NULL_OFFSET)
	{
		return NULL;
	}
	return (CParameterNode*)((char *)mBasePointer + mParentOffset);
}

CHash CParameterNode::GetName() const
{
	return mName;
}

CHash CParameterNode::GetParameterType() const
{
	return mType;
}

//****************************************************************
// Get data from this node

int32 CParameterNode::GetInt() const
{
	if (mType != typeHashInt)
	{
		mLastResult = EPR_WRONG_TYPE;
		return defaultInt;
	}
	mLastResult = EPR_OK;
	return *(reinterpret_cast<const int32*>(&mData));
}

bool CParameterNode::GetBool() const
{
	if (mType != typeHashBool)
	{
		mLastResult = EPR_WRONG_TYPE;
		return defaultBool;
	}
	mLastResult = EPR_OK;
	return *(reinterpret_cast<const bool*>(&mData));
}

float CParameterNode::GetFloat() const
{
	if (mType != typeHashFloat)
	{
		mLastResult = EPR_WRONG_TYPE;
		return defaultFloat;
	}
	mLastResult = EPR_OK;
	return *(reinterpret_cast<const float*>(&mData));
}

const char *CParameterNode::GetString() const
{
	if (mType != typeHashString)
	{
		mLastResult = EPR_WRONG_TYPE;
		return defaultPointerString;
	}
	mLastResult = EPR_OK;
	return (reinterpret_cast<const char*>(&mData));
}

CHash	CParameterNode::GetHash(void) const
{
	if (mType != typeHashHash)
	{
		mLastResult = EPR_WRONG_TYPE;
		return defaultHash;
	}
	mLastResult = EPR_OK;

	uint32 hashVal = *reinterpret_cast<const uint32 *>(&mData);
	return CHash(hashVal);
}

CVector	CParameterNode::GetVector(void) const
{
	if (mType != typeHashVector)
	{
		mLastResult = EPR_WRONG_TYPE;
		return defaultVector;
	}
	mLastResult = EPR_OK;

	CVector retVal;
	retVal.SetX((reinterpret_cast<const float*>(&mData))[0]);
	retVal.SetY((reinterpret_cast<const float*>(&mData))[1]);
	retVal.SetZ((reinterpret_cast<const float*>(&mData))[2]);
	return retVal;
}

EComponentTypeId CParameterNode::GetComponentType(void) const
{
	if (mType != typeHashComponentType)
	{
		mLastResult = EPR_WRONG_TYPE;
		return defaultComponentType;
	}
	mLastResult = EPR_OK;

	return Globals::GetObjectManager().GetComponentTypeFromHash(mName);
}


//****************************************************************
// Get data from a child of this node

EComponentTypeId CParameterNode::GetComponentType(CHash name) const
{
	CParameterNode *child = GetChild(name);
	if (child == NULL)
	{
		mLastResult = EPR_FIELD_NOT_FOUND;
		return defaultComponentType;
	}
	mLastResult = EPR_OK;
	return child->GetComponentType();
}

int32 CParameterNode::GetInt(CHash name) const
{
	CParameterNode *child = GetChild(name);
	if (child == NULL)
	{
		mLastResult = EPR_FIELD_NOT_FOUND;
		return defaultInt;
	}
	mLastResult = EPR_OK;
	return child->GetInt();
}

float CParameterNode::GetFloat(CHash name) const
{
	CParameterNode *child = GetChild(name);
	if (child == NULL)
	{
		mLastResult = EPR_FIELD_NOT_FOUND;
		return defaultFloat;
	}
	mLastResult = EPR_OK;
	return child->GetFloat();
}

bool CParameterNode::GetBool(CHash name) const
{
	CParameterNode *child = GetChild(name);
	if (child == NULL)
	{
		mLastResult = EPR_FIELD_NOT_FOUND;
		return defaultBool;
	}
	mLastResult = EPR_OK;
	return child->GetBool();
}

const char *CParameterNode::GetString(CHash name) const
{
	CParameterNode *child = GetChild(name);
	if (child == NULL)
	{
		mLastResult = EPR_FIELD_NOT_FOUND;
		return defaultPointerString;
	}
	mLastResult = EPR_OK;
	return child->GetString();
}

CHash CParameterNode::GetHash(CHash name) const
{
	CParameterNode *child = GetChild(name);
	if (child == NULL)
	{
		mLastResult = EPR_FIELD_NOT_FOUND;
		return defaultHash;
	}
	mLastResult = EPR_OK;
	return child->GetHash();
}

CVector CParameterNode::GetVector(CHash name) const
{
	CParameterNode *child = GetChild(name);
	if (child == NULL)
	{
		mLastResult = EPR_FIELD_NOT_FOUND;
		return defaultVector;
	}
	mLastResult = EPR_OK;
	return child->GetVector();
}


CParameterNode *CParameterNode::GetChild(CHash name) const
{
	for (CParameterNode *currentChild = GetFirstChild() ; currentChild != NULL ; currentChild = currentChild->GetNext())
	{
		if (currentChild->GetName() == name)
		{
			return currentChild;
		}
	}
	return NULL;
}
