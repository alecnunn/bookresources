// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpCamera2D.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpCamera2D Class Implemantation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpCamera2D.h>

#include <GPG5_gpNodeManager.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::GPG5_gpCamera2D
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpCamera2D::GPG5_gpCamera2D():
GPG5_gpNode(),
eye_ ( GPG5_gpVector( 0.0, 0.0, 0.0 ) ),
up_	 ( GPG5_gpVector( 0.0, 1.0, 0.0 ) ),
translationSpeed_ ( 1.0f),
zoomSpeed_ ( 100.0f),
distance_ (0.0),

deltaX_			(0),
deltaY_			(0),
oldXMouse_	(0),
oldYMouse_	(0),

leftPressed_	 (false),
middlePressed_ (false),
rightPressed_	 (false)

{
	view_ = up_;
	Update();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::~GPG5_gpCamera2D
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpCamera2D::~GPG5_gpCamera2D()
{
}

// +---------------------------
// |	Position
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::Situate
// | Description     : 
// |
// | Parameters : 
// |   position      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::Situate(const GPG5_gpVector& position)
{
	position_ = position;
	eye_ = position_;
	eye_.SetZ(0.0);

	Update();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::SetPosition
// | Description     : 
// |
// | Parameters : 
// |   position      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::SetPosition(const GPG5_gpVector& position)
{
	position_ = position;

	Update();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::SetEye
// | Description     : 
// |
// | Parameters : 
// |   eye           : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::SetEye(const GPG5_gpVector& eye)
{
	eye_ = eye;

	Update();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::GetEye
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPG5_gpVector& GPG5_gpCamera2D::GetEye() const
{
	return eye_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::SetUp
// | Description     : 
// |
// | Parameters : 
// |   up            : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::SetUp(const GPG5_gpVector& up)
{
	up_ = up;

	Update();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::GetUp
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPG5_gpVector& GPG5_gpCamera2D::GetUp() const
{
	return up_;
}

// +---------------------------
// |	Control
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::Move
// | Description     : 
// |
// | Parameters : 
// |   speed         : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::Move(float speed)
{
	GPG5_gpVector work( view_ );
	work *= speed;
	position_+= work;
	eye_+= work;

	Update();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::Strafe
// | Description     : 
// |
// | Parameters : 
// |   speed         : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::Strafe(float speed)
{
	GPG5_gpVector work( right_ );
	work *= speed;
	position_+= work;
	eye_+= work;

	Update();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::Zoom
// | Description     : 
// |
// | Parameters : 
// |   speed         : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::Zoom(float speed)
{
	GPG5_gpVector work(eye_-position_);
	work.Normalize();
	work*=speed;

	GPG5_gpVector workView(eye_-(position_ + work) );
	double distance = workView.Length();

	if ( (distance > 1.0) && (distance < 3000.0) )
	{
		position_ += work;

		Update();
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::Move
// | Description     : 
// |
// | Parameters : 
// |   dir           : 
// |   speed         : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::Move(const GPG5_gpVector& dir, float speed)
{
	GPG5_gpVector work( dir );
	work *= speed;
	position_+= work;
	eye_+= work;
}

// +--------------------------------------------------
// |	Virtual Methods of GPG5_gpNode
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::ProcessNode
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::ProcessNode()
{
	Process();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::Process()
{
	gluLookAt(
		position_.GetX(), position_.GetY(), position_.GetZ(),
		eye_.GetX(), eye_.GetY(), eye_.GetZ(),
		up_.GetX(), up_.GetY(), up_.GetZ()
		);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::ProcessEvent
// | Description     : 
// |
// | Parameters : 
// |   hwnd          : 
// |   message       : 
// |   wParam        : 
// |   lParam        : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpCamera2D::ProcessEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_MOUSEWHEEL:
			{
				short zDelta = HIWORD(wParam);
				if ( zDelta > 0)
				{
					Zoom(zoomSpeed_);
				}
				else
				{
					Zoom(-zoomSpeed_);
				}

				break;
			}
		case WM_MOUSEMOVE:
			{
				if ( leftPressed_ )
				{
					deltaX_ = LOWORD(lParam) - oldXMouse_;
					deltaY_ = HIWORD(lParam) - oldYMouse_;
					
					Move(deltaY_*(distance_/700.0f));
					Strafe(-deltaX_*(distance_/700.0f));

					oldXMouse_ = LOWORD(lParam);
					oldYMouse_ = HIWORD(lParam);

				}

				break;
			}
		case WM_LBUTTONDOWN:
			{
				leftPressed_ = true;

				SetCapture(hwnd);
		
				oldXMouse_ = LOWORD(lParam);
				oldYMouse_ = HIWORD(lParam);
				
				break;
			}
		case WM_LBUTTONUP:
			{
				leftPressed_ = false;

				ReleaseCapture();

				break;
			}
		case WM_MBUTTONDOWN:
			{
				middlePressed_ = true;
				break;
			}
		case WM_MBUTTONUP:
			{
				middlePressed_ = false;
				break;
			}
		case WM_RBUTTONDOWN:
			{
				rightPressed_ = true;

				SetCapture(hwnd);

				oldXMouse_ = LOWORD(lParam);
				oldYMouse_ = HIWORD(lParam);

				break;
			}
		case WM_RBUTTONUP:
			{
				rightPressed_ = false;

				ReleaseCapture();

				break;
			}
		case WM_KEYDOWN:
			{
				switch (wParam)
				{
					case VK_HOME:
						{
							Zoom(zoomSpeed_);
							break;
						}
					case VK_END:
						{
							Zoom(-zoomSpeed_);
							break;
						}

					case 'r':
					case 'R':
						{
							static bool smallSize = true;
							std::string sizeMsg;
							if ( smallSize )
							{
								sizeMsg = std::string("20x20");
							}
							else
							{
								sizeMsg = std::string("100x100");
							}

							std::string infoMsg =  std::string("The size of index is going to be changed to " + sizeMsg + ". Do you want to Continue?");
							if ( MessageBox(hwnd, infoMsg.data(), "GPG5 Generic Pager Test", MB_YESNO) == IDYES )
							{
								if ( smallSize )
								{
									GPG5_gpNodeManager::GetInstance().ReInit(20,20);
								}
								else
								{
									GPG5_gpNodeManager::GetInstance().ReInit(100, 100);
								}

								smallSize = ! smallSize;
							}					
							break;
						}
				}
				break;
			}
	}
	return true;
}

// +-------------------------------------------------------------
// |	Protected Methods
// +-------------------------------------------------------------

// +---------------------------
// |	Position
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpCamera2D::Update
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpCamera2D::Update()
{
	GPG5_gpVector work(eye_-position_);
	distance_ = work.Length();
	right_.Cross(work, view_);
	right_.Normalize();
}

