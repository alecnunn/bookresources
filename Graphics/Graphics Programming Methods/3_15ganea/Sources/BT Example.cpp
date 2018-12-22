
// ----------------------------------------------------------------------------------------------------
//
//						'An optimal dynamic surface caching algorithm'
//
//						   by Alexandre Ganea (aganea@dc-studios.com)
//
// ----------------------------------------------------------------------------------------------------
//										Main Program
// ----------------------------------------------------------------------------------------------------
//
//	This simple application is an example of a surface cache management scheme. It also describes a way
//	of communicating between the Surface Manager and the Engine.
//	For simplicity, this example is uses sprites as display primitives. The class HardwareSpriteEngine
//	emulates the inner processings of a typical sprite-based hardware system. This class is only a
//	illustration of the algorithm in a real-life context, and could be completely replaced by anything
//	else (any real hardware, a 3d-based hardware, ...).
//	
//	The most important part of the application is the SurfaceManager class, which does exactly what
//	Figure.1 describes. The functions AllocateSurface and FreeSurface are the core of this class, they
//	provide the caching mechanism iteself.
//
//	Two other template classes are provided for a better code readability: the Vector2D and the Box2D.
//
//
//
//	Now the display: the main window shows a few sprites moving around the screen, each one governed by
//	a different behaviour. The smallest window offers a view of the Video Memory activity in real-time.
//	Black color is for unused space, any other color is choosen in a semi-random manner, based on the
//	surface's order.
//
//	Once a sprite goes offscreen, its associated video memory is freed. Every time it enters the screen
//	again, the sprite is being assigned a new memory location.
//
// ----------------------------------------------------------------------------------------------------
//	Requirements for compiling and running the application:
//		-	Preferably the latest build of the Platform SDK, available on www.microsoft.com
//		-	OpenPTC library, included in the package.
//		-	A video card with support for 32-bit display buffers is also preferable.
//
// ----------------------------------------------------------------------------------------------------
//	OpenPTC for Windows is a free graphic library available under the GNU Licence.
//	OpenPTC is Copyright © Glenn "Gaffer" Fiedler 1998-1999.
//	For the latest releases and information visit: http://www.gaffer.org/ptc/distributions/Windows
// ----------------------------------------------------------------------------------------------------

#include "windows.h"
#include "ptc.h"
#include <stdlib.h>
#include "math.h"
#include "Sprite Engine.hpp"
#include "VRAM Manager.hpp"

// ----------------------------------------------------------------------------------------------------

#define		Degrees2Radians		3.14159465256f / 180.0f

	enum
	{
		VideoMemoryMemory	=	262144,		// This is the maximum video memory available for the sprites

		SurfacesCount		=	5,
		SpritesCount		=	6,
		ActionsCount		=	5,
	};

// ----------------------------------------------------------------------------------------------------

