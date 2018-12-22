
// ----------------------------------------------------------------------------------------------------
//
//						'An optimal dynamic surface caching algorithm'
//
//						   by Alexandre Ganea (aganea@dc-studios.com)
//
// ----------------------------------------------------------------------------------------------------
//						  The Surface (Video) Memory Management class
// ----------------------------------------------------------------------------------------------------

#include "VRAM Manager.hpp"

// ----------------------------------------------------------------------------------------------------

using namespace Example;

// ----------------------------------------------------------------------------------------------------

Surface::Surface( void ) :	m_pData( NULL ), m_pFilename( NULL ),
							m_PositionInVRAM( HardwareSpriteEngine::InvalidPosition ),
							m_ReferenceCount( 0 ), m_Order( 0 )
{
}

Surface::~Surface( void )
{
	if ( m_pData )
	{
		free( m_pData );	m_pData	=	NULL;
	}

	if ( m_pFilename )
	{
		free( m_pFilename );
	}
}

void Surface::Load ( const char * pFilename )
{
	HANDLE	hImage	=	LoadImage( NULL, pFilename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE );

	BITMAP	BitmapInfo;

	GetObject( hImage, sizeof( BITMAP ), &BitmapInfo );

	if ( m_pData )
	{
		free( m_pData );
	}

	m_pData		=	(unsigned long *) malloc( BitmapInfo.bmWidthBytes * BitmapInfo.bmHeight );

	// Copy (and flipY) the image into the sprite buffer

	for ( long LineCounter = BitmapInfo.bmHeight - 1; LineCounter >= 0; --LineCounter )
	{
		memcpy( (unsigned char *)	m_pData		+	LineCounter * BitmapInfo.bmWidthBytes,
				(unsigned char *)	BitmapInfo.bmBits	+	( BitmapInfo.bmHeight - LineCounter - 1 ) * BitmapInfo.bmWidthBytes,
									BitmapInfo.bmWidthBytes );
	}

	m_Size.Set( BitmapInfo.bmWidth, BitmapInfo.bmHeight );

	switch ( BitmapInfo.bmBitsPixel )
	{
		case 32:	m_ColorFormat	=	HardwareSpriteEngine::Format_32bit_888;	break;
		case 16:	m_ColorFormat	=	HardwareSpriteEngine::Format_16bit_555;	break;
		default:	m_ColorFormat	=	HardwareSpriteEngine::Format_Unknown;
	}

	// Hardcode transparent color for now

	switch ( m_ColorFormat )
	{
		case HardwareSpriteEngine::Format_32bit_888:	m_TransparentColor	=	0xff00ff;	break;
		case HardwareSpriteEngine::Format_16bit_555:	m_TransparentColor	=	0x7c1f;		break;
	}

	DeleteObject( hImage );

	if ( m_pFilename )
	{
		free( m_pFilename );
	}

	m_pFilename	=	(char *) malloc( strlen( pFilename ) + 1 );

	strcpy( m_pFilename, pFilename );
}

char * Surface::GetFilename ( void ) const
{
	return ( m_pFilename );
}

const Vector2D<long> & Surface::GetSize ( void ) const
{
	return ( m_Size );
}

HardwareSpriteEngine::ColorFormat Surface::GetColorFormat ( void ) const
{
	return ( m_ColorFormat );
}

unsigned long Surface::GetTransparentColor ( void ) const
{
	return ( m_TransparentColor );
}

unsigned long * Surface::GetData ( void ) const
{
	return ( m_pData );
}

void Surface::SetPositionInVRAM ( const unsigned long Position )
{
	m_PositionInVRAM	=	Position;
}

unsigned long Surface::GetPositionInVRAM ( void ) const
{
	return ( m_PositionInVRAM );
}

unsigned long Surface::Reference ( const bool bDereference )
{
	if ( !bDereference )
	{
		--m_ReferenceCount;
	}
	else
	{
		++m_ReferenceCount;
	}

	return ( m_ReferenceCount );
}

void Surface::SetOrder ( const unsigned long Order )
{
	m_Order		=	Order;
}

