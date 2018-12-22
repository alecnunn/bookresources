/*
   TODO: Priority queue has different prorities.
*/


#ifndef _BBE_QUEUE_H_
#define _BBE_QUEUE_H_


template <class A>
class CQueue
{
   public:
      CQueue(int size) : m_size(0), m_numItems(0), m_elements(0), m_front(0), m_rear(-1)
      {
         if(size)
            {
               m_size = size;
               m_elements = new A[m_size];
            }
      }
      
      virtual ~CQueue()
      {
         if(m_elements) delete[] m_elements;
         m_elements = NULL;
      }
      
      int Push(A val)
      {
         if(!m_elements) return 0;

         if(m_rear == m_size - 1) { m_rear = -1; m_front++; }
         if(m_front == m_size) m_front = 0;
         
         m_elements[++m_rear] = val;
         if(!isFull()) m_numItems++;

         return 1;
      }

      void Pop()
      {
         if(!isEmpty())
            {
               m_front++;
               if(m_front == m_size) m_front = 0;
               m_numItems--;
            }
      }
      
      int Pop(A *ptr)
      {
         if(PeekFront(ptr))
            {
               Pop();
               return 1;
            }

         return 0;
      }
      
      int PeekFront(A *ptr)
      {
         if(isEmpty() || !m_elements) return 0;

         if(ptr) *ptr = m_elements[m_front];
         return 1;
      }

      A *GetQueue() { return m_elements; }
      int GetSize() { return m_numItems; }
      int GetMaxSize() { return m_size; }
      int isEmpty() { return (m_numItems == 0); }
      int isFull() { return (m_numItems == m_size); }

   private:
      A *m_elements;
      int m_numItems;
      int m_size;
      int m_front, m_rear;
};

template <class A>
class CPriorityQueue
{
   public:
      CPriorityQueue(int size) : m_size(0), m_numItems(0), m_elements(0)
      {
         if(size)
            {
               m_size = size;
               m_elements = new A[m_size];
            }
      }

      virtual ~CPriorityQueue()
      {
         if(m_elements) delete[] m_elements;
         m_elements = NULL;
      }

      int Push(A val)
      {
         if(!m_elements) return 0;

         if(m_numItems == 0) m_elements[m_numItems++] = val;
         else
            {
               for(int i = m_numItems - 1; i >= 0; i--)
                  {
                     if(val > m_elements[i])
                        {
                           if(i+1 != m_size) m_elements[i+1] = m_elements[i];
                        }
                     else
                        break;
                  }

               if(i+1 != m_size) m_elements[i+1] = val;
               else
                  {
                     if(val > m_elements[i])
                        m_elements[i] = val;
                  }

               if(!isFull()) m_numItems++;
            }

         return 1;
      }

      void Pop()
      {
         if(!isEmpty()) m_numItems--;
      }

      int Pop(A *ptr)
      {
         if(PeekMin(ptr))
            {
               Pop();
               return 1;
            }

         return 0;
      }

      int PeekMin(A *ptr)
      {
         if(isEmpty() || !m_elements) return 0;

         if(ptr && (m_numItems - 1 >= 0)) *ptr = m_elements[m_numItems - 1];
         else return 0;

         return 1;
      }

      A *GetQueue() { return m_elements; }
      int GetSize() { return m_numItems; }
      int GetMaxSize() { return m_size; }
      int isEmpty() { return (m_numItems == 0); }
      int isFull() { return (m_numItems == m_size); }

   private:
      A *m_elements;
      int m_numItems;
      int m_size;
};

#endif