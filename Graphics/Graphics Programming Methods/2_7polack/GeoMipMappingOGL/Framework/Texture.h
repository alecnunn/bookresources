//==============================================================
//= Texture.h
//= Original coders: Evan Pipho (evan@codershq.com) and
//=					 Trent Polack (trent@codershq.com)
//==============================================================
//= TGA texture loading routines
//==============================================================
#ifndef __DemoFramework_Texture_H__
#define __DemoFramework_Texture_H__

namespace DemoFramework {
	//TGA image information header
	struct TGAInformationHeader {
		unsigned char m_ucHeader[6];
		unsigned int  m_uiBytesPerPixel;
		unsigned int  m_uiImageSize;
		unsigned int  m_uiTemp;
		unsigned int  m_uiType;	
		unsigned int  m_height;
		unsigned int  m_width;
		unsigned int  m_BPP;
	};

	extern unsigned char g_UTGAcompare[12];
	extern unsigned char g_CTGAcompare[12];

	//texture-loading class
	class Texture {
		unsigned char *m_pData;
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_BPP;
		unsigned int m_ID;

		bool m_isLoaded;

		bool LoadCompressedTGA( void );
		bool LoadUncompressedTGA( void );

	public:


		bool Create( unsigned int width, unsigned int height, unsigned int BPP );

		bool LoadData( const char *filename );

		bool Load( const char *filename, float minFilter, float maxFilter, bool doMipmap= false );
		void Release( void );

		//extract a color from the texture
		void GetColor( unsigned int x, unsigned  int y,
					   unsigned char* pRed, unsigned char* pGreen, unsigned char* pBlue ) {
			unsigned int BPP= m_BPP/8;

			if( ( x<m_width ) && ( y<m_height ) ) {
				*pRed  = m_pData[( ( y*m_height )+x )*BPP];
				*pGreen= m_pData[( ( y*m_height )+x )*BPP+1];
				*pBlue = m_pData[( ( y*m_height )+x )*BPP+2];
			}
		}

		//set one of the texture map's pixel's color
		void SetColor( unsigned int x, unsigned  int y,
							  unsigned char red, unsigned char green, unsigned char blue ) {
			unsigned int BPP= m_BPP/8;

			if( ( x<m_width ) && ( y<m_height ) ) {
				m_pData[( ( y*m_height )+x )*BPP]  = red;
				m_pData[( ( y*m_height )+x )*BPP+1]= green;
				m_pData[( ( y*m_height )+x )*BPP+2]= blue;
			}
		}

		//get a pointer to the texture's data
		unsigned char *GetData( void ) {
			return m_pData;
		}

		//get the texture map's width
		unsigned int GetWidth( void ) {
			return m_width;
		}

		//get the texture map's height
		unsigned int GetHeight( void ) {
			return m_height;
		}

		//get the texture map's number of bits per pixel
		unsigned int GetBPP( void ) {
			return m_BPP;
		}

		//get the texture map's OpenGL ID
		unsigned int GetID( void ) {
			return m_ID;
		}

		//set the texture map's OpenGL ID
		void SetID( unsigned int ID ) {
			m_ID= ID;
		}

		//check to see if a texture is loaded or not
		bool IsLoaded( void ) {
			return m_isLoaded;
		}
	};
}

#endif	//__DemoFramework_Texture_H__