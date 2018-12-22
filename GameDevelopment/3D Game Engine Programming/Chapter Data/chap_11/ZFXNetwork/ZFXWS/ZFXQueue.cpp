// File: ZFXQueue.cpp


#include <stddef.h>        // #def NULL
#include <memory.h>        // memcpy()
#include "ZFXQueue.h"


/*-----------------------------------------------------------*/
/* ZFXQueueElem class implementation                         *
/*-----------------------------------------------------------*/

// allocate memory and copy data in
ZFXQueueElem::ZFXQueueElem(const char *pData, unsigned int nSize) {
   m_pData = NULL;
   m_pNext = NULL;
   m_pData = new char[nSize];
   m_nSize = nSize;
   memcpy(m_pData, pData, nSize);
   } // constructor
/*----------------------------------------------------------------*/

// free allocated memory
ZFXQueueElem::~ZFXQueueElem(void) {
   if (m_pData) {
      delete [] m_pData;
      m_pData = NULL;
      }
   m_pNext = NULL;
   } // destructor
/*----------------------------------------------------------------*/


/*-----------------------------------------------------------*/
/* ZFXQueue class implementation                             *
/*-----------------------------------------------------------*/

// init pointers
ZFXQueue::ZFXQueue(void) {
   m_pTail = NULL;
   m_pHead = NULL;
   m_Count = 0;
   }// constructor
/*----------------------------------------------------------------*/

// free all queued elements and reset members
ZFXQueue::~ZFXQueue(void) {
   while (m_pHead) Dequeue();
   m_pTail = NULL;
   m_pHead = NULL;
   m_Count = 0;
   }// destructor
/*----------------------------------------------------------------*/

// delete the head element
void ZFXQueue::Dequeue(void) {
   ZFXQueueElem *pTemp;

   // already empty
   if (m_Count == 0) return;

   // exactly one element
   else if (m_Count == 1) {
      delete m_pHead;
      m_pHead = NULL;
      m_pTail = NULL;
      }
   else {
      pTemp = m_pHead;
      m_pHead = m_pHead->m_pNext;
      delete pTemp;
      }
   m_Count--;
   } // Dequeue
/*----------------------------------------------------------------*/

// put a new element in at the end of the tail.
void ZFXQueue::Enqueue(const void *pData, unsigned int nSize) {
   ZFXQueueElem *pNew = new ZFXQueueElem((const char*)pData, nSize);

   // this is the first package
   if (m_Count == 0) {
      m_pHead = pNew;
      m_pTail = pNew;
      }
   // this is the second package
   else if(m_Count == 1) {
      m_pHead->m_pNext = pNew;
      m_pTail = pNew;
      }
   else {
      m_pTail->m_pNext = pNew;
      m_pTail = pNew;
      }
   m_Count++;
   } // Enqueue
/*----------------------------------------------------------------*/

// get data from head element and delete it from queue if wished
void ZFXQueue::Front(void *pData, bool bDequeue) {
   if (pData) {
      if (m_pHead) {
         memcpy(pData, m_pHead->m_pData, m_pHead->m_nSize);
         }
      }
   if (bDequeue) Dequeue();
   } // Front
/*----------------------------------------------------------------*/