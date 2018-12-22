// Float routines

#pragma once

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>


namespace Mathematics
{
    /// Floating point mathematics routines.

    namespace Float
    {
	    const float epsilon = 0.00001f;                         ///< floating point epsilon for single precision. todo: verify epsilon value and usage
	    const float epsilon_squared = epsilon * epsilon;        ///< epsilon value squared

	    const float pi = 3.1415926f;                            ///< pi stored at a reasonable precision for single precision floating point.

        /// test for floating point equality within [-epsilon,+epsilon]

	    inline bool equal(float a, float b)
	    {
		    const float d = a - b;
		    if (d<epsilon && d>-epsilon) return true;
		    else return false;
	    }

        /// test for floating equality nearness within [-threshold,+threshold]

	    inline bool equal(float a, float b, float threshold)
	    {
		    const float d = a - b;
		    if (d<threshold && d>-threshold) return true;
		    else return false;
	    }

        /// determine the minimum floating point value

        inline float minimum(float a, float b)
        {
            if (a<b) return a;
            else return b;
        }

        /// determine the maximum floating point value

        inline float maximum(float a, float b)
        {
            if (a>b) return a;
            else return b;
        }

        /// calculate the square root of a floating point number.
        /// todo: find some fast method for doing this.

	    inline float sqrt(float value)
	    {
		    assert(value>=0);
		    return (float) pow(value, 0.5f);			// fast sqrt?
	    }

        /// calculate the inverse square root of a floating point number.
        /// this uses the method outlined by dave eberly rumoured to have
        /// originated from the quake3 source code.

        inline float inverse_sqrt(float x)
        {
            const float xhalf = 0.5f * x;
            int i = *(int*)&x;
            i = 0x5f3759df - (i>>1);
            x = *(float*)&i;
            x = x * (1.5f - xhalf*x*x);         // repeat this line to increase accuracy
            return x;
        }

        /// calculate the sine of a floating point angle in radians.

	    inline float sin(float radians)
	    {
		    return (float) ::sin(radians);
	    }

        /// calculate the cosine of a floating point angle in radians.

	    inline float cos(float radians)
	    {
		    return (float) ::cos(radians);
	    }

        /// calculate the tangent of a floating point angle in radians.

	    inline float tan(float radians)
	    {
		    return (float) ::tan(radians);
	    }

        /// calculate the arcsine of a floating point value. result is in radians.

        inline float asin(float value)
        {
            return (float) ::asin(value);
        }

        /// calculate the arccosine of a floating point value. result is in radians.

        inline float acos(float value)
        {
            return (float) ::acos(value);
        }

        /// calculate the arctangent of a floating point value y/x. result is in radians.

	    inline float atan2(float y, float x)
	    {
		    return (float) ::atan2(y,x);
	    }

        /// calculate the floor of a floating point value.
        /// the floor is the nearest integer strictly less than or equal to the floating point number.

        inline float floor(float value)
        {
            return (float) ::floor(value);
        }

        /// calculate the ceiling of a floating point value.
        /// the ceil is the nearest integer strictly greater than or equal to the floating point number.

        inline float ceiling(float value)
        {                     
            return (float) ::ceil(value);
        }

        /// quickly determine the sign of a floating point number.
        /// note: uses integer aliasing to read the floating point sign bit.

	    inline unsigned int sign(const float& v)
	    {	
		    return (((unsigned int&)v) & 0x80000000);
	    }

        /// fast floating point absolute value.

	    inline float abs(float v)
	    {
		    *(int *)&v &= 0x7fffffff;
		    return v;
	    }

        /// interpolate between interval [a,b] with t in [0,1].

	    inline float lerp(float a, float b, float t)
	    {
		    return a + (b - a) * t;
	    }

        /// snap floating point number to grid.

	    inline float snap(float p, float grid)
	    {
		    return grid ? float( floor((p + grid*0.5f)/grid) * grid) : p;
	    }

		/// swap two floating point numbers

		inline void swap(float &a, float &b)
		{
			const float tmp = a;
			a = b;
			b = tmp;
		}

	    /// intel specific: set single precision mode.

	    inline void singlePrecision()
	    {
		    _controlfp( _PC_24, _MCW_PC );
	    }

	    /// intel specific: set double precision mode.

	    inline void doublePrecision()
	    {
		    _controlfp( _PC_53, _MCW_PC );
	    }

