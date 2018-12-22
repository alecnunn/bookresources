#ifndef __CPARAMETERNODE_H
#define __CPARAMETERNODE_H

#include "ComponentEnums.h"

#define NULL_OFFSET -1

// Temporary forward declaration.
class CVector;
class CHash;

enum EParameterType
{
	EPT_OBJECT,
	EPT_COMPONENT,
	EPT_INT,
	EPT_FLOAT,
	EPT_BOOL,
	EPT_STRING,
	EPT_HASH,
	EPT_VECTOR,
};

enum EParameterResult
{
	EPR_OK,
	EPR_WRONG_TYPE,
	EPR_FIELD_NOT_FOUND
};

// CParameterNode is designed to be mapped on to our binary component file format. It doesn't
// construct or destruct any memory itself. It needs to be told where to look for the data
// before use. This is done by calling the static method SetBasePointer() .
class CParameterNode
{
public:
	CParameterNode()						{}

	CParameterNode	*GetFirstChild() const;
	CParameterNode	*GetNext() const;
	CParameterNode	*GetParent() const;
	static	void	SetBasePointer(CParameterNode *base)	{ mBasePointer = base; }

	CHash			GetName() const;
	CHash			GetParameterType() const;
	static EParameterResult	GetLastResult()		{ return mLastResult; }

	// Get data from this node
	EComponentTypeId	GetComponentType() const;
	int32				GetInt() const;
	float				GetFloat() const;
	bool				GetBool() const;
	const char			*GetString() const;
	CHash				GetHash() const;
	CVector				GetVector() const;


	// Get data from a child of this node. The parameter "name" is the name of the
	// child node from which to read the data.
	EComponentTypeId	GetComponentType(CHash name) const;
	int32				GetInt(CHash name) const;
	float				GetFloat(CHash name) const;
	bool				GetBool(CHash name) const;
	const char			*GetString(CHash name) const;
	CHash				GetHash(CHash name) const;
	CVector				GetVector(CHash name) const;

private:
	CParameterNode *GetChild(CHash name) const;

private:
	sint32			mFirstChildOffset;
	sint32			mNextOffset;
	sint32			mParentOffset;
	uint32			mType;
	uint32			mName;
	void			*mData; // Doesn't point to anything sensible, the data starts at address of mData

	static	CParameterNode		*mCurrentChild;
	static	EParameterResult	mLastResult;
	static	CParameterNode		*mBasePointer;
};

#endif //__CPARAMETERNODE_H