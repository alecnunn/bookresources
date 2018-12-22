// File:	Handle.h
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	11/23/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	Simple handle structure that is formed from an array index and
//			a unique identifier.  To conserve space it is stored in an
//			single integer.  This is mostly a class for data class, the real
//			work of using the index and unique identifier is done elsewhere.
////////////////////////////////////////////////////////////////////////////////

// Include only once per compile.
#pragma once
#ifndef __BMH_HANDLE_HEADER__
#define __BMH_HANDLE_HEADER__

// Desc:	Simple handle structure.
// TODO: Parameterize number of bits for index.
class t_Handle
{
public:

	// Desc:	Construct a new handle.
	// Input:	i_index - Array index of object this handle represents.
	//			i_uniqueID - Unique identifier to ensure handle is still
	//							valid.
	t_Handle(unsigned int i_index, unsigned int i_uniqueID)
		: m_handle(i_index | (i_uniqueID << 16)) {}

	// Desc:	Equality test.
	// Input:	i_rhs - Reference to handle to test against.
	// Output:	true if index and unique identifier are equal, false otherwise.
	bool operator==(const t_Handle &i_rhs) const
	{ return(m_handle == i_rhs.m_handle); }

	// Desc:	Inequality test.
	// Input:	i_rhs - Reference to handle to test against.
	// Output:	true if index and unique identifier are not equal,
	//			false otherwise.
	bool operator!=(const t_Handle &i_rhs) const
	{ return(!(*this == i_rhs)); }

	// Desc:	Get the index that this handle represents.
	// Output:	Integer index.
	unsigned int m_GetIndex() const { return(m_handle & 0xffff); }

	// Desc:	Get the unique identifier for this handle.
	// Output:	Integer assumed to be unique.
	unsigned int m_GetUniqueID() const { return(m_handle >> 16); }

private:

	// Desc:	Internal representation of handle.
	unsigned int m_handle;

}; // class t_Handle

#endif  // __BMH_HANDLE_HEADER__
