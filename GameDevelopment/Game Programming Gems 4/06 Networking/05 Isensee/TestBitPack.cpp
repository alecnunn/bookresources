///////////////////////////////////////////////////////////////////////////////
//
//  TestBitPack.cpp
//
//  Test bed for bit packing code.
//
//  Copyright © 2003 Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.
//
///////////////////////////////////////////////////////////////////////////////

#include "BitPack.h"
#include "GetRand.h"
#include "Platform.h"
#include <vector>

using namespace BitPack;

const int LOW_LEVEL_THOROUGH_ITERATIONS = 1000000;
const int BITPACK_CODEC_ITERATIONS = 100000;

enum UnsignedType
{
    Type_U8,
    Type_U16,
    Type_U32,
    Type_U64
};

enum PackableType
{
    Type_PkUint8,
    Type_PkUint16,
    Type_PkUint32,
    Type_PkUint64,
    Type_PkUintRange8,
    Type_PkUintRange16,
    Type_PkUintRange32,
    Type_PkUintRange64,
    Type_Int8,
    Type_Int16,
    Type_Int32,
    Type_Int64,
    
    Type_PkUInt16_1,
    Type_PkUInt16_2,
    Type_PkUInt16_3,
    Type_PkUInt16_7,
    Type_PkUInt16_8,
    Type_PkUInt16_9,
    Type_PkUInt16_15,

    Type_PkUintRange16_0_1,
    Type_PkUintRange16_100_1000,
    
    Type_Int16_0_1,
    Type_Int16_100_1000,
    Type_Int16_n1_0,
    Type_Int16_n1000_n100,
    
    PackableType_Max
};

//-----------------------------------------------------------------------------
//
// ErrorMsg
//
// Global platform-specific error message code

void ErrorMsg( const char* pMsg, const char* pFile, int nLine )
{
    char msg[ 2048 ];
    sprintf( msg, "%s\nFile:%s Line:%d", pMsg, pFile, nLine );
    MessageBox( NULL, msg, "Error", MB_OK );
    DebugBreak();
}

//-----------------------------------------------------------------------------
//
// TestGemCode
//
// Test the code from the gem text

void TestGemCode()
{
    SOCKET s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

    struct NetData
    {
        unsigned char  MsgType;   // 0 - 28
        unsigned long  Time;      // 0 - 0xFFFFFFFF
        unsigned short Element;   // 148 to 1153
        int            XPosition; // -100,000 to 100,000
        int            Yposition; // -100,000 to 100,000
    };

    NetData nd;
    send( s, (const char*)( &nd ), sizeof(nd), 0 );
    
    struct NetDataX : public BitPackCodec< 5 >
    {
        PkUint<unsigned char,5>              MsgType;
        PkUint<unsigned long>                Time;
        PkUintRange<unsigned short,148,1153> Element;
        PkInt<int,-100000,100000>            XPos;
        PkInt<int,-100000,100000>            YPos;
        
        NetDataX()
        {
            Register( MsgType );
            Register( Time );
            Register( Element );
            Register( XPos );
            Register( YPos );
        }
    };

    NetDataX ndx;
    int size = ndx.GetPackedBytes();
    char* pBitStream = new char [ size ];
    send( s, ndx.Pack( pBitStream ), size, 0 );

    ndx.Unpack( pBitStream );

    delete [] pBitStream;
    closesocket( s );
}

//-----------------------------------------------------------------------------
//
// TestStructAddin
//
// Test an example struct expanded to use bit packing technique

void TestStructAddin()
{
    struct NetData : public BitPackCodec< 5 >
    {
        PkUint<unsigned char,5>              MsgType;
        PkUint<unsigned long>                Time;
        PkUintRange<unsigned short,148,1153> Element;
        PkInt<int,-100000,100000>            XPos;
        PkInt<int,-100000,100000>            YPos;
        
        NetData()
        {
            Register( MsgType );
            Register( Time );
            Register( Element );
            Register( XPos );
            Register( YPos );
        }
    };
    
    NetData nd;
    nd.MsgType = 15;
    nd.Time    = 0x59B33C10;
    nd.Element = 524;
    nd.XPos    = -1033;
    nd.YPos    = 9891;
    
    NetData ndc( nd );
    char* pBitBuffer = new char [ nd.GetPackedBytes() ];
    
    nd.Pack( pBitBuffer );
    nd.Unpack( pBitBuffer );
    
    test( nd.MsgType == ndc.MsgType );
    test( nd.Time    == ndc.Time );
    test( nd.Element == ndc.Element );
    test( nd.XPos    == ndc.XPos );
    test( nd.YPos    == ndc.YPos );
    
    delete [] pBitBuffer;
}

