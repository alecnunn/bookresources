//
// Game Programming Gems 6
// Lock free multithreaded algorithms
// By Toby Jones (thjones@microsoft.com)
// Supports Microsoft Visual C++ and GCC.
//

#include <iostream>
#include <new>
#include <cassert>
#include <vector>
#include <algorithm>

#include <windows.h>
#include <process.h>

#ifdef _MSC_VER
#pragma warning(disable: 4127) // conditional expression is constant
#pragma warning(disable: 4311) // pointer truncation
#endif

//
// uint32_t is defined in standard C, but not yet in standard C++.
//
typedef unsigned int uint32_t;

//------------------------------------------------------------------------------
//
// Definitions of CAS.
//
//------------------------------------------------------------------------------

//
// Define what code we will be using for CAS.
// Intrinsic only available on Visual C++.
//
#ifndef CAS
#define CAS CAS_assembly
//#define CAS CAS_intrinsic
//#define CAS CAS_windows
#endif

// this function is atomic
// bool CAS(uint32_t *ptr, uint32_t oldVal, uint32_t newVal) {
//     if(*ptr == oldVal) {
//         *ptr = newVal;
//         return true;
//     }
//     return false;
// }

//
// All of the CAS functions will operate on a node.  So we define node first.
//
template<typename T> struct node {
    T value;
    node<T> * volatile pNext;

    node() : value(), pNext(NULL) {}
    node(T v) : pNext(NULL), value(v) {}
};

// CAS will assume a multi-processor machine (versus multithread on a single processor).
// On a single processor machine, it might not make sense to spin on a CAS because
// if it fails, there is no way to succeed until the another thread runs (which will be
// on the same processor).  In these cases, if CAS is required to succeed, it might make
// more sense to yield the processor to the other thread via Sleep(1).

// Since we are assuming a multi-processor machine, we will need to use 'lock cmpxchg'
// instead of just cmpxchg, as the bus needs to be locked to synchronize the processors
// access to memory.

//
// Define a version of CAS which uses x86 assembly primitives.
//
template<typename T>
bool CAS_assembly(node<T> * volatile * _ptr, node<T> * oldVal, node<T> * newVal)
{
    register bool f;

#ifdef __GNUC__
    __asm__ __volatile__(
        "lock; cmpxchgl %%ebx, %1;"
        "setz %0;"
            : "=r"(f), "=m"(*(_ptr))
            : "a"(oldVal), "b" (newVal)
            : "memory");
#else
    _asm
    {
        mov ecx,_ptr
        mov eax,oldVal
        mov ebx,newVal
        lock cmpxchg [ecx],ebx
        setz f
    }
#endif // __GNUC__

    return f;
}

//
// Define a version of CAS which uses the Visual C++ InterlockedCompareExchange intrinsic.
//
#ifdef _MSC_VER

// Define intrinsic for InterlockedCompareExchange
extern "C" long __cdecl _InterlockedCompareExchange(long volatile * Dest, long Exchange, long Comp);

#pragma intrinsic (_InterlockedCompareExchange)

template<typename T>
bool CAS_intrinsic(node<T> * volatile * _ptr, node<T> * oldVal, node<T> * newVal)
{
    return _InterlockedCompareExchange((long *)_ptr, (long)newVal, (long)oldVal) == (long)oldVal;
}

#endif  // _MSC_VER

//
// Define a version of CAS which uses the Windows API InterlockedCompareExchange.
//
template<typename T>
bool CAS_windows(node<T> * volatile * _ptr, node<T> * oldVal, node<T> * newVal)
{
    return InterlockedCompareExchange((long *)_ptr, (long)newVal, (long)oldVal) == (long)oldVal;
}

//------------------------------------------------------------------------------
//
// Definitions of CAS2.
//
//------------------------------------------------------------------------------

//
// Define what code we will be using for CAS.
// Intrinsic only available on Visual C++.
// Windows version only available on Windows Vista.
//
#ifndef CAS2
#define CAS2 CAS2_assembly
//#define CAS2 CAS2_intrinsic
//#define CAS2 CAS2_windows
#endif

