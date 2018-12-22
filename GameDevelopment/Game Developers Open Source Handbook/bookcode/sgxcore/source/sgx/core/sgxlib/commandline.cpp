#include "sgx/core/types/types.hpp"
#include "sgx/core/stl/stl.hpp"
#include "sgx/core/helpers/trace.hpp"
#include "sgx/core/sgxlib/commandline.hpp"

CCommandLine::CCommandLine(const int argc, char *argv[])
{
	m_Executable = argv[0];
	// We will assume that any option is application-specific
	// therefore querying for -d will return whatever was after -d
	// even if it is unreleated.

	// So, for every argument, add it to:
	// 1. The argument list
	// 2. The appropriate option list (if the preceeding arg was an opt)
	tBOOL bWasOption = FALSE;
	char last_option;
	for(int i=1;i<argc;++i) {

		// Is this an option?
		if (argv[i][0] == '-') {
			last_option = argv[i][1];
			bWasOption = TRUE;
			continue;
		}
		
		// Process
		if (bWasOption) {
			m_Options.insert(sgxMap<char, sgxString>::value_type(last_option, argv[i]));
			bWasOption = FALSE;
		} else {
			m_Arguments.push_back(argv[i]);
			if (m_ArgumentString != "") {
				m_ArgumentString += " ";
			}
			m_ArgumentString += argv[i];
		}

	}
}

tBOOL	
CCommandLine::GetOption(sgxString &result, const char opt) const
{
	// Find.
	sgxMap<char, sgxString>::const_iterator it = m_Options.find(opt);
	if (it == m_Options.end()) {
		return FALSE;
	} else {
		result = (*it).second;
	}

	return TRUE;
}

tBOOL	
CCommandLine::GetArgument(sgxString &result, const tUINT32 idx) const
{
	if (idx >= m_Arguments.size()) {
		return FALSE;
	}

	result = m_Arguments[idx];
	return TRUE;
}

void	
CCommandLine::GetArgumentString(sgxString &result) const
{
	result = m_ArgumentString;
}

tBOOL	
CCommandLine::GetArgumentString(sgxString &result, const tUINT32 from_idx) const
{
	if (from_idx >= m_Arguments.size()) {
		return FALSE;
	}

	result = "";
	sgxVector<sgxString>::const_iterator it = m_Arguments.begin();
	while(it!=m_Arguments.end()) {
		result += (*it);
		
		if (++it != m_Arguments.end()) {
			result += " ";
		}
	}

	return TRUE;
}

tBOOL	
CCommandLine::GetArgumentString(sgxString &result, const tUINT32 from_idx, const tUINT32 to_idx) const
{
	if (from_idx >= m_Arguments.size() || to_idx >= m_Arguments.size()) {
		return FALSE;
	}

	result = "";
	for(int i=from_idx;i!=to_idx;++i) {
		result += m_Arguments[i];
		
		if (++i != to_idx) {
			result += " ";
		}
	}

	return TRUE;
}