unsigned long Surface::GetOrder ( void ) const
{
	return ( m_Order );
}

unsigned long Surface::GetTotalSize ( void ) const
{
	unsigned long	SurfaceSize		=	m_Size.m_X * m_Size.m_Y;

	switch ( m_ColorFormat )
	{
		case HardwareSpriteEngine::Format_16bit_555:		SurfaceSize		*=	2;	break;
		case HardwareSpriteEngine::Format_32bit_888:
		case HardwareSpriteEngine::Format_32bit_888_Alpha:	SurfaceSize		*=	4;	break;
		default:											SurfaceSize		=	0;	break;
	}

	return ( SurfaceSize );
}

// ----------------------------------------------------------------------------------------------------

Sprite::Sprite ( void ) : m_pImage( NULL ), m_bIsUploaded( false )
{
}

Sprite::~Sprite ( void )
{
	if ( m_bIsUploaded )
	{
		SurfaceManager::GetInstance()->FreeSurface( m_pImage );

		m_bIsUploaded	=	false;
	}
}

void Sprite::SetSurface ( const unsigned long ID )
{
	if ( m_bIsUploaded )
	{
		SurfaceManager::GetInstance()->FreeSurface( m_pImage );

		m_bIsUploaded	=	false;
	}
	
	m_pImage	=	SurfaceManager::GetInstance()->RetrieveSurface( ID );

	m_HardwareSprite.m_Size					=	m_pImage->GetSize();
	m_HardwareSprite.m_ColorFormat			=	m_pImage->GetColorFormat();
	m_HardwareSprite.m_TransparentColor		=	m_pImage->GetTransparentColor();
}

void Sprite::SetSurface ( const char * pName )
{
	if ( m_bIsUploaded )
	{
		SurfaceManager::GetInstance()->FreeSurface( m_pImage );

		m_bIsUploaded	=	false;
	}
	
	m_pImage	=	SurfaceManager::GetInstance()->RetrieveSurface( pName );

	m_HardwareSprite.m_Size					=	m_pImage->GetSize();
	m_HardwareSprite.m_ColorFormat			=	m_pImage->GetColorFormat();
	m_HardwareSprite.m_TransparentColor		=	m_pImage->GetTransparentColor();
}

HardwareSpriteEngine::Entry & Sprite::GetEntry ( void )
{
	return ( m_HardwareSprite );
}

void Sprite::Stream ( void )
{
	Box2D<long>		SpriteBox	=	m_HardwareSprite.ComputeScreenBox();

	Box2D<long>::IntersectionResult		Result	=	HardwareSpriteEngine::GetInstance()->GetDisplayBox().IsInside( SpriteBox );

	if ( Result == Box2D<long>::Outside )
	{
		if ( m_bIsUploaded )
		{
			SurfaceManager::GetInstance()->FreeSurface( m_pImage );

			m_bIsUploaded	=	false;
		}

		return;
	}
	else
	{
		if ( !m_bIsUploaded )
		{
			SurfaceManager::GetInstance()->AllocateSurface( m_pImage );

			m_bIsUploaded	=	true;

			m_HardwareSprite.m_DataAddress		=	m_pImage->GetPositionInVRAM();
		}
	}
}

// ----------------------------------------------------------------------------------------------------

	SurfaceManager *	SurfaceManager::m_pInstance		=	NULL;

	
SurfaceManager::SurfaceManager ( void ) :	m_pSurface( NULL ), m_SurfacesCount( 0 ),
											m_pBinaryTree( NULL ), m_MaximumSurfaceOrder( 0 ),
											m_SmallestSurfaceSize( 0 )
{
	if ( m_pInstance )
	{
		throw ptc::Error( "Cannot create more than one class of type SurfaceManager!" );
	}

	m_pInstance		=	this;
}

SurfaceManager::~SurfaceManager ( void )
{
	if ( m_pInstance == this )
	{
		m_pInstance		=	NULL;
	}
}

SurfaceManager * SurfaceManager::GetInstance ( void )
{
	return ( m_pInstance );
}

// ----------------------------------------------------------------------------------------------------

