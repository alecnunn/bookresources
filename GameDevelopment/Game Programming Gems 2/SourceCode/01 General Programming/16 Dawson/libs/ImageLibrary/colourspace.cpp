#include "priv_precompiled.h"

/*
Copyright 2001 Bruce Dawson, Cygnus Software
For more information, visit http://www.cygnus-software.com/papers/
or e-mail: comments@cygnus-software.com

This code may be redistributed as long as this notice is kept intact.
*/

#include "colourspace.h"
#include <cmath>
#include <iostream>

using namespace std;



/*
TypeRGB implementation
*/

template <class ScalarType>
TypeRGB<ScalarType>::TypeRGB()
	: r(0), g(0), b(0)
{
}

template <class ScalarType>
TypeRGB<ScalarType>::TypeRGB(ScalarType red, ScalarType green, ScalarType blue)
	: r(red), g(green), b(blue)
{
}

template <class ScalarType>
ostream& operator<<(ostream& stream, const TypeRGB<ScalarType>& rhs)
{
	stream << rhs.r << ", ";
	stream << rhs.g << ", ";
	stream << rhs.b;
	return stream;
}

template <class ScalarType>
TypeYUV<ScalarType> TypeRGB<ScalarType>::ToYUV() const
{
	TypeYUV<ScalarType> result;
	result.y = .299 * r + .587 * g + .114 * b;
	result.u = 0.492 * (b - result.y);
	result.v = 0.877 * (r - result.y);
	return result;
}

template <class ScalarType>
TypeYIQ<ScalarType> TypeRGB<ScalarType>::ToYIQ() const
{
	// HACK - this will be unnecessarily slow.
	return ToYUV().ToYIQ();
}

template <class ScalarType>
TypeHSV<ScalarType> TypeRGB<ScalarType>::ToHSV() const
{
	ScalarType max = r;
	if (g > max)
		max = g;
	if (b > max)
		max = b;
	ScalarType min = r;
	if (g < min)
		min = g;
	if (b < min)
		min = b;

	const ScalarType value = max;

	if (0 == max)
	{
		return TypeHSV<ScalarType>(180, 0, 0);
	}
	const ScalarType saturation = (max - min) / max;

	double hue;
	if (max == min)
	{
		hue = 180;
	}
	else
	{
		const ScalarType r2 = (max - r) / (max - min);
		const ScalarType g2 = (max - g) / (max - min);
		const ScalarType b2 = (max - b) / (max - min);
		if (max == r)
		{
			hue = 60 * (b2 - g2);
		}
		else if (max == g)
		{
			hue = 60 * (2 + r2 - b2);
		}
		else
		{
			assert(max == b);
			hue = 60 * (4 + g2 - r2);
		}
	}
	if (hue >= 360)
		hue -= 360;
	if (hue < 0)
		hue += 360;
	return TypeHSV<ScalarType>(hue, saturation, value);
}





/*
TypeYUV implementation
*/

template <class ScalarType>
TypeYUV<ScalarType>::TypeYUV()
	: y(0), u(0), v(0)
{
}

template <class ScalarType>
TypeYUV<ScalarType>::TypeYUV(ScalarType yArg, ScalarType uArg, ScalarType vArg)
	: y(yArg), u(uArg), v(vArg)
{
}

template <class ScalarType>
ostream& operator<<(ostream& stream, const TypeYUV<ScalarType>& rhs)
{
	stream << rhs.y << ", ";
	stream << rhs.u << ", ";
	stream << rhs.v;
	return stream;
}

template <class ScalarType>
TypeRGB<ScalarType> TypeYUV<ScalarType>::ToRGB() const
{
//	assert(U >= -112 && U <= 112);
//	assert(V >= -157 && V <= 157);
	ScalarType r = y + 1.140 * v;
	ScalarType g = y - 0.394 * u - 0.581 * v;
	ScalarType b = y + 2.032 * u;
	return TypeRGB<ScalarType>(r, g, b);
}

const double pi = atan(1.0) * 4;
const double cos33 = cos(33 * pi / 180);
const double sin33 = sin(33 * pi / 180);

template <class ScalarType>
TypeYIQ<ScalarType> TypeYUV<ScalarType>::ToYIQ() const
{
	TypeYIQ<ScalarType> result;
	result.y = y;
	result.i = cos33 * v - sin33 * u;
	result.q = sin33 * v + cos33 * u;
	return result;
}

template <class ScalarType>
TypeHSV<ScalarType> TypeYUV<ScalarType>::ToHSV() const
{
	// HACK - this will be unnecessarily slow.
	return ToRGB().ToHSV();
}





/*
TypeYIQ implementation
*/

template <class ScalarType>
TypeYIQ<ScalarType>::TypeYIQ()
	: y(0), i(0), q(0)
{
}

template <class ScalarType>
TypeYIQ<ScalarType>::TypeYIQ(ScalarType yArg, ScalarType iArg, ScalarType qArg)
	: y(yArg), i(iArg), q(qArg)
{
}

