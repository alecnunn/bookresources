
// ----------------------------------------------------------------------------------------------------
//
//						'An optimal dynamic surface caching algorithm'
//
//						   by Alexandre Ganea (aganea@dc-studios.com)
//
// ----------------------------------------------------------------------------------------------------
//								The sprite-based hardware emulation
// ----------------------------------------------------------------------------------------------------

#include "Sprite Engine.hpp"

// ----------------------------------------------------------------------------------------------------

HardwareSpriteEngine::Entry::Entry ( void ) :	m_DataAddress( 0 ), m_ColorFormat( Format_Unknown ),
												m_bVisibility( true ), m_TransparentColor( 0 ),
												m_pPrevious( NULL ), m_pNext( NULL )
{
}

HardwareSpriteEngine::Entry::~Entry ( void )
{
}

void HardwareSpriteEngine::Entry::Chain ( Entry * pPreviousObject )
{
	if ( !pPreviousObject )	// Unchain
	{
		if ( m_pPrevious )
		{
			m_pPrevious->m_pNext	=	m_pNext;
		}

		if ( m_pNext )
		{
			m_pNext->m_pPrevious	=	m_pPrevious;
		}

		m_pNext			=	NULL;
		m_pPrevious		=	NULL;
	}
	else	// Chain
	{
		pPreviousObject->m_pNext	=	this;

		m_pPrevious		=	pPreviousObject;
	}
}

Box2D<long> HardwareSpriteEngine::Entry::ComputeScreenBox ( void ) const
{
	Box2D<long>		SpriteBox;
	
	SpriteBox.Build( m_Position, m_Size );

	SpriteBox	-=	m_Hotspot;

	return ( SpriteBox );
}

// ----------------------------------------------------------------------------------------------------

	HardwareSpriteEngine *	HardwareSpriteEngine::m_pInstance		=	NULL;

	
HardwareSpriteEngine::HardwareSpriteEngine ( void ) :	m_SpriteMemorySize( 0 ), m_pSpriteMemory( NULL ),
														m_pDisplayList( NULL )
{
	if ( m_pInstance )
	{
		throw ptc::Error( "Cannot create more than one class of type HardwareSpriteEngine!" );
	}

	m_pInstance		=	this;
}

HardwareSpriteEngine::~HardwareSpriteEngine ( void )
{
	SetVideoMemorySize( 0 );

	if ( m_pInstance == this )
	{
		m_pInstance		=	NULL;
	}
}

HardwareSpriteEngine * HardwareSpriteEngine::GetInstance ( void )
{
	return ( m_pInstance );
}

// ----------------------------------------------------------------------------------------------------

void HardwareSpriteEngine::SetVideoMemorySize ( const unsigned long Size )
{
	m_SpriteMemorySize		=	Size;

	delete m_pSpriteMemory;

	if ( Size )
	{
		m_pSpriteMemory		=	new unsigned char[ Size ];

		memset( m_pSpriteMemory, 0, Size );
	}
}

void HardwareSpriteEngine::SetDisplayInfo ( const Box2D<long> & Box, const long Pitch )
{
	m_DisplayBox	=	Box;
	m_DisplayPitch	=	Pitch;
}

void HardwareSpriteEngine::SetDisplayList ( const Entry * pList )
{
	m_pDisplayList			=	pList;
}

