// Graphics Programming Methods
// Metropolis sampling in global illumination
// György Antal, Lászlo Szirmay-Kalos
// 2003.

// builds the Win32 application (based on WinMain())

#pragma once

#include <vector>
#include "Vector.hpp"
#include "Material.hpp"
#include "Scene.hpp"

// The Image class represents the image to be drawn.
// We need this class for applying tone mapping and 
// clamp the radiance values to the range of [0..255] of the Rgb values.
//-----------------------------------------------------------------
class Image {					
	short	width, height;			// widht and height of the image	
	Color**	pixRad;					// pixRad[x][y] is the radiance of the pixel

public:

	//-----------------------------------------------------------------
	Image (short ww, short hh) {	// constructor
		width	= ww;
		height	= hh;
		pixRad	= new Color*[width];
		for (short x = 0; x < width; x++)
			pixRad[x] = new Color[height];
		Clear();
	};

	//-----------------------------------------------------------------
	~Image () {						// destructor
	}

	// clear the pixel values
	//-----------------------------------------------------------------
	void Clear (void) {
		for (short x = 0; x < width; x++) {
			for (short y = 0; y < height; y++)
				pixRad[x][y].Set(0, 0, 0);
		}
	};


	// calculate the average pixel luminance
	//-----------------------------------------------------------------
	double GetAvgPixelLum (void) {	
		double sumPixelLum = 0.0;
		for (short x = 0; x < width; x++) {
			for (short y = 0; y < height; y++) {
				sumPixelLum += pixRad[x][y].Lum();
			}
		}
		return sumPixelLum / width / height;
	};

	// set the radiance value for the pixel
	//-----------------------------------------------------------------
	void SetPixelRad (short x, short y, const Color& col) {	
		double	colorLum = col.Lum();
		assert (!_isnan (colorLum) && _finite(colorLum) && (colorLum >= 0));
		pixRad[x][y] = col; 
	};

	// add the radiance value for the pixel
	//-----------------------------------------------------------------
	void AddPixelRad (short x, short y, const Color& col) { 
		double	colorLum = col.Lum();
		assert (!_isnan (colorLum) && _finite(colorLum) && (colorLum >= 0));
		pixRad[x][y] += col; 
	};

	// get the radiance value for the pixel
	//-----------------------------------------------------------------
	Color	GetPixelRad	(short x, short y) const { return pixRad[x][y]; };
};
