
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			General shared
//*				File:				ign_pointerlist_shared.h
//*				Description:		Shared file with pointer list template
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
//*				11/11/02| 0.0.1		## Initial release															(kp) ##
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_POINTERLIST_SHARED_H_INCLUDED
#define			IGN_POINTERLIST_SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_pointerlist_shared.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_TYPES_SHARED_H_INCLUDED
  #include		"../ign_types_shared.h"
#endif




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------
//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

template<class T> class IGN_TEMPL_POINTERLIST
{
public:
				IGN_TEMPL_POINTERLIST					();
				~IGN_TEMPL_POINTERLIST					();

protected:
			T**											m_ppPool;
			IGNU32										m_u32EntryCount;

public:
			IGNRESULT	AddEntry						(			T*							a_pEntry						);
			IGNRESULT	GetEntry						(	const	IGNU32						a_u32EntryIndex,
																	T**							a_ppEntry						)	const;	
			IGNRESULT	GetEntryCount					(			IGNU32*						a_pu32EntryCount				)	const;
			IGNRESULT	RemoveEntry						(	const	IGNU32						a_u32EntryIndex					);
			IGNRESULT	RemoveAndDeleteEntry			(	const	IGNU32						a_u32EntryIndex					);
			IGNRESULT	RemoveAllEntries				(			void														);
			IGNRESULT	RemoveAndDeleteAllEntries		(			void														);
};															

	


//------------- INLINES ----------------------------------------------------------------------------------------------------------------------------------------------

template<class T> 
IGN_TEMPL_POINTERLIST<T>::IGN_TEMPL_POINTERLIST					()
{	
	//----------------------------------------------------------|------------------------------------------------------------------------------IGN_TEMPL_POINTERLIST-|
	// public method
	// Constructor

	m_ppPool		= static_cast<T**>( malloc( 4 ) );
	m_u32EntryCount = 0; 
}

template<class T> 
IGN_TEMPL_POINTERLIST<T>::~IGN_TEMPL_POINTERLIST				()
{	
	//----------------------------------------------------------|-----------------------------------------------------------------------------~IGN_TEMPL_POINTERLIST-|
	// public method
	// Destructor

	if( NULL!=m_ppPool ) 
		free( m_ppPool ); 
}

template<class T> 
IGNRESULT	IGN_TEMPL_POINTERLIST<T>::AddEntry					(			T*							a_pEntry )
{	
	//----------------------------------------------------------|-------------------------------------------------------------------------------------------AddEntry-|
	// public method
	// Adds the specified entry to the list.

	m_ppPool = static_cast<T**>(	realloc( static_cast<void*>( m_ppPool ), 
									(++m_u32EntryCount)*4 ) );
	if( NULL==m_ppPool )
		return IGN_FAILED;
	m_ppPool[m_u32EntryCount-1] = a_pEntry;

	return IGN_OK;
}

template<class T> 
IGNRESULT	IGN_TEMPL_POINTERLIST<T>::GetEntry					(	const	IGNU32						a_u32EntryIndex,
																			T**							a_ppEntry )	const
{	
	//----------------------------------------------------------|-------------------------------------------------------------------------------------------GetEntry-|
	// public method
	// Retrieves the list entry at specified index.

	if( m_u32EntryCount>a_u32EntryIndex )
	{
		*a_ppEntry = m_ppPool[a_u32EntryIndex];
		return IGN_OK;
	}
	else
		*a_ppEntry = NULL; 

	return IGN_FAILED;
}

template<class T> 
IGNRESULT	IGN_TEMPL_POINTERLIST<T>::GetEntryCount				(			IGNU32*						a_pu32EntryCount ) const
{	
	//----------------------------------------------------------|--------------------------------------------------------------------------------------GetEntryCount-|
	// public method
	// Retrieves the number of list entries.

	*a_pu32EntryCount = m_u32EntryCount; 

	return IGN_OK;
}

template<class T> 
IGNRESULT	IGN_TEMPL_POINTERLIST<T>::RemoveEntry				(	const	IGNU32						a_u32EntryIndex )
{	
	//----------------------------------------------------------|----------------------------------------------------------------------------------------RemoveEntry-|
	// public method
	// Renoves a entry from list without destroying.

	if( m_u32EntryCount>a_u32EntryIndex )
	{
		--m_u32EntryCount;
		memcpy( static_cast<void*>( m_ppPool+a_u32EntryIndex ), 
				static_cast<void*>( m_ppPool+a_u32EntryIndex+1 ), 
				(m_u32EntryCount-a_u32EntryIndex)*4 );
		if( 0!=m_u32EntryCount )
			m_ppPool = static_cast<T**>( realloc(	static_cast<void*>( m_ppPool ), 
													m_u32EntryCount*4 ) );
		if( NULL==m_ppPool )
			return IGN_FAILED;
		return IGN_OK;
	}

	return IGN_FAILED;
}

template<class T> 
IGNRESULT	IGN_TEMPL_POINTERLIST<T>::RemoveAndDeleteEntry		(	const	IGNU32						a_u32EntryIndex )
{	
	//----------------------------------------------------------|-------------------------------------------------------------------------------RemoveAndDeleteEntry-|
	// public method
	// Removes and destroyes a entry from list.

	if( m_u32EntryCount>a_u32EntryIndex )
	{
		delete (*(m_ppPool+a_u32EntryIndex));
		return RemoveEntry( a_u32EntryIndex );
	}
	
	return IGN_OK;
}

template<class T> 
IGNRESULT	IGN_TEMPL_POINTERLIST<T>::RemoveAllEntries			(			void )
{	
	//----------------------------------------------------------|-----------------------------------------------------------------------------------RemoveAllEntries-|
	// public method
	// Removes all list entries without destroying them.

	m_u32EntryCount = 0; 
	free( static_cast<void*>( m_ppPool ) );
	m_ppPool = NULL;

	return IGN_OK;
}

template<class T> 
IGNRESULT	IGN_TEMPL_POINTERLIST<T>::RemoveAndDeleteAllEntries	(			void )
{	
	//----------------------------------------------------------|--------------------------------------------------------------------------RemoveAndDeleteAllEntries-|
	// public method
	// Removes and destroys all list entries.

	while( 0!=m_u32EntryCount )
		if( IGN_FAILED==RemoveAndDeleteEntry( m_u32EntryCount-1 ) )
			return IGN_FAILED; 

	return IGN_OK;
}




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_POINTERLIST_SHARED_H_INCLUDED
