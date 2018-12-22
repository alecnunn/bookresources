#pragma once

class CVector
{
public:
    
    CVector(){}
    CVector( const CVector &other ):X(other.X),Y(other.Y),Z(other.Z) {}
    CVector( float x, float y, float z ):X(x),Y(y),Z(z){}

    ~CVector(void){}


    const CVector& operator=( const CVector & right_value )
    {
        X = right_value.X;
        Y = right_value.Y;
        Z = right_value.Z;
        return *this;
    }

    CVector operator+( const CVector & right_value )
    {
        return CVector( X + right_value.X, Y + right_value.Y, Z + right_value.Z );
    }

    CVector & operator+=( const CVector & right_value )
    {
        X += right_value.X;
        Y += right_value.Y;
        Z += right_value.Z;
        return *this;
    }

    CVector operator-( const CVector & right_value )
    {
        return CVector( X - right_value.X, Y - right_value.Y, Z - right_value.Z );
    }

    CVector & operator-=( const CVector & right_value )
    {
        X -= right_value.X;
        Y -= right_value.Y;
        Z -= right_value.Z;
        return *this;
    }

    CVector operator*( float scale )
    {
        return CVector( X * scale, Y  * scale, Z * scale );
    }

    CVector operator/( float scale )
    {
        float inv_scale = 1/scale;
        return CVector( X * inv_scale, Y  * inv_scale, Z * inv_scale);
    }

    void Normalize();
    float GetDotProduct( const CVector & other );
    float GetLength();

public:

    float X,Y,Z;
};