	    /// intel specific: check if in single precision mode.
        /// note: this routine seems to act unreliably and should not be relied or asserted on.

	    inline bool isSinglePrecision()
	    {
		    unsigned int status = _controlfp( 0, 0 ) & _MCW_PC;
		    return status == _PC_24;
	    }

	    /// intel specific: check if in double precision mode.
        /// note: this routine seems to act unreliably and should not be relied or asserted on.

        inline bool isDoublePrecision()
	    {
		    unsigned int status = _controlfp( 0, 0 ) & _MCW_PC;
		    return status == _PC_53 || status == _PC_64;
	    }

        /// floating point integer aliasing macro.

	    #define MATH_FLTBIT(v) ((int*)&v)[0]

        /// fast floating point to integer conversion.
	    /// warning: this code requires double precision!

	    inline int integer(float v)
	    {
		    double magic = 68719476736.0 * 1.5 + v;
		    return MATH_FLTBIT(magic) >> 16;
	    }

        /// fast floating point to integer conversion with chop.
	    /// warning: this code requires double precision!

        inline int integerChop(const float& v)
	    {
		    const int& u = *(const int*)&v;
		    int s = u >> 31;
		    int e = ((u & 0x7fffffff) >> 23) - 127;
		    return (((((unsigned int)((u & ((1<<23) - 1)) | (1<<23)) << 8) >> (31-e)) ^ (s)) - s ) &~ (e>>31);
	    }

        /// fast floating point to integer conversion with floor.
	    /// warning: this code requires double precision!

	    inline int integerFloor(const float& v)
	    {
		    const int& u = *(const int*)&v;
		    int sign = u >> 31;
		    int exponent = ((u & 0x7fffffff)>>23) - 127;
		    int expsign = ~(exponent >> 31);
		    int mantissa = (u & ((1<<23) - 1));
		    return (((((unsigned int)(mantissa | (1<<23)) << 8) >> (31-exponent)) & expsign) ^ (sign)) + ((!((mantissa<<8) & (((1<<(31-(exponent & expsign)))) - 1))) & sign);
	    }

        /// fast floating point to integer conversion with ceiling.
	    /// warning: this code requires double precision!

	    inline int integerCeiling(const float& v)
	    {
		    int u = (*(const int*)&v) ^ 0x80000000;
		    int sign = u >> 31;
		    int exponent = ((u & 0x7fffffff)>>23) - 127;
		    int expsign = ~(exponent>>31);
		    int mantissa = (u & ((1<<23) - 1));
		    return -(int)((((((unsigned int)(mantissa | (1<<23)) << 8) >> (31-exponent)) & expsign) ^ (sign)) + ((!((mantissa<<8) & (((1<<(31-(exponent & expsign)))) - 1))) & sign & expsign));
	    }

        /// fast floating point to fixed point 16.16.
	    /// warning: this code requires double precision!

	    inline int fixedPoint(float v)
	    {
		    double magic = 68719476736.0 * 1.5 + v;
		    return MATH_FLTBIT(magic);
	    }

		/// Find lowest root to equation ax^2+bx+c=0 in interval [0,upper]
		/// Because the standard way of solving this equation is not stable when a is close to zero!
		/// note: NaN's, INF's and IND's are considered outside the interval.
		/// See "Numerical Recipies in C" paragraph 5.6 for theory.
		/// @param a the a coefficient of the quadratic
		/// @param b the b coefficient of the quadratic
		/// @param c the c coefficient of the quadratic
		/// @param upper the upper bound (the lower bound is always 0)
		/// @param root the lowest quadtratic root found in the interval (valid only if returns true)
		/// @returns true if a root exists in the interval

		inline bool lowestQuadraticRoot(float a, float b, float c, float upper, float &root)
		{
			// exit early if no real solutions exist
			
			const float determinant = b*b - 4*a*c;

			if (determinant<0.0f)
				return false;

			// solve the quadratic equation as described in numerical recipies

			const float q = -0.5f * (b + (b < 0.0f? -1.0f : 1.0f) * Float::sqrt(determinant));
			
			float x1 = q / a;
			float x2 = c / q;

			// order the results

			if (x2<x1)
				swap(x1,x2);

			// lowest root is x1 if in interval

			if (x1>=0.0f && x1<=upper)
			{
				root = x1;
				return true;
			}

			// else x2 is the root if in interval

			if (x2>=0.0f && x2<=upper)
			{
				root = x2;
				return true;
			}

			return false;
		}
    }
}
