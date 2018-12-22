#include "IComponent.h"
#include "Globals.h"
#include "CObjectManager.h"
#include "ICmpEntity.h"

IComponent::IComponent()
{
}

IComponent::~IComponent()
{
}

EMessageResult IComponent::HandleMessage(const CMessage &)
{
	return MR_IGNORED;
}

// Gets the entity interface for this component.
ICmpEntity *IComponent::GetEntity() const
{
	return static_cast<ICmpEntity *>(Globals::GetObjectManager().QueryInterface(mObjectId, IID_ENTITY));
}