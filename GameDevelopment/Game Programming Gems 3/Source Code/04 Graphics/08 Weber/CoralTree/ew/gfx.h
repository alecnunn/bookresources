/******************************************************************************

Coral Tree ew/gfx.h
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/



/******************************* DEFINE's ************************************/


/* main colors */
#if EW_GL==EW_IRISGL

	#error colors not current

	#define EW_BLACK		BLACK		/* 0 */
	#define EW_RED			RED			/* 1 */
	#define EW_GREEN		GREEN		/* 2 */
	#define EW_YELLOW		YELLOW		/* 3 */
	#define EW_BLUE			BLUE		/* 4 */
	#define EW_MAGENTA		MAGENTA		/* 5 */
	#define EW_CYAN			CYAN		/* 6 */
	#define EW_WHITE		WHITE		/* 7 */

	/* extra colors */
	#define EW_MEDGREY		8
	#define EW_PINK			9
	#define EW_LIGHTGREEN	10
	#define EW_OLIVE		11
//	#define EW_LAVENDER		12
	#define EW_PURPLE		13
	#define EW_BLUEGREEN	14
	#define EW_LIGHTGREY	15

	#define EW_DARKGREY		36			/* not reliable */

	#define EW_MAXCOLORS	4096

#else

/*
	#define EW_BLACK		0
	#define EW_RED			1
	#define EW_GREEN		2
	#define EW_YELLOW		3
	#define EW_BLUE			4
	#define EW_MAGENTA		5
	#define EW_CYAN			6
	#define EW_WHITE		7

	#define EW_MEDGREY		8
	#define EW_PINK			9
	#define EW_LIGHTGREEN	10
	#define EW_OLIVE		11
//	#define EW_LAVENDER		12
	#define EW_PURPLE		13
	#define EW_BLUEGREEN	14
	#define EW_LIGHTGREY	15

	#define EW_DARKGREY		12

//	#define EW_MAXCOLORS	17
	#define EW_MAXCOLORS	16
*/

/*
	#define EW_BLACK		0
	#define EW_MEDGREY		1
	#define EW_LIGHTGREY	2
	#define EW_WHITE		3

	#define EW_DARKGREY		4
	#define EW_PINK			5
	#define EW_LIGHTGREEN	6
	#define EW_BLUEGREEN	7

	#define EW_BLUE			8
	#define EW_GREEN		9
	#define EW_CYAN			10
	#define EW_YELLOW		11

	#define EW_RED			12
	#define EW_MAGENTA		13
	#define EW_OLIVE		14
	#define EW_PURPLE		15
*/

	#define EW_BLACK		0
	#define EW_DARKGREY		1
	#define EW_LIGHTGREY	2
	#define EW_WHITE		3

	#define EW_BLUE			4
	#define EW_RED			5
	#define EW_MEDGREY		6
	#define EW_GREEN		7

	#define EW_DARKGREEN	8
	#define EW_DARKCYAN		9
	#define EW_DARKMAGENTA	10
	#define EW_YELLOW		11

	#define EW_DARKBLUE		12
	#define EW_MAGENTA		13
	#define EW_DARKRED		14
	#define EW_CYAN			15

	#define EW_LIGHTGREEN	EW_GREEN
	#define EW_PURPLE		EW_MAGENTA
	#define EW_PINK			EW_MAGENTA
	#define EW_OLIVE		EW_YELLOW
	#define EW_BLUEGREEN	EW_CYAN

	#define EW_MAXCOLORS	16

#endif


/* default GUI colors */
#define EW_DEFAULT_COLOR_PEN			EW_WHITE
#define EW_DEFAULT_COLOR_WIDGET			EW_LIGHTGREY
#define EW_DEFAULT_COLOR_BACKGROUND		EW_LIGHTGREY
#define EW_DEFAULT_COLOR_LIT			EW_WHITE
#define EW_DEFAULT_COLOR_HALF_LIT		EW_LIGHTGREY
#define EW_DEFAULT_COLOR_SHADOW			EW_BLACK
#define EW_DEFAULT_COLOR_HALF_SHADOW	EW_DARKGREY


