// File:	HandleInstance.h
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	11/23/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	Generic handle/pointer pair.
//			This is primarily a data class, the real work is done by the user
//			of this class.
////////////////////////////////////////////////////////////////////////////////

// Include only once per compile.
#pragma once
#ifndef __BMH_HANDLE_INSTANCE_HEADER__
#define __BMH_HANDLE_INSTANCE_HEADER__

// Local headers.
#include "Handle.h"

// Desc:	Generic handle/pointer pair.
// Input:	t_Type - Class type for pointer.
template <typename t_Type> class t_HandleInstance
{
public:

	// Desc:	Initialize an invalid handle instance.
	// Notes:	The default invalid handle instance consists of a zero
	//			handle and a NULL pointer.
	t_HandleInstance() : m_handle(0, 0), m_instance(0) {}

	// Desc:	Initialize a handle instance with the necessary settings.
	// Input:	i_index - Array index of object this handle represents.
	//			i_uniqueID - Unique identifier to ensure handle is still
	//							valid.
	//			i_instance - Pointer to object to associate with handle,
	//				or NULL if no pointer should be associated with handle.
	t_HandleInstance(unsigned int i_index, unsigned int i_uniqueID,
		t_Type *i_instance)
		: m_handle(i_index, i_uniqueID), m_instance(i_instance) {}

	// Desc:	Change the pointer.
	// Input:	i_instance - Pointer to object to associate with handle,
	//				or NULL if no pointer should be associated with handle.
	void m_SetInstance(t_Type *i_instance)
	{ m_instance = i_instance; }

	// Desc:	Get the handle.
	// Output:	Data structure of handle that represents the pointer.
	t_Handle m_GetHandle() const { return(m_handle); }

	// Desc:	Get the pointer.
	// Output:	Pointer to object, or NULL if no object is associated
	//			with the handle instance.
	t_Type *m_GetInstance() const { return(m_instance); }

private:

	// Desc:	Handle that represents the pointer to the instance.
	t_Handle m_handle;

	// Desc:	Pointer to the instance of the object associated with
	//			the handle.
	t_Type *m_instance;

}; // class t_HandleInstance

#endif  // __BMH_HANDLE_INSTANCE_HEADER__
