
#include "MemoryMgr.h"
#include "Heap.h"
#include "HeapFactory.h"
#include <assert.h>



void * operator new (size_t size) 
{
    return operator new (size, HeapFactory::GetDefaultHeap() );
}


void * operator new (size_t size, Heap * pHeap) 
{
    assert(pHeap != NULL);
    return pHeap->Allocate(size);
}


void operator delete (void * pMem) 
{
    if (pMem != NULL)
        Heap::Deallocate (pMem);    
}