//
// Define a version of CAS2 which uses x86 assembly primitives.
//
template<typename T>
bool CAS2_assembly(node<T> * volatile * _ptr, node<T> * old1, uint32_t old2, node<T> * new1, uint32_t new2)
{
    register bool f;
#ifdef __GNUC__
    __asm__ __volatile__(
        "lock; cmpxchg8b %1;"
        "setz %0;"
            : "=r"(f), "=m"(*(_ptr))
            : "a"(old1), "b" (new1), "c" (new2), "d" (old2)
            : "memory");
#else
    _asm
    {
        mov esi,_ptr
        mov eax,old1
        mov edx,old2
        mov ebx,new1
        mov ecx,new2
        lock cmpxchg8b [esi]
        setz f
    }
#endif
    return f;
}

//
// Define a version of CAS2 which uses the Visual C++ InterlockedCompareExchange64 intrinsic.
//
#ifdef _MSC_VER

// Define intrinsic for InterlockedCompareExchange64
extern "C" __int64 __cdecl _InterlockedCompareExchange64(__int64 volatile * Destination, __int64 Exchange, __int64 Comperand);

#pragma intrinsic (_InterlockedCompareExchange64)

template<typename T>
bool CAS2_intrinsic(node<T> * volatile * _ptr, node<T> * old1, uint32_t old2, node<T> * new1, uint32_t new2)
{
    LONGLONG Comperand = reinterpret_cast<LONG>(old1) | (static_cast<LONGLONG>(old2) << 32);
    LONGLONG Exchange  = reinterpret_cast<LONG>(new1) | (static_cast<LONGLONG>(new2) << 32);

    return _InterlockedCompareExchange64(reinterpret_cast<LONGLONG volatile *>(_ptr), Exchange, Comperand) == Comperand;
}

#endif  // _MSC_VER

//
// Define a version of CAS2 which uses the Windows API InterlockedCompareExchange64.
// InterlockedCompareExchange64 requires Windows Vista
//
#if WINVER >= 0x0600

// LONGLONG InterlockedCompareExchange64(LONGLONG volatile * Destination, LONGLONG Exchange, LONGLONG Comperand);

template<typename T>
bool CAS2_windows(node<T> * volatile * _ptr, node<T> * old1, uint32_t old2, node<T> * new1, uint32_t new2)
{
    LONGLONG Comperand = reinterpret_cast<long>(old1) | (static_cast<LONGLONG>(old2) << 32);
    LONGLONG Exchange  = reinterpret_cast<long>(new1) | (static_cast<LONGLONG>(new2) << 32);

    return InterlockedCompareExchange64(reinterpret_cast<LONGLONG volatile *>(_ptr), Exchange, Comperand) == Comperand;
}
#endif  // WINVER >= 0x0600

//------------------------------------------------------------------------------
//
// Parameterized Lock-free Stack
//
//------------------------------------------------------------------------------
template<typename T> class LockFreeStack {
    // NOTE: the order of these members is assumed by CAS2.
    node<T> * volatile _pHead;
    volatile uint32_t  _cPops;

public:
    void Push(node<T> * pNode);
    node<T> * Pop();

    LockFreeStack() : _pHead(NULL), _cPops(0) {}
};

template<typename T> void LockFreeStack<T>::Push(node<T> * pNode)
{
    while(true)
    {
        pNode->pNext = _pHead;
        if(CAS(&_pHead, pNode->pNext, pNode))
        {
            break;
        }
    }
}

template<typename T> node<T> * LockFreeStack<T>::Pop()
{
    while(true)
    {
        node<T> * pHead = _pHead;
        uint32_t  cPops = _cPops;
        if(NULL == pHead)
        {
            return NULL;
        }

        // NOTE: Memory reclaimation is difficult in this context.  If another thread breaks in here
        // and pops the head, and then frees it, then pHead->pNext is an invalid operation.  One solution
        // would be to use hazard pointers (http://researchweb.watson.ibm.com/people/m/michael/ieeetpds-2004.pdf).

        node<T> * pNext = pHead->pNext;
        if(CAS2(&_pHead, pHead, cPops, pNext, cPops + 1))
        {
            return pHead;
        }
    }
}