/* current color indexes (enumeration) (0 not used) */
#define EW_COLOR_UNUSED			0
#define EW_COLOR_PEN			1
#define EW_COLOR_WIDGET			2
#define EW_COLOR_BACKGROUND		3
#define EW_COLOR_LIT			4
#define EW_COLOR_HALF_LIT		5
#define EW_COLOR_SHADOW			6
#define EW_COLOR_HALF_SHADOW	7
#define EW_COLOR_NUMBER			8	// total number of colored elements (starting from 0)
/*****************************************************************************/



/******************************* COLORS **************************************/

// NOTE: greys changed from 0x2FFF->0x4444 and 0x5555 to 0x8888

// NOTE: lavendar replaced by dark grey so that there are only 16 colors

#if EW_WL==EW_XWIN

	// #rrggbb

	#if FALSE
		#error not current
		static const char EwXColorName[EW_MAXCOLORS][16]=
			{
			"black",			"red",		"green",		"yellow",
			"blue",				"magenta",	"cyan",			"white",
			"#555555",			"pink",		"pale green",   "olive drab",
			"medium purple",	"purple",   "cadet blue",   "#AAAAAA",
			"#404040"
			};
	#else
		// not current
		static const char EwXColorName[EW_MAXCOLORS][16]=
			{
			"#000000",  "#FF0000",  "#00FF00",  "#FFFF00",
			"#0000FF",  "#FF00FF",  "#00FFFF",  "#FFFFFF",
			"#888888",  "#C77070",  "#70C770",  "#8F8F37",
			"#444444",  "#8F378F",  "#378F8F",  "#AAAAAA",

//			"#7070C7",  "#8F378F",  "#378F8F",  "#AAAAAA",
//			"#444444"
			};
	#endif

#endif

	// Let this be our standard mapping
	// rrrr,gggg,bbbb

	static const long EwXColor[EW_MAXCOLORS][3]=
		{
/*
		0x0000,	0x0000,	0x0000,
		0xc000,	0xc000,	0xc000,
		0xd777,	0xd777,	0xd777,
		0xFFFF,	0xFFFF,	0xFFFF,

		0x8000,	0x8000,	0x8000,
		0xC777,	0x7000,	0x7000,
		0x7000,	0xC777,	0x7000,
		0x3777,	0x8FFF,	0x8FFF,

		0x0000,	0x0000,	0xFFFF,
		0x0000,	0xFFFF,	0x0000,
		0x0000,	0xFFFF,	0xFFFF,
		0xFFFF,	0xFFFF,	0x0000,

		0xFFFF,	0x0000,	0x0000,
		0x8FFF,	0x3777,	0x8FFF,
		0x8FFF,	0x8FFF,	0x3777,
		0xFFFF,	0x0000,	0xFFFF,
*/
		{0x0000,	0x0000,	0x0000},	// black
		{0x8000,	0x8000,	0x8000},	// dark grey
		{0xc000,	0xc000,	0xc000},	// light grey
		{0xFFFF,	0xFFFF,	0xFFFF},	// white

		{0x0000,	0x0000,	0xFFFF},	// blue
		{0xFFFF,	0x0000,	0x0000},	// red
		{0xA000,	0xA000,	0xA400},	// med grey (very pale blue)
		{0x0000,	0xFFFF,	0x0000},	// green

		{0x0000,	0x8000,	0x0000},	// dark green
		{0x0000,	0x8000,	0x8000},	// dark cyan
		{0x8000,	0x0000,	0x8000},	// dark purple
		{0xFFFF,	0xFFFF,	0x0000},	// yellow

		{0x0000,	0x0000,	0x8000},	// dark blue
		{0xFFFF,	0x0000,	0xFFFF},	// purple
		{0x8000,	0x0000,	0x0000},	// dark red
		{0x0000,	0xFFFF,	0xFFFF}	// cyan
		};

#if EW_WL==EW_WIN32

