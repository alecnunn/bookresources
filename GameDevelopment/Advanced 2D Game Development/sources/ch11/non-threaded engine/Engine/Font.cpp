
#include "Advanced2D.h"

//***mod--changed all floats to double

namespace Advanced2D {

	Font::Font() : Sprite()
	{
		//set character widths to default
		memset(&widths, 0, sizeof(widths));
	}
	
	void Font::Print(int x, int y, std::string text, int color)
	{ 
		double fx = x;
		double fy = y;

		//set font color
		this->setColor( color );

		//draw each character of the string
	    for (unsigned int n=0; n<text.length(); n++)
	    {
			int frame = (int)text[n];
			this->setCurrentFrame( frame );
			this->setX( fx );
			this->setY( fy );
			this->draw();

			//use proportional width if available
			if (widths[frame] == 0) widths[frame] = this->width;
			fx += widths[frame] * this->scaling;
	    }
	}

	bool Font::loadWidthData(std::string filename)
	{
		unsigned char buffer[512];
	
		//open font width data file
		std::ifstream infile;
		infile.open(filename.c_str(), std::ios::binary);
		if (!infile) return false;
	
		//read 512 bytes (2 bytes per character)
		infile.read( (char *)(&buffer), 512 );
		if (infile.bad()) return false;
		infile.close();        
	
		//convert raw data to proportional width data
		for (int n=0; n<256; n++) {
			widths[n] = (int)buffer[n*2];
		}
	
		return true;
	}
	



};
