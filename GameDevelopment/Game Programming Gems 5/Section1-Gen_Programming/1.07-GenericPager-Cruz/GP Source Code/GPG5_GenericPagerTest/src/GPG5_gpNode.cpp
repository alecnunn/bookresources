// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNode.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNode Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpNode.h>

#include <GPG5_gpNodeManager.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::GPG5_gpNode
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNode::GPG5_gpNode():
name_				 ("Base Node"),
position_		 ( GPG5_gpVector( 0.0, 0.0, 0.0 ) ),
orientation_ ( GPG5_gpVector( 0.0, 0.0, 0.0 ) ),
color_			 ( GPG5_gpVector( 0.0, 0.0, 0.0 ) )
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::~GPG5_gpNode
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNode::~GPG5_gpNode()
{
	GPG5_gpNodeManager::GetInstance().RemoveNode(this);
}

// +---------------------------
// |	Name
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::SetName
// | Description     : 
// |
// | Parameters : 
// |   name          : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNode::SetName(const std::string& name)
{
	name_ = name;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::GetName
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const std::string& GPG5_gpNode::GetName() const
{
	return name_;
}

// +---------------------------
// |	Position
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::SetPosition
// | Description     : 
// |
// | Parameters : 
// |   position      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNode::SetPosition(const GPG5_gpVector& position)
{
	position_ = position;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::GetPosition
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPG5_gpVector& GPG5_gpNode::GetPosition() const
{
	return position_;
}

// +---------------------------
// |	Orientation
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::SetOrientation
// | Description     : 
// |
// | Parameters : 
// |   orientation   : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNode::SetOrientation(const GPG5_gpVector& orientation)
{
	orientation_ = orientation;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::GetOrientation
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPG5_gpVector& GPG5_gpNode::GetOrientation() const
{
	return orientation_;
}

// +---------------------------
// |	Color
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::SetColor
// | Description     : 
// |
// | Parameters : 
// |   color         : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNode::SetColor(const GPG5_gpVector& color)
{
	color_ = color;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::GetColor
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPG5_gpVector& GPG5_gpNode::GetColor() const
{
	return color_;
}

// +--------------------------------------------------
// |	Virtual Methods to Implement by a derived class
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::ProcessNode
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNode::ProcessNode()
{
	glPushAttrib(~0);
	glPushMatrix();

		glRotatef(orientation_.GetX(),1.0f,0.0f,0.0f);
		glRotatef(orientation_.GetY(),0.0f,1.0f,0.0f);
		glRotatef(orientation_.GetZ(),0.0f,0.0f,1.0f);
		glTranslatef(position_.GetX(), position_.GetY(), position_.GetZ() );

		glColor3dv( color_.GetVector() );
	
			Process();

	glPopMatrix();
	glPopAttrib();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNode::Process()
{
	//To Redefine
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNode::ProcessEvent
// | Description     : 
// |
// | Parameters : 
// |   hwnd          : 
// |   message       : 
// |   wParam        : 
// |   lParam        : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpNode::ProcessEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//To Redefine if its needed
	return true;
}

