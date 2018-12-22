#include "sgx/core/types/types.hpp"
#include "sgx/core/sgxlib/utils.hpp"
#include "sgx/core/helpers/trace.hpp"
#include <stdarg.h>
#include <cstdio>

CTraceModule::CTraceModule(tINT32 iDefaultLevel/* = SGX_ERR_REPORT_ALL*/)
{
    SetTraceLevel(iDefaultLevel);
}

void 
CTraceModule::SetTraceLevel(tINT32 iLevel)
{
    m_iTraceLevel = sgxRange(iLevel, SGX_ERR_REPORT_ALL, SGX_ERR_REPORT_NONE);
}

tINT32 
CTraceModule::GetTraceLevel(void)
{
    return m_iTraceLevel;
}

void 
CTraceModule::OutputTrace(const char *pMessage, ...)
{
    static char buffer[1024];

    va_list va;

	// TODO: This is not thread-safe
    va_start(va, pMessage);
    vsprintf(buffer, pMessage, va);
    va_end(va);

    sgxOutputTraceMessage(buffer);
}

void 
CTraceModule::OutputTrace(tINT32 idx, const char *pFile, tMEMSIZE iLine, const char *pMessage, ...)
{
    static char *pErrLvl[] = {"", "Info", "Msg", "Warn", "Error", "Fatal", ""};
    static char buffer[1024];
    
    if (idx < m_iTraceLevel) {
        return;
    }

    tUINT32 lvl = sgxRange(idx, SGX_ERR_INFO, SGX_ERR_FATAL);

    sprintf(buffer, "%s(%d): %s : ", pFile, iLine, pErrLvl[lvl]);
    va_list va;

    va_start(va, pMessage);
    vsprintf(strchr(buffer, '\0'), pMessage, va);
    va_end(va);

    sgxOutputTraceMessage(buffer);

    if (idx == SGX_ERR_FATAL) {
        sgxHalt();
    }
}


void 
CTraceModule::OutputError(const char *pMessage, ...)
{
    static char buffer[1024];

    va_list va;

    va_start(va, pMessage);
    vsprintf(buffer, pMessage, va);
    va_end(va);

    sgxOutputTraceMessage(buffer);
    sgxHalt();
}

void 
CTraceModule::OutputError(const char *pFile, tMEMSIZE iLine, const char *pMessage, ...)
{
    static char buffer[1024];
    
    sprintf(buffer, "%s(%d): ", pFile, iLine);
    va_list va;

    va_start(va, pMessage);
    vsprintf(strchr(buffer, '\0'), pMessage, va);
    va_end(va);

    sgxOutputTraceMessage(buffer);
    sgxHalt();
}
