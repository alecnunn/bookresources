//
// Perspective Shadow Maps
// accompanying example code for Game Programming Gems 4
// (w)(c) 2003, Carsten Dachsbacher (dachsbacher@cs.fau.de), Marc Stamminger (stamminger@cs.fau.de)
//
#include <math.h>
#include "vertex3dop.h"

// dot product
float   operator * ( const VERTEX3D &a, const VERTEX3D &b )
{
    return ( a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w );
}

// vector * scalar
void    operator *= ( VERTEX3D &a, const float b )
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    a.w *= b;
}

// normalize vector
void    operator ~ ( VERTEX3D &a )
{
    float t = a * a;

    if ( t > 0 ) t = 1.0f / sqrtf( t );

    a *= t;
}

// multiply matrices
void    multMatrix( MATRIX44 dst, const MATRIX44 &a, const MATRIX44 &b )
{
    int  i, j, k;

    for ( i = 0; i < 16; i++ )
        dst[ i ] = 0.0f;

    for ( i = 0; i < 4; i++ )
        for ( j = 0; j < 4; j++ )
            for ( k = 0; k < 4; k++ )
                dst[ i + 4 * j ] += a[ i + 4 * k ] * b[ k + 4 * j ];
}

// matrix * vector
VERTEX3D    operator * ( const MATRIX44 &matrix, const VERTEX3D &source )
{
    VERTEX3D dest;

    dest.x = source.x * matrix[ 0 + 4 * 0 ] +
             source.y * matrix[ 0 + 4 * 1 ] +
             source.z * matrix[ 0 + 4 * 2 ] + 
             source.w * matrix[ 0 + 4 * 3 ];
    dest.y = source.x * matrix[ 1 + 4 * 0 ] +
             source.y * matrix[ 1 + 4 * 1 ] +
             source.z * matrix[ 1 + 4 * 2 ] + 
             source.w * matrix[ 1 + 4 * 3 ];
    dest.z = source.x * matrix[ 2 + 4 * 0 ] +
             source.y * matrix[ 2 + 4 * 1 ] +
             source.z * matrix[ 2 + 4 * 2 ] + 
             source.w * matrix[ 2 + 4 * 3 ];
    dest.w = source.x * matrix[ 3 + 4 * 0 ] +
             source.y * matrix[ 3 + 4 * 1 ] +
             source.z * matrix[ 3 + 4 * 2 ] + 
             source.w * matrix[ 3 + 4 * 3 ];

    return dest;
}

// inverse matrix
void    InverseMatrixAnglePreserving( MATRIX44 dst, const MATRIX44 src )
{
    float scale;

    scale = src[ 0 + 4 * 0 ] * src[ 0 + 4 * 0 ] +
            src[ 0 + 4 * 1 ] * src[ 0 + 4 * 1 ] +
            src[ 0 + 4 * 2 ] * src[ 0 + 4 * 2 ];

    if ( fabs( scale ) < 0.0001f ) return;

    scale = 1.0f / scale;

    dst[ 0 + 4 * 0 ] = scale * src[ 0 + 4 * 0 ];
    dst[ 1 + 4 * 0 ] = scale * src[ 0 + 4 * 1 ];
    dst[ 2 + 4 * 0 ] = scale * src[ 0 + 4 * 2 ];
    dst[ 0 + 4 * 1 ] = scale * src[ 1 + 4 * 0 ];
    dst[ 1 + 4 * 1 ] = scale * src[ 1 + 4 * 1 ];
    dst[ 2 + 4 * 1 ] = scale * src[ 1 + 4 * 2 ];
    dst[ 0 + 4 * 2 ] = scale * src[ 2 + 4 * 0 ];
    dst[ 1 + 4 * 2 ] = scale * src[ 2 + 4 * 1 ];
    dst[ 2 + 4 * 2 ] = scale * src[ 2 + 4 * 2 ];

    dst[ 0 + 4 * 3 ] = - ( dst[ 0 + 4 * 0 ] * src[ 0 + 4 * 3 ] +
                           dst[ 0 + 4 * 1 ] * src[ 1 + 4 * 3 ] +
                           dst[ 0 + 4 * 2 ] * src[ 2 + 4 * 3 ] );
    dst[ 1 + 4 * 3 ] = - ( dst[ 1 + 4 * 0 ] * src[ 0 + 4 * 3 ] +
                           dst[ 1 + 4 * 1 ] * src[ 1 + 4 * 3 ] +
                           dst[ 1 + 4 * 2 ] * src[ 2 + 4 * 3 ] );
    dst[ 2 + 4 * 3 ] = - ( dst[ 2 + 4 * 0 ] * src[ 0 + 4 * 3 ] +
                           dst[ 2 + 4 * 1 ] * src[ 1 + 4 * 3 ] +
                           dst[ 2 + 4 * 2 ] * src[ 2 + 4 * 3 ] );

    dst[ 3 + 4 * 0 ] = 
    dst[ 3 + 4 * 1 ] = 
    dst[ 3 + 4 * 2 ] = 0.0f;
    dst[ 3 + 4 * 3 ] = 1.0f;
};

