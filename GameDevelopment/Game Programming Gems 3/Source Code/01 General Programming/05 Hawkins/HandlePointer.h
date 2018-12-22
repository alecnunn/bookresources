// File:	HandlePointer.h
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	11/23/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	Handle-based smart pointer.
////////////////////////////////////////////////////////////////////////////////

// Include only once per compile.
#pragma once
#ifndef __BMH_HANDLE_POINTER_HEADER__
#define __BMH_HANDLE_POINTER_HEADER__

// Local headers.
#include "HandleManager.h"

// Desc:	Handle-based smart pointer.
// Input:	t_Type - Type of object pointed to by smart pointer.
template <typename t_Type> class t_HandlePointer
{
private:

	// Desc:	Proxy class designed for if(smart_pointer) test.
	// Notes:	The private undefined operator delete is designed to
	//			prevent delete smart_pointer which would have
	//			undesirable side effects.
	class t_TestProxy
	{ void operator delete(void*); };

public:

	// Desc:	Create a new handle pointer from a dumb pointer.
	// Input:	i_instance - dumb pointer
	// Notes:	Explicit conversion is required to prevent accidental
	//			creation of multiple handles for the same pointer.
	explicit t_HandlePointer(t_Type *i_instance)
	{
		// Create a new handle.
		t_Handle l_handle = m_HandleManagerInstance().m_Create(i_instance);

		// Store the handle and cache the pointer which is assumed to
		// be valid on handle pointer creation.
		m_handleInstance = t_HandleInstance<t_Type>(l_handle.m_GetIndex(),
			l_handle.m_GetUniqueID(), i_instance);
	}

	// Desc:	Enable if(smart_pointer) conditional.
	// Output:	Pointer to proxy object if handle reference to a valid
	//			pointer, or NULL if the handle is invalid.
	operator t_TestProxy*() const
	{
		// Test to see if handle is valid using operator!
		if(!(*this)) {
			// Invalid handle, return NULL so conditional results in false.
			return(0);
		}

		// Return a valid pointer to a static proxy object so conditional
		// results in true.
		static t_TestProxy l_testProxy;
		return(&l_testProxy);
	}

	// Desc:	Test validity of handle.
	// Output:	True if handle is valid, false otherwise.
	bool operator!() const
	{
		// Update the cached pointer to the correct pointer if the
		// handle is valid, or NULL if the handle is invalid.
		m_UpdateCachedPointer();

		// Return result of operator! on cached pointer.
		return(!m_handleInstance.m_GetInstance());
	}

	// Desc:	Dereference.
	// Output:	Dumb pointer.
	t_Type *operator->()
	{
		// Return cached dumb pointer.
		return(m_handleInstance.m_GetInstance());
	}

	// Desc:	Dereference.
	// Output:	Reference to object.
	t_Type &operator*()
	{
		// Return dereferenced cached dumb pointer.
		return(*(m_handleInstance.m_GetInstance()));
	}

	// Desc:	Delete the pointer this handle refers to and invalidate the
	//			handle.
	// Input:	i_handlePointer - Pointer to destroy.
	// Notes:	Does nothing if handle is already invalid.  This is done
	//			as a friend function to prevent adding member functions
	//			to the smart pointer that can make the resulting usage
	//			inconsistent by causing the user to apply operator-> and
	//			operator. to the same variable.
	friend inline void g_Destroy(t_HandlePointer &i_handlePointer)
	{
		// Update the cached pointer to the correct pointer if the
		// handle is valid, or NULL if the handle is invalid.
		i_handlePointer.m_UpdateCachedPointer();

		// Delete the cached pointer.
		delete i_handlePointer.m_handleInstance.m_GetInstance();

		// Invalidate the handle.
		i_handlePointer.m_HandleManagerInstance().m_Destroy(
			i_handlePointer.m_handleInstance.m_GetHandle());

		// Clear the cached pointer.
		i_handlePointer.m_handleInstance.m_SetInstance(0);
	}

	// Desc:	Get the dumb pointer from a smart pointer.
	// Input:	i_handlePointer - Smart pointer that contains dumb pointer.
	// Notes:	This function should be used infrequently.  This offers no
	//			control over the validity of the pointer and therefore can
	//			cause unexpected results.  This is done as a friend function
	//			to prevent adding member functions to the smart pointer that
	//			can make the resulting usage inconsistent by causing the
	//			user to apply operator-> and operator. to the same variable.
	friend inline t_Type *g_GetDumbPointer(
		const t_HandlePointer &i_handlePointer)
	{
		// Update the cached pointer to the correct pointer if the
		// handle is valid, or NULL if the handle is invalid.
		i_handlePointer.m_UpdateCachedPointer();

		// Return the cached pointer.
		return(i_handlePointer.m_handleInstance.m_GetInstance());
	}

private:

	// Desc:	Single global instance of handle manager for object type.
	// Output:	Reference to handle manager.
	// Notes:	Verify compiler creates only one instance of handle manager.
	static t_HandleManager<t_Type> &m_HandleManagerInstance()
	{
		// Create a local static instance.
		static t_HandleManager<t_Type> l_handleManager;

		// Return reference to static instance.
		return(l_handleManager);
	}

	// Desc:	Update the cached pointer to the correct pointer for the
	//			stored handle.
	// Notes:	Sets cached pointer to NULL if handle is invalid.
	void m_UpdateCachedPointer() const
	{
		// Resolve handle and store the result.
		m_handleInstance.m_SetInstance(
			m_HandleManagerInstance().m_Resolve(
			m_handleInstance.m_GetHandle()));
	}

	// Desc:	Handle/pointer pair.
	// Notes:	Mutable so cached pointer can be updated while maintain
	//			const modifier on operations that are logically const.
	mutable t_HandleInstance<t_Type> m_handleInstance;

}; // class t_HandlePointer

#endif  // __BMH_HANDLE_POINTER_HEADER__
