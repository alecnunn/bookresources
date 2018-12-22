
#ifndef IMAGE_INCLUDED
#define IMAGE_INCLUDED

#pragma once


#include "TomsLib.h"
#include "TomsD3DLib.h"


// 1024x1024
#define MAX_MIPMAP_LEVELS 11



struct Pixel
{
	union
	{
		struct
		{
			ui8 cB;
			ui8 cG;
			ui8 cR;
			ui8 cA;
		};
		ui8 comp[4];	// 0=B, 1=G, 2=R, 3=A
		ui32 dword;		// ARGB
	};

	Pixel ( DWORD dw = 0 )
	{
		dword = dw;
	}
};



class Image
{

public:
	// Level 0 is 1x1, level 1 is 2x2, level 2 is 4x4, level 3 is 8x8, etc.
	Pixel *ppixLevels[MAX_MIPMAP_LEVELS];
	// NOTE! This is actually the number of levels-1, so that size = 1<<iNumLevels.
	int iNumLevels;

	// The "real-world" size of the topmost level's texels.
	// Normally this is 1, but if the top 2 mipmap levels have been discarded, this will be 4.
	// And we store log2 of this size.
	int iSizeLog2;

	Image ( int iLevels, int iNewSizeLog2 = 0 )
	{
		ASSERT ( iLevels >= 0 );
		ASSERT ( iLevels < MAX_MIPMAP_LEVELS );
		iNumLevels = iLevels;
		// Allocate the space. Do it all at once to be friendly.
		int iTotalSize = 0;
		for ( int i = 0; i <= iNumLevels; i++ )
		{
			iTotalSize += 1 << ( 2 * i );
		}
		Pixel *ppix = new Pixel [ iTotalSize ];
		for ( int i = 0; i <= iNumLevels; i++ )
		{
			ppixLevels[i] = ppix;
			ppix += 1 << ( 2 * i );
		}

		iSizeLog2 = iNewSizeLog2;
	}

	~Image ( void )
	{
		// Level 0 actually points to the complete chunk of data.
		delete[] ppixLevels[0];
	}

	// Generate the mipmap levels from the topmost level.
	// Any previous data in the mipmaps is discarded.
	void GenerateMipmaps ( void );

	// Copies all the mipmap levels to the texture.
	void CopyToD3DTex ( LPDIRECT3DTEXTURE8 pd3dtex );

	// Create & load the image.
	static Image *LoadFromFile ( char *pcFilename, bool bTakeAlphaFromLuminance = false );

	static void ChangeD3DDevice ( LPDIRECT3DDEVICEn pd3ddevNew );

};







#endif //#ifndef IMAGE_INCLUDED