//-----------------------------------------------------------------------------
//
// TestPackUnpack
//
// Perform basic testing with Pack and Unpack functions

void TestPackUnpack()
{
    char BitBuffer[1024];
    memset( BitBuffer, 0, 1024 );
    
    unsigned char  a( unsigned char(0xAA) );
    unsigned short b( unsigned short(0xBBBB) );
    unsigned long  c( unsigned long(0xCCCCCCCC) );
    uint64         d( uint64(0xDDDDDDDDDDDDDDDD) );
    
    const int aBits = ( rand() % ( sizeof(a) * CHAR_BIT ) ) + 1;
    const int bBits = ( rand() % ( sizeof(b) * CHAR_BIT ) ) + 1;
    const int cBits = ( rand() % ( sizeof(c) * CHAR_BIT ) ) + 1;
    const int dBits = ( rand() % ( sizeof(d) * CHAR_BIT ) ) + 1;
    
    a >>= ( ( sizeof(a) * CHAR_BIT ) - aBits );
    b >>= ( ( sizeof(b) * CHAR_BIT ) - bBits );
    c >>= ( ( sizeof(c) * CHAR_BIT ) - cBits );
    d >>= ( ( sizeof(d) * CHAR_BIT ) - dBits );
    
    Pack( a, aBits, BitBuffer, 0 );
    Pack( b, bBits, BitBuffer, aBits );
    Pack( c, cBits, BitBuffer, aBits + bBits );
    Pack( d, dBits, BitBuffer, aBits + bBits + cBits );
    
    unsigned char  aD( unsigned char(0) );
    unsigned short bD( unsigned short(0) );
    unsigned long  cD( unsigned long(0) );
    uint64         dD( uint64(0) );

    Unpack( &aD, aBits, BitBuffer, 0 );
    Unpack( &bD, bBits, BitBuffer, aBits );
    Unpack( &cD, cBits, BitBuffer, aBits + bBits );
    Unpack( &dD, dBits, BitBuffer, aBits + bBits + cBits );
    
    test( a == aD );
    test( b == bD );
    test( c == cD );
    test( d == dD );
}

//-----------------------------------------------------------------------------
//
// TestPackUnpackThorough
//
// Perform thorough testing of the low-level Pack and Unpack functions

