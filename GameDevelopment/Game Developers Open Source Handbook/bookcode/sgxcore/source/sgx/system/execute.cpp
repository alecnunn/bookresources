#include "sgx/core/core.hpp"
#include "sgx/system/system.hpp"

ExecuteProperties::ExecuteProperties()
{
	sgxError("Must have at least a command");
}


ExecuteProperties::ExecuteProperties(const sgxString &command)
: m_Command(command)
{
}

ExecuteProperties::ExecuteProperties(const sgxString &command, const sgxString &arguments)
: m_Command(command), m_Arguments(arguments)
{
}

ExecuteProperties::ExecuteProperties(const sgxString &command, const sgxVector<sgxString> &arglist)
: m_Command(command)
{
sgxVector<sgxString>::const_iterator it = arglist.begin();

	for(;;) {
		m_Arguments += (*it);
		if (++it == arglist.end()) {
			break;
		} else {
			m_Arguments += " ";
		}
	}
}

tINT32	
ExecuteProperties::Execute()
{
	return sgxSystemExecute(*this);
}