void SurfaceManager::CreateSurfaceArray ( const unsigned long Count, const char ** pFiles )
{
	delete[] m_pSurface;

	if ( !Count )
	{
		m_pSurface			=	NULL;
		m_SurfacesCount		=	0;

		return;
	}

	m_pSurface			=	new Surface[ Count ];
	m_SurfacesCount		=	Count;

	for ( unsigned long SurfaceCounter = 0; SurfaceCounter < Count; ++SurfaceCounter, ++pFiles )
	{
		m_pSurface[ SurfaceCounter ].Load( *pFiles );
	}
}

Example::Surface * SurfaceManager::RetrieveSurface ( const unsigned long ID )
{
	if ( ID >= m_SurfacesCount || !m_pSurface )
	{
		return ( NULL );
	}

	return ( m_pSurface + ID );
}

Example::Surface * SurfaceManager::RetrieveSurface ( const char * pName )
{
	for ( unsigned long SurfaceCounter = 0; SurfaceCounter < m_SurfacesCount; ++SurfaceCounter )
	{
		if ( strstr( m_pSurface[ SurfaceCounter ].GetFilename(), pName ) )
		{
			return ( m_pSurface + SurfaceCounter );
		}
	}

	return ( NULL );
}

// ----------------------------------------------------------------------------------------------------

void SurfaceManager::Initialize ( void )
{
	unsigned long	CurrentSurfaceSize;
	unsigned long	Counter;

	m_SmallestSurfaceSize		=	( 1 << 30 );

	// Find the smallest surface type

	for ( Counter = 0; Counter < m_SurfacesCount; ++Counter )
	{
		CurrentSurfaceSize	=	m_pSurface[ Counter ].GetTotalSize();

		if ( CurrentSurfaceSize < m_SmallestSurfaceSize )
		{
			m_SmallestSurfaceSize		=	CurrentSurfaceSize;
		}
	}

	// Calculate the corresponding order for each surface

	for ( Counter = 0; Counter < m_SurfacesCount; ++Counter )
	{
		CurrentSurfaceSize	=	m_pSurface[ Counter ].GetTotalSize() / m_SmallestSurfaceSize;

		unsigned long	Order	=	1;

		while ( CurrentSurfaceSize >>= 1 )
		{
			++Order;
		}

		m_pSurface[ Counter ].SetOrder( Order );
	}

	// Get maximum order

	unsigned long	TreeNodes		=	( HardwareSpriteEngine::GetInstance()->GetMemorySize() / m_SmallestSurfaceSize );
	
	m_MaximumSurfaceOrder	=	1;

	CurrentSurfaceSize	=	TreeNodes;

	while ( CurrentSurfaceSize >>= 1 )
	{
		++m_MaximumSurfaceOrder;
	}

	// Build and initialize tree

	m_pBinaryTree	=	new BinaryNode[ TreeNodes ];

	m_pBinaryTree->RightBranchAvailableSpace	=	m_MaximumSurfaceOrder;
	m_pBinaryTree->LeftBranchAvailableSpace		=	0;
	m_pBinaryTree->bRightBranchIsLinear			=	true;
	m_pBinaryTree->bLeftBranchIsLinear			=	false;

	BinaryNode *	pCurrentNode	=	m_pBinaryTree + 1;

	for ( unsigned long Level = 1; Level < m_MaximumSurfaceOrder; ++Level )
	{
		unsigned long	FreeSpace	=	m_MaximumSurfaceOrder - Level;

		for ( Counter = 0; Counter < (unsigned) ( 1 << ( Level - 1 ) ); ++Counter, ++pCurrentNode )
		{
			pCurrentNode->LeftBranchAvailableSpace		=	FreeSpace;
			pCurrentNode->RightBranchAvailableSpace		=	FreeSpace;

			pCurrentNode->bLeftBranchIsLinear			=	true;
			pCurrentNode->bRightBranchIsLinear			=	true;
		}
	}
}

