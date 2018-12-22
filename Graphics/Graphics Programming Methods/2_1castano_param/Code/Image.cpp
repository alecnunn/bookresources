/*============================================================================
	Title: image.cpp
	Module: MagicAppearance
	Author: Ignacio Castaño
	Description: Image manipulation and filtering routines.
	Changes:
		08/08/2002 - Ignacio Castaño
			File added.

============================================================================*/

/*----------------------------------------------------------------------------
	Include
----------------------------------------------------------------------------*/

#include <stdio.h>

#include "Raster.h"
#include "RenderMan.h"
//#include "types.h"
//#include "math.h"
//#include "file.h"



/*----------------------------------------------------------------------------
	Structs
----------------------------------------------------------------------------*/

/** An element of the eucliean distance map. */
struct edm_elem {
	int x;
	int y;
	float d;
};


/*----------------------------------------------------------------------------
	Functions
----------------------------------------------------------------------------*/

/** Clear image. */
static void RA_Image_Clear( RA_Image * img ) {
	int size = img->width * img->height;
	for( int i=0; i<size; i++ ) {
		img->features[i].attrib[0] = 0;
		img->features[i].attrib[1] = 0;
		img->features[i].attrib[2] = 0;
		img->features[i].attrib[3] = 0;
		img->features[i].samples = 0;
	}
}

/** Compute euclidean squared distance. */
static float dist( int ax, int ay, int bx, int by ) {
	float dx = float(bx-ax);
	float dy = float(by-ay);
	return dx*dx + dy*dy;
}

/** Check neighbour, this is the core of the EDT algorithm. */
static void CheckNeighbour( int x, int y, edm_elem & e, edm_elem & n ) {
	float d = dist( x, y, n.x, n.y );
	if( d < e.d ) {
		e.x = n.x;
		e.y = n.y;
		e.d = d;
	}
}

/** */
static int Feature_IsEmpty( const RA_Feature & f ) {
	return f.samples==0;
}




/** Create a new empty image. */
RASTER_API RA_Image * RA_Image_New( int w, int h ) {

	RA_Image * img = new RA_Image;
	img->features = new RA_Feature[w*h];
	img->width = w;
	img->height = h;

	RA_Image_Clear( img );

	return img;
}


/** Delete image. */
RASTER_API void RA_Image_Delete( RA_Image * img ) {
	delete [] img->features;
	img->features = NULL;
	img->width = 0;
	img->height = 0;
	delete img;
}


/** Copy the image. */
RASTER_API RA_Image * RA_Image_Clone( RA_Image * img ) {

	RA_Image * clone = RA_Image_New( img->width, img->height );

	int size = img->width * img->height;
	for( int i=0; i<size; i++ ) {
		clone->features[i].attrib[0] = img->features[i].attrib[0];
		clone->features[i].attrib[1] = img->features[i].attrib[1];
		clone->features[i].attrib[2] = img->features[i].attrib[2];
		clone->features[i].attrib[3] = img->features[i].attrib[3];
		clone->features[i].samples = img->features[i].samples;
	}

	return clone;
}



/** Euclidean distance transform. */
RASTER_API void RA_Filter_EDT( RA_Image * img ) {

	edm_elem * edm = new edm_elem[img->width*img->height];

	int x, y;

	for( y=0; y<img->height; y++ ) {
		for( x=0; x<img->width; x++ ) {
			RA_Feature & src = img->features[y*img->width+x];

			if( Feature_IsEmpty( src ) ) {
				edm[y*img->width+x].x = img->width*img->width;
				edm[y*img->width+x].y = img->width*img->width;
				edm[y*img->width+x].d = float(img->width*img->width + img->width*img->width);
			}
			else {
				edm[y*img->width+x].x = x;
				edm[y*img->width+x].y = y;
				edm[y*img->width+x].d = 0;
			}
		}
	}

	// First pass.
	for( y=0; y<img->height; y++ ) {
		for( x=0; x<img->width; x++ ) {
			int x0 = clamp( x-1, 0, img->width-1 );
			int x1 = clamp( x+1, 0, img->width-1 );
			int y0 = clamp( y-1, 0, img->height-1 );

			edm_elem & e = edm[y*img->width + x ];

			CheckNeighbour( x, y, e, edm[y0*img->width + x0] );
			CheckNeighbour( x, y, e, edm[y0*img->width + x] );
			CheckNeighbour( x, y, e, edm[y0*img->width + x1] );
			CheckNeighbour( x, y, e, edm[y*img->width + x0] );
		}

		for( x=img->width-1; x>=0; x-- ) {
			int x1 = clamp( x+1, 0, img->width-1 );

			edm_elem & e = edm[y*img->width+x];
		
			CheckNeighbour( x, y, e, edm[y*img->width + x1] );
		}
	}

	// Third pass.
	for( y=img->height-1; y>=0; y-- ) {
		for( x=img->width-1; x>=0; x-- ) {
			int x0 = clamp( x-1, 0, img->width-1 );
			int x1 = clamp( x+1, 0, img->width-1 );
			int y1 = clamp( y+1, 0, img->height-1 );

			edm_elem & e = edm[y*img->width + x];

			CheckNeighbour( x, y, e, edm[y*img->width + x1] );
			CheckNeighbour( x, y, e, edm[y1*img->width + x0] );
			CheckNeighbour( x, y, e, edm[y1*img->width + x] );
			CheckNeighbour( x, y, e, edm[y1*img->width + x1] );
		}

		for( x=0; x<img->width; x++ ) {
			int x0 = clamp( x-1, 0, img->width-1 );

			edm_elem & e = edm[y*img->width + x];
			
			CheckNeighbour( x, y, e, edm[y*img->width + x0] );
		}
	}

	for( y=0; y<img->height; y++ ) {
		for( x=0; x<img->width; x++ ) {
			RA_Feature & src = img->features[y*img->width+x];

			if( Feature_IsEmpty( src ) ) {
				int sx = edm[y*img->width+x].x;
				int sy = edm[y*img->width+x].y;
				if( sx>=0 && sx<img->width && sy>=0 && sy<img->height )
					src = img->features[sy*img->width + sx];
			}
		}
	}

	delete [] edm;
}


