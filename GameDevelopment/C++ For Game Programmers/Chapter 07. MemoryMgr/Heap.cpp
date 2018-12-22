

#include "heap.h"
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>


#define MEMSYSTEM_SIGNATURE  0xDEADC0DE

int Heap::s_nNextAllocNum = 1;


struct AllocHeader {
    int           nSignature;
    int           nAllocNum;
    int           nSize;
    Heap *        pHeap;
    AllocHeader * pNext;
    AllocHeader * pPrev;
};



void Heap::Initialize()
{
    m_bActive        = false;
    m_bytesAllocated = 0;
    m_bytesPeak      = 0;
    m_nInstances     = 0;
    m_pHeadAlloc     = NULL;

    m_pParent        = NULL;
    m_pFirstChild    = NULL;
    m_pNextSibling   = NULL;
    m_pPrevSibling   = NULL;
}


void Heap::Activate (const char * name)
{
    assert (name != NULL);
    assert (strlen(name) < NAMELENGTH);
    strcpy (m_name, name);
    m_bytesAllocated = 0;
    m_bytesPeak      = 0;
    m_nInstances     = 0;
    m_bActive        = true;
}

void Heap::Deactivate ()
{
    strcpy (m_name, "");
    // Destroy chain
    m_bytesAllocated = 0;
    m_bytesPeak      = 0;
    m_nInstances     = 0;
    m_bActive = false;
}


const char * Heap::GetName() const
{
    return m_name;
}


void Heap::PrintTreeInfo (int indentLevel) const
{
    PrintInfo(indentLevel);
    Heap * pChild = m_pFirstChild;
    while (pChild != NULL)
    {
        pChild->PrintTreeInfo(indentLevel + 1);
        pChild = pChild->m_pNextSibling;
    }
}


void Heap::PrintInfo (int indentLevel) const
{
    for (int i=0; i<indentLevel; ++i)
        printf ("  ");

    size_t totalBytes = 0;
    size_t totalPeakBytes = 0;
    int totalInstances = 0;
    GetTreeStats(totalBytes, totalPeakBytes, totalInstances);

    int spacing = 20 - indentLevel*2;
    printf ("%-*s %6d %6d %5d  %6d %6d %5d\n", 
            spacing, m_name, 
            m_bytesAllocated, m_bytesPeak, m_nInstances,
            totalBytes, totalPeakBytes, totalInstances);
}


void Heap::GetTreeStats (size_t & totalBytes, size_t & totalPeak, 
                         int & totalInstances) const
{
    totalBytes     += m_bytesAllocated;
    totalPeak      += m_bytesPeak;
    totalInstances += m_nInstances;

    Heap * pChild = m_pFirstChild;
    while (pChild != NULL)
    {
        pChild->GetTreeStats(totalBytes, totalPeak, totalInstances);
        pChild = pChild->m_pNextSibling;
    }
}




void * Heap::Allocate (size_t bytes)
{
    size_t nRequestedBytes = bytes + sizeof(AllocHeader);
    char * pMem = (char *)malloc (nRequestedBytes);
    AllocHeader * pHeader = (AllocHeader *)pMem;
    pHeader->nSignature = MEMSYSTEM_SIGNATURE;
    pHeader->pHeap = this;
    pHeader->nSize = bytes;
    pHeader->pNext = m_pHeadAlloc;
    pHeader->pPrev = NULL;
    pHeader->nAllocNum = s_nNextAllocNum++;

    if (m_pHeadAlloc != NULL)
        m_pHeadAlloc->pPrev = pHeader;
    m_pHeadAlloc = pHeader;

    m_bytesAllocated += bytes;
    if (m_bytesAllocated > m_bytesPeak)
        m_bytesPeak = m_bytesAllocated;
    m_nInstances++;

    void * pStartMemBlock = pMem + sizeof(AllocHeader);
    return pStartMemBlock; 
}


void Heap::Deallocate (void * pMem)
{
    AllocHeader * pHeader = (AllocHeader *)((char *)pMem -
                             sizeof(AllocHeader));
    assert (pHeader->nSignature == MEMSYSTEM_SIGNATURE);
    pHeader->pHeap->Deallocate(pHeader);
}


void Heap::Deallocate (AllocHeader * pHeader)
{
    if (pHeader->pPrev == NULL)
    {
        assert(pHeader == m_pHeadAlloc);
        m_pHeadAlloc = pHeader->pNext;
    }        
    else
        pHeader->pPrev->pNext = pHeader->pNext;

    if (pHeader->pNext != NULL)
        pHeader->pNext->pPrev = pHeader->pPrev;   

    m_bytesAllocated -= pHeader->nSize;
    m_nInstances--;
    free (pHeader);
}


int Heap::ReportMemoryLeaks (int nBookmark1, int nBookmark2)
{
    int nLeaks = 0;

    AllocHeader * pHeader = m_pHeadAlloc;
    while (pHeader != NULL) 
    {
        if (pHeader->nAllocNum >= nBookmark1 &&
            pHeader->nAllocNum < nBookmark2 )
        {
            printf ("Leak in %s. Size: %d, address: 0x%0Xd\n", 
                     m_name, pHeader->nSize, (char *)pHeader + sizeof(AllocHeader));
            nLeaks++;
        }
        pHeader = pHeader->pNext;
    }
    return nLeaks;
}



int Heap::GetMemoryBookmark () 
{
    return s_nNextAllocNum;
}


void Heap::AttachTo (Heap * pParent)
{
    assert (pParent != NULL);

    if (pParent == m_pParent)
        return;

    // First detach itself from its current parent
    if (m_pPrevSibling != NULL)
        m_pPrevSibling->m_pNextSibling = m_pNextSibling;

    if (m_pNextSibling != NULL)
        m_pNextSibling->m_pPrevSibling = m_pPrevSibling;

    if (m_pParent != NULL)
        if (m_pParent->m_pFirstChild == this)
            m_pParent->m_pFirstChild = m_pNextSibling;

    // Now attach itself to the new parent
    m_pNextSibling = pParent->m_pFirstChild;
    m_pPrevSibling = NULL;
    m_pParent      = pParent;
    pParent->m_pFirstChild = this;
}



/*
void Heap::AddChild (Heap * pChild)
{
    assert(pChild != NULL);

    if (m_pFirstChild != NULL)
        m_pFirstChild->m_pPrevSibling = pChild;
    pChild->m_pNextSibling = m_pFirstChild;
    pChild->m_pPrevSibling = NULL;
    m_pFirstChild = pChild;
}


void Heap::RemoveChild (Heap * pChild)
{
    assert(pChild != NULL);

    if (m_pFirstChild == pChild)
        m_pFirstChild = pChild->m_pNextSibling;

    if (pChild->m_pPrevSibling != NULL)
        pChild->m_pPrevSibling = pChild->m_pNextSibling;

    if (pChild->m_pNextSibling != NULL)
        pChild->m_pNextSibling = pChild->m_pPrevSibling;
}

*/
