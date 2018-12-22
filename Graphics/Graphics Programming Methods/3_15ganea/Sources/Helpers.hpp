
// ----------------------------------------------------------------------------------------------------
//
//						'An optimal dynamic surface caching algorithm'
//
//						   by Alexandre Ganea (aganea@dc-studios.com)
//
// ----------------------------------------------------------------------------------------------------
//						 Basic classes for managing 2d vectors and boxes
// ----------------------------------------------------------------------------------------------------

#pragma once

#include "windows.h"
#include "ptc.h"

// ----------------------------------------------------------------------------------------------------

template <class T> class Vector2D
{

public:

	inline							Vector2D		( void );
	inline							Vector2D		( const Vector2D<T> & Vector );
	inline							Vector2D		( const T X, const T Y );

	// ----------------------------------------------------------------------------------------

	inline	void					Clear			( void );
	inline	void					Set				( const T X, const T Y );
	
	// ----------------------------------------------------------------------------------------

	inline	Vector2D<T>				operator -		( const Vector2D<T> & Vector ) const;
	inline	Vector2D<T>				operator +		( const Vector2D<T> & Vector ) const;
	
	inline	Vector2D<T>				operator -		( const T Value ) const;
	inline	Vector2D<T>				operator +		( const T Value ) const;

	inline	Vector2D<T>				operator /		( const T Value ) const;

	// ----------------------------------------------------------------------------------------
	
	T		m_X;
	T		m_Y;
};

template <class T> inline Vector2D<T>::Vector2D ( void ) :	m_X( 0 ), m_Y( 0 )
{
}

template <class T> inline Vector2D<T>::Vector2D ( const Vector2D<T> & Vector ) :
	m_X( Vector.m_X ), m_Y( Vector.m_Y )
{
}

template <class T> inline Vector2D<T>::Vector2D ( const T X, const T Y ) :
	m_X( X ), m_Y( Y )
{
}

template <class T> inline void Vector2D<T>::Clear ( void )
{
	m_X		=	0;
	m_Y		=	0;
}

template <class T> inline void Vector2D<T>::Set ( const T X, const T Y )
{
	m_X		=	X;
	m_Y		=	Y;
}

template <class T> inline Vector2D<T> Vector2D<T>::operator + ( const Vector2D<T> & Vector ) const
{
	return ( Vector2D<T>( m_X + Vector.m_X, m_Y + Vector.m_Y ) );
}

template <class T> inline Vector2D<T> Vector2D<T>::operator - ( const Vector2D<T> & Vector ) const
{
	return ( Vector2D<T>( m_X - Vector.m_X, m_Y - Vector.m_Y ) );
}

template <class T> inline Vector2D<T> Vector2D<T>::operator + ( const T Value ) const
{
	return ( Vector2D<T>( m_X + Value, m_Y + Value ) );
}

template <class T> inline Vector2D<T> Vector2D<T>::operator - ( const T Value ) const
{
	return ( Vector2D<T>( m_X - Value, m_Y - Value ) );
}

