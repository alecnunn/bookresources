/*****************************************************************************/
/*                                                                           */
/*                    S  e  n  s  a  u  r  a     L  t  d                     */
/*                    ==================================                     */
/*                                                                           */
/*                            (C) Copyright 2000                             */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*      File:           ZoomFx.h                                             */
/*                                                                           */
/*      Version:        1.0                                                  */
/*      Data:           01-Feb-2000                                          */
/*                                                                           */
/*****************************************************************************/


#ifndef	_ZOOMFX_H_
#define	_ZOOMFX_H_

 
/*****************************************************************************/
/*                                                                           */
/*      G U I D s                                                            */
/*                                                                           */
/*****************************************************************************/


#ifdef	__cplusplus
extern	"C"
{
#endif

/* ZoomFX buffer property set GUID {CD5368E0-3450-11D3-8B6E-00105A9B7BBC} */
DEFINE_GUID( DSPROPSETID_ZOOMFX_BufferProperties,
             0xCD5368E0,		
             0x3450,			
             0x11D3,			
             0x8B, 0x6E,		
             0x00, 0x10, 0x5A, 0x9B, 0x7B, 0xBC );

#ifdef	__cplusplus
}
#endif



 
/*****************************************************************************/
/*                                                                           */
/*      P r o p e r t y   s e t   I D s                                      */
/*                                                                           */
/*****************************************************************************/


/* NOTE: Get and Set methods are valid for all properties */
typedef enum
{
	/* all ZoomFX properties (data in ZOOMFX_BUFFERPROPERTIES structure) */
	DSPROPERTY_ZOOMFXBUFFER_ALL = 0,

	/* sound source bounding box (data in ZOOMFX_BOX structure) */
	DSPROPERTY_ZOOMFXBUFFER_BOX,

	/* sound source orientation (data in ZOOMFX_ORIENTATION structure) */
	DSPROPERTY_ZOOMFXBUFFER_ORIENTATION,

	/* near field MacroFX effect (data in LONG type) */
	DSPROPERTY_ZOOMFXBUFFER_MACROFX_EFFECT,
};



 
/*****************************************************************************/
/*                                                                           */
/*      S t r u c t u r e s                                                  */
/*                                                                           */
/*****************************************************************************/


#pragma pack( push, 4 )

/* min/max coordinates that define the sound source bounding box extents */
typedef struct
{
	D3DVECTOR vMin;
	D3DVECTOR vMax;
} ZOOMFX_BOX, *LPZOOMFX_BOX;


/* vectors that define the sound source orientation */
typedef	struct
{
	D3DVECTOR vFront;
	D3DVECTOR vTop;
} ZOOMFX_ORIENTATION, *LPZOOMFX_ORIENTATION;


/* structure to get/set all ZoomFX properties at once */
typedef	struct
{
	ZOOMFX_BOX box;
	ZOOMFX_ORIENTATION orientation;
	LONG lMacroFx;
} ZOOMFX_BUFFERPROPERTIES, *LPZOOMFX_BUFFERPROPERTIES;

#pragma pack( pop )



 
/*****************************************************************************/
/*                                                                           */
/*      R a n g e s   a n d   d e f a u l t s                                */
/*                                                                           */
/*****************************************************************************/


/* ZoomFX bounding box coordinates - default is all 0 to disable ZoomFX */
#define	ZOOMFXBUFFER_BOX_DEFAULT \
		{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }


/* ZoomFX orientation vectors */
#define	ZOOMFXBUFFER_ORIENTATION_DEFAULT \
		{ { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } }


/* MacroFX percentage effect - can be varied from 0 (i.e. off) to 200% */
/* 100% equates to normal human hearing */
#define	ZOOMFXBUFFER_MACROFX_MIN	0
#define	ZOOMFXBUFFER_MACROFX_MAX	200
#define	ZOOMFXBUFFER_MACROFX_DEFAULT	100


/* ZoomFX all properties default */
#define	ZOOMFXBUFFER_ALL_DEFAULT \
		ZOOMFXBUFFER_BOX_DEFAULT, \
		ZOOMFXBUFFER_ORIENTATION_DEFAULT, \
		ZOOMFXBUFFER_MACROFX_DEFAULT

#endif
