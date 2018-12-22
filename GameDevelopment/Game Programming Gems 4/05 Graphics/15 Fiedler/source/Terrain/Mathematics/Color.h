// Color class

#pragma once

#include <stdio.h>
#include "Float.h"


namespace Mathematics
{
    /// A color triplet plus alpha.
    /// Contains red, green, blue and alpha values in the range [0.0f,1.0f].

    class Color
    {
    public:

        /// default constructor.
        /// default color is black with zero alpha.

        Color()
        {
            r = 0;
            g = 0;
            b = 0;
            a = 1;
        }

        /// construct color with given r,g,b and optional alpha value a.
        /// note: r,g,b,a must be in the range [0,1].

	    Color(float r, float g, float b, float a = 1.0f)
	    {
		    assert(r>=0);
		    assert(r<=1);
		    assert(g>=0);
		    assert(g<=1);
		    assert(b>=0);
		    assert(b<=1);
		    assert(a>=0);
		    assert(a<=1);

		    this->r = r;
		    this->g = g;
		    this->b = b;
		    this->a = a;
	    }

        /// copy constructor

        Color(const Color &other)
	    {
		    *this = other;
	    }

        /// pack color into 32bit color integer.
        /// note: you must be in double precision floating point mode on x86
        /// systems because this method uses fast float to integer conversion
        /// with Float::integer method.

	    unsigned int packed() const
	    {
		    const unsigned int a = (unsigned int) Float::integer(this->a * 255);
		    const unsigned int r = (unsigned int) Float::integer(this->r * 255);
		    const unsigned int g = (unsigned int) Float::integer(this->g * 255);
		    const unsigned int b = (unsigned int) Float::integer(this->b * 255);

		    assert(a>=0);
		    assert(r>=0);
		    assert(g>=0);
		    assert(b>=0);

		    assert(a<=255);
		    assert(r<=255);
		    assert(g<=255);
		    assert(b<=255);

		    return (a<<24) | (r<<16) | (g<<8) | b;
	    }

        /// assignment operator

	    Color& operator =(const Color &other)
	    {
		    r = other.r;
		    g = other.g;
		    b = other.b;
		    a = other.a;
            return *this;
	    }

        /// equals operator

	    bool operator ==(const Color &other) const
	    {
		    if (Float::equal(r,other.r) && Float::equal(g,other.g) && Float::equal(b,other.b) && Float::equal(a,other.a)) return true;
		    else return false;
	    }

        /// not equals operator

	    bool operator !=(const Color &other) const
	    {
		    return !(*this==other);
	    }

        /// print a basic representation of the color data to stdout.

	    void print() const
	    {
		    printf("Color[%f, %f, %f, %f]\n", r, g, b, a);
	    }

	    float r;        ///< the red color component in range [0,1].
        float g;        ///< the green color component in range [0,1].
        float b;        ///< the blue color component in range [0,1].
        float a;        ///< the alpha color component in range [0,1]. 0 is transparent, 1 is fully opaque.
    };
}
