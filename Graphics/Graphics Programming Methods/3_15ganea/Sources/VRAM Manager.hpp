
// ----------------------------------------------------------------------------------------------------
//
//						'An optimal dynamic surface caching algorithm'
//
//						   by Alexandre Ganea (aganea@dc-studios.com)
//
// ----------------------------------------------------------------------------------------------------
//						  The Surface (Video) Memory Management class
// ----------------------------------------------------------------------------------------------------

#pragma once

#include "Helpers.hpp"
#include "Sprite Engine.hpp"

// ----------------------------------------------------------------------------------------------------

namespace Example
{
	// ----------------------------------------------------------------------------------------

	struct Surface
	{
	public:

													Surface					( void );
													~Surface				( void );

			void									Load					( const char * pFilename );
			char *									GetFilename				( void ) const;
			const Vector2D<long> &					GetSize					( void ) const;
			HardwareSpriteEngine::ColorFormat		GetColorFormat			( void ) const;
			unsigned long							GetTransparentColor		( void ) const;
			unsigned long *							GetData					( void ) const;

			void									SetPositionInVRAM		( const unsigned long Position );
			unsigned long							GetPositionInVRAM		( void ) const;

			unsigned long							Reference				( const bool bReference = true );

			void									SetOrder				( const unsigned long Order );
			unsigned long							GetOrder				( void ) const;

			unsigned long							GetTotalSize			( void ) const;

			// --------------------------------------------------------------------------------

	private:

			Vector2D<long>							m_Size;
			HardwareSpriteEngine::ColorFormat		m_ColorFormat;
			unsigned long							m_TransparentColor;
			unsigned long							m_PositionInVRAM;
			unsigned long							m_ReferenceCount;
			unsigned long							m_Order;

			unsigned long *							m_pData;
			char *									m_pFilename;
	};

	// ----------------------------------------------------------------------------------------

	struct Sprite
	{
	public:
											Sprite			( void );
											~Sprite			( void );

			void							SetSurface		( const unsigned long ID );
			void							SetSurface		( const char * pName );

			HardwareSpriteEngine::Entry &	GetEntry		( void );

			void							Stream			( void );

			// --------------------------------------------------------------------------------
	private:

			Surface	*						m_pImage;
			HardwareSpriteEngine::Entry 	m_HardwareSprite;
			bool							m_bIsUploaded;
	};

	// ----------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------

	struct SurfaceManager
	{
		struct BinaryNode
		{
			unsigned short		LeftBranchAvailableSpace : 7;
			unsigned short		RightBranchAvailableSpace : 7;
			unsigned short		bLeftBranchIsLinear : 1;
			unsigned short		bRightBranchIsLinear : 1;
		};

			// --------------------------------------------------------------------------------

		public:
									SurfaceManager			( void );
									~SurfaceManager			( void );

		static	SurfaceManager *	GetInstance				( void );

				void				CreateSurfaceArray		( const unsigned long Count, const char ** pFiles );
				Surface *			RetrieveSurface			( const unsigned long ID );
				Surface *			RetrieveSurface			( const char * pName );

				void				Initialize				( void );

				unsigned long		GetTotalTreeNodes		( void ) const;

				void				AllocateSurface			( Surface * pSurface );
				void				FreeSurface				( Surface * pSurface );

				void				RenderMemory			( unsigned long * pBuffer, const Vector2D<long> & DisplaySize, const unsigned long NodesPerRow, const unsigned long NodeSize );

	private:
				void				UpdateParents			( unsigned long PreviousNode, unsigned long Level, const bool bRestore );

				unsigned long		GetColorFromOrder		( const unsigned long Order );

			// --------------------------------------------------------------------------------
	private:

		static	SurfaceManager *	m_pInstance;

				Surface *			m_pSurface;
				unsigned long		m_SurfacesCount;

				BinaryNode *		m_pBinaryTree;
				unsigned long		m_MaximumSurfaceOrder;
				unsigned long		m_SmallestSurfaceSize;
	};
}

// ----------------------------------------------------------------------------------------------------
