#include "vector.h"
#include <math.h>

void CVector::Normalize()
{
    float inv_length = 1 / GetLength();
    
    X *= inv_length;
    Y *= inv_length;
    Z *= inv_length;
}

float CVector::GetDotProduct(const CVector & other )
{
    return X*other.X + Y*other.Y + Z*other.Z;
}

float CVector::GetLength()
{
    return sqrt(X*X + Y*Y + Z*Z);
}