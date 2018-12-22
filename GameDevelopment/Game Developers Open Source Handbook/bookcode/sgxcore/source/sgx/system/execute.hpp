#ifndef SGX_SYSTEM_SYSTEM_EXECUTE_HPP
#define SGX_SYSTEM_SYSTEM_EXECUTE_HPP	1

#include "sgx/core/types/types.hpp"
#include "sgx/core/stl/stl.hpp"

class ExecuteProperties {
public:
	sgxString	m_Command;
	sgxString	m_Arguments;
	sgxString	m_Stdin;
	sgxString	m_Stdout;
	sgxString	m_Stderr;

	ExecuteProperties(const sgxString &command);
	ExecuteProperties(const sgxString &command, const sgxVector<sgxString> &arglist);
	ExecuteProperties(const sgxString &command, const sgxString &arguments);

	tINT32	Execute();

private:
	ExecuteProperties();
};

tINT32 sgxSystemExecute(const ExecuteProperties &props);

#endif /* SGX_SYSTEM_SYSTEM_EXECUTE_HPP */
