/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#ifndef	COLOURLIB_H
#define	COLOURLIB_H

#include <iosfwd>

/*
This file contains classes that represent different colour spaces. Each
class is implemented as a template to allow it to be instantiated with
any type - typically unsigned char or double.

Each class should have conversion functions to convert to all of the
other colour spaces.

I don't use operators to do the conversions because automatic conversions
are frequently confusing, error prone, and they hide the performance costs -
without any clear benefit.

The implementation of the template classes is in the .cpp file and the
classes are explicitly instantiated for unsigned char and double. If you
wish to use them for other types you will need to add more explicit
instantiations. Keeping the implementation in the .cpp file is more
difficult for template classes, but it avoids having all clients see
all of the implementation code as well as iostreams and other headers.
*/

template <class ScalarType> class TypeRGB;
template <class ScalarType> class TypeYUV;
template <class ScalarType> class TypeYIQ;
template <class ScalarType> class TypeHSV;

template <class ScalarType> class TypeRGB
{
public:
	TypeRGB();
	TypeRGB(ScalarType red, ScalarType green, ScalarType blue);

	//ScalarType& operator[](int index) {return &r[index]; }
	//const ScalarType& operator[](int index) const {return &r[index]; }
	friend std::ostream& operator<<(std::ostream& stream, const TypeRGB& rhs);
	TypeYUV<ScalarType> ToYUV() const;
	TypeYIQ<ScalarType> ToYIQ() const;
	TypeHSV<ScalarType> ToHSV() const;

	ScalarType	r, g, b;
};

template <class ScalarType> class TypeYUV
{
public:
	TypeYUV();
	TypeYUV(ScalarType yArg, ScalarType uArg, ScalarType vArg);
	friend std::ostream& operator<<(std::ostream& stream, const TypeYUV& rhs);
	TypeRGB<ScalarType> ToRGB() const;
	TypeYIQ<ScalarType> ToYIQ() const;
	TypeHSV<ScalarType> ToHSV() const;

	ScalarType	y, u, v;
};

template <class ScalarType> class TypeYIQ
{
public:
	TypeYIQ();
	TypeYIQ(ScalarType yArg, ScalarType iArg, ScalarType qArg);
	friend std::ostream& operator<<(std::ostream& stream, const TypeYIQ& rhs);
	TypeRGB<ScalarType> ToRGB() const;
	TypeYUV<ScalarType> ToYUV() const;
	TypeHSV<ScalarType> ToHSV() const;

	ScalarType	y, i, q;
};

template <class ScalarType> class TypeHSV
{
public:
	TypeHSV();
	TypeHSV(ScalarType hArg, ScalarType sArg, ScalarType vArg);
	friend std::ostream& operator<<(std::ostream& stream, const TypeHSV& rhs);
	TypeRGB<ScalarType> ToRGB() const;
	TypeYUV<ScalarType> ToYUV() const;
	TypeYIQ<ScalarType> ToYIQ() const;

	double		h;	// Hue fits badly into an unsigned char - let's not pretend it fits.
	ScalarType	s, v;
};

#endif