void TestPackUnpackThorough()
{
    typedef std::vector<uint64> ValueList;
    typedef std::vector<int> BitList;
    typedef std::vector<UnsignedType> TypeList;

    ValueList valueList;    // Values stored in the bit buffer
    BitList   bitList;      // Bit length of each value
    TypeList  typeList;     // Type of each value
    valueList.reserve( LOW_LEVEL_THOROUGH_ITERATIONS );
    bitList.reserve( LOW_LEVEL_THOROUGH_ITERATIONS );
    typeList.reserve( LOW_LEVEL_THOROUGH_ITERATIONS );

    // Unsigned values    
    unsigned char a;
    unsigned short b;
    unsigned long c;
    uint64 d;

    // Big buffer
    char* pBitBuffer = new char [ LOW_LEVEL_THOROUGH_ITERATIONS * sizeof(uint64) ];
    memset( pBitBuffer, 0, LOW_LEVEL_THOROUGH_ITERATIONS * sizeof(uint64) );
    
    // Do lots of packing
    int nStartBit = 0;
    int nBits = 0;
    for( int i = 0; i < LOW_LEVEL_THOROUGH_ITERATIONS; ++i )
    {
        // Determine what type to pack
        UnsignedType uType = UnsignedType( rand() % 4 );
        typeList.push_back( uType );
        
        // Determine how many bits to pack
        switch( uType )
        {
        case Type_U8:
            nBits = rand() % ( sizeof( unsigned char ) * CHAR_BIT ) + 1;
            bitList.push_back( nBits );
            a = GetRandValue<unsigned char>( nBits );
            valueList.push_back( uint64(a) );
            Pack( a, nBits, pBitBuffer, nStartBit );
            break;
        case Type_U16:
            nBits = rand() % ( sizeof( unsigned short ) * CHAR_BIT ) + 1;
            bitList.push_back( nBits );
            b = GetRandValue<unsigned short>( nBits );
            valueList.push_back( uint64(b) );
            Pack( b, nBits, pBitBuffer, nStartBit );
            break;
        case Type_U32:
            nBits = rand() % ( sizeof( unsigned long ) * CHAR_BIT ) + 1;
            bitList.push_back( nBits );
            c = GetRandValue<unsigned long>( nBits );
            valueList.push_back( uint64(c) );
            Pack( c, nBits, pBitBuffer, nStartBit );
            break;
        case Type_U64:
            nBits = rand() % ( sizeof( uint64 ) * CHAR_BIT ) + 1;
            bitList.push_back( nBits );
            d = GetRandValue<uint64>( nBits );
            valueList.push_back( uint64(d) );
            Pack( d, nBits, pBitBuffer, nStartBit );
            break;
        default:
            assert( false );
            break;
        }
        nStartBit += nBits;
    }
    
    // Unpack and verify
    nStartBit = 0;
    for( int i = 0; i < LOW_LEVEL_THOROUGH_ITERATIONS; ++i )
    {
        UnsignedType uType = typeList[ i ];
        uint64 expectedValue = valueList[ i ];
        nBits = bitList[ i ];
        switch( uType )
        {
        case Type_U8:
            Unpack( &a, nBits, pBitBuffer, nStartBit );
            test( uint64(a) == expectedValue );
            break;
        case Type_U16:
            Unpack( &b, nBits, pBitBuffer, nStartBit );
            test( uint64(b) == expectedValue );
            break;
        case Type_U32:
            Unpack( &c, nBits, pBitBuffer, nStartBit );
            test( uint64(c) == expectedValue );
            break;
        case Type_U64:
            Unpack( &d, nBits, pBitBuffer, nStartBit );
            test( uint64(d) == expectedValue );
            break;
        default:
            assert( false );
            break;
        }
        nStartBit += nBits;
    }
    
    delete [] pBitBuffer;
}

//-----------------------------------------------------------------------------
//
// GetRandomPackable
//
// Create a Packable item of various types filled with random data. Caller
// is responsible for freeing the returned object.

