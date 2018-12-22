// File:	main.cpp
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	02/02/02
//
// Copyright 2002, all rights reserved.
//
// Desc:	Example application.
////////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

// Standard template library.
#include <string>
#include <iostream>

// Handle library.
#include <HandlePointer.h>

// Desc:	Example class to use with handle-based smart pointers.
class t_StringOut
{
public:

	// Desc:	Create new string class for printing.
	// Input:	i_string - String to print.
	explicit t_StringOut(const std::string &i_string) : m_string(i_string) {}

	// Desc:	Write string to standard out.
	void m_Print() { std::cout << m_string << std::endl; }

private:

	// Desc:	String to print.
	const std::string m_string;

}; // class t_StringOut

// Desc:	Type definition for convenience.
typedef t_HandlePointer<t_StringOut> t_StringOutHandle;

// Desc:	Output class.
class t_ExampleOut {
public:

	// Desc:	Initialize with string handles.
	t_ExampleOut(t_StringOutHandle i_string1, t_StringOutHandle i_string2,
		t_StringOutHandle i_string3)
		: m_string1(i_string1), m_string2(i_string2), m_string3(i_string3)
	{}

	// Desc:	Print strings.
	void m_Print()
	{
		// Do nothing if first string is invalid.
		if(m_string1) {
			// Print first string.
			m_string1->m_Print();

			// Print invalid or second string, depending on validity of
			// second string.
			if(!m_string2) {
				std::cout << "an invalid" << std::endl;
			} else {
				m_string2->m_Print();
			}

			// Print third string if valid.
			if(m_string3) {
				m_string3->m_Print();
			}
		} else {
			std::cout << "This is an example of" << std::endl;
			std::cout << "invalid" << std::endl;
			std::cout << "handle pointers." << std::endl;
		}

		// Print blank line.
		std::cout << std::endl;
	}

private:

	// Desc:	Strings to print.
	t_StringOutHandle m_string1;
	t_StringOutHandle m_string2;
	t_StringOutHandle m_string3;

}; // class t_ExampleOut

// Desc:	Application entry.
// Notes:	Although Visual C++ 6.0 allows main to be defined with a void
//			return value, the C++ Standard requires an int return value.
int main()
{
	// Create handle pointers.
	t_StringOutHandle l_string1(new t_StringOut("This is an example of"));
	t_StringOutHandle l_string2(new t_StringOut("a valid"));
	t_StringOutHandle l_string3(new t_StringOut("handle pointer."));

	// Create sentence.
	t_ExampleOut l_sentence(l_string1, l_string2, l_string3);

	// First sentence.
	l_sentence.m_Print();

	// Destroy second string.
	g_Destroy(l_string2);

	// Second sentence.
	l_sentence.m_Print();

	// Destroy remaining strings.
	g_Destroy(l_string1);
	g_Destroy(l_string3);

	// Third sentence.
	l_sentence.m_Print();

	// Wait for key press.
	std::cout << "Press enter to continue." << std::endl;
	fgetc(stdin);

	// Notes:	Visual C++ 6.0 does not match the C++ Standard by allowing an
	//			implicit return(0) in the main function.
	return(0);
}
