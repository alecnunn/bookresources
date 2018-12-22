/*===========================================================================
	Title: EDT.cpp
	Module: GPM/EDT
	Author: Ignacio Castaño
	Description: Main file.
	Changes:
		11/01/2003 - Ignacio Castaño
			File added.

===========================================================================*/

/*---------------------------------------------------------------------------
	Doc:
---------------------------------------------------------------------------*/

/** @file EDT.cpp
 * Main file.
**/


/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

//#pragma comment(linker,"/ENTRY:WinMainCRTStartup")
#pragma comment(linker,"/ENTRY:mainCRTStartup")
#pragma comment(linker,"/SUBSYSTEM:CONSOLE")
//#pragma comment(linker,"/SUBSYSTEM:WINDOWS")



/*---------------------------------------------------------------------------
	Headers:
---------------------------------------------------------------------------*/

#include "EDT.h"
#include <math.h>
#include "png.h"



/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

HINSTANCE GInstance;



/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/

Image::Image( void ) {
	w = 0;
	h = 0;
	buffer = NULL;
}

Image::~Image( void ) {
	free();
}

Image * Image::clone( void ) {
	Image * img = new Image();
	img->w = w;
	img->h = h;
	img->buffer = new pixel[w*h];
	memcpy( img->buffer, buffer, sizeof(pixel)*w*h );
	return img;
}


bool Image::open( const char * name ) {

	const int number = 8;
	uint8 header[number];

	FILE *fp = fopen(name, "rb");
    if( !fp ) return false;
    fread(header, 1, number, fp);
    bool is_png = !png_sig_cmp(header, 0, number);
    if( !is_png ) return false;


	// Set up a read buffer and check the library version
	png_structp png_ptr;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) 
		return NULL;


	// Allocate/initialize a memory block for the image information
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return NULL;
	}


	// Set up the error handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		piDebug( "*** LoadPNG: Error reading png file.\n" );
		return NULL;
	}


	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, number);


	// Retrieve the image header information
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
				 &color_type, &interlace_type, NULL, NULL);

	// Expand paletted colors into true RGB triplets
	if( color_type == PNG_COLOR_TYPE_PALETTE )
		png_set_expand(png_ptr);

	// Expand grayscale images to a full 8 bits
	if( color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8 )
		png_set_expand(png_ptr);

	if( bit_depth == 16 )
		png_set_strip_16(png_ptr);

	// Expand paletted or RGB images with transparency to full alpha
	// channels so the data will be available as RGBA quartets
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);

	// remove alpha channel.
	if( color_type & PNG_COLOR_MASK_ALPHA )
        png_set_strip_alpha(png_ptr);

	png_read_update_info( png_ptr, info_ptr );


	// Read the image
	uint8 * img_data = new uint8[height*width*3];
	png_bytep * row_data = new png_bytep[sizeof(png_byte) * height];

	for (unsigned i = 0; i < height; i++)
		row_data[i] = &img_data[width * 3 * i];

	png_read_image(png_ptr, row_data);
	delete row_data;
	

	// Finish things up
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


	
	// convert the surface to the internal format
	w = width;
	h = height;
	buffer = new pixel[w*h];

	for( int y=0; y<h; y++ ) {
		for( int x=0; x<w; x++ ) {
			pixel * p = &buffer[y*w+x];
			p->r = img_data[(y*w+x)*3+0];
			p->g = img_data[(y*w+x)*3+1];
			p->b = img_data[(y*w+x)*3+2];

			if( p->r==0 && p->g==0 && p->b==0 ) {
				p->x = w*w;
				p->y = h*h;
				p->dist = float(w*w + h*h);
			}
			else {
				p->x = x;
				p->y = y;
				p->dist = 0;
			}
			p->e = false;
		}
	}

	delete [] img_data;

	piDebug( "--- open: Image '%s' loaded correctly.\n", name );
	return true;
}



void Image::empty( int w, int h ) {
	free();
	this->w = w;
	this->h = h;
	buffer = new pixel[w*h];
}


void Image::free( void ) {
	if( buffer ) {
		w = 0;
		h = 0;
		delete [] buffer; buffer = NULL;
	}
}


/** Clear the image. */
void Image::clear( uint8 r, uint8 g, uint8 b ) {
	for( int y=0; y<h; y++ ) {
		for( int x=0; x<w; x++ ) {
			pixel & p = buffer[y*w+x];
			p.r = r;
			p.g = g;
			p.b = b;
			if( p.r==0 && p.g==0 && p.b==0 ) {
				p.x = x*x;
				p.y = y*y;
				p.dist = float(x*x+y*y);
			}
			else {
				p.x = x;
				p.y = y;
				p.dist = 0;
			}
		}
	}
}




/** Open a small window showing the image. */
void Image::show( const char * title, int x, int y ) {
	if( title==NULL ) title = "MiniUI";
	MiniUI::AddWindow( this, title, x, y );
}







/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/



int main( int argc, char *argv[] ) {

	MiniUI::Init();

	Image * a = new Image;
	if( argv[1]==NULL ) {
		if( !a->open( "man.png" ) ) {
			piDebug( "--- Cannot open 'man.png'\n" );
			return 0;
		}
	}
	else {
		if( !a->open( argv[1] ) ) {
			piDebug( "--- Cannot open input image: '%s'\n", argv[1] );
			return 0;
		}
	}


	a->show( "original" );

	Image * b = a->clone();
	b->sed4();
	b->edt();
	b->show( "edt 4sed" );
	delete b;

	b = a->clone();
	b->sed8();
	b->edt();
	b->show( "edt 8sed" );
	delete b;

	b = a->clone();
	b->sed4();
	b->cuisenaire();
	b->edt();
	b->show( "edt cuisenaire" );
	delete b;

	delete a;


	while( MiniUI::Update() ) {
	};

	MiniUI::Shut();

	return 0;
}

