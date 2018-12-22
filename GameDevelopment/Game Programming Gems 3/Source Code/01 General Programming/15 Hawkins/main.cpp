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
#include <iostream>

// Log library.
#include <FlagConfiguration.h>
#include <Log.h>

// Desc:	Only provide logging in the debug build.
#ifdef _DEBUG
#define t_BaseFlag t_FlagImplementation
#else // _DEBUG
#define t_BaseFlag t_FlagDisable
#endif // _DEBUG

// Desc:	Create an error log.
// Notes:	Errors are always logged so this log type is a const boolean
//			log that dispatches to cerr.
typedef t_BaseFlag<t_FlagBooleanMutatorPolicy, t_FlagBooleanMutatorPolicy,
	t_FlagConstantPolicy> t_ConstantFlag;
t_LogImplementation<t_ConstantFlag, t_StandardErrorDispatchPolicy>
	__ERROR(true);

// Desc:	Create a run-time configurable log type.
typedef t_BaseFlag<t_FlagConfigurationPolicy<>, t_FlagBooleanMutatorPolicy,
	t_FlagConstantPolicy> t_ConfigurationFlag;
typedef t_LogImplementation<t_ConfigurationFlag> t_Log;

// Desc:	Create example logs.
t_Log __MAIN("MAIN");
t_Log __WARNING("WARNING");
t_Log __EXAMPLE("EXAMPLE");

// Desc:	Application entry.
// Notes:	Although Visual C++ 6.0 allows main to be defined with a void
//			return value, the C++ Standard requires an int return value.
int main()
{
	// Log function entry.
	LOG(__MAIN) << "Function Entry: int main()\n";

	// Perform some calculations that can be tracked through logging.
	int lValue = 100;
	for(int lDivisor = 1; lDivisor < 6; ++lDivisor) {
		// Compute modulo divisor.
		int lModifiedDivisor = lDivisor % 4;

		// Test for divide by zero.
		if(lModifiedDivisor) {
			// Test for remainder.
			if(lValue % lModifiedDivisor) {
				LOG(__WARNING) << "WARNING: Remainder " <<
					(lValue % lModifiedDivisor) << " lost\n";
			}

			// Perform division.
			LOG(__EXAMPLE) << lValue << " / " << lModifiedDivisor << " = " <<
				(lValue / lModifiedDivisor) << "\n";
			lValue /= lModifiedDivisor;
		} else {
			LOG(__ERROR) << "ERROR: Division by zero\n";
		}
	}

	// Report result.
	std::cout << "Value = " << lValue << std::endl;

	// Wait for key press.
	std::cout << "Press enter to continue." << std::endl;
	fgetc(stdin);

	// Notes:	Visual C++ 6.0 does not match the C++ Standard by allowing an
	//			implicit return(0) in the main function.
	return(0);
}