//------------------------------------------------------------------------------
//
// Parameterized Lock-free Queue
//
//------------------------------------------------------------------------------
template<typename T> class LockFreeQueue {
    // NOTE: the order of these members is assumed by CAS2.
    node<T> * volatile _pHead;
    volatile uint32_t  _cPops;
    node<T> * volatile _pTail;
    volatile uint32_t  _cPushes;

public:
    void Add(node<T> * pNode);
    node<T> * Remove();

    LockFreeQueue(node<T> * pDummy) : _cPops(0), _cPushes(0)
    {
        _pHead = _pTail = pDummy;
    }
};

template<typename T> void LockFreeQueue<T>::Add(node<T> * pNode) {
    pNode->pNext = NULL;

    uint32_t cPushes;
    node<T> * pTail;

    while(true)
    {
        cPushes = _cPushes;
        pTail = _pTail;

        // NOTE: The Queue has the same consideration as the Stack.  If _pTail is
        // freed on a different thread, then this code can cause an access violation.

        // If the node that the tail points to is the last node
        // then update the last node to point at the new node.
        if(CAS(&(_pTail->pNext), reinterpret_cast<node<T> *>(NULL), pNode))
        {
            break;
        }
        else
        {
            // Since the tail does not point at the last node,
            // need to keep updating the tail until it does.
            CAS2(&_pTail, pTail, cPushes, _pTail->pNext, cPushes + 1);
        }
    }

    // If the tail points to what we thought was the last node
    // then update the tail to point to the new node.
    CAS2(&_pTail, pTail, cPushes, pNode, cPushes + 1);
}

template<typename T> node<T> * LockFreeQueue<T>::Remove() {
    T value = T();
    node<T> * pHead;

    while(true)
    {
        uint32_t cPops = _cPops;
        uint32_t cPushes = _cPushes;
        pHead = _pHead;
        node<T> * pNext = pHead->pNext;

        // Verify that we did not get the pointers in the middle
        // of another update.
        if(cPops != _cPops)
        {
            continue;
        }
        // Check if the queue is empty.
        if(pHead == _pTail)
        {
            if(NULL == pNext)
            {
                pHead = NULL; // queue is empty
                break;
            }
            // Special case if the queue has nodes but the tail
            // is just behind. Move the tail off of the head.
            CAS2(&_pTail, pHead, cPushes, pNext, cPushes + 1);
        }
        else if(NULL != pNext)
        {
            value = pNext->value;
            // Move the head pointer, effectively removing the node
            if(CAS2(&_pHead, pHead, cPops, pNext, cPops + 1))
            {
                break;
            }
        }
    }
    if(NULL != pHead)
    {
        pHead->value = value;
    }
    return pHead;
}

//------------------------------------------------------------------------------
//
// Parameterized Lock-free Freelist
//
//------------------------------------------------------------------------------
template<typename T> class LockFreeFreeList {
    //
    // Memory reclaimation is generally difficult with lock-free algorithms,
    // so we bypass the situation by making the object own all of the memory,
    // and creating and destroying the nodes on the thread that controls the
    // object's lifetime.  Any thread synchronization should be done at that
    // point.
    //
    LockFreeStack<T> _Freelist;
    node<T> * _pObjects;
    const uint32_t _cObjects;

public:
    //
    // cObjects is passed to the constructor instead of a template parameter
    // to minimize the code bloat of multiple freelists with varying sizes,
    // but each using the same underlying type.
    //
    LockFreeFreeList(uint32_t cObjects) : _cObjects(cObjects)
    {
        //
        // The Freelist may live on the stack, so we allocate the
        // actual nodes on the heap to minimize the space hit.
        //
        _pObjects = new node<T>[cObjects];
        FreeAll();
    }
    ~LockFreeFreeList()
    {
#ifdef _DEBUG
        for(uint32_t ix = 0; ix < _cObjects; ++ix)
        {
            assert(_Freelist.Pop() != NULL);
        }
#endif

        delete[] _pObjects;
    }
    void FreeAll()
    {
        for(uint32_t ix = 0; ix < _cObjects; ++ix)
        {
            _Freelist.Push(&_pObjects[ix]);
        }
    }
    T * NewInstance()
    {
        node<T> * pInstance = _Freelist.Pop();
        return new(&pInstance->value) T;
    }
    void FreeInstance(T * pInstance)
    {
        pInstance->~T();
        _Freelist.Push(reinterpret_cast<node<T> *>(pInstance));
    }
};

