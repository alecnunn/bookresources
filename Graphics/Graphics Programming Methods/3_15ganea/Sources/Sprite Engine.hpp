
// ----------------------------------------------------------------------------------------------------
//
//						'An optimal dynamic surface caching algorithm'
//
//						   by Alexandre Ganea (aganea@dc-studios.com)
//
// ----------------------------------------------------------------------------------------------------
//								The sprite-based hardware emulation
// ----------------------------------------------------------------------------------------------------

#pragma once

#include "Helpers.hpp"

// ----------------------------------------------------------------------------------------------------

class HardwareSpriteEngine
{
public:
	
	enum ColorFormat
	{
		Format_Unknown,
		Format_16bit_555,
		Format_32bit_888,
		Format_32bit_888_Alpha,
	};

	enum
	{
		InvalidPosition		=	(unsigned long) -1
	};

	// ----------------------------------------------------------------------------------------

	struct Entry
	{
						Entry					( void );
						~Entry					( void );

		void			Chain					( Entry * pPreviousObject );
		Box2D<long>		ComputeScreenBox		( void ) const;

		// ------------------------------------------------------------------------------------

		Vector2D<long>			m_Position;
		Vector2D<long>			m_Size;
		Vector2D<long>			m_Hotspot;
		unsigned long			m_DataAddress;
		ColorFormat				m_ColorFormat;
		unsigned long			m_TransparentColor;

		unsigned long			m_bVisibility : 1;

	private:

		friend class HardwareSpriteEngine;

		Entry *					m_pPrevious;
		Entry *					m_pNext;
	};

	// ----------------------------------------------------------------------------------------

										HardwareSpriteEngine	( void );
										~HardwareSpriteEngine	( void );

		static	HardwareSpriteEngine *	GetInstance				( void );

				void					SetVideoMemorySize		( const unsigned long Size );
				void					SetDisplayInfo			( const Box2D<long> & Box, const long Pitch );

				void					SetDisplayList			( const Entry * pList );

				void					Render					( unsigned long * pBuffer );

				const Box2D<long> &		GetDisplayBox			( void ) const;
				unsigned long			GetMemorySize			( void ) const;
				unsigned char *			GetVideoMemory			( void ) const;

				void					UploadSurface			( const unsigned long Destination, const unsigned long SurfaceSize, const unsigned long * pSurfaceData );

	// ----------------------------------------------------------------------------------------
				
private:

		static	HardwareSpriteEngine *	m_pInstance;

				Box2D<long>				m_DisplayBox;
				long					m_DisplayPitch;

				unsigned long			m_SpriteMemorySize;
				unsigned char *			m_pSpriteMemory;

		const	Entry *					m_pDisplayList;
};

// ----------------------------------------------------------------------------------------------------