int APIENTRY WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	try
	{
		Vector2D<long>		DisplaySize( 640, 480 );

		Console				ExampleWindow;
		
		Format				DisplayFormat( 32,0x00FF0000,0x0000FF00,0x000000FF );
		
		Surface				ExampleBackBuffer( DisplaySize.m_X, DisplaySize.m_Y, DisplayFormat );
		
		ExampleWindow.option( "windowed output" );
		ExampleWindow.option( "fixed window" );

		ExampleWindow.open( "Binary tree example", DisplaySize.m_X, DisplaySize.m_Y, DisplayFormat );

		// --------------------------------------------------------------------------------------------
		// Initialize the hardware emulation
		// --------------------------------------------------------------------------------------------

		HardwareSpriteEngine				SpriteEngine;

		SpriteEngine.SetDisplayInfo( Box2D<long>( 0, 0, DisplaySize.m_X - 1, DisplaySize.m_Y - 1 ), DisplaySize.m_X );

		SpriteEngine.SetVideoMemorySize( VideoMemoryMemory );

		// --------------------------------------------------------------------------------------------
		// --------------------------------------------------------------------------------------------

		using namespace	Example;

		const char *			pFiles[ SurfacesCount ]	=
		{
			"Artwork\\Fish.bmp",
			"Artwork\\Leaf.bmp",
			"Artwork\\Man.bmp",
			"Artwork\\Snowflake.bmp",
			"Artwork\\Plane.bmp",
		};

		SurfaceManager		Manager;

		Manager.CreateSurfaceArray( SurfacesCount, pFiles );

		// --------------------------------------------------------------------------------------------
		// Create the sprite display list
		// --------------------------------------------------------------------------------------------

		Sprite				mySprite[ SpritesCount ];
		
		const Vector2D<long>	Position[ SpritesCount ]	=		// Initial sprite positions
		{
			Vector2D<long>( 40, 120 ),
			Vector2D<long>( 10, 50 ),
			Vector2D<long>( 200, 300 ),
			Vector2D<long>( 140, 400 ),
			Vector2D<long>( 600, 180 ),
			Vector2D<long>( 400, 220 ),
		};

		for ( long Counter = 0; Counter < SpritesCount; ++Counter )
		{
			mySprite[ Counter ].SetSurface( Counter % SurfacesCount );

			mySprite[ Counter ].GetEntry().m_Position		=	Position[ Counter ];
			
			mySprite[ Counter ].GetEntry().m_Hotspot		=	mySprite[ Counter ].GetEntry().m_Size / 2;

			if ( Counter )
			{
				mySprite[ Counter ].GetEntry().Chain( &mySprite[ Counter - 1 ].GetEntry() );
			}
		}

		mySprite[ 4 ].GetEntry().m_Hotspot		=	mySprite[ 4 ].GetEntry().m_Size;

		Manager.Initialize();		// this should be called only once all surfaces have been loaded

		// Note that the display list can also be changed on the fly, in the main loop
		
		SpriteEngine.SetDisplayList( &mySprite[ 0 ].GetEntry() );

		// --------------------------------------------------------------------------------------------
		// Build Memory Display Window
		// --------------------------------------------------------------------------------------------

		enum
		{
			HorizontalNodes		=	32,
			BlockSize			=	16,
		};

		Vector2D<long>		MemoryDisplaySize( HorizontalNodes * BlockSize, ( Manager.GetTotalTreeNodes() / HorizontalNodes ) * BlockSize );

		if ( !MemoryDisplaySize.m_Y )
		{
			MemoryDisplaySize.m_Y	=	BlockSize;
		}

		Console				MemoryWindow;
		ptc::Surface		MemoryBackBuffer( MemoryDisplaySize.m_X, MemoryDisplaySize.m_Y, DisplayFormat );

		MemoryWindow.option( "windowed output" );
		MemoryWindow.option( "fixed window" );

		MemoryWindow.open( "VRAM Display", MemoryDisplaySize.m_X, MemoryDisplaySize.m_Y, DisplayFormat );

		// --------------------------------------------------------------------------------------------

		long	Sprite1Counter	=	0;
		long	Sprite11Counter	=	50;
		long	Sprite2Counter	=	0;
		long	Sprite3Counter	=	0;

		// --------------------------------------------------------------------------------------------
		//											Main loop
		// --------------------------------------------------------------------------------------------

		while ( !ExampleWindow.key() )
		{
			// ----------------------------------------------------------------------------------------
			// Process sprites
			// ----------------------------------------------------------------------------------------

			for ( long Counter = 0; Counter < SpritesCount; ++Counter )
			{
				// ---------- Apply some behaviour to each sprite

				switch ( Counter % ActionsCount )
				{
					case 0:
					{
						++mySprite[ Counter ].GetEntry().m_Position.m_X;
						
						if ( mySprite[ Counter ].GetEntry().m_Position.m_X > DisplaySize.m_X + 64 )
						{
							mySprite[ Counter ].GetEntry().m_Position.m_X	=	-64;
						}

					} break;

					case 1:
					{
						mySprite[ Counter ].GetEntry().m_Position.m_X	=	( DisplaySize.m_X / 2 ) + (long) ( sin( Sprite1Counter * 3 * Degrees2Radians ) * Sprite11Counter );
						mySprite[ Counter ].GetEntry().m_Position.m_Y	=	( DisplaySize.m_Y / 2 ) + (long) ( cos( Sprite1Counter * Degrees2Radians ) * Sprite11Counter );

					} break;

					case 2:
					{
						mySprite[ Counter ].GetEntry().m_Position.m_X	=	( DisplaySize.m_X / 2 ) + (long) ( sin( Sprite1Counter * Degrees2Radians ) * 400 );

					} break;

					case 3:
					{
						mySprite[ Counter ].GetEntry().m_Position.m_X	=	30 +						(long) ( sin( Sprite1Counter * 2 * Degrees2Radians ) * 100 );
						mySprite[ Counter ].GetEntry().m_Position.m_Y	=	( DisplaySize.m_Y - 30 ) +	(long) ( cos( Sprite1Counter * 2 * Degrees2Radians ) * 100 );

					} break;

					case 4:
					{
						mySprite[ Counter ].GetEntry().m_Position.m_Y		-=	2;
						mySprite[ Counter ].GetEntry().m_Position.m_X		-=	1;

						if ( mySprite[ Counter ].GetEntry().m_Position.m_X < -64 )
						{
							mySprite[ Counter ].GetEntry().m_Position.m_X		=	DisplaySize.m_X + 64;
						}

						if ( mySprite[ Counter ].GetEntry().m_Position.m_Y < -64 )
						{
							mySprite[ Counter ].GetEntry().m_Position.m_Y		=	DisplaySize.m_Y + 64;
						}

					} break;
				}
				
				// ----------- Update sprite data, it is here were the sprite are actually
				// loaded or unloaded to/from the video ram.

				mySprite[ Counter ].Stream();
			}
			

			++Sprite1Counter;
			
			Sprite11Counter	=	200 + (long) ( sin( ( Sprite1Counter / 2 ) * Degrees2Radians ) * 150 );

			// ----------------------------------------------------------------------------------------
			// Display sprites (this step is usually done in hardware)
			// ----------------------------------------------------------------------------------------

			SpriteEngine.Render( (unsigned long *) ExampleBackBuffer.lock() );

			ExampleBackBuffer.unlock();

			ExampleBackBuffer.copy( ExampleWindow );
			ExampleBackBuffer.clear();

			ExampleWindow.update();

			// ----------------------------------------------------------------------------------------
			// Display video memory
			// ----------------------------------------------------------------------------------------

			Manager.RenderMemory( (unsigned long *) MemoryBackBuffer.lock(), MemoryDisplaySize, HorizontalNodes, BlockSize );

			MemoryBackBuffer.unlock();

			MemoryBackBuffer.copy( MemoryWindow );
			MemoryBackBuffer.clear();

			MemoryWindow.update();

			// ----------------------------------------------------------------------------------------
		}
	}
	catch ( Error & error )
	{
		error.report();
	}

	return ( 0 );
}

// ----------------------------------------------------------------------------------------------------