//------------------------------------------------------------------------------
//
// Test/Demo code for Lock-free Algorithms
//
//------------------------------------------------------------------------------
struct MyStruct
{
    int iValue;
    short sValue;
    char cValue;
};

typedef double TEST_TYPE;
const bool FULL_TRACE = false;

//
// Verify Assembly version of CAS.
//
void Test_CAS_assembly()
{
    std::cout << "Testing CAS_assembly...";

    node<MyStruct> oldVal;
    node<MyStruct> newVal;
    node<MyStruct> * pNode = &newVal;
    if(CAS_assembly(&pNode, &oldVal, &newVal))
    {
        std::cout << "CAS is INCORRECT." << std::endl;
    }
    else
    {
        pNode = &oldVal;
        if(!CAS_assembly(&pNode, &oldVal, &newVal))
        {
            std::cout << "CAS is INCORRECT." << std::endl;
        }
        else if(pNode != &newVal)
        {
            std::cout << "CAS is INCORRECT." << std::endl;
        }
        else
        {
            std::cout << "CAS is correct." << std::endl;
        }
    }
}

//
// Verify compiler intrinsic version of CAS.
//
#ifdef _MSC_VER
void Test_CAS_intrinsic()
{
    std::cout << "Testing CAS_intrinsic...";

    node<MyStruct> oldVal;
    node<MyStruct> newVal;
    node<MyStruct> * pNode = &newVal;
    if(CAS_intrinsic(&pNode, &oldVal, &newVal))
    {
        std::cout << "CAS is INCORRECT." << std::endl;
    }
    else
    {
        pNode = &oldVal;
        if(!CAS_intrinsic(&pNode, &oldVal, &newVal))
        {
            std::cout << "CAS is INCORRECT." << std::endl;
        }
        else if(pNode != &newVal)
        {
            std::cout << "CAS is INCORRECT." << std::endl;
        }
        else
        {
            std::cout << "CAS is correct." << std::endl;
        }
    }
}
#endif

//
// Verify Windows API version of CAS.
//
void Test_CAS_windows()
{
    std::cout << "Testing CAS_windows...";

    node<MyStruct> oldVal;
    node<MyStruct> newVal;
    node<MyStruct> * pNode = &newVal;
    if(CAS_windows(&pNode, &oldVal, &newVal))
    {
        std::cout << "CAS is INCORRECT." << std::endl;
    }
    else
    {
        pNode = &oldVal;
        if(!CAS_windows(&pNode, &oldVal, &newVal))
        {
            std::cout << "CAS is INCORRECT." << std::endl;
        }
        else if(pNode != &newVal)
        {
            std::cout << "CAS is INCORRECT." << std::endl;
        }
        else
        {
            std::cout << "CAS is correct." << std::endl;
        }
    }
}

template<typename T>
struct CAS2Test
{
    node<T> * pNode;
    uint32_t  tag;
    CAS2Test(node<T> * pnewNode, uint32_t newTag) : pNode(pnewNode), tag(newTag) {}
};

