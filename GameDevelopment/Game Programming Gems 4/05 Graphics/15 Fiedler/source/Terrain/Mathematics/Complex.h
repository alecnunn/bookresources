// Complex number class

#pragma once

#include <stdio.h>
#include "Float.h"


namespace Mathematics
{
    /// A complex number.
    /// z = Re(z) + i Im(z).

    class Complex
    {
    public:

        /// default constructor.
        /// does nothing for speed.

        Complex() {};

        /// construct a complex number with real component and no imaginary component.

	    Complex(float real)
	    {
		    x = real;
		    y = 0;
	    }

        /// construct a complex number with real component x, imaginary component y.

        Complex(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        /// construct complex number from polar coordinates.
        /// @param modulus the length of the complex number
        /// @argument the angle anti-clockwise from the positive real axis in radians.

	    void polar(float modulus, float argument)
	    {
		    x = modulus * Float::cos(argument);
		    y = modulus * Float::sin(argument);
	    }

        /// set the complex number to identity.
        /// @post x=1, y=0.

	    void identity()
	    {
		    x = 1;
		    y = 0;
	    }

        /// negate the complex number

	    void negate()
	    {
		    x = -x;
		    y = -y;
	    }

        /// add another complex number to this number

        void add(const Complex &complex)
        {
            x += complex.x;
            y += complex.y;
        }

        /// subtract another complex number from this number

        void subtract(const Vector &complex)
        {
            x -= complex.x;
            y -= complex.y;
        }

        /// multiply this complex number by a scalar

        void multiply(float scalar)
        {
            x *= scalar;
            y *= scalar;
        }

        /// divide this number by a scalar

	    void divide(float scalar)
	    {
		    assert(scalar!=0);
		    const float inv = 1.0f / scalar;
		    x *= inv;
		    y *= inv;
	    }

        /// calculate modulus.
        /// note: requires sqrt!

        float modulus() const
        {
		    return Float::sqrt(x*x + y*y);
        }

        /// calculate modulus squared

        float modulus_squared() const
        {
            return x*x + y*y;
        }

        /// normalize complex number.
        /// @post modulus=1.

        Complex& normalize()
        {
            const float magnitude = Float::sqrt(x*x + y*y);
            if (magnitude>Float::epsilon)
            {
                const float scale = 1.0f / magnitude;
                x *= scale;
                y *= scale;
            }
		    return *this;
        }

        /// return a normalized complex number in the same direction as this number.

	    Complex normal() const
	    {
		    Complex complex;
            const float magnitude = Float::sqrt(x*x + y*y);
            if (magnitude>Float::epsilon)
            {
                const float scale = 1.0f / magnitude;
                complex.x *= scale;
                complex.y *= scale;
            }
		    else
		    {
			    complex = *this;
		    }
		    return complex;
	    }

        /// test if this complex number is normalized

	    bool normalized() const
	    {
		    return Float::equal(modulus(),1);
	    }

        /// invert complex number.
        /// note: the inverse of a complex number can be used to perform complex division.

	    Complex& invert()
	    {
		    const float magnitude_squared = x*x + y*y;
		    if (magnitude_squared>Float::epsilon_squared)
		    {
			    const float inverse_magnitude_squared = 1.0f / magnitude_squared;
			    x *= inverse_magnitude_squared;
			    y *= -inverse_magnitude_squared;
		    }
		    return *this;
	    }

        /// return inverse of complex number.

	    Complex inverse() const
	    {
		    Complex complex;
		    const float magnitude_squared = x*x + y*y;
            if (magnitude_squared>Float::epsilon_squared)
            {
			    const float inverse_magnitude_squared = 1.0f / magnitude_squared;
                complex.x = x * inverse_magnitude_squared;
                complex.y = -y * inverse_magnitude_squared;
            }
		    else
		    {
			    complex = *this;
		    }
		    return complex;
	    }

        /// tests if this complex number is invertible

	    bool invertible() const
	    {
		    const float magnitude_squared = x*x + y*y;
            return magnitude_squared>Float::epsilon_squared;
	    }

        /// return conjugate of this complex number

	    Complex conjugate() const
	    {
		    return Complex(x, -y);
	    }

        /// get the real component

	    float real() const
	    {
		    return x;
	    }

        /// get the imaginary component

	    float imaginary() const
	    {
		    return y;
	    }

        /// calculate the argument.
        /// note: this is the angle measured counter-clockwise from the positive real axis in radians.

	    float argument() const
	    {
		    return (float) atan2(y,x);
	    }

        /// equals operator

	    bool operator ==(const Complex &other) const
	    {
		    if (Float::equal(x,other.x) && Float::equal(y,other.y)) return true;
		    else return false;
	    }

        /// not equals operator

	    bool operator !=(const Complex &other) const
	    {
		    return !(*this==other);
	    }

        /// print a basic text representation of complex number to stdout.

	    void print() const
	    {
		    printf("Complex[%f, %f]\n", x, y);
	    }

	    friend inline Complex operator-(const Complex &a);
	    friend inline Complex operator+(const Complex &a, const Complex &b);
	    friend inline Complex operator-(const Complex &a, const Complex &b);
	    friend inline Complex operator*(const Complex &a, const Complex &b);
	    friend inline Complex operator/(const Complex &a, const Complex &b);
	    friend inline Complex& operator+=(Complex &a, const Complex &b);
	    friend inline Complex& operator-=(Complex &a, const Complex &b);
	    friend inline Complex& operator*=(Complex &a, const Complex &b);
	    friend inline Complex& operator/=(Complex &a, const Complex &b);

	    friend inline Complex operator*(const Complex &a, float s);
	    friend inline Complex operator/(const Complex &a, float s);
	    friend inline Complex& operator*=(Complex &a, float s);
	    friend inline Complex& operator/=(Complex &a, float s);
	    friend inline Complex operator*(float s, const Complex &a);
	    friend inline Complex& operator*=(float s, Complex &a);

        float x;        ///< real component
        float y;        ///< imaginary component
    };


