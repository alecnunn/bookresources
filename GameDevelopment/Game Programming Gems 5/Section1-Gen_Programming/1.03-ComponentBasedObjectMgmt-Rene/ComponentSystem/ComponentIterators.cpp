#include "types.h"
#include "ComponentIterators.h"
#include "CObjectManager.h"
#include "SObjectManagerDB.h"
#include "Globals.h"
#include <assert.h>

///////////////////////////////////////////////////////
// CComponentByTypeIterator methods
///////////////////////////////////////////////////////

CComponentByTypeIterator::CComponentByTypeIterator(EComponentTypeId type)
{
	Reset(type);
}

CComponentByTypeIterator &CComponentByTypeIterator::operator++() // Prefix increment operator
{
	if (!IsDone())
	{ // Let's not iterate past end() shall we
		++mIter;
	}
	return *this;
}

CComponentByTypeIterator CComponentByTypeIterator::operator++(int) // Postfix increment operator
{
	CComponentByTypeIterator retVal = *this;
	++(*this); // Call the prefix increment operator
	return retVal;
}

bool CComponentByTypeIterator::IsDone(void) const
{
	if (mType == CID_INVALID)
		return true;
	return (mIter == Globals::GetObjectManager().mDB->mComponentTypeToComponentMap[mType].end());
}

IComponent	*CComponentByTypeIterator::GetComponent(void) const
{
	if (IsDone())
	{
		return NULL;
	}
	IComponent* pComponent = (*mIter).second;
	return pComponent;
}

void CComponentByTypeIterator::Reset(EComponentTypeId type)
{
	mType = type;
	if (mType == CID_INVALID)
		return;
	mIter = Globals::GetObjectManager().mDB->mComponentTypeToComponentMap[type].begin();
}

///////////////////////////////////////////////////////
// CComponentByInterfaceIterator methods
///////////////////////////////////////////////////////

CComponentByInterfaceIterator::CComponentByInterfaceIterator(EInterfaceId interfaceType) :
mInterfaceType(interfaceType),
mCompIter(CID_INVALID) // We're going to reset it before it's used
{
	SetToFirstValidComponent();
}

void CComponentByInterfaceIterator::SetToFirstValidComponent()
{
	SObjectManagerDB &db = *Globals::GetObjectManager().mDB;
	const int32 numComponentTypes = static_cast<int32>(db.mInterfaceTypeToComponentTypes[mInterfaceType].size());

	if (numComponentTypes <= 0)
		return; // There can't be any valid component. No component types implement this interface.

	// Set to the first possible component type
	mCompTypeIter = db.mInterfaceTypeToComponentTypes[mInterfaceType].begin();
	EComponentTypeId compType = (*mCompTypeIter);
	mCompIter.Reset(compType);

	if (mCompIter.IsDone())
	{ // The first component type gave us nothing
		// Now that we've got both iterators going, we simply need to call the increment operator
		// to get a valid component pointer (if there is one)
		++(*this);
	}
}

CComponentByInterfaceIterator &CComponentByInterfaceIterator::operator++()
{
	SObjectManagerDB &db = *Globals::GetObjectManager().mDB;
	std::set<EComponentTypeId>::iterator compTypeEndIter = db.mInterfaceTypeToComponentTypes[mInterfaceType].end();

	if (mCompTypeIter == compTypeEndIter)
	{ // We're definitely at the end where there are no more component pointers.
		return *this;
	}
	
	++mCompIter;

	// In case we just incremented mCompIter off the end, we'll keep checking
	// the next component type until we either run out of types or find a component
	while (!mCompIter.IsDone())
	{
		++mCompTypeIter;
		if (mCompTypeIter == compTypeEndIter)
			return *this; // We've run out of types, so we're done
		mCompIter.Reset(*mCompTypeIter);
	}

	return *this;
}

CComponentByInterfaceIterator CComponentByInterfaceIterator::operator++(int)
{
	CComponentByInterfaceIterator retVal = *this;
	++(*this); // Call the prefix increment operator
	return retVal;
}

bool CComponentByInterfaceIterator::IsDone(void) const
{
	SObjectManagerDB &db = *Globals::GetObjectManager().mDB;
	std::set<EComponentTypeId>::iterator compTypeEndIter = db.mInterfaceTypeToComponentTypes[mInterfaceType].end();

	if (mCompTypeIter == compTypeEndIter)
	{ // We're definitely at the end where there are no more component pointers.
		return true;
	}

	return false;
}


IComponent	*CComponentByInterfaceIterator::GetComponent(void) const
{
	// This call to IsDone() needs to be here, otherwise it could potentially try to read from bad addresses
	if (IsDone())
	{
		return NULL;
	}
	return mCompIter.GetComponent();
}

///////////////////////////////////////////////////////
// CComponentByObjectIterator methods
///////////////////////////////////////////////////////

CComponentByObjectIterator::CComponentByObjectIterator(CObjectId oId) : mObjectId(oId), mIndex(0)
{
	SetToFirstValidComponent();
}

void CComponentByObjectIterator::SetToFirstValidComponent()
{
	assert (NUM_COMPONENT_TYPE_IDS > 0);
	SObjectManagerDB &db = *Globals::GetObjectManager().mDB;
	if (db.mComponentTypeToComponentMap[mIndex].find(mObjectId) == db.mComponentTypeToComponentMap[mIndex].end())
	{ // Couldn't find a component belonging to oId in the first list, now a simple increment will do
		++(*this);
	}
}

CComponentByObjectIterator &CComponentByObjectIterator::operator++()
{
	// There can be only one component implementing an interface in each object, so we don't have to 
	// check the rest of the map mIndex is pointing at. Move on until we have found another.

	SObjectManagerDB &db = *Globals::GetObjectManager().mDB;

	while (++mIndex < NUM_COMPONENT_TYPE_IDS)
	{
		CComponentMap &compMap = db.mComponentTypeToComponentMap[mIndex];
		if (compMap.find(mObjectId) != compMap.end())
			break; // We found a component pointer, so we'll stop here.
	}

	return *this;
}

CComponentByObjectIterator CComponentByObjectIterator::operator++(int)
{
	CComponentByObjectIterator retVal = *this;
	++(*this);
	return retVal;
}

bool CComponentByObjectIterator::IsDone(void) const
{
	return mIndex >= NUM_COMPONENT_TYPE_IDS;
}


IComponent	*CComponentByObjectIterator::GetComponent(void) const
{
	if (IsDone())
	{
		return NULL;
	}

	SObjectManagerDB &db = *Globals::GetObjectManager().mDB;
	IComponent *pComponent = (db.mComponentTypeToComponentMap[mIndex].find(mObjectId))->second;
	return pComponent;
}