template <class T> inline Vector2D<T> Vector2D<T>::operator / ( const T Value ) const
{
	return ( Vector2D<T>( m_X / Value, m_Y / Value ) );
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

template <class T> class Box2D
{

public:

	inline									Box2D				( void );
	inline									Box2D				( const Box2D<T> & Box );
	inline									Box2D				( const T Left, const T Top, const T Right, const T Bottom );

	// ----------------------------------------------------------------------------------------

	inline			T						GetWidth			( void ) const;
	inline			T						GetHeight			( void ) const;
	
	// ----------------------------------------------------------------------------------------

	inline			void					Build				( const Vector2D<T> & Position, const Vector2D<T> & Size );

	enum IntersectionResult
	{
		Outside,
		CompletelyInside,
		PartiallyInside,
	};

	inline			IntersectionResult		IsInside			( const Box2D<T> & Box ) const;

	inline			void					Clip				( const Box2D<T> & Box );

	inline			Vector2D<T>				ComputeDistance		( const Box2D<T> & Box );

	// ----------------------------------------------------------------------------------------

	inline	const	Box2D<T> &				operator +=			( const Vector2D<T> & Offset );
	inline	const	Box2D<T> &				operator -=			( const Vector2D<T> & Offset );

	// ----------------------------------------------------------------------------------------
	
	Vector2D<T>		m_TopLeft;
	Vector2D<T>		m_BottomRight;
};
	
template <class T> inline Box2D<T>::Box2D ( void )
{
}

template <class T> inline Box2D<T>::Box2D ( const Box2D<T> & Box ) :
	m_TopLeft( Box.m_TopLeft ), m_BottomRight( Box.m_BottomRight )
{
}

template <class T> inline Box2D<T>::Box2D ( const T Left, const T Top, const T Right, const T Bottom ) :
	m_TopLeft( Left, Top ), m_BottomRight( Right, Bottom )
{
}

template <class T> inline T Box2D<T>::GetWidth ( void ) const
{
	return ( m_BottomRight.m_X - m_TopLeft.m_X + 1 );
}

template <class T> inline T Box2D<T>::GetHeight ( void ) const
{
	return ( m_BottomRight.m_Y - m_TopLeft.m_Y + 1 );
}

template <class T> inline void Box2D<T>::Build ( const Vector2D<T> & Position, const Vector2D<T> & Size )
{
	m_TopLeft		=	Position;
	m_BottomRight	=	Position + Size - 1;
}

template <class T> inline Box2D<T>::IntersectionResult Box2D<T>::IsInside ( const Box2D<T> & Box ) const
{
	if (	Box.m_BottomRight.m_X < m_TopLeft.m_X || Box.m_TopLeft.m_X > m_BottomRight.m_X ||
			Box.m_BottomRight.m_Y < m_TopLeft.m_Y || Box.m_TopLeft.m_Y > m_BottomRight.m_Y		)
	{
		return ( Outside );
	}

	if (	Box.m_TopLeft.m_X >= m_TopLeft.m_X && Box.m_BottomRight.m_X <= m_BottomRight.m_X &&
			Box.m_TopLeft.m_Y >= m_TopLeft.m_Y && Box.m_BottomRight.m_Y <= m_BottomRight.m_Y	)
	{
		return ( CompletelyInside );
	}

	return ( PartiallyInside );
}

template <class T> inline void Box2D<T>::Clip ( const Box2D<T> & Box )
{
	if ( Box.m_TopLeft.m_X > m_TopLeft.m_X )
	{
		m_TopLeft.m_X		=	Box.m_TopLeft.m_X;
	}

	if ( Box.m_BottomRight.m_X < m_BottomRight.m_X )
	{
		m_BottomRight.m_X	=	Box.m_BottomRight.m_X;
	}

	if ( Box.m_TopLeft.m_Y > m_TopLeft.m_Y )
	{
		m_TopLeft.m_Y		=	Box.m_TopLeft.m_Y;
	}

	if ( Box.m_BottomRight.m_Y < m_BottomRight.m_Y )
	{
		m_BottomRight.m_Y	=	Box.m_BottomRight.m_Y;
	}
}

template <class T> inline Vector2D<T> Box2D<T>::ComputeDistance ( const Box2D<T> & Box )
{
	return ( Vector2D<T>( Box.m_TopLeft.m_X - m_TopLeft.m_X, Box.m_TopLeft.m_Y - m_TopLeft.m_Y ) );
}

template <class T> inline const	Box2D<T> & Box2D<T>::operator += ( const Vector2D<T> & Offset )
{
	m_TopLeft.m_X		+=	Offset.m_X;
	m_TopLeft.m_Y		+=	Offset.m_Y;
	m_BottomRight.m_X	+=	Offset.m_X;
	m_BottomRight.m_Y	+=	Offset.m_Y;

	return ( *this );
}

template <class T> inline const	Box2D<T> & Box2D<T>::operator -= ( const Vector2D<T> & Offset )
{
	m_TopLeft.m_X		-=	Offset.m_X;
	m_TopLeft.m_Y		-=	Offset.m_Y;
	m_BottomRight.m_X	-=	Offset.m_X;
	m_BottomRight.m_Y	-=	Offset.m_Y;

	return ( *this );
}

// ----------------------------------------------------------------------------------------------------
