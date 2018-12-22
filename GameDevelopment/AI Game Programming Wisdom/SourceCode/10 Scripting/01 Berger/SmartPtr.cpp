#include <assert.h>

#include "SmartPtr.H"


RefCount::RefCount()
  : m_refs( 0 )
{
}


RefCount::~RefCount()
{
  assert( m_refs == 0 );
}


void
RefCount::AddReference()
{
  ++m_refs;
}


void
RefCount::RemoveReference()
{
  assert( m_refs != 0 );

  --m_refs;

  if ( m_refs == 0 )
    delete this;
}