//
// Verify Assembly version of CAS.
//
void Test_CAS2_assembly()
{
    std::cout << "Testing CAS2_assembly...";

    node<MyStruct> oldVal;
    node<MyStruct> newVal;

    CAS2Test<MyStruct> myStruct(&newVal, 0xABCD);

    if(CAS2_assembly(&myStruct.pNode, &oldVal, 0xABCD, &newVal, 0xAAAA))
    {
        // should not succeed if pointers don't match
        std::cout << "CAS2 is INCORRECT." << std::endl;
    }
    else if(CAS2_assembly(&myStruct.pNode, &newVal, 0xAAAA, &oldVal, 0xABCD))
    {
        // should not succeed if tags don't match
        std::cout << "CAS2 is INCORRECT." << std::endl;
    }
    else
    {
        myStruct.pNode = &oldVal;
        if(!CAS2_assembly(&myStruct.pNode, &oldVal, 0xABCD, &newVal, 0xAAAA))
        {
            std::cout << "CAS2 is INCORRECT." << std::endl;
        }
        else if(myStruct.pNode != &newVal)
        {
            std::cout << "CAS2 is INCORRECT." << std::endl;
        }
        else if(myStruct.tag != 0xAAAA)
        {
            std::cout << "CAS2 is INCORRECT." << std::endl;
        }
        else
        {
            std::cout << "CAS2 is correct." << std::endl;
        }
    }
}

//
// Verify compiler intrinsic version of CAS.
//
#ifdef _MSC_VER
void Test_CAS2_intrinsic()
{
    std::cout << "Testing CAS2_intrinsic...";

    node<MyStruct> oldVal;
    node<MyStruct> newVal;

    CAS2Test<MyStruct> myStruct(&newVal, 0xABCD);

    if(CAS2_intrinsic(&myStruct.pNode, &oldVal, 0xABCD, &newVal, 0xAAAA))
    {
        // should not succeed if pointers don't match
        std::cout << "CAS2 is INCORRECT." << std::endl;
    }
    else if(CAS2_intrinsic(&myStruct.pNode, &newVal, 0xAAAA, &oldVal, 0xABCD))
    {
        // should not succeed if tags don't match
        std::cout << "CAS2 is INCORRECT." << std::endl;
    }
    else
    {
        myStruct.pNode = &oldVal;
        if(!CAS2_intrinsic(&myStruct.pNode, &oldVal, 0xABCD, &newVal, 0xAAAA))
        {
            std::cout << "CAS2 is INCORRECT." << std::endl;
        }
        else if(myStruct.pNode != &newVal)
        {
            std::cout << "CAS2 is INCORRECT." << std::endl;
        }
        else if(myStruct.tag != 0xAAAA)
        {
            std::cout << "CAS2 is INCORRECT." << std::endl;
        }
        else
        {
            std::cout << "CAS2 is correct." << std::endl;
        }
    }
}
#endif

//
// Verify Windows API version of CAS2.
//
#if WINVER >= 0x0600
void Test_CAS2_windows()
{
    std::cout << "Testing CAS2_windows...";

    node<MyStruct> oldVal;
    node<MyStruct> newVal;

    CAS2Test<MyStruct> myStruct(&newVal, 0xABCD);

    if(CAS2_windows(&myStruct.pNode, &oldVal, 0xABCD, &newVal, 0xAAAA))
    {
        // should not succeed if pointers don't match
        std::cout << "CAS2 is INCORRECT." << std::endl;
    }
    else if(CAS2_windows(&myStruct.pNode, &newVal, 0xAAAA, &oldVal, 0xABCD))
    {
        // should not succeed if tags don't match
        std::cout << "CAS2 is INCORRECT." << std::endl;
    }
    else
    {
        myStruct.pNode = &oldVal;
        if(!CAS2_windows(&myStruct.pNode, &oldVal, 0xABCD, &newVal, 0xAAAA))
        {
            std::cout << "CAS2 is INCORRECT." << std::endl;
        }
        else if(myStruct.pNode != &newVal)
        {
            std::cout << "CAS2 is INCORRECT." << std::endl;
        }
        else if(myStruct.tag != 0xAAAA)
        {
            std::cout << "CAS2 is INCORRECT." << std::endl;
        }
        else
        {
            std::cout << "CAS2 is correct." << std::endl;
        }
    }
}
#endif  // WINVER >= 0x0600

void HandleWait(HANDLE & hThread)
{
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
}

template<typename T>
void CreateNode(node<T> * & pNode)
{
    pNode = new node<T>;
}