void SurfaceManager::AllocateSurface ( Example::Surface * pSurface )
{
	if ( pSurface->Reference() == 1 )
	{
		unsigned long	Order				=	pSurface->GetOrder();

		if ( m_pBinaryTree->RightBranchAvailableSpace < Order )
		{
			// Not enough free space to store this surface

			pSurface->SetPositionInVRAM( HardwareSpriteEngine::InvalidPosition );

			return;
		}

		// ------------------------- Finds the best node in VRAM

		unsigned long	NextNodeIndex	=	1;
		BinaryNode *	pCurrentNode;
		unsigned long	Level;

		for ( Level = m_MaximumSurfaceOrder; Level > Order; --Level )
		{
			pCurrentNode	=	m_pBinaryTree + NextNodeIndex;

			if ( pCurrentNode->LeftBranchAvailableSpace < Order )
			{
				NextNodeIndex	=	( NextNodeIndex << 1 ) + 1;	// take right side
			}
			else if ( pCurrentNode->RightBranchAvailableSpace < Order )
			{
				NextNodeIndex	=	( NextNodeIndex << 1 );		// take left side
			}
			else
			{
				if ( pCurrentNode->LeftBranchAvailableSpace <= pCurrentNode->RightBranchAvailableSpace )
				{
					NextNodeIndex	=	( NextNodeIndex << 1 );	// take left side
				}
				else
				{
					NextNodeIndex	=	( NextNodeIndex << 1 ) + 1;	// take right side
				}
			}
		}

		// --------------------- Find the position in VRAM and upload surface data

		unsigned long	Position	=	( ( NextNodeIndex - ( 1 << ( m_MaximumSurfaceOrder - Level ) ) ) << ( Order - 1 ) ) * m_SmallestSurfaceSize;

		HardwareSpriteEngine::GetInstance()->UploadSurface( Position, pSurface->GetTotalSize(), pSurface->GetData() );
		
		pSurface->SetPositionInVRAM( Position );

		// -------------------------------------- Update the parents backwards

		UpdateParents( NextNodeIndex, Order, false );
	}
}

void SurfaceManager::FreeSurface ( Example::Surface * pSurface )
{
	if ( !pSurface->Reference( false ) )
	{
		unsigned long	Order				=	pSurface->GetOrder();
		unsigned long	Position			=	pSurface->GetPositionInVRAM();

		if ( Position == HardwareSpriteEngine::InvalidPosition )
		{
			return;
		}

		// ------------------------------- Gets the node index from the position

		Position	/=	m_SmallestSurfaceSize;

		unsigned long	 Index	=	( 1 << ( m_MaximumSurfaceOrder - Order ) ) + ( Position >> ( Order - 1 ) );

		// -------------------------------------- Update the parents backwards

		UpdateParents( Index, Order, true );

		// --------------------------------------- Reset the surface position

		pSurface->SetPositionInVRAM( HardwareSpriteEngine::InvalidPosition );
	}
}

void SurfaceManager::UpdateParents ( unsigned long PreviousNode, unsigned long Level, const bool bRestore )
{
	BinaryNode *	pCurrentNode;
	unsigned long	FreeSpace		=	bRestore ? Level : 0;
	bool			bIsLinear		=	bRestore ? true : false;

	for ( ; Level <= m_MaximumSurfaceOrder; ++Level )
	{
		pCurrentNode	=	m_pBinaryTree + ( PreviousNode >> 1 );

		if ( PreviousNode & 1 )	// node was on right side
		{
			pCurrentNode->RightBranchAvailableSpace		=	FreeSpace;
			pCurrentNode->bRightBranchIsLinear			=	bIsLinear;
		}
		else						// node was on left side
		{
			pCurrentNode->LeftBranchAvailableSpace		=	FreeSpace;
			pCurrentNode->bLeftBranchIsLinear			=	bIsLinear;
		}
		
		PreviousNode	>>=		1;

		if ( pCurrentNode->LeftBranchAvailableSpace == pCurrentNode->RightBranchAvailableSpace )
		{
			if ( pCurrentNode->bLeftBranchIsLinear && pCurrentNode->bRightBranchIsLinear )
			{
				++FreeSpace;
			}
		}
		else
		{
			FreeSpace	=	max( pCurrentNode->LeftBranchAvailableSpace, pCurrentNode->RightBranchAvailableSpace );
		}

		if ( pCurrentNode->bLeftBranchIsLinear && pCurrentNode->bRightBranchIsLinear )
		{
			bIsLinear		=	true;
		}
		else
		{
			bIsLinear		=	false;
		}
	}
}