static COLORREF EwWin32Colortable[EW_MAXCOLORS]=
	{
/*		0x00bbggrr,*/

/*
		// real BGR (standard EW colors)
		0x00000000,		// black
		0x00c0c0c0,		// med grey
		0x00e0e0e0,		// light grey
		0x00FFFFFF,		// white

		0x00808080,		// dark grey
		0x007070C7,		// pink
		0x0070C770,		// lightgreen
		0x008F8F37,		// bluegreen

		0x00FF0000,		// blue
		0x0000FF00,		// green
		0x00FFFF00,		// cyan
		0x0000FFFF,		// yellow

		0x000000FF,		// red
		0x008F378F,		// magenta
		0x00378F8F,		// olive drab
		0x00FF00FF,		// purple
*/

		// match to Win32
		0x00000000,		// black
		0x00808080,		// dark grey
		0x00c0c0c0,		// light grey
		0x00ffffff,		// white

		0x00ff0000,		// blue
		0x000000ff,		// red
		0x00a4a0a0,		// med grey (very pale blue)
		0x0000ff00,		// green

		0x00008000,		// dark green
		0x00808000,		// dark cyan
		0x00800080,		// dark purple
		0x0000ffff,		// yellow

		0x00800000,		// dark blue
		0x00ff00ff,		// purple
		0x00000080,		// dark red
		0x00ffff00,		// cyan

//		0x00c0dcc0,		// pale green
//		0x00008080		// dark yellow
//		0x00f0caa6,		// pale bluegreen
//		0x00f0fbff		// antique white (yellowish)

/*
		0x00000000,		// black
		0x00c0c0c0,		// med grey
		0x00f0fbff,		// light grey -> yellowish white
		0x00ffffff,		// white

		0x00808080,		// dark grey
		0x00000080,		// pink -> dark red
		0x00c0dcc0,		// lightgreen -> very light green
		0x00808000,		// bluegreen -> hard blue green

		0x00ff0000,		// blue
		0x0000ff00,		// green
		0x00ffff00,		// cyan
		0x0000ffff,		// yellow

		0x000000ff,		// red
		0x00800080,		// magenta -> dark purple
		0x00008080,		// olive drab -> dark yellow
		0x00ff00ff		// purple
*/

//		0x00f0caa6,		// light grey -> very pale bluegreen
//		0x00e0e0e0,		// light grey -> dithered
//		0x00800000,		// dark blue
//		0x00a4a0a0,		// med grey -> very pale blue
//		0x00008000,		// lightgreen -> dark green

/*
		0x00F0FBFF,
		0x00C0DCC0,
		0x00F0CAA6,
*/

	};

#endif

/*****************************************************************************/



/******************************* MACRO's *************************************/

#define EW_INVERT_Y(y,sy)		( (sy) - (y) - 1 )

#if EW_GL==EW_X_GFX || EW_GL==EW_GDI
	#define EW_REMAP_Y(y,sy)	EW_INVERT_Y(y,sy)
#else
	#define EW_REMAP_Y(y,sy)	(y)
#endif

/*
#if EW_GL==EW_OPENGL

#define ewTranslate(x,y,z)		glTranslatef(x,y,z)
#define ewPushMatrix()			glPushMatrix()
#define ewPopMatrix()			glPopMatrix()

#else

#define ewTranslate(x,y,z)		translatef(x,y,z)
#define ewPushMatrix()			pushmatrix()
#define ewPopMatrix()			popmatrix()

#endif
*/

/*****************************************************************************/



/******************************* TYPEDEF's ***********************************/


#if EW_GL==EW_X_GFX

	/* Pure X Windows */

	typedef long			EW_COLORTYPE;

#elif EW_GL==EW_OPENGL

	/* OpenGL */

	typedef GLint			EW_COLORTYPE;	// int

	#if EW_WL==EW_XWIN

		/* OpenGL on X */

	#elif EW_WL==EW_WIN32

		/* OpenGL on Win32 */

	#else

		#error EW_WL not properly defined

	#endif

#elif EW_GL==EW_IRISGL

	/* IrisGL on Mex */

	typedef Colorindex		EW_COLORTYPE;	// unsigned short

#elif EW_GL==EW_GDI

	/* GDI on Win32 */

	typedef long			EW_COLORTYPE;

#else

	#error EW_GL not properly defined

#endif


/*****************************************************************************/



/******************************************************************************
class	EW_ColorMapping : public GPL_Array<long>

******************************************************************************/
class EW_ColorMapping : public GPL_Array<long>
	{
	public:
		void	SetMapToIndex(long mapindex,long colorindex)
					{
					InsertElement(-1-colorindex,mapindex);
					};

		void	SetMapToColor(long mapindex,long hexcolor)
					{
					InsertElement(hexcolor,mapindex);
					};

		long	IsIndex(long mapindex)
					{
					return (GetElement(mapindex)<0);
					};

		long	GetMapColor(long mapindex)
					{
					return GetElement(mapindex);
					};
 
		long	GetMapIndex(long mapindex)
					{
					return (-1-GetElement(mapindex));
					};
 
	};
