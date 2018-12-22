
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			General shared
//*				File:				ign_valuelist_shared.h
//*				Description:		Shared file with value list template
//*
//********************************************************************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Status:				In progress
//*
//********************************************************************************************************************************************************************
//*
//*				History:
//*
//*				11/22/02| 0.0.1		## Initial release															(kp) ##
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_VALUELIST_SHARED_H_INCLUDED
#define			IGN_VALUELIST_SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_valuelist_shared.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_TYPES_SHARED_H_INCLUDED
  #include		"../ign_types_shared.h"
#endif




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------
//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

template<class T> class IGN_TEMPL_VALUELIST
{
public:
				IGN_TEMPL_VALUELIST						();
				~IGN_TEMPL_VALUELIST					();

protected:
			T*											m_pPool;
			IGNU32										m_u32EntryCount;

public:
			IGNRESULT	AddEntry						(			T&							a_Entry							);
			IGNRESULT	AddEntry						(			T							a_Entry							);
			IGNRESULT	GetEntry						(	const	IGNU32						a_u32EntryIndex,
																	T*							a_pEntry						)	const;	
			IGNRESULT	GetEntryCount					(			IGNU32*						a_pu32EntryCount				)	const;
			IGNRESULT	RemoveEntry						(	const	IGNU32						a_u32EntryIndex					);
			IGNRESULT	RemoveAllEntries				(			void														);
};															

	


//------------- INLINES ----------------------------------------------------------------------------------------------------------------------------------------------

template<class T> 
IGN_TEMPL_VALUELIST<T>::IGN_TEMPL_VALUELIST						()
{	
	//----------------------------------------------------------|--------------------------------------------------------------------------------IGN_TEMPL_VALUELIST-|
	// public method
	// Constructor

	m_pPool			= static_cast<T*>( malloc( sizeof( T ) ) );
	m_u32EntryCount = 0; 
}

template<class T> 
IGN_TEMPL_VALUELIST<T>::~IGN_TEMPL_VALUELIST					()
{	
	//----------------------------------------------------------|-------------------------------------------------------------------------------~IGN_TEMPL_VALUELIST-|
	// public method
	// Destructor

	if( NULL!=m_pPool ) 
		free( m_pPool ); 
}

template<class T> 
IGNRESULT	IGN_TEMPL_VALUELIST<T>::AddEntry					(			T&							a_Entry )
{	
	//----------------------------------------------------------|-------------------------------------------------------------------------------------------AddEntry-|
	// public method
	// Adds the specified entry to the list.

	m_pPool = static_cast<T*>(	realloc( static_cast<void*>( m_pPool ), 
									(++m_u32EntryCount)*sizeof( T ) ) );
	if( NULL==m_pPool )
		return IGN_FAILED;
	*(m_pPool+(m_u32EntryCount-1)) = a_Entry;

	return IGN_OK;
}

template<class T> 
IGNRESULT	IGN_TEMPL_VALUELIST<T>::AddEntry					(			T							a_Entry )
{	
	//----------------------------------------------------------|-------------------------------------------------------------------------------------------AddEntry-|
	// public method
	// Adds the specified entry to the list.

	m_pPool = static_cast<T*>(	realloc( static_cast<void*>( m_pPool ), 
									(++m_u32EntryCount)*sizeof( T ) ) );
	if( NULL==m_pPool )
		return IGN_FAILED;
	*(m_pPool+(m_u32EntryCount-1)) = a_Entry;

	return IGN_OK;
}

template<class T> 
IGNRESULT	IGN_TEMPL_VALUELIST<T>::GetEntry					(	const	IGNU32						a_u32EntryIndex,
																			T*							a_pEntry )	const
{	
	//----------------------------------------------------------|-------------------------------------------------------------------------------------------GetEntry-|
	// public method
	// Retrieves the list entry at specified index.

	if( m_u32EntryCount>a_u32EntryIndex )
	{
		*a_pEntry = *(m_pPool+a_u32EntryIndex);
		return IGN_OK;
	}

	return IGN_FAILED;
}

template<class T> 
IGNRESULT	IGN_TEMPL_VALUELIST<T>::GetEntryCount				(			IGNU32*						a_pu32EntryCount ) const
{	
	//----------------------------------------------------------|--------------------------------------------------------------------------------------GetEntryCount-|
	// public method
	// Retrieves the number of list entries.

	*a_pu32EntryCount = m_u32EntryCount; 

	return IGN_OK;
}

template<class T> 
IGNRESULT	IGN_TEMPL_VALUELIST<T>::RemoveEntry					(	const	IGNU32						a_u32EntryIndex )
{	
	//----------------------------------------------------------|----------------------------------------------------------------------------------------RemoveEntry-|
	// public method
	// Renoves a entry from list without destroying.

	if( m_u32EntryCount>a_u32EntryIndex )
	{
		--m_u32EntryCount;
		memcpy( static_cast<void*>( m_pPool+a_u32EntryIndex ), 
				static_cast<void*>( m_pPool+a_u32EntryIndex+1 ), 
				(m_u32EntryCount-a_u32EntryIndex)*sizeof( T ) );
		if( 0!=m_u32EntryCount )
			m_pPool = static_cast<T*>( realloc(	static_cast<void*>( m_pPool ), 
											m_u32EntryCount*sizeof( T ) ) );
		if( NULL==m_pPool )
			return IGN_FAILED;
		return IGN_OK;
	}

	return IGN_FAILED;
}

template<class T> 
IGNRESULT	IGN_TEMPL_VALUELIST<T>::RemoveAllEntries			(			void )
{	
	//----------------------------------------------------------|-----------------------------------------------------------------------------------RemoveAllEntries-|
	// public method
	// Removes all list entries without destroying them.

	m_u32EntryCount = 0; 
	free( static_cast<void*>( m_pPool ) );
	m_pPool = NULL;

	return IGN_OK;
}




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_VALUELIST_SHARED_H_INCLUDED