unsigned long SurfaceManager::GetTotalTreeNodes ( void ) const
{
	return ( HardwareSpriteEngine::GetInstance()->GetMemorySize() / m_SmallestSurfaceSize );
}

void SurfaceManager::RenderMemory ( unsigned long * pBuffer, const Vector2D<long> & DisplaySize, const unsigned long NodesPerRow, const unsigned long NodeSize )
{
	memset( pBuffer, 0xff, DisplaySize.m_Y * 4 );

	for ( unsigned long Counter = 0; Counter < m_SurfacesCount; ++Counter )
	{
		if ( m_pSurface[ Counter ].GetPositionInVRAM() == HardwareSpriteEngine::InvalidPosition )
		{
			continue;
		}

		unsigned long	BlocksToFill	=	m_pSurface[ Counter ].GetTotalSize() / m_SmallestSurfaceSize;
		unsigned long	CurrentBlock	=	m_pSurface[ Counter ].GetPositionInVRAM() / m_SmallestSurfaceSize;

		while ( BlocksToFill )
		{
			unsigned long	BlockY	=	( CurrentBlock / NodesPerRow ) * NodeSize;
			unsigned long	BlockX	=	( CurrentBlock % NodesPerRow ) * NodeSize;
			unsigned long	Color	=	GetColorFromOrder( m_pSurface[ Counter ].GetOrder() );

			for ( unsigned long BlockCounter = 0; BlockCounter < NodeSize; ++BlockCounter, ++BlockY )
			{
				for ( unsigned long LineCounter = 0; LineCounter < NodeSize; ++LineCounter )
				{
					pBuffer[ BlockX + LineCounter + BlockY * DisplaySize.m_X ]	=	Color;
				}
			}

			--BlocksToFill;
			++CurrentBlock;
		}
	}
}

// ----------------------------------------------------------------------------------------------------
//					This function is an adapted version from the source code of:
//
//							'A Fast HSL-to-RGB Transform' by Ken Fishkin
//								"Graphics Gems", Academic Press, 1990
// ----------------------------------------------------------------------------------------------------

unsigned long SurfaceManager::GetColorFromOrder ( const unsigned long Order )
{
	double	Hue			=	Order / (double) m_MaximumSurfaceOrder;
	double	Saturation	=	1.0f;
	double	Light		=	0.5f;

    double	Value;

    Value	=	( Light <= 0.5 ) ? ( Light * ( 1.0 + Saturation ) ) : ( Light + Saturation - Light * Saturation );

    if ( Value <= 0 )
	{
		return ( 0 );
    }
	else
	{
		double	M;
		double	SV;
		int		Sextant;
		double	FractionalPart, Vsf, Middle1, Middle2;

		M			=	Light + Light - Value;
		SV			=	( Value - M ) / Value;
		Hue			*=	6.0;
		
		Sextant			=	(int) Hue;
		FractionalPart	=	Hue - Sextant;
		
		Vsf			=	Value * SV * FractionalPart;
		Middle1		=	M + Vsf;
		Middle2		=	Value - Vsf;
		
		double	Red, Green, Blue;

		switch ( Sextant )
		{
			case 0:		Red	= Value;	Green = Middle1;	Blue = M;		break;
			case 1:		Red	= Middle2;	Green = Value;		Blue = M;		break;
			case 2:		Red	= M;		Green = Value;		Blue = Middle1; break;
			case 3:		Red	= M;		Green = Middle2;	Blue = Value;	break;
			case 4:		Red	= Middle1;	Green = M;			Blue = Value;	break;
			case 5:		Red	= Value;	Green = M;			Blue = Middle2; break;
		}

		return ( ( (long)( Red * 255 ) ) | ( (long)( Green * 255 ) << 8 ) | ( (long)( Blue * 255 ) << 16 ) );
    }
}

// ----------------------------------------------------------------------------------------------------
