#include "standard_headers.hpp"
#include "base_class.hpp"
#include "intelligent_pointer.hpp"

//************************************************************
// Base Object stuff
//************************************************************
CBaseObject::CBaseObject(void)
{
}

//************************************************************
CBaseObject::~CBaseObject(void)
{
	Clear();
}

//************************************************************
void CBaseObject::Clear(void)
{
	std::list<CBaseIntelligentPointer*>::iterator it;

	for(it=IntelligentPointers.begin(); it!=IntelligentPointers.end(); ++it)
	{
		(*it)->ClearItem();
	}
	IntelligentPointers.clear();
}

//************************************************************
void CBaseObject::RemoveIntelligentPointer(CBaseIntelligentPointer *p) const
{
	IntelligentPointers.remove(p);
}

//************************************************************
void CBaseObject::AddIntelligentPointer(CBaseIntelligentPointer *p) const
{
	IntelligentPointers.push_back(p);
}

