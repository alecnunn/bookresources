#include "types.h"
#include "CObjectManager.h"
#include "SObjectManagerDB.h"
#include "ComponentIterators.h"
#include "CParameterNode.h"
#include <assert.h>
#include "CMessage.h"

// All the component classes
#include "CCmpEntity.h"
#include "CCmpRoom.h"
#include "CCmpActor.h"
#include "CCmpCollectable.h"
#include "CCmpInventory.h"
#include "CCmpPlayer.h"
#include "CCmpPuzzleLogic.h"
#include "CCmpDescription.h"



CObjectManager::CObjectManager() : mDB(NULL)
{
}

CObjectManager::~CObjectManager()
{
	DeInit();
}

void CObjectManager::Init()
{
	if (mDB != NULL)
	{
		delete mDB;
	}
	mDB = new SObjectManagerDB();

	RegisterAllComponentTypes();
}

void CObjectManager::DeInit(void)
{
	if (mDB != NULL)
	{
		DestroyAllComponents();

		// Now get rid of the database
		delete mDB;
		mDB = NULL;
	}
}

bool CObjectManager::LoadObjectsFromFile(const char *fileName)
{
	void* buffer;
	FILE *pObjFile = fopen(fileName, "rb");
	if (pObjFile == NULL)
	{
		return false;
	}

	fseek(pObjFile, 0, SEEK_END);
	int32 fileSize = ftell(pObjFile);
	fseek(pObjFile, 0, SEEK_SET);

	buffer = malloc(fileSize);
	if (buffer == NULL)
		return false;

	fread(buffer, 1, fileSize, pObjFile);
	LoadObjects((CParameterNode *)buffer);
	free(buffer);

	return true;
}

bool CObjectManager::LoadObjects(CParameterNode *rootNode)
{
	CParameterNode::SetBasePointer(rootNode);

	for (CParameterNode *curNode = rootNode ; curNode != NULL ; curNode = curNode->GetNext())
	{
		if (curNode->GetParameterType() != CHash("OBJECT"))
		{
			return false;
		}
		CObjectId oId = CreateObject(curNode);
		//CreateObject(curNode);
		PostMessage(oId, MT_OBJECT_CREATED);
	}
	return true;
}

void CObjectManager::DestroyAllComponents()
{
	for (int32 compType = 0 ; compType < NUM_COMPONENT_TYPE_IDS ; ++compType)
	{
		for (CComponentByTypeIterator iter(static_cast<EComponentTypeId>(compType)) ; !iter.IsDone() ; ++iter)
		{
			IComponent *pComp = iter.GetComponent();
			// Deinit and delete each component
			pComp->Deinit();
			mDB->mComponentTypeInfo[compType].mDestructionMethod(pComp);
		}

		// Clear the map
		mDB->mComponentTypeToComponentMap[compType].clear();
	}
}

CComponentByTypeIterator CObjectManager::GetComponentsByComponentTypeId(EComponentTypeId type)
{
	CComponentByTypeIterator iter(type);
	return iter;
}

CComponentByInterfaceIterator CObjectManager::GetComponentsByInterfaceId(EInterfaceId iid)
{
	CComponentByInterfaceIterator iter(iid);
	return iter;
}

CComponentByObjectIterator CObjectManager::GetComponentsByObjectId(CObjectId oid)
{
	CComponentByObjectIterator iter(oid);
	return iter;
}

// This method calls the static method RegisterComponentType() in each component class. This method
// in turn sets up everything necessary for the component in question.
void CObjectManager::RegisterAllComponentTypes()
{
	CCmpEntity::RegisterComponentType();
	CCmpRoom::RegisterComponentType();
	CCmpActor::RegisterComponentType();
	CCmpCollectable::RegisterComponentType();
	CCmpInventory::RegisterComponentType();
	CCmpPlayer::RegisterComponentType();
	CCmpPuzzleLogic::RegisterComponentType();
	CCmpDescription::RegisterComponentType();
}

void CObjectManager::RegisterInterfaceWithComponent(EInterfaceId iid, EComponentTypeId cid)
{
	mDB->mInterfaceTypeToComponentTypes[iid].insert(cid);
}

void CObjectManager::RegisterComponentType(EComponentTypeId compID, ComponentCreationMethod pCreationFn, ComponentDestructionMethod pDestructionFn, CHash typeHash)
{
	mDB->mComponentTypeInfo[compID].mCreationMethod = pCreationFn;
	mDB->mComponentTypeInfo[compID].mDestructionMethod = pDestructionFn;
	mDB->mComponentTypeInfo[compID].mTypeHash = typeHash;
}

IComponent *CObjectManager::QueryInterface(CObjectId oId, EInterfaceId iid)
{
	std::set<EComponentTypeId> &compTypeSet = mDB->mInterfaceTypeToComponentTypes[iid];
	std::set<EComponentTypeId>::iterator compTypeSetIter;

	for (compTypeSetIter = compTypeSet.begin() ; compTypeSetIter != compTypeSet.end() ; ++compTypeSetIter)
	{
		EComponentTypeId compType = (*compTypeSetIter);
		CComponentMap &compMap = mDB->mComponentTypeToComponentMap[compType];
		CComponentMap::iterator compMapIter = compMap.find(oId);
		if (compMapIter != compMap.end())
		{ // We found our component
			return (*compMapIter).second;
		}
	}
	return NULL;
}

