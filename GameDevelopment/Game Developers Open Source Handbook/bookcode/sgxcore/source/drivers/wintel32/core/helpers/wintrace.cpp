#include "sgx/core/core.hpp"
#include "drivers/wintel32/wintel32core.hpp"
#include "drivers/wintel32/core/helpers/wintrace.hpp"

void
CWin32Output::Trace(const char *pMessage)
{
	OutputDebugString(pMessage);
	OutputDebugString("\n");
}

void
CWin32Output::Create()
{
	sgxRegisterOutputTrace(CWin32Output::Trace);
}