template<typename T>
void DeleteNode(node<T> * & pNode)
{
    delete pNode;
}

//
// Stress the multithreaded stack code.
//
template<typename T, int NUMTHREADS>
class StressStack
{
    LockFreeStack<T> _stack;

    static const unsigned int cNodes = 100;    // nodes per thread

    struct ThreadData
    {
        StressStack<T, NUMTHREADS> * pStress;
        DWORD thread_num;
    };

    std::vector<ThreadData> _aThreadData;
    std::vector<node<T> *> _apNodes;

public:
    StressStack() : _aThreadData(NUMTHREADS), _apNodes(cNodes * NUMTHREADS) {}

    //
    // The stack stress will spawn a number of threads (4096 in our tests), each of which will
    // push and pop nodes onto a single stack.  We expect that no access violations will occur
    // and that the stack is empty upon completion.
    //
    void operator()()
    {
        std::cout << "Running Stack Stress..." << std::endl;

        //
        // Create all of the nodes.
        //
        std::for_each(_apNodes.begin(), _apNodes.end(), CreateNode<T>);

        unsigned int ii;
        for(ii = 0; ii < _aThreadData.size(); ++ii)
        {
            _aThreadData[ii].pStress = this;
            _aThreadData[ii].thread_num = ii;
        }

        std::vector<HANDLE> aHandles(NUMTHREADS);
        for(ii = 0; ii < aHandles.size(); ++ii)
        {
            unsigned int tid;
            aHandles[ii] = (HANDLE)_beginthreadex(NULL, 0, StackThreadFunc, &_aThreadData[ii], 0, &tid);
        }

        //
        // Wait for the threads to exit.
        //
        std::for_each(aHandles.begin(), aHandles.end(), HandleWait);

        //
        // Delete all of the nodes.
        //
        std::for_each(_apNodes.begin(), _apNodes.end(), DeleteNode<T>);

        //
        // Ideas for improvement:
        //  We could verify that there is a 1-1 mapping between values pushed and values popped.
        //  Verify the count of pops in the stack matches the number of pops for each thread.
        //
    } // void operator()()

    static unsigned int __stdcall StackThreadFunc(void * pv)
    {
        unsigned int tid = GetCurrentThreadId();
        ThreadData * ptd = reinterpret_cast<ThreadData *>(pv);
        if(FULL_TRACE)
        {
            std::cout << tid << " adding" << std::endl;
        }

        unsigned int ii;
        for(ii = 0; ii < cNodes; ++ii)
        {
            ptd->pStress->_stack.Push(ptd->pStress->_apNodes[ptd->thread_num * cNodes + ii]);
        }

        if(FULL_TRACE)
        {
            std::cout << tid << " removing" << std::endl;
        }

        for(ii = 0; ii < cNodes; ++ii)
        {
            ptd->pStress->_apNodes[ptd->thread_num * cNodes + ii] = ptd->pStress->_stack.Pop();
        }

        return 0;
    }
};  // class StressStack

//
// Stress the multithreaded queue code.
//
template<typename T, int NUMTHREADS>
class StressQueue
{
    LockFreeQueue<T> _queue;

    struct ThreadData
    {
        StressQueue<T, NUMTHREADS> * pStress;
        DWORD thread_num;
    };

    std::vector<ThreadData> _aThreadData;
    std::vector<node<T> *> & _apNodes;

public:
    static const unsigned int cNodes = 100;     // nodes per thread

    StressQueue(std::vector<node<T> *> & apNodes) : _queue(apNodes[0]), _aThreadData(NUMTHREADS), _apNodes(apNodes) {}

