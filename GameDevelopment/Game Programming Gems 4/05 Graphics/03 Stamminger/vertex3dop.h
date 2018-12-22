//
// Perspective Shadow Maps
// accompanying example code for Game Programming Gems 4
// (w)(c) 2003, Carsten Dachsbacher (dachsbacher@cs.fau.de), Marc Stamminger (stamminger@cs.fau.de)
//
#ifndef VERTEX3DOP__H
#define VERTEX3DOP__H

typedef struct
{
    float   x, y, z, w;
}VERTEX3D;

typedef float MATRIX44[ 16 ];

void        multMatrix( MATRIX44 dst, const MATRIX44 &a, const MATRIX44 &b );
void        InverseMatrixAnglePreserving( MATRIX44 dest, const MATRIX44 source );

float       operator * ( const VERTEX3D &a, const VERTEX3D &b );

void        operator *= ( VERTEX3D &a, const float b );

void        operator ~ ( VERTEX3D &a );

VERTEX3D    operator * ( const MATRIX44 &matrix, const VERTEX3D &source );


#endif