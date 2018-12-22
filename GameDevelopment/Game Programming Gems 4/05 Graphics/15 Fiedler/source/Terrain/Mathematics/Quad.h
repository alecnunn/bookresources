// Quad class

#pragma once

#include "Mathematics/Vector.h"


namespace Mathematics
{
    /// Quad on some plane in 3-space

    class Quad
    {
    public:

        Quad() {};

        Quad(const Vector &a, const Vector &b, const Vector &c, const Vector &d, const Vector &normal)
        {
            // a b
            // d c

            this->a = a;
            this->b = b;
            this->c = c;
            this->d = d;

            this->normal = normal;
        }

        Vector a,b,c,d;
        Vector normal;
    };
}
