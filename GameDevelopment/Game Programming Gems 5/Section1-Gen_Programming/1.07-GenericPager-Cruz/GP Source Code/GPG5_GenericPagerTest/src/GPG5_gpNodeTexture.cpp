// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeTexture.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeTexture Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpNodeTexture.h>

#include <GPG5_gpNodeManager.h>
#include <fstream.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::GPG5_gpNodeTexture
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeTexture::GPG5_gpNodeTexture():
GPG5_gpNode(),

path_						(""),
id_							(-1),
numComponents_	( 3 ),
totalWidth_			( 0 ),
totalHeight_		( 0 ),
startX_					( 0 ),
endX_						( 0 ),
startY_					( 0 ),
endY_						( 0 ),
loadWidth_			( 0 ),
loadHeight_			( 0 ),
isSetup_				( false ),
data_						( 0 )
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::~GPG5_gpNodeTexture
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeTexture::~GPG5_gpNodeTexture()
{
	glDeleteTextures(1, &id_);
	CleanData();
}

// +---------------------------
// |	Setup & Load
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::Setup
// | Description     : 
// |
// | Parameters : 
// |   path          : 
// |   numComponents : 
// |   totalWidth    : 
// |   totalHeight   : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeTexture::Setup(const std::string& path, int numComponents, int totalWidth, int totalHeight)
{
	path_					 = path;
	numComponents_ = numComponents;
	totalWidth_		 = totalWidth;
	totalHeight_	 = totalHeight;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::Load
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeTexture::Load()
{
	Load(0, totalWidth_-1, 0, totalHeight_-1);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::Load
// | Description     : 
// |
// | Parameters : 
// |   startX        : 
// |   endX          : 
// |   startY        : 
// |   endY          : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeTexture::Load(int startX, int endX, int startY, int endY )
{
	startX_ = startX;
	endX_		= endX;

	startY_ = startY;
	endY_		= endY;

	loadWidth_  = (endX+1) - startX;
	loadHeight_ = (endY+1) - startY;

	CleanData();

	fstream file;
  
	data_ = new unsigned char[loadWidth_*loadHeight_*numComponents_];
	unsigned char* workImage = data_;

	file.open(path_.data(), ios::in | ios::nocreate | ios::binary);

	if ( ! file.is_open() )
	{
		CleanData();
		std::string errorMsg =  std::string("File: ") + path_ + std::string(" was not found. Do you want to continue?");
		if ( MessageBox(NULL, errorMsg.data(), "GPG5 Generic Pager Test", MB_YESNO) == IDNO )
		{
			GPG5_gpNodeManager::GetInstance().Terminate();
		}
	}

	int numBytesPerFile = (loadWidth_)*numComponents_;
	int offsetX = (totalWidth_ * numComponents_) - ( (startX* numComponents_) + numBytesPerFile );

	//Seeks Y Position
	file.seekg( (startY*totalWidth_*numComponents_), ios::beg);

	for(int col = 0; col < loadHeight_ ; col++)
	{
		//Seeks X Position
		file.seekg(startX_*numComponents_, ios::cur);

		file.read(workImage, numBytesPerFile);
		workImage += numBytesPerFile;

		file.seekg(offsetX, ios::cur);
	}

	file.close();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::Set
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeTexture::Set()
{
	if ( ! isSetup_ )
	{
		glGenTextures(1, &id_);
		glBindTexture(GL_TEXTURE_2D, id_);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

		GLenum glFormat;

		switch( numComponents_ )
		{
			case 1:
				{
					glFormat = GL_LUMINANCE;
					break;
				}
			case 3:
				{
					glFormat = GL_RGB;
					break;
				}
			case 4:
				{
					glFormat = GL_RGBA;
					break;
				}
		}

		if ( data_ )
		{
			/*
			// +--------------------
			// |	Mode A
			// +--------------------
			
			glTexImage2D(
										GL_TEXTURE_2D, 
										0,
										numComponents_,
										loadWidth_,
										loadHeight_,
										0,
										glFormat,
										GL_UNSIGNED_BYTE,
										data_
									);
			CleanData();
			*/
			/*
			// +--------------------
			// |	Mode B
			// +--------------------
			gluBuild2DMipmaps(
												GL_TEXTURE_2D,
												numComponents_,
												loadWidth_,
												loadHeight_,
												glFormat,
												GL_UNSIGNED_BYTE,
												data_
											);
			CleanData();
			*/
			
			// +--------------------
			// |	Mode C
			// +--------------------
			int newWidth  = GetNearPowerOf2( loadWidth_ );
			int newHeight = GetNearPowerOf2( loadHeight_);

			if(
					(newWidth	 == loadWidth_) &&
					(newHeight == loadHeight_)
				)
			{
				//Both are already PowerOf2
				glTexImage2D(
											GL_TEXTURE_2D, 
											0,
											numComponents_,
											loadWidth_,
											loadHeight_,
											0,
											glFormat,
											GL_UNSIGNED_BYTE,
											data_
										);
				CleanData();
			}
			else
			{
				unsigned char* newData = new unsigned char[newWidth*newHeight*numComponents_];
				gluScaleImage
					(
						glFormat,
						loadWidth_,
						loadHeight_,
						GL_UNSIGNED_BYTE,
						data_,
						newWidth,
						newHeight,
						GL_UNSIGNED_BYTE,
						newData
					);
				CleanData();

				glTexImage2D(
											GL_TEXTURE_2D, 
											0,
											numComponents_,
											newWidth,
											newHeight,
											0,
											glFormat,
											GL_UNSIGNED_BYTE,
											newData
										);
				delete[](newData);
				newData = NULL;
			}
			
		}
		isSetup_ = true;
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::IsSet
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpNodeTexture::IsSet() const
{
	return isSetup_;
}

// +---------------------------
// |	Apply
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::Apply
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeTexture::Apply()
{
	Set();
	glBindTexture( GL_TEXTURE_2D, id_);
}

// +---------------------------
// |	Data
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::GetData
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
unsigned char* GPG5_gpNodeTexture::GetData() const
{
	return data_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::CleanData
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeTexture::CleanData()
{
	delete[] ( data_ );
	data_ = 0;
}

// +--------------------------------------------------
// |	Virtual Methods of GPG5_gpNode
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeTexture::Process()
{
	//Do Nothing
}

// +-------------------------------------------------------------
// |	Protected Methods
// +-------------------------------------------------------------

// +---------------------------
// |	Power of 2
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTexture::GetNearPowerOf2
// | Description     : 
// |
// | Parameters : 
// |   number        : 
// | 
// +-------------------------------------------------------------
int GPG5_gpNodeTexture::GetNearPowerOf2(int number) const
{
	int powerOf2 = 1;

	bool found = false;
	int work = 1;
	int power = 1;

	while ( work < number )
	{
		work *= 2;
		power ++;
	}

	powerOf2 = work;
	
	int previous = work / 2;
	if ( (number - previous) < (work - number ) )
	{
		powerOf2 = previous;
	}

	return powerOf2;
}