    //
    // The queue stress will spawn a number of threads (4096 in our tests), each of which will
    // add and remove nodes on a single queue.  We expect that no access violations will occur
    // and that the queue is empty (except for the dummy node) upon completion.
    //
    void operator()()
    {
        std::cout << "Running Queue Stress..." << std::endl;

        unsigned int ii;
        for(ii = 0; ii < _aThreadData.size(); ++ii)
        {
            _aThreadData[ii].pStress = this;
            _aThreadData[ii].thread_num = ii;
        }

        std::vector<HANDLE> aHandles(NUMTHREADS);
        for(ii = 0; ii < aHandles.size(); ++ii)
        {
            unsigned int tid;
            aHandles[ii] = (HANDLE)_beginthreadex(NULL, 0, QueueThreadFunc, &_aThreadData[ii], 0, &tid);
        }

        //
        // Wait for the threads to exit.
        //
        std::for_each(aHandles.begin(), aHandles.end(), HandleWait);

        //
        // Ideas for improvement:
        //  We could verify that there is a 1-1 mapping between values added and values removed.
        //  Verify the count of pops in the queue matches the number of pops for each thread.
        //
    } // void operator()()

    static unsigned int __stdcall QueueThreadFunc(void * pv)
    {
        unsigned int tid = GetCurrentThreadId();
        ThreadData * ptd = reinterpret_cast<ThreadData *>(pv);
        if(FULL_TRACE)
        {
            std::cout << tid << " adding" << std::endl;
        }

        unsigned int ii;
        for(ii = 0; ii < cNodes; ++ii)
        {
            ptd->pStress->_queue.Add(ptd->pStress->_apNodes[ptd->thread_num * cNodes + ii + 1]);
        }

        if(FULL_TRACE)
        {
            std::cout << tid << " removing" << std::endl;
        }

        for(ii = 0; ii < cNodes; ++ii)
        {
            ptd->pStress->_queue.Remove();
        }

        return 0;
    }
};  // class StressQueue

//
// Demonstrate the lock-free freelist.
// The freelist is based off of ideas found in the freelist article in Game
// Programming Gems 4 by Paul Glinker.  Other ideas for improvement can be
// found in the freelist article in Game Programming Gems 5 by Nathan Mefford.
//
void Demo_Freelist()
{
    std::cout << "Demo of Freelist...";

    //
    // Create a Freelist of MyStructs with 10 elements.
    //
    LockFreeFreeList<MyStruct> fl(10);

    //
    // Allocate a new MyStruct object.
    //
    MyStruct * pStruct = fl.NewInstance();

    //
    // Destroy the MyStruct object and return it to the Freelist.
    //
    fl.FreeInstance(pStruct);

    std::cout << "done" << std::endl;
}

//
// Nothing of importance happens here. :)
//
int main(int, char **)
{
    //
    // Test CAS and CAS2
    //
    Test_CAS_assembly();
#ifdef _MSC_VER
    Test_CAS_intrinsic();
#endif
    Test_CAS_windows();

    Test_CAS2_assembly();
#ifdef _MSC_VER
    Test_CAS2_intrinsic();
#endif
#if WINVER >= 0x0600
    Test_CAS2_windows();
#endif

    //
    // Test Lock-free Stack
    //
    StressStack<TEST_TYPE, 4096>()();

    // Demo the stack
    node<MyStruct> Nodes[10];

    LockFreeStack<MyStruct> stack;
    stack.Push(&Nodes[1]);
    stack.Pop();        // returns &Nodes[1]
    stack.Pop();        // returns NULL

    //
    // Test Lock-free Queue
    //
    std::vector<node<TEST_TYPE> *> apNodes(StressQueue<TEST_TYPE, 4096>::cNodes * 4096 + 1);    // 4096 threads, and 1 extra dummy node
    std::for_each(apNodes.begin(), apNodes.end(), CreateNode<TEST_TYPE>);

    StressQueue<TEST_TYPE, 4096> theQueue(apNodes);
    theQueue();

    std::for_each(apNodes.begin(), apNodes.end(), DeleteNode<TEST_TYPE>);

    // Demo the queue
    LockFreeQueue<MyStruct> queue(&Nodes[0]);   // Nodes[0] is dummy node

    queue.Add(&Nodes[1]);
    queue.Remove();     // returns &Nodes[1]
    queue.Remove();     // returns NULL;

    //
    // Demonstrate Lock-free Freelist
    //
    Demo_Freelist();

    return 0;
}