void HardwareSpriteEngine::Render ( unsigned long * pBuffer )
{
	if ( !m_pDisplayList || !m_pSpriteMemory || !pBuffer )
	{
		return;
	}

	const Entry *		pCurrentEntry	=	m_pDisplayList;

	for ( ; pCurrentEntry; pCurrentEntry = pCurrentEntry->m_pNext )
	{
		if ( !pCurrentEntry->m_bVisibility )
		{
			continue;
		}

		Box2D<long>		SpriteBox	=	pCurrentEntry->ComputeScreenBox();

		Box2D<long>::IntersectionResult		Result	=	m_DisplayBox.IsInside( SpriteBox );

		if ( Result == Box2D<long>::Outside )
		{
			continue;
		}

		Box2D<long>		ClippedSpriteBox	=	SpriteBox;
		
		ClippedSpriteBox.Clip( m_DisplayBox );

		Vector2D<long>	SpriteRenderOffset	=	SpriteBox.ComputeDistance( ClippedSpriteBox );

		unsigned long *		pCurrentPixel	=	pBuffer + ClippedSpriteBox.m_TopLeft.m_X + ClippedSpriteBox.m_TopLeft.m_Y * m_DisplayPitch;
		long				SpritePitch		=	m_DisplayPitch - ClippedSpriteBox.GetWidth();

		if ( pCurrentEntry->m_DataAddress == InvalidPosition )
		{
			for ( long Y = ClippedSpriteBox.m_TopLeft.m_Y; Y <= ClippedSpriteBox.m_BottomRight.m_Y; ++Y )
			{
				for ( long X = ClippedSpriteBox.m_TopLeft.m_X; X <= ClippedSpriteBox.m_BottomRight.m_X; ++X )
				{
					*pCurrentPixel++	=	0xff0000;
				}

				pCurrentPixel	+=	SpritePitch;
			}
		}
		else
		{
			switch ( pCurrentEntry->m_ColorFormat )
			{
				case Format_16bit_555:
				{
					unsigned short *	pSourcePixel	=	(unsigned short *) ( m_pSpriteMemory + pCurrentEntry->m_DataAddress ) + SpriteRenderOffset.m_X + SpriteRenderOffset.m_Y * pCurrentEntry->m_Size.m_X;
					long				SourcePitch		=	pCurrentEntry->m_Size.m_X - ClippedSpriteBox.GetWidth();

					for ( long Y = ClippedSpriteBox.m_TopLeft.m_Y; Y <= ClippedSpriteBox.m_BottomRight.m_Y; ++Y )
					{
						for ( long X = ClippedSpriteBox.m_TopLeft.m_X; X <= ClippedSpriteBox.m_BottomRight.m_X; ++X, ++pCurrentPixel, ++pSourcePixel )
						{
							if ( *pSourcePixel != pCurrentEntry->m_TransparentColor )
							{
								*pCurrentPixel	=	  ( ( *pSourcePixel >> 10 ) << 19 ) |
													( ( ( *pSourcePixel >> 5 ) & 0x1f ) << 11 ) |
													  ( ( *pSourcePixel & 0x1f ) << 3 );
							}
						}

						pCurrentPixel	+=	SpritePitch;
						pSourcePixel	+=	SourcePitch;
					}

				} break;

				case Format_32bit_888:
				{
					unsigned long *		pSourcePixel	=	(unsigned long *) ( m_pSpriteMemory + pCurrentEntry->m_DataAddress ) + SpriteRenderOffset.m_X + SpriteRenderOffset.m_Y * pCurrentEntry->m_Size.m_X;
					long				SourcePitch		=	pCurrentEntry->m_Size.m_X - ClippedSpriteBox.GetWidth();

					for ( long Y = ClippedSpriteBox.m_TopLeft.m_Y; Y <= ClippedSpriteBox.m_BottomRight.m_Y; ++Y )
					{
						for ( long X = ClippedSpriteBox.m_TopLeft.m_X; X <= ClippedSpriteBox.m_BottomRight.m_X; ++X, ++pCurrentPixel, ++pSourcePixel )
						{
							if ( *pSourcePixel != pCurrentEntry->m_TransparentColor )
							{
								*pCurrentPixel	=	 *pSourcePixel;
							}
						}

						pCurrentPixel	+=	SpritePitch;
						pSourcePixel	+=	SourcePitch;
					}

				} break;

				default:
				{
					for ( long Y = ClippedSpriteBox.m_TopLeft.m_Y; Y <= ClippedSpriteBox.m_BottomRight.m_Y; ++Y )
					{
						for ( long X = ClippedSpriteBox.m_TopLeft.m_X; X <= ClippedSpriteBox.m_BottomRight.m_X; ++X )
						{
							*pCurrentPixel++	=	0xffffff;
						}

						pCurrentPixel	+=	SpritePitch;
					}
				}
			}
		}
	};
}

const Box2D<long> & HardwareSpriteEngine::GetDisplayBox ( void ) const
{
	return ( m_DisplayBox );
}

unsigned long HardwareSpriteEngine::GetMemorySize ( void ) const
{
	return ( m_SpriteMemorySize );
}

void HardwareSpriteEngine::UploadSurface ( const unsigned long Destination, const unsigned long SurfaceSize, const unsigned long * pSurfaceData )
{
	memcpy( m_pSpriteMemory + Destination, pSurfaceData, SurfaceSize );
}

unsigned char * HardwareSpriteEngine::GetVideoMemory ( void ) const
{
	return ( m_pSpriteMemory );
}

// ----------------------------------------------------------------------------------------------------
