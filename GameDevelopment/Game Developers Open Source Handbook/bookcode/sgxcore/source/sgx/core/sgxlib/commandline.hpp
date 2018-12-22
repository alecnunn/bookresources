#ifndef SGX_CORE_SGXLIB_COMMANDLINE_HPP
#define SGX_CORE_SGXLIB_COMMANDLINE_HPP	1

#include "sgx/core/stl/stl.hpp"
#include "sgx/core/helpers/trace.hpp"

class CCommandLine {
public:
	CCommandLine(int argc, char *argv[]);

	tBOOL	GetOption(sgxString &result, const char opt) const;
	tBOOL	GetArgument(sgxString &result, const tUINT32 idx) const;
	tBOOL	GetArgumentString(sgxString &result, const tUINT32 from_idx) const;
	tBOOL	GetArgumentString(sgxString &result, const tUINT32 from_idx, const tUINT32 to_idx) const;
	void	GetArgumentString(sgxString &result) const;

private:
	CCommandLine() { sgxError("No default construction, please!"); }

	sgxString				m_Executable;
	sgxString				m_ArgumentString;
	sgxVector<sgxString>	m_Arguments;
	sgxMap<char, sgxString>	m_Options;
};

#endif /* SGX_CORE_SGXLIB_COMMANDLINE_HPP */
