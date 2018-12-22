/*============================================================================
	Title: Raster.h
	Module: Pi/Raster
	Author: Ignacio Castaño
	Date: 14/01/2003
	Description: Rasterization module main header.
============================================================================*/

#ifndef _PI_RASTER_H_
#define _PI_RASTER_H_

/*----------------------------------------------------------------------------
	Documentation
----------------------------------------------------------------------------*/

/** @file Raster.h
 * Rasterization module main header.
**/

/*----------------------------------------------------------------------------
	Includes
----------------------------------------------------------------------------*/

//#include "Core.h"
#include "Vector.h"

// instead of Core
#include "PAR.h"


/*----------------------------------------------------------------------------
	Definitions
----------------------------------------------------------------------------*/

// API definition.
//#ifdef RASTER_EXPORTS
//#define RASTER_API DLL_EXPORT
//#else
//#define RASTER_API DLL_IMPORT
//#endif
#define RASTER_API

#define RA_CALLBACK	_CDECL

/** Interpolation modes. */
enum {
	PI_POINT_MODE_LINEAR		= 0x00010000,
	PI_POINT_MODE_CUBIC			= 0x00020000,
	PI_NORMAL_MODE_LINEAR		= 0x00040000,
	PI_NORMAL_MODE_QUADRATIC	= 0x00080000
};




/*----------------------------------------------------------------------------
	Types
----------------------------------------------------------------------------*/

/** A feature is a 4d vector. Each feature is 12 bytes. */
typedef struct {
	uint16 attrib[4];
	uint32 samples;
} RA_Feature;


/** An image is a bidimensional array of features. */
typedef struct {

	RA_Feature * features;	///< Array of features.

	int width;				///< Width of the image.
	int height;				///< Height of the image.

} RA_Image;


/** A generic vertex with many attributes. */
typedef struct {
	Vec3 pos;		///< Position.
	Vec3 nor;		///< Normal.
	Vec3 tan;		///< Tangent.
	Vec3 bin;		///< Binormal.
	Vec3 tex;		///< Texcoord 0.
	Vec3 tx1;		///< Texcoord 1.
	Vec3 col;		///< Color.
	Vec3 bar;		///< Barycentric coordinates.
} RA_Vertex;


/** A triangle made by 3 vertices. */
typedef RA_Vertex	RA_Triangle [3];


/** Callbacks. */
//@{
	/** A callback to sample the environment. */
	typedef int (RA_CALLBACK * RA_SAMPLING_CALLBACK)( void * param, const RA_Vertex * vertex, const RA_Vertex * dx, const RA_Vertex * dy, RA_Feature * hit );
//@}





/*----------------------------------------------------------------------------
	Functions
----------------------------------------------------------------------------*/

/** Feature extraction. */
//@{
	/** Process this triangle. */
	RASTER_API void RA_Process( RA_Image *, RA_Triangle, int flags, RA_SAMPLING_CALLBACK, void * param );

	/** Draw triangle outlines. */
	RASTER_API void RA_DrawOutlines( RA_Image *, RA_Triangle );
//@}


/** Image manipulation. */
//@{
	/** Create a new empty image. */
	RASTER_API RA_Image * RA_Image_New( int w, int h );

	/** Create a new empty image. */
	RASTER_API void RA_Image_Delete( RA_Image * );

	/** Create a new image from the given one. */
	RASTER_API RA_Image * RA_Image_Clone( RA_Image * );
//@}


/** Image filtering. */
//@{
	/** Edge expansion filter. Performs n iterations. */
	RASTER_API void RA_Filter_Expand( RA_Image *, int n );

	/** Pull push filter, as in the lumigraph. */
	RASTER_API void RA_Filter_PullPush( RA_Image * );

	/** Euclidean distance transform. */
	RASTER_API void RA_Filter_EDT( RA_Image * );

	/** Antialiasing by supersampling at the given rate (2, 4, 8, 16, etc). */
	RASTER_API void RA_Filter_Downsample( RA_Image *, int n );

	/** Normalize normal map. */
	RASTER_API void RA_Filter_Normalize( RA_Image * );
//@}






#endif // _PI_RASTER_H_
