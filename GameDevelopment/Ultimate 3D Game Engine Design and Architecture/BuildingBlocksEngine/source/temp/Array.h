/*

*/


#ifndef _BBE_ARRAYS_H_
#define _BBE_ARRAYS_H_

#include<BBAssert.h>
#include<string.h>


template <class A>
class CArray
{
   public:
      CArray()
      {
         m_numElements = 0;
         m_expandSize = 1;

         m_elements = new A[1];
         memset(m_elements, 0, sizeof(A));

         m_size = 1;
      }

      CArray(int max) : m_numElements(0), m_expandSize(1)
      {
         m_elements = (max > 0) ? new A[max] : 0;
         memset(m_elements, 0, sizeof(A) * max);

         m_size = max;
      }

      virtual ~CArray()
      {
         if(m_elements != NULL)
            delete[] m_elements;

         m_elements = NULL;
         m_numElements = 0;
         m_size = 0;
         m_expandSize = 0;
      }

      void ResetList()
      {
         m_numElements = 0;
      }

      void Clear()
      {
         if(m_elements && m_size)
            memset(m_elements, 0, sizeof(A) * m_size);

         m_numElements = 0;
      }

      void SetExpandSize(int size) { m_expandSize = size; }

      int Expand()
      {
         if(m_numElements >= m_size)
            {
               if(m_expandSize)
                  {
                     A *temp = new A[m_size + m_expandSize];

                     if(!temp)
                        return 0;

                     memcpy(temp, m_elements, sizeof(A) * m_size);

                     delete[] m_elements;
                     m_elements = temp;

                     m_size += m_expandSize;

                     return 1;
                  }
            }

         return 0;
      }

      virtual int Insert(A val)
      {
         if(!m_elements || !m_size)
            return 0;

         if(m_numElements >= m_size)
            {
               if(!Expand())
                  return 0;
            }

         m_elements[m_numElements] = val;
         m_numElements++;
         
         return 1;
      }
      
      virtual int Delete(A val)
      {
         for(int i = 0; i < m_numElements; i++)
            {
               if(m_elements[i] == val)
                  break;
            }

         if(i == m_numElements)
            {
               return 0;
            }
         else
            {
               for(int k = i; k < m_numElements; k++)
                  m_elements[k] = m_elements[k+1];
            
               m_numElements--;
            }

         return 1;
      }
      
      int DeleteLast()
      {
         if(m_numElements <= 0)
            return 0;
         
         m_numElements--;

         return 1;
      }

      virtual int Find(A searchKey)
      {
         if(!m_elements)
            return -1;

         for(int i = 0; i < m_numElements; i++)
            {
               if(m_elements[i] == searchKey)
                  return i;
            }

         return -1;
      }

      A& operator[](int index) const
      {
         assert(index >= 0 && index < m_size && m_elements && m_size);
         return m_elements[index];
      }

      A GetElement(int index)
      {
         A garbage;

         if(index < 0 || index >= m_numElements || !m_elements || !m_size)
            {
               memset(&garbage, 0, sizeof(A));
               return garbage;
            }

         return m_elements[index];
      }

      A *GetElementPtr(int index)
      {
         if(index < 0 || index >= m_numElements || !m_elements || !m_size)
            return NULL;

         return &m_elements[index];
      }

      int GetElementCount()
      {
         return m_numElements;
      }

      int GetSize()
      {
         return m_size;
      }

      int GetExpandSize()
      {
         return m_expandSize;
      }

      A *GetElements()
      {
         return m_elements;
      }

   protected:
      A *m_elements;
      int m_numElements;
      int m_size;
      int m_expandSize;
};

template <class A>
class COrderedArray : public CArray<A>
{
   public:
      COrderedArray(int max) : CArray<A>(max)
      {
      }

      ~COrderedArray()
      {
      }

      int Insert(A val)
      {
         if(!m_elements || !m_size)
            return 0;

         if(m_numElements >= m_size)
            {
               if(!Expand())
                  return 0;
            }

         for(int i = 0; i < m_numElements; i++)
            {
               if(m_elements[i] > val)
                  break;
            }

         for(int k = m_numElements; k > i; k--)
            {
               m_elements[k] = m_elements[k - 1];
            }

         m_elements[i] = val;
         m_numElements++;

         return 1;
      }
      
      int Delete(A val)
      {
         int i = Find(val);

         if(i == m_numElements)
            {
               return 0;
            }
         else
            {
               for(int k = i; k < m_numElements; k++)
                  {
                     m_elements[k] = m_elements[k+1];
                  }
            
               m_numElements--;
            }

         return 1;
      }

      int Find(A searchKey)
      {
         if(!m_elements)
            return -1;

         int lowerBound = 0;
         int upperBound = m_numElements - 1;
         int current = 0;

         while(1)
            {
               current = (lowerBound + upperBound) >> 1;
               
               if(m_elements[current] == searchKey)
                  {
                     return current;
                  }
               else if(lowerBound > upperBound)
                  {
                     return -1;
                  }
               else
                  {
                     if(m_elements[current] < searchKey)
                        lowerBound = current + 1;
                     else
                        upperBound = current - 1;
                  }
            }

         return -1;
      }
};

#endif