/* Copyright (C) Scott Bilas, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */
#include <vector>
#include <cassert>

template <typename DATA, typename HANDLE>
class HandleMgr
{
private:
    // private types
    typedef std::vector <DATA>         UserVec;
    typedef std::vector <unsigned int> MagicVec;
    typedef std::vector <unsigned int> FreeVec;

    // private data
    UserVec  m_UserData;     // data we're going to get to
    MagicVec m_MagicNumbers; // corresponding magic numbers
    FreeVec  m_FreeSlots;    // keeps track of free slots in the db

public:

// Lifetime.

    HandleMgr( void )  {  }
   ~HandleMgr( void )  {  }

// Handle methods.

    // acquisition
    DATA* Acquire( HANDLE& handle );
    void  Release( HANDLE  handle );

    // dereferencing
    DATA*       Dereference( HANDLE handle );
    const DATA* Dereference( HANDLE handle ) const;

    // other query
    unsigned int GetUsedHandleCount( void ) const
        {  return ( m_MagicNumbers.size() - m_FreeSlots.size() );  }
    bool HasUsedHandles( void ) const
        {  return ( !!GetUsedHandleCount() );  }
};

template <typename DATA, typename HANDLE>
DATA* HandleMgr <DATA, HANDLE> :: Acquire( HANDLE& handle )
{
    // if free list is empty, add a new one otherwise use first one found

    unsigned int index;
    if ( m_FreeSlots.empty() )
    {
        index = m_MagicNumbers.size();
        handle.Init( index );
        m_UserData.push_back( DATA() );
        m_MagicNumbers.push_back( handle.GetMagic() );
    }
    else
    {
        index = m_FreeSlots.back();
        handle.Init( index );
        m_FreeSlots.pop_back();
        m_MagicNumbers[ index ] = handle.GetMagic();
    }
    return ( m_UserData.begin() + index );
}

template <typename DATA, typename HANDLE>
void HandleMgr <DATA, HANDLE> :: Release( HANDLE handle )
{
    // which one?
    unsigned int index = handle.GetIndex();

    // make sure it's valid
    assert( index < m_UserData.size() );
    assert( m_MagicNumbers[ index ] == handle.GetMagic() );

    // ok remove it - tag as unused and add to free list
    m_MagicNumbers[ index ] = 0;
    m_FreeSlots.push_back( index );
}

template <typename DATA, typename HANDLE>
inline DATA* HandleMgr <DATA, HANDLE>
:: Dereference( HANDLE handle )
{
    if ( handle.IsNull() )  return ( 0 );

    // check handle validity - $ this check can be removed for speed
    // if you can assume all handle references are always valid.
    unsigned int index = handle.GetIndex();
    if (   ( index >= m_UserData.size() )
        || ( m_MagicNumbers[ index ] != handle.GetMagic() ) )
    {
        // no good! invalid handle == client programming error
        assert( 0 );
        return ( 0 );
    }

    return ( m_UserData.begin() + index );
}

template <typename DATA, typename HANDLE>
inline const DATA* HandleMgr <DATA, HANDLE>
:: Dereference( HANDLE handle ) const
{
    // this lazy cast is ok - non-const version does not modify anything
    typedef HandleMgr <DATA, HANDLE> ThisType;
    return ( const_cast <ThisType*> ( this )->Dereference( handle ) );
}