IComponent *CObjectManager::CreateComponent(CObjectId oId, CParameterNode *paramNode)
{
	// Get the component type
	EComponentTypeId cTypeId = paramNode->GetComponentType();

	if (cTypeId == CID_INVALID)
	{ // Don't know this one
		return NULL;
	}

	// Call the component creation method
	assert(mDB->mComponentTypeInfo[cTypeId].mCreationMethod);
	IComponent *pComp = mDB->mComponentTypeInfo[cTypeId].mCreationMethod();

	if (pComp != NULL)
	{
		pComp->SetObjectId(oId);
		pComp->Init(oId, *paramNode);
	}
	return pComp;
}

CObjectId CObjectManager::CreateObject(CParameterNode *paramNode)
{
	return CreateObject(paramNode, paramNode->GetName());
}

CObjectId CObjectManager::CreateObject(CParameterNode *paramNode, CObjectId oId)
{
	assert(paramNode->GetParameterType() == CHash("OBJECT"));

	if (ObjectExists(oId))
	{
		return CHash();
	}

	// Create all the components, and add them to the db
	for (CParameterNode *child = paramNode->GetFirstChild() ; child != NULL ; child = child->GetNext())
	{
		IComponent *comp = CreateComponent(oId, child);
		AddComponentToDb(oId, comp);
	}
	return oId;
}

EComponentTypeId CObjectManager::GetComponentTypeFromHash(CHash typeName) const
{
	for (int32 i = 0 ; i < NUM_COMPONENT_TYPE_IDS ; ++i)
	{
		if (mDB->mComponentTypeInfo[i].mTypeHash == typeName)
		{
			return static_cast<EComponentTypeId>(i);
		}
	}
	return CID_INVALID;
}

bool CObjectManager::ObjectExists(CObjectId oId) const
{
	for (int32 compType = 0 ; compType < NUM_COMPONENT_TYPE_IDS ; ++compType)
	{
		CComponentMap &compMap = mDB->mComponentTypeToComponentMap[compType];
		CComponentMap::iterator compMapIter = compMap.find(oId);
		if (compMapIter != compMap.end())
		{ // We found a component belonging to this object, so the object exists.
			return true;
		}
	}
	return false;
}

bool CObjectManager::AddComponentToDb(CObjectId oId, IComponent *pComp)
{ // Inserts pComp with the key oId, and returns whether the insert was successful
	return (mDB->mComponentTypeToComponentMap[pComp->GetComponentTypeId()].insert(CComponentMap::value_type(oId, pComp))).second;
}

bool CObjectManager::IsInterfaceImplementedByComponent(EInterfaceId iId, IComponent *pComp) const
{
	if (mDB->mInterfaceTypeToComponentTypes[iId].find(pComp->GetComponentTypeId()) == mDB->mInterfaceTypeToComponentTypes[iId].end())
	{
		return false;
	}
	return true;
}

void CObjectManager::SubscribeToMessageType(EComponentTypeId ctid, EMessageType mtid)
{
	mDB->mMessageTypeToComponentTypes[mtid].insert(ctid);
}

void CObjectManager::PostMessage(CObjectId oid, const CMessage &msg)
{
	PostMessage( oid, *const_cast<CMessage *>(&msg));
}

void CObjectManager::PostMessage(CObjectId oid, CMessage &msg)
{
	std::set<EComponentTypeId>	&compSet = mDB->mMessageTypeToComponentTypes[msg.mType];
	std::set<EComponentTypeId>::iterator compSetIter;

	// Find the component types that subscribe to this message
	for (compSetIter = compSet.begin() ; compSetIter != compSet.end() ; ++compSetIter)
	{
		EComponentTypeId	ctid = (*compSetIter); // Found one

		// Go through the components of this component type and send message
		for (CComponentByTypeIterator ci =  GetComponentsByComponentTypeId(ctid) ; !ci.IsDone() ; ++ci)
		{
			IComponent *pComp = ci.GetComponent();
			if (pComp->GetObjectId() == oid)
			{
				pComp->HandleMessage(msg);
			}
		}
	}
}

void CObjectManager::BroadcastMessage(const CMessage &msg)
{
	BroadcastMessage(*const_cast<CMessage *>(&msg));
}

void CObjectManager::BroadcastMessage(CMessage &msg)
{
	std::set<EComponentTypeId>	&compSet = mDB->mMessageTypeToComponentTypes[msg.mType];
	std::set<EComponentTypeId>::iterator compSetIter;

	// Find the component types that subscribe to this message
	for (compSetIter = compSet.begin() ; compSetIter != compSet.end() ; ++compSetIter)
	{
		EComponentTypeId	ctid = (*compSetIter); // Found one

		// Go through the components of this component type and send message
		for (CComponentByTypeIterator ci =  GetComponentsByComponentTypeId(ctid) ; !ci.IsDone() ; ++ci)
		{
			ci.GetComponent()->HandleMessage(msg);
		}
	}
}



