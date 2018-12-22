

#ifndef HEAPFACTORY_H_
#define HEAPFACTORY_H_

#include "Heap.h"



class HeapFactory
{
public:
    static Heap * CreateHeap (const char * name);
    static Heap * CreateHeap (const char * name, const char * parent);
	static void DestroyHeap (Heap * pHeap);

	static Heap * GetDefaultHeap ();
    static void   PrintInfo ();

    static int  GetMemoryBookmark ();
    static void ReportMemoryLeaks (int nBookmark);
    static void ReportMemoryLeaks (int nBookmark1, int nBookmark2);

    
private:
	static Heap * GetRootHeap ();
    static Heap * FindHeap (const char * name);
    static Heap * CreateNewHeap (const char * name);
    static void PrintHeapTree(Heap * pHeap);

    static void Initialize();

    enum { MAXHEAPS = 512 };

    static Heap * s_pRootHeap;   
    static Heap * s_pDefaultHeap;   
    static Heap s_heaps[MAXHEAPS];
};


#endif