Packable* GetRandomPackable( PackableType packableType )
{
    Packable* p = NULL;
    int nBits = 0;
    
    switch( packableType )
    {
    case Type_PkUint8:
    {
        PkUint<unsigned char>* pi = new PkUint< unsigned char >;
        nBits = sizeof(unsigned char) * CHAR_BIT; 
        *pi = GetRandValue< unsigned char >( nBits );
        p = pi;
        break;
    }
    case Type_PkUint16:
    {
        PkUint<unsigned short>* pi = new PkUint< unsigned short >;
        nBits = sizeof(unsigned short) * CHAR_BIT; 
        *pi = GetRandValue< unsigned short >( nBits );
        p = pi;
        break;
    }
    case Type_PkUint32:
    {
        PkUint<unsigned long>* pi = new PkUint< unsigned long >;
        nBits = sizeof(unsigned long) * CHAR_BIT; 
        *pi = GetRandValue< unsigned long >( nBits );
        p = pi;
        break;
    }
    case Type_PkUint64:
    {
        PkUint<uint64>* pi = new PkUint< uint64 >;
        nBits = sizeof(uint64) * CHAR_BIT; 
        *pi = GetRandValue< uint64 >( nBits );
        p = pi;
        break;
    }
    case Type_PkUintRange8:
    {
        PkUintRange<unsigned char,0,255>* pi = new PkUintRange< unsigned char, 0 , 255 >;
        nBits = BitsRequired< 255 - 0 >::GetValue;
        *pi = GetRandValue< unsigned char >( nBits ) + 0;
        p = pi;
        break;
    }
    case Type_PkUintRange16:
    {
        PkUintRange<unsigned short,0,65000>* pi = new PkUintRange< unsigned short, 0 , 65000 >;
        nBits = BitsRequired< 65000 - 0 >::GetValue;
        unsigned short v = GetRandValue< unsigned short >( nBits ) + 0;
        *pi = Confine( v, 0, 65000 );
        p = pi;
        break;
    }
    case Type_PkUintRange32:
    {
        PkUintRange<unsigned long,0,12345678>* pi = new PkUintRange< unsigned long, 0 , 12345678 >;
        nBits = BitsRequired< 12345678 - 0 >::GetValue;
        unsigned long v = GetRandValue< unsigned long >( nBits ) + 0;
        *pi = Confine( v, 0, 12345678 );
        p = pi;
        break;
    }
    case Type_PkUintRange64:
    {
        PkUintRange<uint64,0,123456789000>* pi = new PkUintRange< uint64, 0 , 123456789000 >;
        nBits = BitsRequired< 123456789000 - 0 >::GetValue;
        uint64 v = GetRandValue< uint64 >( nBits ) + 0;
        *pi = Confine( v, 0, 123456789000 );
        p = pi;
        break;
    }
    case Type_Int8:
    {
        PkInt<char,-120,100>* pi = new PkInt< char, -120 , 100 >;
        nBits = BitsRequired< 100 - -120 >::GetValue;
        char v = GetRandValue< char >( nBits ) + -120;
        *pi = Confine( v, -120, 100 );
        p = pi;
        break;
    }
    case Type_Int16:
    {
        PkInt<short,-20000,20000>* pi = new PkInt< short, -20000 , 20000 >;
        nBits = BitsRequired< 20000 - -20000 >::GetValue;
        short v = GetRandValue< short >( nBits ) + -20000;
        *pi = Confine( v, -20000, 20000 );
        p = pi;
        break;
    }
    case Type_Int32:
    {
        PkInt<long,-2000000,2000000>* pi = new PkInt< long, -2000000 , 2000000 >;
        nBits = BitsRequired< 2000000 - -2000000 >::GetValue;
        long v = GetRandValue< long >( nBits ) + -2000000;
        *pi = Confine( v, -2000000, 2000000 );
        p = pi;
        break;
    }
    case Type_Int64:
    {
        PkInt<int64,-20000001234,20000004321>* pi = new PkInt< int64, -20000001234 , 20000004321 >;
        nBits = BitsRequired< 20000004321 - -20000001234 >::GetValue;
        int64 v = GetRandValue< int64 >( nBits ) + -20000001234;
        *pi = Confine( v, -20000001234, 20000004321 );
        p = pi;
        break;
    }
    case Type_PkUInt16_1:
    {
        PkUint<unsigned short,1>* pi = new PkUint< unsigned short, 1 >;
        nBits = 1; 
        *pi = GetRandValue< unsigned short >( nBits );
        p = pi;
        break;
    }
    case Type_PkUInt16_2:
    {
        PkUint<unsigned short,2>* pi = new PkUint< unsigned short, 2 >;
        nBits = 2; 
        *pi = GetRandValue< unsigned short >( nBits );
        p = pi;
        break;
    }
    case Type_PkUInt16_3:
    {
        PkUint<unsigned short,3>* pi = new PkUint< unsigned short, 3 >;
        nBits = 3; 
        *pi = GetRandValue< unsigned short >( nBits );
        p = pi;
        break;
    }
    case Type_PkUInt16_7:
    {
        PkUint<unsigned short,7>* pi = new PkUint< unsigned short, 7 >;
        nBits = 7; 
        *pi = GetRandValue< unsigned short >( nBits );
        p = pi;
        break;
    }
    case Type_PkUInt16_8:
    {
        PkUint<unsigned short,8>* pi = new PkUint< unsigned short, 8 >;
        nBits = 8; 
        *pi = GetRandValue< unsigned short >( nBits );
        p = pi;
        break;
    }
    case Type_PkUInt16_9:
    {
        PkUint<unsigned short,9>* pi = new PkUint< unsigned short, 9 >;
        nBits = 9;
        *pi = GetRandValue< unsigned short >( nBits );
        p = pi;
        break;
    }
    case Type_PkUInt16_15:
    {
        PkUint<unsigned short,15>* pi = new PkUint< unsigned short, 15 >;
        nBits = 15;
        *pi = GetRandValue< unsigned short >( nBits );
        p = pi;
        break;
    }
    case Type_PkUintRange16_0_1:
    {
        PkUintRange<unsigned short,0,1>* pi = new PkUintRange< unsigned short, 0 , 1 >;
        nBits = BitsRequired< 1 - 0 >::GetValue;
        *pi = GetRandValue< unsigned short >( nBits ) + 0;
        p = pi;
        break;
    }
    case Type_PkUintRange16_100_1000:
    {
        PkUintRange<unsigned short,100,1000>* pi = new PkUintRange< unsigned short, 100 , 1000 >;
        nBits = BitsRequired< 1000 - 100 >::GetValue;
        unsigned short v = GetRandValue< unsigned short >( nBits ) + 100;
        *pi = Confine( v, 100, 1000 );
        p = pi;
        break;
    }
    case Type_Int16_0_1:
    {
        PkInt<short,0,1>* pi = new PkInt< short, 0 , 1 >;
        nBits = BitsRequired< 1 - 0 >::GetValue;
        *pi = GetRandValue< short >( nBits ) + 0;
        p = pi;
        break;
    }
    case Type_Int16_100_1000:
    {
        PkInt<short,100,1000>* pi = new PkInt< short, 100 , 1000 >;
        nBits = BitsRequired< 1000 - 100 >::GetValue;
        short v = GetRandValue< short >( nBits ) + 100;
        *pi = Confine( v, 100, 1000 );
        p = pi;
        break;
    }
    case Type_Int16_n1_0:
    {
        PkInt<short,-1,0>* pi = new PkInt< short, -1 , 0 >;
        nBits = BitsRequired< 0 - -1 >::GetValue;
        *pi = GetRandValue< short >( nBits ) + -1;
        p = pi;
        break;
    }
    case Type_Int16_n1000_n100:
    {
        PkInt<short,-1000,-100>* pi = new PkInt< short, -1000 , -100 >;
        nBits = BitsRequired< -100 - -1000 >::GetValue;
        short v = GetRandValue< short >( nBits ) + -1000;
        *pi = Confine( v, -1000, -100 );
        p = pi;
        break;
    }
    default:
        assert( false );
        break;
    }
    assert( p != NULL );
    return p;
}

