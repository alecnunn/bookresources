/* Copyright (C) Scott Bilas, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */
#include <cassert>

template <typename TAG>
class Handle
{
    union
    {
        enum
        {
            // sizes to use for bit fields
            MAX_BITS_INDEX = 16,
            MAX_BITS_MAGIC = 16,

            // sizes to compare against for asserting dereferences
            MAX_INDEX = ( 1 << MAX_BITS_INDEX) - 1,
            MAX_MAGIC = ( 1 << MAX_BITS_MAGIC) - 1,
        };

        struct
        {
            unsigned m_Index : MAX_BITS_INDEX;  // index into resource array
            unsigned m_Magic : MAX_BITS_MAGIC;  // magic number to check
        };
        unsigned int m_Handle;
    };

public:

// Lifetime.

    Handle( void ) : m_Handle( 0 )  {  }

    void Init( unsigned int index );

// Query.

    unsigned int GetIndex ( void ) const  {  return (  m_Index  );  }
    unsigned int GetMagic ( void ) const  {  return (  m_Magic  );  }
    unsigned int GetHandle( void ) const  {  return (  m_Handle );  }
    bool         IsNull   ( void ) const  {  return ( !m_Handle );  }

    operator unsigned int ( void ) const  {  return (  m_Handle );  }
};

template <typename TAG>
void Handle <TAG> :: Init( unsigned int index )
{
    assert( IsNull() );             // don't allow reassignment
    assert( index <= MAX_INDEX );   // verify range

    static unsigned int s_AutoMagic = 0;
    if ( ++s_AutoMagic > MAX_MAGIC )
    {
        s_AutoMagic = 1;    // 0 is used for "null handle"
    }

    m_Index = index;
    m_Magic = s_AutoMagic;
}

template <typename TAG>
inline bool operator != ( Handle <TAG> l, Handle <TAG> r )
    {  return ( l.GetHandle() != r.GetHandle() );  }

template <typename TAG>
inline bool operator == ( Handle <TAG> l, Handle <TAG> r )
    {  return ( l.GetHandle() == r.GetHandle() );  }

