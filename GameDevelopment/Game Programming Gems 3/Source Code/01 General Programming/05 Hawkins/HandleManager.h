// File:	HandleManager.h
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	11/23/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	Manage creation, conversion, and destruction of handles.
//			Here I have allowed multiple handle managers of the same type to
//			be created for greater flexibility, but this class is a good
//			candidate for the Single Pattern from the Design Patterns book
//			by the Gang of Four.
////////////////////////////////////////////////////////////////////////////////

// Include only once per compile.
#pragma once
#ifndef __BMH_HANDLE_MANAGER_HEADER__
#define __BMH_HANDLE_MANAGER_HEADER__

// Standard template library.
#include <vector>

// Local headers.
#include "HandleInstance.h"

// Desc:	Handle manager for a particular class.
// Input:	t_Type - Type of object for which the handles are associated.
template <typename t_Type> class t_HandleManager
{
public:

	// Desc:	Initialize manager.
	// Notes:	The unique identifier is initialized to one and incremented
	//			by two in order to ensure zero is never a valid unique ID.
	t_HandleManager() : m_nextUniqueID(1), m_freeIndex(0) {}

	// Desc:	Create a new handle and associate it with a pointer.
	// Input:	i_instance - Pointer to object instance that handle
	//							will represent.
	// Output:	Unique handle that represents the provided pointer.
	// Notes:	This manager does not prevent two handles from representing
	//			the same pointer.  If this is the case make sure that all
	//			handles associated with the object are destroyed when the
	//			object is deleted.
	t_Handle m_Create(t_Type *i_instance)
	{
		// Create an invalid handle if a NULL pointer is provided.
		if(!i_instance) {
			return(t_Handle(0, 0));
		}

		// Make a local copy of the current free index so the stored
		// free index can be updated.
		unsigned int l_freeIndex = m_freeIndex;

		// If the next free index is outside the range of the array,
		// enlarge the array to the size necessary to accomodate the
		// index.
		if(l_freeIndex >= m_handles.size()) {
			m_handles.resize(l_freeIndex + 1);
		}

		// If the free index points to a handle that has a valid index
		// then the stored index should be set to that index.  This
		// happens because handle destruction fills in this field when
		// if cleans out the handle.
		if(unsigned int l_nextFreeIndex =
			m_handles[l_freeIndex].m_GetHandle().m_GetIndex()) {
			m_freeIndex = l_nextFreeIndex;
		} else {
			// Otherwise, increment stored free index.
			++m_freeIndex;
		}

		// Set the handle at the index two a valid handle instance using
		// the index, a unique identifier, and the provided pointer.
		m_handles[l_freeIndex] =
			t_HandleInstance<t_Type>(l_freeIndex, m_nextUniqueID, i_instance);

		// Increment the unique identifier by two, since it is initialized
		// to one it will never be zero even if it wraps around.  While this
		// does not guarantee an absolutely unique identifier, the chances
		// are miniscule that both index and identifier will be duplicated
		// in any reasonable amount of time.
		m_nextUniqueID += 2;

		// Return the handle that was created.
		return(m_handles[l_freeIndex].m_GetHandle());
	}

	// Desc:	Convert a handle to the associated pointer.
	// Input:	i_handle - Handle to convert.
	// Output:	Associated pointer, or NULL if handle is invalid.
	t_Type *m_Resolve(const t_Handle &i_handle) const
	{
		// Get the index out of the handle.
		unsigned int l_index = i_handle.m_GetIndex();

		// Verify the index is in range and the handle matches
		// the handle stored at than index.
		if((l_index > m_handles.size()) ||
			(m_handles[l_index].m_GetHandle() != i_handle)) {
			// Return NULL due to invalid handle.
			return(0);
		}

		// Return pointer at the indexed location.
		return(m_handles[l_index].m_GetInstance());
	}

	// Desc:	Destroy a handle.
	// Input:	i_handle - Handle to invalidate.
	// Output:	true if handle was invalidated, false if handle is already
	//			invalid.
	// Notes:	Normal used to invalidate handles due to destruction of
	//			associated object.
	bool m_Destroy(const t_Handle &i_handle)
	{
		// Get the index out of the handle.
		unsigned int l_index = i_handle.m_GetIndex();

		// Verify the index is in range and the handle matches
		// the handle stored at than index.
		if((l_index > m_handles.size()) ||
			(m_handles[l_index].m_GetHandle() != i_handle)) {
			// Handle is already invalid.
			return(false);
		}

		// Clear indexed handle entry and change index to current free
		// index to maintain list of free indices.
		m_handles[l_index] = t_HandleInstance<t_Type>(m_freeIndex, 0, 0);

		// Change the free index to point to the newly destroy index.
		m_freeIndex = l_index;

		// Handle has been invalidated.
		return(true);
	}

private:

	// Desc:	Next unique identifier to assign during handle creation.
	unsigned int m_nextUniqueID;

	// Desc:	First free index for handle creation.
	unsigned int m_freeIndex;

	// Desc:	Array of handle/pointer pairs.
	std::vector< t_HandleInstance<t_Type> > m_handles;

}; // class t_HandleManager

#endif  // __BMH_HANDLE_MANAGER_HEADER__
