//
// AbstractProperty.h
//

#ifndef _ABSTRACT_PROPERTY_H
#define _ABSTRACT_PROPERTY_H

#include "PropertyTypes.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CAbstractProperty
//
// Base class for all properties.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CAbstractProperty
{

public:

	//----------------------------------------------------------------------------------------------
	// Constructor. Takes in property name.
	inline CAbstractProperty( const char* szName );

	//----------------------------------------------------------------------------------------------
	// Returns the name of this property.
	inline const char*              GetName() const;

	//----------------------------------------------------------------------------------------------
	// Returns the type of this property.
	virtual ePropertyType			GetType() const = 0;
    
protected :

	const char*				m_szName;			// Property name

};

inline CAbstractProperty::CAbstractProperty( const char* szName ) :
	m_szName		( szName			)
{
}

inline const char* CAbstractProperty::GetName() const
{
	return m_szName;
}

#endif	// _ABSTRACT_PROPERTY_H