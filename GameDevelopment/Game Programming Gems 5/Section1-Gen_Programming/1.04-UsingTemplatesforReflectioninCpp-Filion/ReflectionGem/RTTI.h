//
// RTTI.h
//

#ifndef __RTTI_H__
#define __RTTI_H__

#include <list>
#include <vector>

using namespace std;

#include "AbstractProperty.h"

//-------------------------------------------------------------------------------------------------
class CBaseObject;
class CRTTI;

//-------------------------------------------------------------------------------------------------
typedef DWORD			ClassID;
typedef CBaseObject*	(*ClassFactoryFunc)( ClassID );
typedef bool			(*RegisterReflectionFunc)();

//-------------------------------------------------------------------------------------------------
const DWORD CLASSNAME_LENGTH = 32;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CRTTI
//
// RTTI description structure.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CRTTI
{

public:

	//----------------------------------------------------------------------------------------------
	// RTTI constructor.
	// The first paramter is a stub. I am not sure why this stub is necessary - removing ths stub will
	// confuse the .NET compiler and produce compile errors with subsequent parameters. If anybody knows 
	// why this is so, feel free to e-mail me at dfilion@hotmail.com
	//
	// The RTTI structure constructor takes in the following parameters:
	//		dwStub			Just a stub
	//		CLID			A unique class ID
	//		szClassName		The name of the class type this RTTI structure represents
	//		pBaseClassRTTI	The parent RTTI structure for this RTTI structure
	//		pFactory		A factory function callback for creating an instance of the bound class type
	//		pReflectionFunc	Callback called by the system to register the reflective properties
	CRTTI(	DWORD dwStub, ClassID CLID, const char* szClassName, CRTTI* pBaseClassRTTI, 
			ClassFactoryFunc pFactory, RegisterReflectionFunc pReflectionFunc );

	//----------------------------------------------------------------------------------------------
	// Fills a vector with all properties of the represented class type, including all ancestor types.
	void	EnumProperties( vector<CAbstractProperty*>& o_Result );

	//----------------------------------------------------------------------------------------------
	// Returns true if the RTTI structure is of the type specified by CLID.
	inline bool				IsTypeOf( ClassID CLID );

	// Returns true if the RTTI structure is derived from the type specified by CLID.
	inline bool				IsDerivedFrom( ClassID CLID );

	//----------------------------------------------------------------------------------------------
	// Gets base class' RTTI structure.
	inline CRTTI*			GetAncestorRTTI();

	//----------------------------------------------------------------------------------------------
	// Gets the class ID.
	inline ClassID			GetCLID();

	//----------------------------------------------------------------------------------------------
	// Gets the class name.
	inline const char*		GetClassName();

	//----------------------------------------------------------------------------------------------
	// Gets the class factory function.
	inline ClassFactoryFunc	GetClassFactory();

	//----------------------------------------------------------------------------------------------
	// Provides access to the properties bound to this run-time type. Does not include ancestor class
	// properties. Use EnumProperties to include ancestor properties.
	inline	list<CAbstractProperty*>::iterator	GetFirstProperty();
	inline	list<CAbstractProperty*>::iterator	GetLastProperty();
	inline	list<CAbstractProperty*>*			GetProperties();

private:

	//---------------------------------------------------------------------------------------------
	ClassID						m_CLID;									// Class ID
	char						m_szClassName[CLASSNAME_LENGTH];		// Class name
	CRTTI*						m_pBaseRTTI;							// Base class RTTI structure
	ClassFactoryFunc			m_pObjectFactory;						// Factory function
	list<CAbstractProperty*>	m_Properties;							// Property list

};

inline CRTTI* CRTTI::GetAncestorRTTI()
{
	return m_pBaseRTTI;
}

inline ClassFactoryFunc CRTTI::GetClassFactory()
{
	return m_pObjectFactory;
}

inline const char* CRTTI::GetClassName()
{
	return m_szClassName;
}

inline ClassID CRTTI::GetCLID()
{
	return m_CLID;
}

inline list<CAbstractProperty*>::iterator CRTTI::GetFirstProperty()
{
	return m_Properties.begin();
}

inline list<CAbstractProperty*>* CRTTI::GetProperties()
{
	return &m_Properties;
}

inline bool CRTTI::IsDerivedFrom( ClassID CLID )
{
	if( m_CLID == CLID )
		return true;
	else if( m_pBaseRTTI )
		return m_pBaseRTTI->IsDerivedFrom( CLID );

	return false;
}

inline bool CRTTI::IsTypeOf( ClassID CLID )
{
	return m_CLID == CLID;
}

inline list<CAbstractProperty*>::iterator CRTTI::GetLastProperty()
{
	return m_Properties.end();
}

#endif //__RTTI_H__