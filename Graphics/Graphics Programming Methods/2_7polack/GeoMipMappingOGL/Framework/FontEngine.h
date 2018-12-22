//==============================================================
//= FontEngine.h
//= Original coder: Trent Polack (trent@codershq.com)
//==============================================================
//= A simple 2D font engine
//==============================================================
#ifndef __DemoFramework_FontEngine_H__
#define __DemoFramework_FontEngine_H__

//font constants
#define FONT_W 8
#define FONT_H 8
#define FONT_DRAW_W 8
#define FONT_DRAW_H 9
#define FONT_TEX_W 64
#define FONT_TEX_H 128

namespace DemoFramework {
	//a simple multiplatform font engine
	class FontEngine
	{
		static unsigned char m_fontData[16384];
		static unsigned int m_fontID;

		float m_color[4];

		int m_screenWidth, m_screenHeight;

	public:

		void Create( int screenWidth, int screenHeight );

		void BeginTextMode( void );
		void EndTextMode( void );
		void Print( int x, int y, char *text, ... );

		//-
		//set the font rendering color
		inline void SetColor( float r, float g, float b, float a ) {
			m_color[0]= r;	m_color[1]= g;	m_color[2]= b;	m_color[3]= a;
		}

	FontEngine( void ) {
		SetColor( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	~FontEngine( void ) { }
	};
}


#endif	//__DemoFramework_FontEngine_H__