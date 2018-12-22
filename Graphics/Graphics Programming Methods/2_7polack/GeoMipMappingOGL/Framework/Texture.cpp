//==============================================================
//= Texture.h
//= Original coders: Evan Pipho (evan@codershq.com) and
//=					 Trent Polack (trent@codershq.com)
//==============================================================
//= TGA texture loading routines
//==============================================================
#include <cstdio>
#include <memory.h>

#include <SDL_opengl.h>

#include "Texture.h"

unsigned char DemoFramework::g_UTGAcompare[12]= {
	0, 0, 2,  0, 0, 0, 0, 0, 0, 0, 0, 0
};

unsigned char DemoFramework::g_CTGAcompare[12]= {
	0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


//allocate room for a new texture
bool DemoFramework::Texture::Create( unsigned int width, unsigned int height, unsigned int BPP ) {
	//set the member variables
	m_width = width;
	m_height= height;
	m_BPP   = BPP;

	//allocate memory
	m_pData= new unsigned char [m_width*m_height*( m_BPP/8 )];
	if( !m_pData ) {
		printf( "Out of memory for image memory allocation\n" );
		return false;
	}

	//set the loaded flag
	m_isLoaded= true;
	return true;
}

//load texture data (and only texture data) in
bool DemoFramework::Texture::LoadData( const char *filename ) {
	FILE *pFile;
	int start;
	int end;
	int size;

	//open the file for reading (in binary mode)
	pFile= fopen( filename, "rb" );

	//check to see if we were able to open the file
	if( pFile == NULL ) {
		printf( "Could not open file %s\n", filename );
		return false;
	}
		
	//Get file length
	fseek( pFile, 0, SEEK_END );
	end  = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );
	start= ftell( pFile );
	size = end - start;

	//allocate the data buffer (temporary)
	m_pData= new unsigned char [size];

	//read the image's data into the buffer
	if( fread( m_pData, sizeof( unsigned char ), size, pFile ) != ( unsigned )size ) {
		//the file is corrupted
		printf( "%s is corrupted, could not read all data\n", filename );
		if( m_pData )
			delete[] m_pData;

		return false;
	}

	//check to see if the file is an uncompressed TGA
	if( memcmp( m_pData, g_UTGAcompare, 12 ) == 0 ) {
		//load the file using the uncompressed TGA Loader
		if( !LoadUncompressedTGA( ) ) {
			//could not load the file
			printf( "Could not load uncompressed TGA %s\n", filename );
			if( m_pData )
				delete[] m_pData;

			return false;
		}

	} else if( memcmp( m_pData, g_CTGAcompare, 12 ) == 0 ) {
		//check to see if the file is a compressed TGA

		//load the file using the compressed TGA loader
		if( !LoadCompressedTGA( ) ) {
			//could not load the file
			if( m_pData )
				delete[] m_pData;

			printf( "Could not load compressed TGA %s\n", filename );
			return false;
		}

	} else {
		//the file is not supported by our image loader
		printf( "%s is not a supported image type\n", filename );
		if( m_pData ) {
			delete[] m_pData;
		}
		return false;
	}

	//the file's data was successfully loaded
	m_isLoaded= true;
	printf( "Loaded %s correctly\n", filename );
	return true;
}

//load a texture into OpenGL
bool DemoFramework::Texture::Load( const char *filename, float minFilter, float maxFilter, bool doMipmap ) {
	int	type;

	//load the file's data in
	if( !LoadData( filename ) )	return false;

	//set the image's OpenGL BPP type
	if( m_BPP == 24 )	type= GL_RGB;
	else	type= GL_RGBA;

	//build the texture for use with OpenGL
	glGenTextures( 1, &m_ID );
	glBindTexture( GL_TEXTURE_2D, m_ID );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter );						

	//create the texture normally
	if( !doMipmap )
		glTexImage2D( GL_TEXTURE_2D, 0, type, m_width, m_height, 
					  0, type, GL_UNSIGNED_BYTE, m_pData );

	//create a mipmapped texture
	else
		gluBuild2DMipmaps( GL_TEXTURE_2D, type, m_width, m_height, 
						   type, GL_UNSIGNED_BYTE, m_pData );

	//the image has been successfully loaded
	m_isLoaded= true;
	return true;
}

//release a texture
void DemoFramework::Texture::Release( void ) {
	if( m_isLoaded ) {
		delete[] m_pData;

		m_width = 0;
		m_height= 0;
		m_BPP   = 0;

		m_isLoaded= false;
	}
}

