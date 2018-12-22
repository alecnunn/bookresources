// File:	Log.h
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	10/16/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	Generic policy-based logging.
//			For more information on the concept of policies and other generic
//			programming techniques see Modern C++ Design.
////////////////////////////////////////////////////////////////////////////////

// Include only once per compile.
#pragma once
#ifndef __BMH_LOG_HEADER__
#define __BMH_LOG_HEADER__

// Standard template library.
#include <string>
#include <sstream>

// Local headers.
#include "Flag.h"	// For default flag policy.

// Desc:	Syntax for logging calls.
// Notes:	A preprocessor define is required here to preserve the short
//			circuit behaviour of the logical and for increased performance
//			on disabled logs.
#define LOG(type) (type) && (type)

// Desc:	Log buffer policy that uses the standard template library
//			string stream class.
struct t_StringStreamBufferPolicy
{
	// Desc:	Buffer is a standard string stream.
	typedef std::stringstream t_Type;

	// Desc:	Return the string stored in the buffer.
	// Input:	i_buffer - String stream buffer containing string.
	// Output:	String in buffer.
	static inline const std::string m_ToString(const t_Type &i_buffer)
	{ return(i_buffer.str()); }

	// Desc:	Clear the contents of the buffer.
	// Input:	o_buffer - Buffer to clear.
	static inline void m_Clear(t_Type &o_buffer)
	{ o_buffer.str(std::string()); }

}; // struct t_StringStreamBufferPolicy

// Desc:	Dispatch string to standard output.
struct t_StandardOutDispatchPolicy
{
	// Desc:	Dispatch string.
	// Input:	i_string - String to dispatch.
	static void m_Dispatch(const std::string &i_string);
};

// Desc:	Dispatch string to standard error.
struct t_StandardErrorDispatchPolicy
{
	// Desc:	Dispatch string.
	// Input:	i_string - String to dispatch.
	static void m_Dispatch(const std::string &i_string);
};

// Desc:	Generic policy-based logging.
// Input:	t_FlagPolicy - Policy determining logging enable/disable.
//			t_DispatchPolicy - Policy determining logging string dispatch.
//			t_BufferPolicy - Policy determining logging string buffer type.
template <
class t_FlagPolicy = t_FlagImplementation<>,
class t_DispatchPolicy = t_StandardOutDispatchPolicy,
class t_BufferPolicy = t_StringStreamBufferPolicy
>
class t_LogImplementation
{
public:

	// Desc:	Inherit flag initialization type.
	typedef t_FlagPolicy::t_InitializationType t_InitializationType;

	// Desc:	Inherit flag assignment type.
	typedef t_FlagPolicy::t_AssignmentType t_AssignmentType;

	// Desc:	Initialize flag.
	// Input:	i_value - Value to be converted to boolean.
	explicit t_LogImplementation(t_InitializationType i_value)
		: m_flag(i_value) {}

	// Desc:	Assign flag.
	// Input:	i_value - Value to be converted to boolean.
	// Output:	Reference to this log.
	t_LogImplementation &operator=(const t_AssignmentType &i_value)
	{ m_flag = i_value; return(*this); }

	// Desc:	If flag is true, dispatch the buffer when log is used as
	//			boolean.
	// Output:	Flag value, m_Dispatch should always return true.
	// Notes:	Dispatch on bool is used as to allow the use of log
	//			in logical and expression to take advantage of short-circuit
	//			behaviour for performance reasons.
	operator bool() { return(m_flag && m_Dispatch()); }

	// Desc:	Template member function to translate operator<< to
	//			string buffer operator<<.
	// Input:	i_value - Value to add to string buffer.
	// Output:	Reference to this log.
	template <typename t_Type>
		t_LogImplementation& operator<<(t_Type i_value)
	{ m_buffer << i_value; return(*this); }

private:

	// Desc:	Use buffer and dispatch policy to dispatch buffer string
	//			to dispatch destinations.
	// Output:	Always true.
	bool m_Dispatch()
	{
		// Convert buffer to string.
		const std::string l_string = t_BufferPolicy::m_ToString(m_buffer);

		// Only dispatch when the buffer contains something.
		if(!l_string.empty()) {
			// Convert buffer to string and dispatch that string.
			t_DispatchPolicy::m_Dispatch(l_string);

			// Clear the buffer.
			t_BufferPolicy::m_Clear(m_buffer);
		}

		// Always return true.
		return(true);
	}

	// Desc:	Log is implemented in terms of the flag policy.
	t_FlagPolicy m_flag;

	// Desc:	Log string buffer based on buffer policy type.
	t_BufferPolicy::t_Type m_buffer;

}; // class t_Log

#endif  // __BMH_LOG_HEADER__
