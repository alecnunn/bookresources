#pragma once
#include <memory.h>

#include "World.hpp"
#include "Camera.hpp"

/*! A rectangular bitmap to store image sythesis results before plotting
 them to screen or saving them to file*/
class ScreenBuffer {
public:
	//! radiance map
	Color*	m_radiance;

	//! color map
	Color*	m_rgb;

	//! horizontal and vertical resolution
	long	m_width, m_height;

	//! total number of pixels
	long	m_nPixel;

	ScreenBuffer ();
	~ScreenBuffer ();

	//! performs radiance -> color map conversion
	void Syncronize ();

	//! create a screen buffer of the given size
	void Initialize (Camera* pCamera);

	//! flush the image with zeros
	void Clear () {	memset (m_radiance, '\0', sizeof(Color) * m_nPixel); }


	//! set pixel radiance
	void SetRadiance (int x, int y, const Color& radiance) {
		m_radiance [y * m_width + x] = radiance;
	}

	//! add radiance to pixel radiance
	void AccumulateRadiance (int x, int y, const Color& radiance) {
		m_radiance [y * m_width + x]+=radiance;
	}
	
	//! add radiance to pixel radiance
	void AccumulateRadiance (long pixelID, const Color& radiance) {
		m_radiance [pixelID]+=radiance;
	}

	//! save the colorMap (m_rgb) into a .tga file
	bool saveImage(const char* fname);
};
