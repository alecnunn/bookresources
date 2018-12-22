/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __BASETHREAD_H__
#define __BASETHREAD_H__

//---------------------------------------------------------------------------------------
#include "baseos.h"
#include <process.h>
#ifdef  MS_WIN32
  #include <eh.h>
#endif //MS_WIN32    

//---------------------------------------------------------------------------------------
// Macros 
#define INVALID_HANDLE(x) ((x) == INVALID_HANDLE_VALUE)
#define VALID_HANDLE(x) ((x) != INVALID_HANDLE_VALUE)
#define DELETTHIS	-10

//---------------------------------------------------------------------------------------
// inline static thread proc. builds a function name from class name
// and 'Callback'. Allways this roll's up a member
// function '_method' passing the stop event.
typedef unsigned (__stdcall *PTHREAD_START)(void*);
#define IMPLEMENT_TP(_class, _method)                                   \
protected:                                                              \
static unsigned __stdcall _class##ThrCallback(void* pVoid){             \
        _class* pThis = reinterpret_cast<_class*>(pVoid);               \
        HANDLE hStartUp = pThis->GetStartUpEvent();                     \
        ::SetEvent(hStartUp);                                           \
        ::Sleep(0);                                                     \
        pThis->m_dwExitCode = 0;                                        \
        pThis->m_dwExitCode = pThis->_method(pThis->m_hStop);           \
        return  0;														\
    };                                                                  \
