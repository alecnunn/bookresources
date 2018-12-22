// File:	Flag.h
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	10/07/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	Generic policy-based debug flag.
//			For more information on the concept of policies and other generic
//			programming techniques see Modern C++ Design.
////////////////////////////////////////////////////////////////////////////////

// Include only once per compile.
#pragma once
#ifndef __BMH_FLAG_HEADER__
#define __BMH_FLAG_HEADER__

// Desc:	Treat as standard boolean.
struct t_FlagBooleanMutatorPolicy
{
	// Desc:	Policy initialization type.
	typedef bool t_Type;

	// Desc:	Conversion policy from mutator type to boolean.
	// Input:	i_value - Value to be converted to boolean.
	// Output:  Boolean result of conversion.
	static inline bool m_Convert(t_Type i_value)
	{ return(i_value); }
};

// Desc:	Boolean policy that allows assignment.
struct t_FlagMutablePolicy
{
	typedef bool t_Boolean;
};

// Desc:	Boolean policy that prevents assignment.
struct t_FlagConstantPolicy
{
	typedef const bool t_Boolean;
};

// Desc:	Generic policy-based debug flag.
// Input:	t_InitializationPolicy - Policy determining initialization type.
//			t_AssignmentPolicy - Policy determining assignment type.
//			t_BooleanPolicy - Policy determining storage type.
template <
class t_InitializationPolicy = t_FlagBooleanMutatorPolicy,
class t_AssignmentPolicy = t_FlagBooleanMutatorPolicy,
class t_BooleanPolicy = t_FlagMutablePolicy
>
class t_FlagImplementation
{
public:

	// Desc:	Inherit initialization type.
	typedef t_InitializationPolicy::t_Type t_InitializationType;

	// Desc:	Inherit assignment type.
	typedef t_AssignmentPolicy::t_Type t_AssignmentType;

	// Desc:	Initialize flag.
	// Input:	i_value - Value to be converted to boolean.
	explicit t_FlagImplementation(t_InitializationType i_value)
		: m_flag(t_InitializationPolicy::m_Convert(i_value)) {}

	// Desc:	Assign flag.
	// Input:	i_value - Value to be converted to boolean.
	// Output:	Reference to this flag.
	t_FlagImplementation &operator=(const t_AssignmentType &i_value)
	{ m_flag = t_AssignmentPolicy::m_Convert(i_value); return(*this); }

	// Desc:	Act as boolean type.
	// Output:	Current state of flag.
	operator bool() const { return(m_flag); }

private:

	// Desc:	Current state.
	t_BooleanPolicy::t_Boolean m_flag;

}; // class t_FlagImplementation

// Desc:	Flag impersonator designed to compile out in release builds.
// Input:	t_InitializationPolicy - Policy determining initialization type.
//			t_AssignmentPolicy - Policy determining assignment type.
//			t_BooleanPolicy - Policy determining storage type.
template <
class t_InitializationPolicy = t_FlagBooleanMutatorPolicy,
class t_AssignmentPolicy = t_FlagBooleanMutatorPolicy,
class t_BooleanPolicy = t_FlagMutablePolicy
>
class t_FlagDisable
{
public:

	// Desc:	Inherit initialization type.
	typedef t_InitializationPolicy::t_Type t_InitializationType;

	// Desc:	Inherit assignment type.
	typedef t_AssignmentPolicy::t_Type t_AssignmentType;

	// Desc:	Initialize flag.
	// Input:	i_value - Ignored.
	explicit t_FlagDisable(t_InitializationType i_value) {}

	// Desc:	Assign flag.
	// Input:	i_value - Ignored.
	// Output:	Reference to this flag.
	t_FlagDisable &operator=(const t_AssignmentType &i_value)
	{ return(*this); }

	// Desc:	Act as boolean type.
	// Output:	False.
	operator bool() const { return(false); }

}; // class t_FlagDisable

#endif  // __BMH_FLAG_HEADER__
