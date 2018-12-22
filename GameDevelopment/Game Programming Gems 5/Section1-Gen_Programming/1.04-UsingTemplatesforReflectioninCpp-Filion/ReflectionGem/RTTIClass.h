//
// RTTIClass.h
//

#ifndef _RTTICLASS_H
#define _RTTICLASS_H

#include <typeinfo.h>
#include "RTTI.h"
#include "Property.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CRTTIClass
//
// CRTTIClass is the RTTI "sandwich class" being used to augment a class with RTTI support. Classes
// supporting RTTI need to derive from this class, with their ancestor specified as the BaseClass
// template parameter.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class BaseClass, ClassID CLID> class CRTTIClass : public BaseClass
{

public :

	const static	ClassID	ClassCLID	= CLID;
	
	//----------------------------------------------------------------------------------------------
	// Constructor
	CRTTIClass();
	
	//----------------------------------------------------------------------------------------------
	// Default factory function. Creates an instance of T. Called by the system to dynamically create
	// class instances from class IDs.
	static T*	Create();

	//----------------------------------------------------------------------------------------------
	// Default reflection registration function. Does nothing by default.
	static void	RegisterReflection();

	//----------------------------------------------------------------------------------------------
	// Registers a property. Takes in the property name, its getter and setter functions, and the property
	// type as a template parameter. Should be called from within a user-defined RegisterReflection function.
	template <class PropertyType> 
	static	void RegisterProperty(	const char* szName, typename CProperty<T, PropertyType>::GetterType Getter, 
									typename CProperty<T, PropertyType>::SetterType Setter )
	{
		CProperty<T, PropertyType>* pProperty = new CProperty<T, PropertyType>( szName, Getter, Setter );
		T::GetClassRTTI()->GetProperties()->push_back( pProperty );
		CPropertySystem::GetProperties()->push_back( pProperty );
	}

	//----------------------------------------------------------------------------------------------
	// Returns RTTI info associated with this class type.
	static inline CRTTI* GetClassRTTI()
	{
		return &ms_RTTI;
	}

	//----------------------------------------------------------------------------------------------
	// Returns RTTI info associated with this class instance.
	virtual CRTTI* GetRTTI()
	{
		return &ms_RTTI;
	}

protected :

	static CRTTI	ms_RTTI;				// RTTI structure
																		
};

// The first parameter (0) is a stub. See CRTTI constructor for details.
template <class T, class BaseClass, ClassID CLID> CRTTI CRTTIClass<T, BaseClass, CLID>::ms_RTTI
	( 0, CLID, typeid(T).name(), BaseClass::GetClassRTTI(), (ClassFactoryFunc)T::Create, 
	(RegisterReflectionFunc)T::RegisterReflection );

template <class T, class BaseClass, ClassID CLID> 
CRTTIClass<T, BaseClass, CLID>::CRTTIClass()
{
}

template <class T, class BaseClass, ClassID CLID> 
T* CRTTIClass<T, BaseClass, CLID>::Create()
{
	return new T();
}

template <class T, class BaseClass, ClassID CLID> 
void CRTTIClass<T, BaseClass, CLID>::RegisterReflection()
{
}


#endif	// _RTTICLASS_H