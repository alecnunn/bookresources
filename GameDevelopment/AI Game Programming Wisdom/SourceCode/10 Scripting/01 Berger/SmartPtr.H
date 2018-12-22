#ifndef __SmartPtr_H__
#define __SmartPtr_H__


// Define an intrusive reference counting base class.  The SmartPtr class
// expects that its type is derived from this class.  This class handles all
// the details of keeping the reference variable, and it will delete itself
// when the reference count reaches zero.
class RefCount {
public:
  // Define the class's default constructor & destructor.  Note that the
  // destructor must be virtual since this class will actually "delete this".
  // If the destructor isn't virtual, then the derived class's destructor
  // won't be called.
  RefCount();
  virtual ~RefCount();


  // These two api functions increment and decrement the reference count for
  // this class.  When the reference count is decremented to zero, this object
  // will delete itself.  The SmartPtr class will call these two functions
  // automatically.
  void AddReference();
  void RemoveReference();


private:
  // This data member keeps track of the number of references currently active.
  unsigned int m_refs;
};



// Define a smart pointer template.  This smart pointer is reference counded,
// so the allocated memory does not have to manually deleted.  This is very
// handy for Bison since it's common to toss pointers around a lot in complex
// grammars.  Note that this smart pointer requires that the class be derived
// from RefCount.
template <class T>
class SmartPtr {
public:
  // This is the default constructor and the constructor that can be used to
  // create a smart pointer to a specific pointer.  The smart pointer will
  // point to the given pointer, and it will increment the reference count of
  // the object.
  SmartPtr( T *ptr = NULL )
    : m_ptr( NULL )
  {
    SetPtr( ptr );
  }

  // This constructor allows one smart pointer to be created from an other
  // smart pointer type.  This is useful in up/down casting a pointer through
  // the inheritance tree.  Since static_cast is used to do the actual pointer
  // conversion, compile errors will still be thrown if the two smart pointers
  // are not compatiable.
  //
  // Note that this constructor is explicit.  This helps the developer to not
  // accidently cast a smart poitner from one type to another type.
  template <class Y> explicit SmartPtr( const SmartPtr<Y>& s )
    : m_ptr( NULL )
  {
    SetPtr( static_cast<T *>( s.GetPtr() ) );
  }

  // This is the copy constructor.  This smart pointer will initialize itself
  // to the given smart pointer, and it will also properly increment the
  // object's reference counter.
  SmartPtr( const SmartPtr& s )
    : m_ptr( NULL )
  {
    SetPtr( s.GetPtr() );
  }

  // This is the destructor for the smart pointer.  This function sets the
  // internal pointer to NULL.  This will handle decrementing the reference
  // counter and maybe deleting the object that this smart pointer points to.
  ~SmartPtr()
  {
    SetPtr( NULL );
  }

  // The smart pointer needs to override the default assignment operator to
  // make sure that the reference counting is properly handled.
  SmartPtr& operator=( const SmartPtr& rhs )
  {
    SetPtr( rhs.GetPtr() );
    return *this;
  }

  // This is just like the default assignment operator; however, this
  // operator= expects that the given type is the same pointer type.  This is
  // the most common way of changing what a smart pointer points to.
  SmartPtr& operator=( T *rhs )
  {
    SetPtr( rhs );
    return *this;
  }

  // This function returns the true pointer that this smart pointer refers
  // to.  This is mainly useful internally, but it might be useful external
  // source code if you don't want to call the T* typecast operator.
  T* GetPtr() const   { return m_ptr;  }

  // These next three functions define the various operators that are needed
  // by a smart pointer to act & feel like a normal pointer.
  operator T*()       { return m_ptr;  }
  T& operator*()      { return *m_ptr; }
  T* operator->()     { return m_ptr;  }


private:
  // This function handles the details of changing the internal pointer.  It
  // will handle incrementing and decrementing the object's reference as
  // needed.
  void SetPtr( T *ptr );

  // This pointer points to the true object for this smart pointer.
  T *m_ptr;
};




template <class T>
void
SmartPtr<T>::SetPtr( T *ptr )
{
  // Before we can change the internal pointer data member, it is important to
  // decrement the reference count on our existing object.
  if ( m_ptr != NULL )
    m_ptr->RemoveReference();

  // Set our pointer data member to refer to this new object.
  m_ptr = ptr;

  // Now that we have control over this new object, make sure to increment the
  // reference counter for the new object.
  if ( m_ptr != NULL )
    m_ptr->AddReference();
}


#endif // __SmartPtr_H__
