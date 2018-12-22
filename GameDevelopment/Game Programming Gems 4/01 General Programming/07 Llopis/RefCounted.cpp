
#include "RefCounted.h"


RefCounted::RefCounted () :
	m_refCount (0)
{
}


RefCounted::~RefCounted ()
{
}


int RefCounted::GetRefCount () const
{
	return m_refCount;
}


int RefCounted::AddRef ()
{
	m_refCount++;
	return m_refCount;
}


int RefCounted::Release ()
{
	int tmpRefCount;

	m_refCount--;
	tmpRefCount = m_refCount;

	if ( m_refCount <= 0 )
		delete this;

	return tmpRefCount;
}

