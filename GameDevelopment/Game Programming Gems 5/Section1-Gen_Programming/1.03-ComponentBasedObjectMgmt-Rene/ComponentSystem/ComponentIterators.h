#ifndef __COMPONENTITERATORS_H
#define __COMPONENTITERATORS_H

#include "ComponentEnums.h"
#include "CObjectId.h"
#include "SObjectManagerDB.h"

class IComponent;

// This class iterates over all the components of a given type in the object manager db.
// It would be used like this:
// for (CComponentByTypeIterator iter(CID_MY_TYPE) ; !iter.IsDone() ; ++iter)
//		Do whatever it is you need to do with iter.GetComponent();
class CComponentByTypeIterator
{
public:
	CComponentByTypeIterator(EComponentTypeId type);
	CComponentByTypeIterator &operator++(); // Prefix increment operator
	CComponentByTypeIterator operator++(int); // Postfix increment operator

	bool		IsDone(void) const; // Have we iterated beyond the end?
	IComponent	*GetComponent(void) const;

	void		Reset(EComponentTypeId type);
private:
	CComponentMap::iterator mIter;
	EComponentTypeId	mType;
};

class CComponentByInterfaceIterator
{
public:
	CComponentByInterfaceIterator(EInterfaceId interfaceType);
	CComponentByInterfaceIterator &operator++();
	CComponentByInterfaceIterator operator++(int);

	bool		IsDone(void) const;
	IComponent	*GetComponent(void) const;
private:
	void SetToFirstValidComponent();

	std::set<EComponentTypeId>::iterator	mCompTypeIter;
	CComponentByTypeIterator				mCompIter;
	EInterfaceId							mInterfaceType;
};

class CComponentByObjectIterator
{
public:
	CComponentByObjectIterator(CObjectId oId);
	CComponentByObjectIterator &operator++();
	CComponentByObjectIterator operator++(int);

	bool		IsDone(void) const;
	IComponent	*GetComponent(void) const;
private:
	void SetToFirstValidComponent();

	CObjectId					mObjectId;
	int32						mIndex;
};

#endif //__COMPONENTITERATORS_H