//load a compressed TGA in
bool DemoFramework::Texture::LoadCompressedTGA( void ) {
	TGAInformationHeader TGAInfo;
	unsigned char* pFileBuffer;
	unsigned char* pColorBuffer;
	unsigned char  chunkHeader;	
	unsigned int   pixelCount;
	unsigned int   currentPixel;
	unsigned int   currentByte;
	short		   i;

	pFileBuffer= m_pData;

	//skip past the image header
	pFileBuffer+= 12;

	memcpy( TGAInfo.m_ucHeader, pFileBuffer, sizeof( unsigned char[6] ) );
	
	//allocate memory for the image data buffer
	Create( TGAInfo.m_ucHeader[1] * 256 + TGAInfo.m_ucHeader[0],
			TGAInfo.m_ucHeader[3] * 256 + TGAInfo.m_ucHeader[2],
			TGAInfo.m_ucHeader[4] );
	
	//set the class's member variables
	TGAInfo.m_BPP   = m_BPP;
	TGAInfo.m_height= m_height;
	TGAInfo.m_width = m_width;

	//advance the file buffer
	pFileBuffer+= 6;

	//make sure that the image's dimensions are supported by this loaded
	if( ( m_width<=0 ) || ( m_height<=0 ) || ( ( m_BPP!=24 ) && ( m_BPP!=32 ) ) )
		return false;

	TGAInfo.m_uiBytesPerPixel= TGAInfo.m_BPP/8;
	TGAInfo.m_uiImageSize	  = ( TGAInfo.m_uiBytesPerPixel*TGAInfo.m_width*TGAInfo.m_height );

	pixelCount  = TGAInfo.m_height * TGAInfo.m_width;
	currentPixel= 0;
	currentByte = 0;
	pColorBuffer= new unsigned char [TGAInfo.m_uiBytesPerPixel];

	do {
		chunkHeader= 0;

		chunkHeader= *( unsigned char* )pFileBuffer;
		pFileBuffer++;

		if( chunkHeader < 128 ) {
			chunkHeader++;

			//read RAW color values
			for( i=0; i<chunkHeader; i++ ) {
				memcpy( pColorBuffer, pFileBuffer, TGAInfo.m_uiBytesPerPixel );
				pFileBuffer+= TGAInfo.m_uiBytesPerPixel;
				
				//flip R and B color values around
				m_pData[currentByte]  = pColorBuffer[2];
				m_pData[currentByte+1]= pColorBuffer[1];
				m_pData[currentByte+2]= pColorBuffer[0];

				if( TGAInfo.m_uiBytesPerPixel == 4 )
					m_pData[currentByte+3]= pColorBuffer[3];

				currentByte += TGAInfo.m_uiBytesPerPixel;
				currentPixel++;

				//make sure too many pixels have not been read in
				if( currentPixel > pixelCount ) {
					delete[] pColorBuffer;
					delete[] m_pData;

					return false;
				}
			}
		} else {
			//the chunk header is greater than 128 RLE data
			chunkHeader-= 127;

			memcpy( pColorBuffer, pFileBuffer, TGAInfo.m_uiBytesPerPixel );
			pFileBuffer+= TGAInfo.m_uiBytesPerPixel;

			//copy the color into the image data as many times as needed
			for( i=0; i < chunkHeader; i++ ) {
				//switch R and B bytes around while copying
				m_pData[currentByte]  = pColorBuffer[2];					
				m_pData[currentByte+1]= pColorBuffer[1];
				m_pData[currentByte+2]= pColorBuffer[0];

				if( TGAInfo.m_uiBytesPerPixel == 4 )
					m_pData[currentByte+3]= pColorBuffer[3];

				currentByte += TGAInfo.m_uiBytesPerPixel;
				currentPixel++;

				//make sure that we have not written too many pixels
				if( currentPixel > pixelCount ) {
					delete[] pColorBuffer;
					delete[] m_pData;

					return false;
				}
			}
		}
	} while( currentPixel < pixelCount );	//loop while there are still pixels left

	//the compressed TGA has been successfully loaded
	return true;					
}

//load an uncompressed TGA in
bool DemoFramework::Texture::LoadUncompressedTGA( void )
{
	TGAInformationHeader TGAInfo;
	unsigned char* pFileBuffer= m_pData;
	unsigned int colorSwap;

	//skip past the TGA header in the data buffer
	pFileBuffer+= 12;

	//copy the header data
	memcpy( TGAInfo.m_ucHeader, pFileBuffer, sizeof( unsigned char[6] ) );

	//allocate memory for the image's data buffer
	Create( TGAInfo.m_ucHeader[1]*256+TGAInfo.m_ucHeader[0],
			TGAInfo.m_ucHeader[3]*256+TGAInfo.m_ucHeader[2],
			TGAInfo.m_ucHeader[4] );

	//set the class's member variables
	TGAInfo.m_BPP   = m_BPP;
	TGAInfo.m_width = m_width;
	TGAInfo.m_height= m_height;

	pFileBuffer+= 6;

	if( ( m_width<=0 ) || ( m_height<=0 ) || ( ( m_BPP!=24 ) && ( m_BPP!=32 ) ) )
		return false;

	TGAInfo.m_uiBytesPerPixel= m_BPP/8;
	TGAInfo.m_uiImageSize	  = ( TGAInfo.m_uiBytesPerPixel*m_width*m_height );

	//copy the image data
	memcpy( m_pData, pFileBuffer, TGAInfo.m_uiImageSize );

	//byte swapping ( optimized by Steve Thomas )
	for( colorSwap=0; colorSwap < ( int )TGAInfo.m_uiImageSize; colorSwap+= TGAInfo.m_uiBytesPerPixel ) {
		m_pData[colorSwap]^= m_pData[colorSwap+2]^=
		m_pData[colorSwap]^= m_pData[colorSwap+2];
	}

	//the uncompressed TGA has been successfully loaded
	return true;
}
