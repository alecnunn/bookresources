//Custom allocator class that manages its own heap for efficiency.
/*NOTE: THIS CODE IS VERY BUGGY AND IS ONLY MEANT FOR THE PURPOSE OF DEMONSTRATING
UNIT TESTING.  DO NOT ACTUALLY USE THIS IN PRODUCTION CODE.*/

#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <cassert>
#include <set>

template<typename T>
class auto_heap
     {
public:
     auto_heap(size_t heap_size)
          {
          m_pheap = (T*)::operator new(heap_size*sizeof(T) );
          }
     ~auto_heap()
          {
          ::operator delete(reinterpret_cast<void*>(m_pheap) );
          m_pheap = NULL;
          }
     T* m_pheap;
     };

template <typename T, size_t heap_size>
class heap_allocator
     {
     ///@todo heap_allocator(const heap_allocator& that) {};
     ///@todo void operator=(const heap_allocator& that) {};
public:
     template <typename T>
     class handle
          {
     public:
          handle(T* that_pointer, size_t size, bool in_managed_heap) :
          m_pointer(that_pointer), m_size(size), m_in_managed_heap(in_managed_heap)
               {}
          bool operator<(const handle that) const
               { return m_pointer < that.m_pointer; }

          T* m_pointer;
          size_t m_size;
          bool m_in_managed_heap;
          };
     // type definitions
     typedef T        value_type;
     typedef T*       pointer;
     typedef const T* const_pointer;
     typedef T&       reference;
     typedef const T& const_reference;
     typedef size_t    size_type;
     typedef ptrdiff_t difference_type;

     // rebind allocator to type U
     //---------------------------------------------------------------------
     template <typename U, size_t heap_size>
     struct rebind
          {
          typedef heap_allocator<U, heap_size> other;
          };

     //---------------------------------------------------------------------
     // return address of values
     inline pointer address (reference value) const
          {
          return &value;
          }

     //---------------------------------------------------------------------
     inline const_pointer address (const_reference value) const
          {
          return &value;
          }

     //---------------------------------------------------------------------
     heap_allocator() throw()
          {
          m_bytes_allocated_marker = 0;
          }

     //---------------------------------------------------------------------
     ~heap_allocator() throw()
          {
          //assert(!m_heap_alloc_table.size() && "not all heap pointers were deallocated");
          }

     //---------------------------------------------------------------------
     // return maximum number of elements that can be allocated
     inline size_type max_size() const throw()
          {
          return heap_size/sizeof(T);
          }

     //---------------------------------------------------------------------
     // initialize elements of allocated storage p with value value
     inline void construct(pointer p, const T& value)
          {
          //initialize memory with placement new
          new((void*)p) T(value);
          }

     //---------------------------------------------------------------------
     // destroy elements of initialized storage p
     inline void destroy(pointer p)
          {
          //destroy objects by calling their destructor
          p->~T();
          }

     //---------------------------------------------------------------------
     //allocate but don't initialize num elements of type T
     pointer allocate(size_type num, const void* phint = NULL)
          {
          assert(m_heap.m_pheap && "internal heap not allocated");
          if (num < 1)
               {
               throw std::bad_alloc();
               }
          //request too large, so allocate from the default free store
          if ( (num > heap_size) )
               {
               return new T[num];
               }

          T* praw = m_heap.m_pheap + m_bytes_allocated_marker;
          m_bytes_allocated_marker += (num * sizeof(T) );

          if(m_bytes_allocated_marker > heap_size)
               {
               //heap may just be fragmented
               assert(m_heap_alloc_table.size() && "internal heap fragmented, but pointer allocation table is empty");

               //set marker to the last area in our heap for the next allocation
               m_bytes_allocated_marker = heap_size;
               //try to find a block of memory elsewhere in the heap.
               //first see if we can allocate from them beginning
               if ((m_heap.m_pheap+num) <= m_heap_alloc_table.begin()->m_pointer)
                    {
                    m_heap_alloc_table.insert(handle<T>(m_heap.m_pheap, num, true) );
                    return m_heap.m_pheap;
                    }
               //otherwise, try to find some memory in between the other fragments
               T* inner_frament_pointer = NULL;

               for (std::set<handle<T> >::iterator pointer_iter = m_heap_alloc_table.begin();
                    /*quit before the last element--we only want blocks between other blocks*/
                    std::distance(pointer_iter, m_heap_alloc_table.end() ) > 1;
                    ++pointer_iter)
                    {
                    //set the new block to start after the current block by adding its
                    //address with it's size in bytes
                    inner_frament_pointer = pointer_iter->m_pointer + pointer_iter->m_size;
                    m_heap_alloc_table.insert(handle<T>(inner_frament_pointer, num, true) );
                    return inner_frament_pointer;
                    }
               //memory is too fragmented--resort to the default free store
               char* free_store_pointer = new T[num];
               m_heap_alloc_table.insert(handle<T>(free_store_pointer, num, false) );
               return free_store_pointer;
               }

          //add allocation pointer and its number of unit to lookup table
          m_heap_alloc_table.insert(handle<T>(praw, num, true) );

          return praw;
          }

     //---------------------------------------------------------------------
     // deallocate storage p of deleted elements
     bool deallocate(pointer p, size_type num)
          {
          if (p)
               {
               std::set<handle<T> >::iterator pointer_iter =
                         m_heap_alloc_table.find(handle<T>(p,num,true));
               if (pointer_iter != m_heap_alloc_table.end() )
                    {
                    //get a temporary iterator to our pointer, increment it, and compare to end()
                    //to see if this was the last element in our set
                    std::set<handle<T> >::iterator end_iter = pointer_iter;
                    if ((++end_iter) == m_heap_alloc_table.end() )
                         {
                         //if this pointer is the last in the table then move the
                         //allocator marker
                         m_bytes_allocated_marker -= pointer_iter->m_size;
                         }
                    m_heap_alloc_table.erase(pointer_iter);
                    }
               else
                    {
                    return false;
                    }
               //free the memory
               if (num < 1)
                    {
                    throw std::out_of_range("");
                    }
               (num == 1) ? delete p : delete [] p;
               p = NULL;
               return true;
               }
          else
               {
               return false;
               }
          }

     //---------------------------------------------------------------------
     inline void clear()
          {
          m_heap_alloc_table.clear();
          memset(m_heap.m_pheap, 0, heap_size);
          }

private:
     static auto_heap<T> m_heap;
     size_t m_bytes_allocated_marker;
     std::set<handle<T> > m_heap_alloc_table;
     };

template <typename T, size_t heap_size>
auto_heap<T> heap_allocator<T, heap_size>::m_heap(heap_size);

#endif //__ALLOCATOR_H__