/** Antialiasing by supersampling at the given rate (2, 4, 8, 16, etc). */
RASTER_API void RA_Filter_Downsample( RA_Image * img, int n ) {

	if( n!=2 && n!=4 && n!=8 && n!=16 ) return;

	int w = img->width / n;
	int h = img->height / n;
	RA_Feature * aux = new RA_Feature[w*h];

	int x, y;
	for( y=0; y<h; y++ ) {
		for( x=0; x<w; x++ ) {
			RA_Feature & dst = aux[y*w + x];

			Vec4 attrib( 0, 0, 0, 0 );
			dst.samples = 0;
			int samples = 0;

			int i, j;
			for( j=0; j<n; j++ ) {
				for( i=0; i<n; i++ ) {
					RA_Feature &src = img->features[ (y*n+j)*img->width + x*n+i ];
					if( src.samples ) {
						samples++;
						attrib.x += UnPackWordInFloat( src.attrib[0] );
						attrib.y += UnPackWordInFloat( src.attrib[1] );
						attrib.z += UnPackWordInFloat( src.attrib[2] );
						attrib.w += UnPackWordInFloat( src.attrib[3] );
						dst.samples += src.samples;
					}
				}
			}

			if( samples ) {
				attrib.x *= 1.0f/float(samples);
				attrib.y *= 1.0f/float(samples);
				attrib.z *= 1.0f/float(samples);
				attrib.w *= 1.0f/float(samples);
			}

			dst.attrib[0] = PackFloatInWord( attrib.x );
			dst.attrib[1] = PackFloatInWord( attrib.y );
			dst.attrib[2] = PackFloatInWord( attrib.z );
			dst.attrib[3] = PackFloatInWord( attrib.w );
		}
	}

	delete [] img->features;
	img->features = aux;
	img->width = w;
	img->height = h;

}


/** Normalize normal map. */
/*RASTER_API void RA_Filter_Normalize( RA_Image * img ) {
	int x, y;
	for( y=0; y<img->height; y++ ) {
		for( x=0; x<img->width; x++ ) {
			RA_Feature & f = img->features[y*img->width+x];
			Vec4_Normalize3( f.attrib, f.attrib );
		}
	}
}*/



/** Save color features. */
/*RASTER_API void RA_Save_Color( RA_Image * img, const char * name ) {

	// write a tga without compression
	FILE * fp = fopen( name, "wb" );
	if( fp==NULL ) return;

	RA_FileWrite8( fp, 0 );				// id_length
	RA_FileWrite8( fp, 0 );				// colormap_type
	RA_FileWrite8( fp, 2 );				// image_type
										
	RA_FileWrite16( fp, 0 );			// colormap_index
	RA_FileWrite16( fp, 0 );			// colormap_length
	RA_FileWrite8( fp, 0 );				// colormap_size

	RA_FileWrite16( fp, 0 );			// x_origin
	RA_FileWrite16( fp, 0 );			// y_origin
	RA_FileWrite16( fp, img->width );	// width
	RA_FileWrite16( fp, img->height );	// height

	RA_FileWrite8( fp, 24 );			// pixel_size
	RA_FileWrite8( fp, 0 );				// attributes

	// Write range compressed vectors
	for( int i=0; i<img->width*img->height; i++ ) {
		RA_FileWrite8( fp, CompressRange8( img->features[i].attrib[0] ) );
		RA_FileWrite8( fp, CompressRange8( img->features[i].attrib[1] ) );
		RA_FileWrite8( fp, CompressRange8( img->features[i].attrib[2] ) );
	}

	fclose( fp );
}*/


/** Save alpha features. */
/*RASTER_API void RA_Save_Alpha( RA_Image * img, const char * name ) {

}*/


/** Save color and alpha features. */
/*RASTER_API void RA_Save_ColorAlpha( RA_Image * img, const char * name ) {

	// write a tga without compression
	FILE * fp = fopen( name, "wb" );
	if( fp==NULL ) return;

	RA_FileWrite8( fp, 0 );				// id_length
	RA_FileWrite8( fp, 0 );				// colormap_type
	RA_FileWrite8( fp, 2 );				// image_type
										
	RA_FileWrite16( fp, 0 );			// colormap_index
	RA_FileWrite16( fp, 0 );			// colormap_length
	RA_FileWrite8( fp, 0 );				// colormap_size

	RA_FileWrite16( fp, 0 );			// x_origin
	RA_FileWrite16( fp, 0 );			// y_origin
	RA_FileWrite16( fp, img->width );	// width
	RA_FileWrite16( fp, img->height );	// height

	RA_FileWrite8( fp, 32 );			// pixel_size
	RA_FileWrite8( fp, 0 );				// attributes

	// Write range compressed vectors
	for( int i=0; i<img->width*img->height; i++ ) {
		RA_FileWrite8( fp, CompressRange8( img->features[i].attrib[0] ) );
		RA_FileWrite8( fp, CompressRange8( img->features[i].attrib[1] ) );
		RA_FileWrite8( fp, CompressRange8( img->features[i].attrib[2] ) );
		RA_FileWrite8( fp, CompressRange8( img->features[i].attrib[3] ) );
	}

	fclose( fp );

}*/