virtual PTHREAD_START GetThreadProc() {return _class##::_class##ThrCallback;};\
UINT _method(const HANDLE hStophandle)
// safe closes handles
#define SAFE_CLOSEHANDLE(_handle)  if(VALID_HANDLE(_handle))\
                                    {\
                                        BOOL b = ::CloseHandle(_handle);\
                                        assert(b);\
                                        if(b)\
                                            _handle = INVALID_HANDLE_VALUE;\
                                    }     
// Wraps the _beginthreadex to accept the parameters as CreateThread call
#define BEGINTHREADEX(psa, cbStack, pfnStartAddr, pvParam, fdwCreate, pdwThreadID)  \
                    ((HANDLE) _beginthreadex((void*)psa, (UINT)cbStack,             \
                                    (PTHREAD_START)(pfnStartAddr),                  \
                                    (void*)pvParam, (UINT)(fdwCreate),              \
                                    (UINT*)(pdwThreadID)))

//---------------------------------------------------------------------------------------
// main class of thread
class CThread
{
public:
    CThread():m_hThread(INVALID_HANDLE_VALUE),
            m_hStop(INVALID_HANDLE_VALUE),
            m_hStartUp(INVALID_HANDLE_VALUE),
            m_internalThrID(0),
            m_dwExitCode(0),m_nLoopCount(0)
    {
        ::InterlockedIncrement(&CThread::INTERNALTHRID);
        ::InterlockedExchange(&m_internalThrID, CThread::INTERNALTHRID);
        printf("Thread() %d \r\n", m_internalThrID);
        m_testAtClose = TRUE;
    }
    CThread::~CThread()
    {
        if(m_testAtClose)
        {
            assert (0 == IsAlive());
            assert(INVALID_HANDLE(m_hStop));
            assert(INVALID_HANDLE(m_hThread));
            assert(INVALID_HANDLE(m_hStartUp));
        }
        ::InterlockedDecrement(&CThread::INTERNALTHRID);
        printf("~Thread() %d \r\n", m_internalThrID);
    }

    operator HANDLE(){
        return m_hThread;
    }
    virtual BOOL Start()
    {
        assert(INVALID_HANDLE(m_hThread));
        assert(INVALID_HANDLE(m_hStop));

        if( VALID_HANDLE(m_hThread) ||
                VALID_HANDLE(m_hStop) ||
                VALID_HANDLE(m_hStartUp))
            return FALSE;
        // manual reset event
        m_hStop = ::CreateEvent(NULL, TRUE, FALSE, 0);
        if(INVALID_HANDLE(m_hStop)){
            Stop();
            return FALSE;
        }
        // automatic reset event. Blocks creation until thread foo enters
        m_hStartUp = ::CreateEvent(NULL, FALSE, FALSE, 0);
        if(INVALID_HANDLE(m_hStartUp)){
            Stop();
            return FALSE;
        }
        assert(GetThreadProc());
        DWORD  nDummy = 0;
        HANDLE hTemp = ::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetThreadProc(),
                                      static_cast<void*>(this),
                                      CREATE_SUSPENDED, &nDummy);
        if(0 == hTemp){
            Stop();
            return FALSE;
        }
        m_hThread = hTemp;
        if(-1 == ::ResumeThread(m_hThread)){
            Kill();
            return FALSE;
        }
        // wait's for safe startup
        DWORD dwRetCode = ::WaitForSingleObject(m_hStartUp, WAIT_START);
        assert(VALID_HANDLE(m_hStartUp));
        SAFE_CLOSEHANDLE(m_hStartUp);
        if(dwRetCode != WAIT_OBJECT_0){
            Kill();
            return FALSE;
        }
        return IsAlive() ? TRUE : FALSE;
    }

    virtual void CThread::Stop()
    {
        if(VALID_HANDLE(m_hStop) && VALID_HANDLE(m_hThread)){
            //      no multithreaded dll link
            //      DWORD dwRetVal = ::SignalObjectAndWait(m_hStop, m_hThread,
            //                                             WAIT_TERMINATE, FALSE);
            SetEvent(m_hStop);
            Sleep(1000);
            DWORD dwRetVal=WaitForSingleObject(m_hThread, WAIT_TERMINATE);
            if(WAIT_OBJECT_0 != dwRetVal){
                Kill();
            }
        }
        SafeCloseHandlers();
        assert (0 == IsAlive());
    }
    void Kill(){                            // unsave call
        if(VALID_HANDLE(m_hThread))
            if(IsAlive()){
                BOOL b = ::TerminateThread(m_hThread, 0);
                assert(b);
            }
        Sleep(1);
        SafeCloseHandlers();
    };
    BOOL IsAlive()
    {
        if(VALID_HANDLE(m_hThread)){
            DWORD code = 0;
            BOOL  b    = GetExitCodeThread(m_hThread, &code);
            assert(b);
            return (code == STILL_ACTIVE);
        }
        return FALSE;
    }

    void CThread::IssueStop()
    {
        assert(VALID_HANDLE(m_hStop));
        if(m_hStop>0){
            BOOL b = ::SetEvent(m_hStop);
            assert(b);
        }
    }
    long IsSuspended(){
        long lr;
        ::InterlockedExchange(&lr,m_suspend);
        return lr;
    }
    void Resume(){
        int i = ::ResumeThread(m_hThread);
        if(-1 == i){
            int err = GetLastError();
            assert(0);
        }
        ::InterlockedDecrement(&m_suspend);
    }
    void Suspend(){
        int i = ::SuspendThread(m_hThread);
        if(-1 == i){
            int err = GetLastError();
            assert(0);
        }
        ::InterlockedIncrement(&m_suspend);
    }

    LONG    GetSpinCount()const{
        return m_nLoopCount;
    };
    LONG    GetID()const{
        return m_internalThrID;
    }

protected:
    virtual PTHREAD_START GetThreadProc(){
        return  0;
    };
    HANDLE GetStartUpEvent() const{
        return m_hStartUp;
    }
    void IncrementLoopCount() {++m_nLoopCount;}     // increments
    void SafeCloseHandlers(){                       // closes all handlers
        SAFE_CLOSEHANDLE(m_hStartUp);
        SAFE_CLOSEHANDLE(m_hThread);
        SAFE_CLOSEHANDLE(m_hStop);
    }
protected:
    //  only valid when running.
    HANDLE          m_hThread;
    HANDLE          m_hStop;
    HANDLE          m_hStartUp;
    DWORD           m_dwExitCode;
    DWORD           m_nLoopCount;
    long            m_internalThrID;
    long            m_suspend;
    BOOL			m_testAtClose;
public:
    static long     INTERNALTHRID;
    static DWORD    WAIT_START;
    static DWORD    WAIT_TERMINATE;
};

__declspec (selectany) DWORD CThread::WAIT_START	 = 30000;    // 3 seconds. mco
__declspec (selectany) DWORD CThread::WAIT_TERMINATE = 10000;    // 10 seconds. mco
__declspec (selectany) long  CThread::INTERNALTHRID	 = 0;


#endif //__BASETHREAD_H__


