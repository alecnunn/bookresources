//
// Property.h
//

#ifndef _PROPERTY_H
#define _PROPERTY_H

#include "TypedProperty.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CProperty
//
// Fully defined property class. Specifies property's name, type and the getter/setters are bound as
// members of a specific class type.
//
// Template parameter OwnerType is the class that the getter and setter are a member of and template
// parameter T is the property type.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class OwnerType, class T> class CProperty : public CTypedProperty<T>
{

public:

	//----------------------------------------------------------------------------------------------
	typedef T		(OwnerType::*GetterType)();				// Getter function
	typedef void	(OwnerType::*SetterType)( T Value );	// Setter function

	//----------------------------------------------------------------------------------------------
	// Constructor. Takes in property's name, getter and setter functions.
	inline CProperty( const char* szName, GetterType Getter, SetterType Setter );

	//----------------------------------------------------------------------------------------------
	// Determines the value of this property.
	virtual T		GetValue( CBaseObject* pObject );
	virtual void	SetValue( CBaseObject* pObject, T Value );

protected:

    GetterType		m_Getter;
    SetterType		m_Setter;
 
};

template <class OwnerType, class T>
inline CProperty<OwnerType, T>::CProperty( const char* szName, GetterType Getter, SetterType Setter ) :
	CTypedProperty<T>	( szName		),
	m_Getter			( Getter		),
	m_Setter			( Setter		)
{
}

template <class OwnerType, class T> 
T CProperty<OwnerType, T>::GetValue( CBaseObject* pOwner )
{
	return (((OwnerType*)pOwner)->*m_Getter)();
}

template <class OwnerType, class T> 
void CProperty<OwnerType, T>::SetValue(CBaseObject* pOwner, T Value )
{
	if ( !m_Setter)
	{
		ASSERT( false ); // Cannot write to a read-only property
		return;
	}
	(((OwnerType*)pOwner)->*m_Setter)( Value );
}

#endif	// _PROPERTY_H