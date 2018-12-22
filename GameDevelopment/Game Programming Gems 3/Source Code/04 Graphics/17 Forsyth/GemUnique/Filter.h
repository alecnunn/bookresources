
#ifndef FILTER_INCLUDED
#define FILTER_INCLUDED

#pragma once



#include "Image.h"




struct Position2D
{
	int X, Y;

	Position2D ( int iX = 0, int iY = 0 )
	{
		X = iX;
		Y = iY;
	}

	Position2D operator+ ( Position2D const &other ) const
	{
		return Position2D ( X + other.X, Y + other.Y );
	}

	Position2D operator- ( Position2D const &other ) const
	{
		return Position2D ( X - other.X, Y - other.Y );
	}

	Position2D operator>> ( int iShift ) const
	{
		return Position2D ( X >> iShift, Y >> iShift );
	}

	Position2D operator<< ( int iShift ) const
	{
		return Position2D ( X << iShift, Y << iShift );
	}

	Position2D operator* ( int iMult ) const
	{
		return Position2D ( X * iMult, Y * iMult );
	}

	bool operator== ( Position2D const &other ) const
	{
		return ( ( X == other.X ) & ( Y == other.Y ) );
	}

};


struct Rect2D
{
	Position2D tlc, brc;

	Rect2D ( Position2D thetlc, Position2D thebrc )
	{
		tlc = thetlc;
		brc = thebrc;
	}
	Rect2D ( int iLeft, int iTop, int iRight, int iBottom )
	{
		tlc.X = iLeft;
		tlc.Y = iTop;
		brc.X = iRight;
		brc.Y = iBottom;
	}

#ifdef _DEBUG
	void Check ( void )
	{
		if ( tlc.X == brc.X )
		{
			// Should be the NULL rect.
			ASSERT ( tlc.X == 0 );
			ASSERT ( tlc.Y == 0 );
			ASSERT ( brc.X == 0 );
			ASSERT ( brc.Y == 0 );
		}
		else
		{
			ASSERT ( tlc.X < brc.X );
			ASSERT ( tlc.Y < brc.Y );
		}
	}
#else
	void Check ( void ){}
#endif

	const Rect2D Intersect ( const Rect2D &other );

	Rect2D operator+ ( Position2D const &other ) const
	{
		return Rect2D ( tlc + other, brc + other );
	}

	Rect2D operator- ( Position2D const &other ) const
	{
		return Rect2D ( tlc - other, brc - other );
	}

	bool operator== ( Rect2D const &other ) const
	{
		return ( ( tlc == other.tlc ) && ( brc == other.brc ) );
	}
};

const Rect2D rect2dNull ( 0, 0, 0, 0 );

inline const Rect2D Rect2D::Intersect ( const Rect2D &other )
{
	Rect2D res = *this;
	if ( tlc.X < other.tlc.X )
	{
		if ( brc.X < other.tlc.X )
		{
			return rect2dNull;
		}
		res.tlc.X = other.tlc.X;
	}
	if ( tlc.Y < other.tlc.Y )
	{
		if ( brc.Y < other.tlc.Y )
		{
			return rect2dNull;
		}
		res.tlc.Y = other.tlc.Y;
	}
	if ( brc.X > other.brc.X )
	{
		if ( tlc.X > other.brc.X )
		{
			return rect2dNull;
		}
		res.brc.X = other.brc.X;
	}
	if ( brc.Y > other.brc.Y )
	{
		if ( tlc.Y > other.brc.Y )
		{
			return rect2dNull;
		}
		res.brc.Y = other.brc.Y;
	}
	return res;
}



// Gets a "random" number - 0<=n<iRange.
// u32Offset[1-3] define a "type" of random number. Just bung in any old vaguely unique thing.
inline int GetRandom ( int iRange,
						ui32 u32Seed,
						ui32 u32Offset1 = 0,
						ui32 u32Offset2 = 0,
						ui32 u32Offset3 = 0 )
{
	u32Seed ^= u32Offset1;
	u32Seed ^= u32Offset2;
	u32Seed ^= u32Offset3;

	// Not a good generator, just hacked til it looks OK in this app.
	u32Seed ^= ( u32Seed >> 5 ) ^ ( u32Seed << 8 );
	u32Seed *= 0x9647;
	u32Seed ^= u32Seed >> 4;


	u32Seed &= 0xffff;
	ASSERT ( iRange >= 0 );
	ASSERT ( iRange <= 0xffff );
	u32Seed *= iRange;
	u32Seed >>= 16;

	return u32Seed;
}







// Base class - don't create these directly.
class Filter
{
public:
	DWORD dwLayerNumber;		// Determines what order things are applied in.

protected:

	Filter *pfilterNext;		// Next one in the chain.
	Filter *pfilterPrev;		// Previous one in the chain.

