/*

*/


#ifndef _BBE_STACK_H_
#define _BBE_STACK_H_


template <class A>
class CStack
{
   public:
      CStack(int size) : m_size(0), m_top(-1), m_elements(0), m_expandSize(0)
      {
         if(size)
            {
               m_size = size;
               m_elements = new A[m_size];
            }
      }
      
      virtual ~CStack()
      {
         if(m_elements) delete[] m_elements;
         m_elements = NULL;
      }
      
      int Expand()
      {
         if(m_expandSize)
            {
               A *temp = new A[m_size + m_expandSize];
               if(!temp) return 0;

               memcpy(temp, m_elements, sizeof(A) * m_size);

               delete[] m_elements;
               m_elements = temp;

               m_size += m_expandSize;
               return 1;
            }

         return 0;
      }
      
      int Push(A val)
      {
         if(!m_elements) return 0;
         if(isFull()) if(!Expand()) return 0;

         m_elements[++m_top] = val;
         return 1;
      }

      void Pop() { if(!isEmpty() && m_elements) m_top--; }
      
      int Pop(A *ptr)
      {
         int hr = Peek(ptr);
         if(hr) { Pop(); return 1; }
         return 0;
      }
      
      int Peek(A *ptr)
      {
         if(isEmpty() || !m_elements) return 0;

         if(ptr) *ptr = m_elements[m_top];
         return 1;
      }

      A *GetStack() { return m_elements; }
      int GetSize() { return m_top; }
      int GetMaxSize() { return m_size; }
      void SetExpandSize(int size) { m_expandSize = size; }
      void GetExpandSize() { return m_expandSize; }
      int isEmpty() { return (m_top == -1); }
      int isFull() { return (m_top == m_size - 1); }

   private:
      A *m_elements;
      int m_top;
      int m_size;
      int m_expandSize;
};

#endif