template <class ScalarType>
ostream& operator<<(ostream& stream, const TypeYIQ<ScalarType>& rhs)
{
	stream << rhs.y << ", ";
	stream << rhs.i << ", ";
	stream << rhs.q;
	return stream;
}

template <class ScalarType>
TypeRGB<ScalarType> TypeYIQ<ScalarType>::ToRGB() const
{
	TypeRGB<ScalarType> result;
	result.r = y + 0.956 * i + 0.620 * q;
	result.g = y - 0.272 * i - 0.647 * q;
	result.b = y - 1.108 * i + 1.705 * q;
	return result;
}

template <class ScalarType>
TypeYUV<ScalarType> TypeYIQ<ScalarType>::ToYUV() const
{
	// HACK - this will be unnecessarily slow.
	return ToRGB().ToYUV();
}

template <class ScalarType>
TypeHSV<ScalarType> TypeYIQ<ScalarType>::ToHSV() const
{
	// HACK - this will be unnecessarily slow.
	return ToRGB().ToHSV();
}





/*
TypeHSV implementation
*/

template <class ScalarType>
TypeHSV<ScalarType>::TypeHSV()
	: h(0), s(0), v(0)
{
}

template <class ScalarType>
TypeHSV<ScalarType>::TypeHSV(ScalarType hArg, ScalarType sArg, ScalarType vArg)
	: h(hArg), s(sArg), v(vArg)
{
}


template <class ScalarType>
ostream& operator<<(ostream& stream, const TypeHSV<ScalarType>& rhs)
{
	stream << rhs.h << ", ";
	stream << rhs.s << ", ";
	stream << rhs.v;
	return stream;
}

template <class ScalarType>
TypeRGB<ScalarType> TypeHSV<ScalarType>::ToRGB() const
{
	if (s == 0)
	{
		return TypeRGB<ScalarType>(v, v, v);
	}
	assert(h >= 0);
	assert(h < 360);
	const double hue = h / 60.0;
	const int i = int(hue);
	const double f = hue - i;
	const double p = v * (1 - s);
	const double q = v * (1 - (s * f));
	const double t = v * (1 - (s * (1 - f)));

	if (i == 0)
		return TypeRGB<ScalarType>(v, t, p);
	if (i == 1)
		return TypeRGB<ScalarType>(q, v, p);
	if (i == 2)
		return TypeRGB<ScalarType>(p, v, t);
	if (i == 3)
		return TypeRGB<ScalarType>(p, q, v);
	if (i == 4)
		return TypeRGB<ScalarType>(t, p, v);
	assert(i == 5);
	return TypeRGB<ScalarType>(v, p, q);
}

template <class ScalarType>
TypeYUV<ScalarType> TypeHSV<ScalarType>::ToYUV() const
{
	// HACK - this will be unnecessarily slow.
	return ToRGB().ToYUV();
}

template <class ScalarType>
TypeYIQ<ScalarType> TypeHSV<ScalarType>::ToYIQ() const
{
	// HACK - this will be unnecessarily slow.
	return ToRGB().ToYIQ();
}





// Instantiate the colour classes and any friend functions for
// the types I expect to use. This means that I can use the template
// without seeing the implementation.

// Instantiate for type 'unsigned char'

template
class TypeRGB<unsigned char>;
template
ostream& operator<<(ostream& stream, const TypeRGB<unsigned char>& rhs);

template
class TypeYUV<unsigned char>;
template
ostream& operator<<(ostream& stream, const TypeYUV<unsigned char>& rhs);

template
class TypeYIQ<unsigned char>;
template
ostream& operator<<(ostream& stream, const TypeYIQ<unsigned char>& rhs);

template
class TypeHSV<unsigned char>;
template
ostream& operator<<(ostream& stream, const TypeHSV<unsigned char>& rhs);



// Instantiate for type 'float'

template
class TypeRGB<float>;
template
ostream& operator<<(ostream& stream, const TypeRGB<float>& rhs);

template
class TypeYUV<float>;
template
ostream& operator<<(ostream& stream, const TypeYUV<float>& rhs);

template
class TypeYIQ<float>;
template
ostream& operator<<(ostream& stream, const TypeYIQ<float>& rhs);

template
class TypeHSV<float>;
template
ostream& operator<<(ostream& stream, const TypeHSV<float>& rhs);



// Instantiate for type 'double'

template
class TypeRGB<double>;
template
ostream& operator<<(ostream& stream, const TypeRGB<double>& rhs);

template
class TypeYUV<double>;
template
ostream& operator<<(ostream& stream, const TypeYUV<double>& rhs);

template
class TypeYIQ<double>;
template
ostream& operator<<(ostream& stream, const TypeYIQ<double>& rhs);

template
class TypeHSV<double>;
template
ostream& operator<<(ostream& stream, const TypeHSV<double>& rhs);
