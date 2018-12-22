// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeAxis.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeAxis Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpNodeAxis.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeAxis::GPG5_gpNodeAxis
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeAxis::GPG5_gpNodeAxis():
GPG5_gpNode()

{
	lengths_.SetComponents(10.0, 10.0, 10.0);

	xColor_.SetComponents(1.0, 0.0, 0.0);
	yColor_.SetComponents(0.0, 1.0, 0.0);
	zColor_.SetComponents(0.0, 0.0, 1.0);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeAxis::~GPG5_gpNodeAxis
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeAxis::~GPG5_gpNodeAxis()
{
}

// +---------------------------
// |	Axis Lengths
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeAxis::SetLengths
// | Description     : 
// |
// | Parameters : 
// |   lengths       : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeAxis::SetLengths(const GPG5_gpVector& lengths)
{
	lengths_ = lengths;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeAxis::GetLenghts
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPG5_gpVector& GPG5_gpNodeAxis::GetLenghts() const
{
	return  lengths_;	
}

// +---------------------------
// |	Axis Colors
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeAxis::SetAxisColor
// | Description     : 
// |
// | Parameters : 
// |   xColor        : 
// |   yColor        : 
// |   zColor        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeAxis::SetAxisColor(const GPG5_gpVector& xColor, const GPG5_gpVector& yColor, const GPG5_gpVector& zColor)
{
	xColor_ = xColor;
	yColor_ = yColor;
	zColor_ = zColor;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeAxis::GetAxisColor
// | Description     : 
// |
// | Parameters : 
// |   xColor        : 
// |   yColor        : 
// |   zColor        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeAxis::GetAxisColor(GPG5_gpVector& xColor, GPG5_gpVector& yColor, GPG5_gpVector& zColor) const
{
	xColor = xColor_;
	yColor = yColor_;
	zColor = zColor_;
}

// +--------------------------------------------------
// |	Virtual Methods of GPG5_gpNode
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeAxis::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeAxis::Process()
{
	glColor3dv( xColor_.GetVector() );
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(lengths_.GetX(),0.0f,0.0f);
	glEnd();

	glColor3dv( yColor_.GetVector() );
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,lengths_.GetY(),0.0f);
	glEnd();

	glColor3dv( zColor_.GetVector() );
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,lengths_.GetZ());
	glEnd();

	glFlush();
}
