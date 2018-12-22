// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeQuad.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeQuad Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpNodeQuad.h>

#include <GPG5_gpNodeTexture.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::GPG5_gpNodeQuad
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeQuad::GPG5_gpNodeQuad():
GPG5_gpNode(),

lineWidth_ (1.0),

fillMode_ ( GL_LINE ),
faceMode_ (GL_FRONT_AND_BACK),

pTexture_ ( NULL ),
isEnabledTexture_ ( false )
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::~GPG5_gpNodeQuad
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeQuad::~GPG5_gpNodeQuad()
{
}

// +--------------------------
// |	Size
// +--------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::SetSize
// | Description     : 
// |
// | Parameters : 
// |   width         : 
// |   height        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeQuad::SetSize( double width, double height )
{
	cornerSW_ = GPG5_gpVector(0.0, 0.0, 0.0);
	cornerSE_ = GPG5_gpVector(width, 0.0, 0.0);
	cornerNE_ = GPG5_gpVector(width, height, 0.0);
	cornerNW_ = GPG5_gpVector(0.0, height, 0.0);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::GetSize
// | Description     : 
// |
// | Parameters : 
// |   width         : 
// |   height        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeQuad::GetSize( double& width, double& height ) const
{
	width	 = cornerSE_.GetX() - cornerNW_.GetX();
	height = cornerSE_.GetY() - cornerNW_.GetY();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::SetCorners
// | Description     : 
// |
// | Parameters : 
// |   cornerNW      : 
// |   cornerNE      : 
// |   cornerSE      : 
// |   cornerSW      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeQuad::SetCorners(const GPG5_gpVector& cornerNW, const GPG5_gpVector& cornerNE, const GPG5_gpVector& cornerSE,	const GPG5_gpVector& cornerSW)
{
	cornerNW_ = cornerNW;
	cornerNE_ = cornerNE;
	cornerSE_ = cornerSE;
	cornerSW_ = cornerSW;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::GetCorners
// | Description     : 
// |
// | Parameters : 
// |   cornerNW      : 
// |   cornerNE      : 
// |   cornerSE      : 
// |   cornerSW      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeQuad::GetCorners(GPG5_gpVector& cornerNW, GPG5_gpVector& cornerNE, GPG5_gpVector& cornerSE,	GPG5_gpVector& cornerSW) const
{
	cornerNW = cornerNW_;
	cornerNE = cornerNE_;
	cornerSE = cornerSE_;
	cornerSW = cornerSW_;
}

// +---------------------------
// |	Line Width
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::SetLineWidth
// | Description     : 
// |
// | Parameters : 
// |   lineWidth     : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeQuad::SetLineWidth(float lineWidth)
{
	lineWidth_ = lineWidth;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::GetLineWidth
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
float GPG5_gpNodeQuad::GetLineWidth() const
{
	return lineWidth_;
}

// +---------------------------
// |	Fill Mode
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::SetFillMode
// | Description     : 
// |
// | Parameters : 
// |   mode          : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeQuad::SetFillMode(unsigned int mode)
{
	fillMode_ = mode;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::GetFillMode
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
unsigned int GPG5_gpNodeQuad::GetFillMode() const
{
	return fillMode_;
}

// +---------------------------
// |	Face Mode
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::SetFaceMode
// | Description     : 
// |
// | Parameters : 
// |   faceMode      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeQuad::SetFaceMode(unsigned int faceMode)
{
	faceMode_ = faceMode;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::GetFaceMode
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
unsigned int GPG5_gpNodeQuad::GetFaceMode() const
{
	return faceMode_;
}

// +---------------------------
// |	Texture
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::SetTexture
// | Description     : 
// |
// | Parameters : 
// |   pTexture      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeQuad::SetTexture( GPG5_gpNodeTexture* pTexture )
{
	pTexture_ = pTexture;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::GetTexture
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeTexture* GPG5_gpNodeQuad::GetTexture() const
{
	return pTexture_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::EnableTexture
// | Description     : 
// |
// | Parameters : 
// |   isEnabledTexture : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeQuad::EnableTexture(bool isEnabledTexture)
{
	isEnabledTexture_ = isEnabledTexture;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::IsEnabledTexture
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpNodeQuad::IsEnabledTexture() const
{
	return isEnabledTexture_;
}

// +--------------------------------------------------
// |	Virtual Methods of GPG5_gpNode
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeQuad::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeQuad::Process()
{
	glPolygonMode(faceMode_, fillMode_);

	if ( isEnabledTexture_ )
	{
		glEnable(GL_TEXTURE_2D);
		if ( pTexture_ )
		{

			pTexture_->Apply();

			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3dv( cornerNW_.GetVector() );
		
				glTexCoord2f(1.0f, 0.0f);
				glVertex3dv( cornerNE_.GetVector() );
		
				glTexCoord2f(1.0f, 1.0f);
				glVertex3dv( cornerSE_.GetVector() );
		
				glTexCoord2f(0.0f, 1.0f);
				glVertex3dv( cornerSW_.GetVector() );
			glEnd();
		}

	}
	else
	{
		glDisable(GL_TEXTURE_2D);

		glLineWidth( lineWidth_ );
		
		glBegin(GL_QUADS);
			glVertex3dv( cornerNW_.GetVector() );
			glVertex3dv( cornerNE_.GetVector() );
			glVertex3dv( cornerSE_.GetVector() );
			glVertex3dv( cornerSW_.GetVector() );
		glEnd();
	}
}

