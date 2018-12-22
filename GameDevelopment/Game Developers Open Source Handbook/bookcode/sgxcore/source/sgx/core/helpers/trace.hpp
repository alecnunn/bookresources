#ifndef SGX_CORE_HELPERS_TRACE_HPP
#define SGX_CORE_HELPERS_TRACE_HPP	1

#include "sgx/core/stl/stl.hpp"

#define SGX_ERR_REPORT_NONE  6
#define SGX_ERR_REPORT_ALL   0

#define SGX_ERR_FATAL       5
#define SGX_ERR_ERROR       4
#define SGX_ERR_WARNING     3
#define SGX_ERR_MESSAGE     2
#define SGX_ERR_INFO        1

#define SID_FATAL(Msg)      SGX_ERR_FATAL,__FILE__,__LINE__,Msg
#define SID_ERROR(Msg)      SGX_ERR_ERROR,__FILE__,__LINE__,Msg
#define SID_WARNING(Msg)    SGX_ERR_WARNING,__FILE__,__LINE__,Msg
#define SID_MESSAGE(Msg)    SGX_ERR_MESSAGE,__FILE__,__LINE__,Msg
#define SID_INFO(Msg)       SGX_ERR_INFO,__FILE__,__LINE__,Msg

#define SID(Msg)            SID_WARNING(Msg)

//
// Low-level, platform-specific, output
// There must be an implementation of this, present. Even if
// it's only the null driver version.
//
typedef void (*cbfnOutputTrace)(const char *pMessage);
void sgxOutputTraceMessage(const char *pMessage);
void sgxRegisterOutputTrace(const cbfnOutputTrace pfn);

//
// Cross-platform generic output producers
//
void sgxHaltProcessor();

//
#ifdef SGX_DEBUG
  #define sgxError                      CGlobalTrace::Get()->OutputError
  #define sgxTrace                      CGlobalTrace::Get()->OutputTrace
  #define sgxTraceLimit(limit, trace)   do { static tUINT32 n=limit; if (n && n--){ trace }} while(0)
  #define sgxAssert(expr)               do { if (!(expr)) { sgxTrace(SGX_ERR_ERROR,__FILE__,__LINE__,#expr); sgxBreak(); } }while(0)
  #define sgxAssertRts(expr, rval)      do { if (!(expr)) { sgxTrace(SGX_ERR_ERROR,__FILE__,__LINE__,#expr); sgxBreak(); return rval; } }while(0)
  #define sgxCompileAssert(expr)        typedef char COMPILE_ASSERT[(expr)?1:-1]
  #define sgxHalt()                     sgxHaltProcessor()

  #if SGX_PLATFORM_WINTEL32
    #define sgxBreak()	                do { __asm { int 3 } } while(0)
  #else
    #define sgxBreak()	                sgxHalt() 
  #endif

  #define sgxBreakAfter(count)          do { static tUINT32 curr=0; if (++curr > count) sgxBreak(); }while(0)

  #define SGX_SCOPE(fn)                 CScopeTrace scope(fn, __FILE__, __LINE__)
#else
  #define sgxError                      sizeof
  #define sgxTrace                      sizeof
  #define sgxTraceLimit(limit, trace)     
  #define sgxAssert(expr)               sizeof(expr)
  #define sgxAssertRts(expr, rval)      do { if (!(expr)) { return rval; } }while(0)
  #define sgxCompileAssert(expr)
  #define sgxBreak()
  #define sgxBreakAfter(count)
  #define sgxHalt()
  #define SGX_SCOPE(fn)
#endif

//
// Basic handling code
//
class CTraceModule
{
public:
    CTraceModule(tINT32 iDefaultLevel = SGX_ERR_REPORT_ALL);
    void    SetTraceLevel(tINT32 iLevel);
    tINT32  GetTraceLevel(void);

    void    OutputTrace(const char *pMessage, ...);
    void    OutputTrace(const char *pFile, tMEMSIZE iLine, const char *pMessage, ...);
    void    OutputTrace(tINT32 idx, const char *pFile, tMEMSIZE iLine, const char *pMessage, ...);

    void    OutputError(const char *pMessage, ...);
    void    OutputError(const char *pFile, tMEMSIZE iLine, const char *pMessage, ...);

private:
    tINT32  m_iTraceLevel;
};

class CGlobalTrace : public CTraceModule
{
public:
    static CGlobalTrace   *Get();

protected:
    static CGlobalTrace   *ms_pSingleton;
};

class CScopeTrace
{
public:
    CScopeTrace(const char *pFunction) 
    { 
        sgxTrace("FN: Enter %s", pFunction);
        m_FunctionName = sgxString(pFunction);
    }

    CScopeTrace(const char *pFunction, const char *pFilename, tMEMSIZE iLine) 
    { 
        sgxTrace("FN: Enter %s %s(%d)", pFunction, pFilename, iLine);
        m_FunctionName = sgxString(pFunction);
    }

    ~CScopeTrace()
    {
        sgxTrace("FN: Exit %s", m_FunctionName.c_str());
    }

private:
    sgxString m_FunctionName;
};



#endif /* SGX_CORE_HELPERS_TRACE_HPP */