//-----------------------------------------------------------------------------
//
// GetValue
//
// Extract the value of the given packable object given its type. Not for use
// in real code -- this uses nasty casts to convert to the proper concrete type.

int64 GetValue( const Packable* p, PackableType packableType )
{
    int64 v = 0;
    switch( packableType )
    {
    case Type_PkUint8:
    {
        PkUint<unsigned char>* pi = (PkUint< unsigned char >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUint16:
    {
        PkUint<unsigned short>* pi = (PkUint< unsigned short >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUint32:
    {
        PkUint<unsigned long>* pi = (PkUint< unsigned long >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUint64:
    {
        PkUint<uint64>* pi = (PkUint< uint64 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUintRange8:
    {
        PkUintRange<unsigned char,0,255>* pi = (PkUintRange< unsigned char, 0 , 255 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUintRange16:
    {
        PkUintRange<unsigned short,0,65000>* pi = (PkUintRange< unsigned short, 0 , 65000 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUintRange32:
    {
        PkUintRange<unsigned long,0,12345678>* pi = (PkUintRange< unsigned long, 0 , 12345678 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUintRange64:
    {
        PkUintRange<uint64,0,123456789000>* pi = (PkUintRange< uint64, 0 , 123456789000 >*)( p );
        v = *pi;
        break;
    }
    case Type_Int8:
    {
        PkInt<char,-120,100>* pi = (PkInt< char, -120 , 100 >*)( p );
        v = *pi;
        break;
    }
    case Type_Int16:
    {
        PkInt<short,-20000,20000>* pi = (PkInt< short, -20000 , 20000 >*)( p );
        v = *pi;
        break;
    }
    case Type_Int32:
    {
        PkInt<long,-2000000,2000000>* pi = (PkInt< long, -2000000 , 2000000 >*)( p );
        v = *pi;
        break;
    }
    case Type_Int64:
    {
        PkInt<int64,-20000001234,20000004321>* pi = (PkInt< int64, -20000001234 , 20000004321 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUInt16_1:
    {
        PkUint<unsigned short,1>* pi = (PkUint< unsigned short, 1 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUInt16_2:
    {
        PkUint<unsigned short,2>* pi = (PkUint< unsigned short, 2 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUInt16_3:
    {
        PkUint<unsigned short,3>* pi = (PkUint< unsigned short, 3 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUInt16_7:
    {
        PkUint<unsigned short,7>* pi = (PkUint< unsigned short, 7 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUInt16_8:
    {
        PkUint<unsigned short,8>* pi = (PkUint< unsigned short, 8 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUInt16_9:
    {
        PkUint<unsigned short,9>* pi = (PkUint< unsigned short, 9 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUInt16_15:
    {
        PkUint<unsigned short,15>* pi = (PkUint< unsigned short, 15 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUintRange16_0_1:
    {
        PkUintRange<unsigned short,0,1>* pi = (PkUintRange< unsigned short, 0 , 1 >*)( p );
        v = *pi;
        break;
    }
    case Type_PkUintRange16_100_1000:
    {
        PkUintRange<unsigned short,100,1000>* pi = (PkUintRange< unsigned short, 100 , 1000 >*)( p );
        v = *pi;
        break;
    }
    case Type_Int16_0_1:
    {
        PkInt<short,0,1>* pi = (PkInt< short, 0 , 1 >*)( p );
        v = *pi;
        break;
    }
    case Type_Int16_100_1000:
    {
        PkInt<short,100,1000>* pi = (PkInt< short, 100 , 1000 >*)( p );
        v = *pi;
        break;
    }
    case Type_Int16_n1_0:
    {
        PkInt<short,-1,0>* pi = (PkInt< short, -1 , 0 >*)( p );
        v = *pi;
        break;
    }
    case Type_Int16_n1000_n100:
    {
        PkInt<short,-1000,-100>* pi = (PkInt< short, -1000 , -100 >*)( p );
        v = *pi;
        break;
    }
    default:
        assert( false );
        break;
    }
    return v;
}

//-----------------------------------------------------------------------------
//
// TestBitPackCodec
//
// Perform thorough testing of the BitPackCodec class

void TestBitPackCodec()
{
    typedef std::vector< PackableType > TypeList;
    typedef std::vector< Packable* > PackableList;
    typedef std::vector< int64 > ValueList;

    TypeList typeList;          // Packable type
    PackableList packableList;  // List of Packable items
    ValueList valueList;        // List of values
    typeList.reserve( BITPACK_CODEC_ITERATIONS );
    packableList.reserve( BITPACK_CODEC_ITERATIONS );
    valueList.reserve( BITPACK_CODEC_ITERATIONS );

    for( int i = 0; i < BITPACK_CODEC_ITERATIONS; ++i )
    {
        BitPackCodec< BITPACK_CODEC_ITERATIONS > bpc;
        int nItems = ( rand() % BITPACK_CODEC_ITERATIONS ) + 1;
        for( int j = 0; j < nItems; ++j )
        {
            PackableType packableType = PackableType( rand() % PackableType_Max );
            typeList.push_back( packableType );
            Packable* p = GetRandomPackable( packableType );
            packableList.push_back( p );
            valueList.push_back( GetValue( p, packableType ) );
            bpc.Register( *p );
        }
        
        char* pBitBuffer = new char [ bpc.GetPackedBytes() ];
        bpc.Pack( pBitBuffer );
        bpc.Unpack( pBitBuffer );
        delete [] pBitBuffer;
        
        for( int j = 0; j < nItems; ++j )
        {
            int64 expectedValue = valueList[ j ];
            PackableType packableType = typeList[ j ];
            Packable* p = packableList[ j ];
            int64 actualValue = GetValue( p, packableType );
            test( actualValue == expectedValue );
            
            delete p;
        }
        
        typeList.clear();
        packableList.clear();
        valueList.clear();
    }
}

//-----------------------------------------------------------------------------
//
// Main
//
// Test driver

int CDECL main()
{
    srand( GetTickCount() );
    TestGemCode();
    TestStructAddin();
    TestPackUnpack();
    TestPackUnpackThorough();
    TestBitPackCodec();
    MessageBox( NULL, "Test complete", "Complete", MB_OK );
}
