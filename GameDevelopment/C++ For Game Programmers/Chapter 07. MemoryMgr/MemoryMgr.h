
#ifndef MEMORYMGR_H_
#define MEMORYMGR_H_


#include "HeapFactory.h"
#include <cstddef>


void * operator new (size_t size);
void * operator new (size_t size, Heap * pHeap);
void operator delete (void * pMem, size_t size); 



#define DECLARE_HEAP \
    public: \
        static void * operator new(size_t size); \
        static void operator delete(void * p, size_t size); \
    private: \
        static Heap * s_pHeap; 


#define DEFINE_HEAP(className,heapName) \
    Heap * className::s_pHeap = NULL; \
    void * className::operator new(size_t size) { \
        if (s_pHeap==NULL)  \
            s_pHeap = HeapFactory::CreateHeap(heapName); \
        return ::operator new(size, s_pHeap); \
    } \
    void className::operator delete(void * p, size_t size) { \
        ::operator delete(p); \
    }


#define DEFINE_HIERARCHICALHEAP(className,heapName,parentName) \
    Heap * className::s_pHeap = NULL; \
    void * className::operator new(size_t size) { \
        if (s_pHeap==NULL)  \
            s_pHeap = HeapFactory::CreateHeap(heapName,parentName); \
        return ::operator new(size, s_pHeap); \
    } \
    void className::operator delete(void * p, size_t size) { \
        ::operator delete(p); \
    }





#endif