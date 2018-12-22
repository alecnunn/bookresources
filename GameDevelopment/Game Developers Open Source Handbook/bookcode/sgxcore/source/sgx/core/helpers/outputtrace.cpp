#include "sgx/core/types/types.hpp"
#include "sgx/core/stl/stl.hpp"
#include "sgx/core/helpers/trace.hpp"

namespace {
	cbfnOutputTrace pcbOutputTrace;
};

void sgxRegisterOutputTrace(const cbfnOutputTrace pfn)
{
	pcbOutputTrace = pfn;
}

void sgxOutputTraceMessage(const char *pMessage)
{
	if (pcbOutputTrace) {
		(*pcbOutputTrace)(pMessage);
	}
}