    inline Complex operator-(const Complex &a)
    {
	    return Complex(-a.x, -a.y);
    }

    inline Complex operator+(const Complex &a, const Complex &b)
    {
	    return Complex(a.x+b.x, a.y+b.y);
    }

    inline Complex operator-(const Complex &a, const Complex &b)
    {
	    return Complex(a.x-b.x, a.y-b.y);
    }

    inline Complex operator*(const Complex &a, const Complex &b)
    {
	    return Complex(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
    }

    inline Complex operator/(const Complex &a, const Complex &b)
    {
	    return a * b.inverse();		// this should be optimized!
    }

    inline Complex& operator+=(Complex &a, const Complex &b)
    {
	    a.x += b.x;
	    a.y += b.y;
	    return a;
    }

    inline Complex& operator-=(Complex &a, const Complex &b)
    {
	    a.x -= b.x;
	    a.y -= b.y;
	    return a;
    }

    inline Complex& operator*=(Complex &a, const Complex &b)
    {
        const float cx = a.x*b.x - a.y*b.y;
        const float cy = a.x*b.y + a.y*b.x;
	    a.x = cx;
	    a.y = cy;
	    return a;
    }

    inline Complex& operator/=(Complex &a, const Complex &b)
    {
	    a /= b.inverse();			// this should be optimized!
	    return a;
    }

    inline Complex operator*(const Complex &a, float s)
    {
	    return Complex(a.x*s, a.y*s);
    }

    inline Complex operator/(const Complex &a, float s)
    {
	    assert(s!=0);
	    return Complex(a.x/s, a.y/s);
    }

    inline Complex& operator*=(Complex &a, float s)
    {
	    a.x *= s;
	    a.y *= s;
	    return a;
    }

    inline Complex& operator/=(Complex &a, float s)
    {
	    assert(s!=0);
	    a.x /= s;
	    a.y /= s;
	    return a;
    }

    inline Complex operator*(float s, const Complex &a)
    {
	    return Complex(a.x*s, a.y*s);
    }

    inline Complex& operator*=(float s, Complex &a)
    {
	    a.x *= s;
	    a.y *= s;
	    return a;
    }
}