	Filter ( Filter *pfilterNewPrev, DWORD dwNewLayerNumber )
	{
		dwLayerNumber = dwNewLayerNumber;
		pfilterPrev = pfilterNewPrev;
		if ( pfilterPrev != NULL )
		{
			ASSERT ( dwLayerNumber < pfilterPrev->dwLayerNumber );
			pfilterNext = pfilterPrev->pfilterNext;
			pfilterPrev->pfilterNext = this;
			if ( pfilterNext != NULL )
			{
				ASSERT ( dwLayerNumber > pfilterNext->dwLayerNumber );
				ASSERT ( pfilterNext->pfilterPrev == pfilterPrev );
				pfilterNext->pfilterPrev = this;
			}
		}
		else
		{
			pfilterNext = NULL;
		}
	}

public:
	virtual ~Filter ( void )
	{
		if ( pfilterNext != NULL )
		{
			ASSERT ( pfilterNext->pfilterPrev == this );
			pfilterNext->pfilterPrev = pfilterPrev;
		}
		if ( pfilterPrev != NULL )
		{
			ASSERT ( pfilterPrev->pfilterNext == this );
			pfilterPrev->pfilterNext = pfilterNext;
		}
	}

	// Renders the filter.
	virtual void Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed ) = 0;

	Filter *Next ( void ) const
	{
		return pfilterNext;
	}

	Filter *Prev ( void ) const
	{
		return pfilterPrev;
	}

};




// A placeholder - used for the root of the linked list.
class FilterRoot : public Filter
{
public:
	FilterRoot ( void ) : Filter ( NULL, 0xffffffff ) {}

	virtual ~FilterRoot ( void ) {}

	virtual void Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed )
	{
		// Don't do this.
		ASSERT ( FALSE );
	}
};




#if 0
class FilterAlphaBlendLayer : public Filter
{
protected:
	Image *pimage;
	Position2D p2dPosition;
	Position2D p2dRandomRange;
public:
	FilterAlphaBlendLayer ( Filter *pfilterNewPrev,
							DWORD dwNewLayerNumber,
							Image *pTheImage,
							Position2D ThePosition,
							Position2D TheRandomRange
							) : Filter ( pfilterNewPrev, dwNewLayerNumber )
	{
		pimage = pTheImage;
		p2dPosition = ThePosition;
		p2dRandomRange = TheRandomRange;
	}

	virtual ~FilterAlphaBlendLayer ( void ) {}

	virtual void Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed );
};
#endif



// A layer that is offset and wrapped.
// It gets alpha-blended on using a non-wrapped alpha "blob" shape. The position of the
// alpha does not change - it is always centred on the passed-in position (the pos of the vertex).
// The same alpha blob is used by everything. The size of this blob should be
// twice the gap from one vertex to the next, or odd things will happen.
// The X,Y offset of the source is found using the seed.
// The image may be any size - it will be wrapped.
class FilterLandscapeLayerAlpha : public Filter
{
protected:
	Image *pimage;
	static Image *pimageAlpha;
public:
	FilterLandscapeLayerAlpha (	Filter *pfilterNewPrev,
								DWORD dwNewLayerNumber,
								Image *pTheImage
								) : Filter ( pfilterNewPrev, dwNewLayerNumber )
	{
		pimage = pTheImage;
		if ( pimageAlpha == NULL )
		{
			// Load up the standard alpha blob shared by all landscape alpha layers.
			pimageAlpha = Image::LoadFromFile ( "Landscape blob.bmp", true );
		}
	}

	virtual ~FilterLandscapeLayerAlpha ( void ) {}

	virtual void Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed );
};




// Same as FilterLandscapeLayerAlpha, except with a different alpha-blend.
// This should be the first of the LandscapeLayers to be applied.
class FilterLandscapeLayerBase : public FilterLandscapeLayerAlpha
{
public:
	FilterLandscapeLayerBase (	Filter *pfilterNewPrev,
								DWORD dwNewLayerNumber,
								Image *pTheImage
								) : FilterLandscapeLayerAlpha ( pfilterNewPrev, dwNewLayerNumber, pTheImage )
	{
		// Nothing extra needs doing.
	}

	virtual ~FilterLandscapeLayerBase ( void ) {}

	virtual void Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed );
};


// Same as FilterLandscapeLayerAlpha, except with a different alpha-blend.
// This should be the last of the LandscapeLayers to be applied.
class FilterLandscapeLayerFinal : public FilterLandscapeLayerAlpha
{
public:
	FilterLandscapeLayerFinal (	Filter *pfilterNewPrev,
								DWORD dwNewLayerNumber,
								Image *pTheImage
								) : FilterLandscapeLayerAlpha ( pfilterNewPrev, dwNewLayerNumber, pTheImage )
	{
		// Nothing extra needs doing.
	}

	virtual ~FilterLandscapeLayerFinal ( void ) {}

	virtual void Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed );
};




class FilterAlphaColourLayer : public Filter
{
protected:
	Image *pimage;
	Pixel pixColour;
	Position2D p2dPosition;
public:
	FilterAlphaColourLayer (	Filter *pfilterNewPrev,
								DWORD dwNewLayerNumber,
								Image *pTheImage,
								Pixel pixTheColour,
								Position2D ThePosition
								) : Filter ( pfilterNewPrev, dwNewLayerNumber )
	{
		pimage = pTheImage;
		p2dPosition = ThePosition;
		pixColour = pixTheColour;
	}

	virtual ~FilterAlphaColourLayer ( void ) {}

	virtual void Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed );
};





#endif //#ifndef FILTER_INCLUDED




