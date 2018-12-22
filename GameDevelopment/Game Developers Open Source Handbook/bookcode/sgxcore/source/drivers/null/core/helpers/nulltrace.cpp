#include "sgx/core/core.hpp"
#include "drivers/null/nullcore.hpp"
#include "drivers/null/core/helpers/nulltrace.hpp"

void
CNullOutput::Trace(const char *pMessage)
{
	// NULLSTUB : Write the pMessage, with cr, to a debug window
}

void
CNullOutput::Create()
{
	sgxRegisterOutputTrace(CNullOutput::Trace);
}
