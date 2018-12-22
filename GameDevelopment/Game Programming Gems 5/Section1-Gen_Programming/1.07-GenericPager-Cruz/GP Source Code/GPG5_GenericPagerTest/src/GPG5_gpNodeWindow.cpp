// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeWindow.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeWindow Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpNodeWindow.h>

#include <GPG5_gpNodeManager.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWindow::GPG5_gpNodeWindow
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeWindow::GPG5_gpNodeWindow():
GPG5_gpNodeQuad(),
GPwindow(),
isActive_ (false),
speed_	(15.0f)
{
	originalColor_ = color_;
	originalLineWidth_ = lineWidth_;
	SetFillMode( GL_LINE );
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWindow::~GPG5_gpNodeWindow
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeWindow::~GPG5_gpNodeWindow()
{
}

// +---------------------------
// |	Active
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWindow::Active
// | Description     : 
// |
// | Parameters : 
// |   isActive      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWindow::Active(bool isActive)
{
	isActive_ = isActive;

	if ( isActive_ )
	{
		originalLineWidth_ = GetLineWidth();
		originalColor_ = GetColor();
		color_ = GPG5_gpVector(1.0, 1.0, 0.0);
		GPG5_gpNodeManager::GetInstance().GetEventManager().AddNode(this);	
	}
	else
	{
		lineWidth_ = originalLineWidth_;
			color_ = originalColor_;
		GPG5_gpNodeManager::GetInstance().GetEventManager().RemoveNode(this);	
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWindow::IsActive
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpNodeWindow::IsActive() const
{
	return isActive_;
}

// +--------------------------------------------------
// |	Virtual Methods of GPG5_gpNode
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWindow::SetPosition
// | Description     : 
// |
// | Parameters : 
// |   position      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWindow::SetPosition(const GPG5_gpVector& position)
{
	GPG5_gpNodeQuad::SetPosition( position );

	GPposition pos;
	pos.x_ = (GPunit) position_.GetX() + (currentIndex_.GetWidth()*(radius_+1));
	pos.y_ = (GPunit) position_.GetY() + (currentIndex_.GetHeight()*(radius_+1));

	Update( pos );
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWindow::ProcessEvent
// | Description     : 
// |
// | Parameters : 
// |   hwnd          : 
// |   message       : 
// |   wParam        : 
// |   lParam        : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpNodeWindow::ProcessEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_KEYDOWN:
			{
				switch (wParam)
				{
					case VK_F1:
						{
							std::string helpInfo;
							helpInfo += "\'F1\':\n";
							helpInfo += "\tHelp Window\n";
							helpInfo += "\n";
							helpInfo += "\'R\' or \'r\':\n";
							helpInfo += "\tResize Index\n";
							helpInfo += "\n";
							helpInfo += "\'Up\':\n";
							helpInfo += "\tMove Active Window Up\n";
							helpInfo += "\n";
							helpInfo += "\'Down\':\n";
							helpInfo += "\tMove Active Window Down\n";
							helpInfo += "\n";
							helpInfo += "\'Left\':\n";
							helpInfo += "\tMove Active Window Left\n";
							helpInfo += "\n";
							helpInfo += "\'Right\':\n";
							helpInfo += "\tMove Active Window Right\n";
							helpInfo += "\n";
							helpInfo += "\'Page Up\':\n";
							helpInfo += "\tIncrease Radius Active Window\n";
							helpInfo += "\n";
							helpInfo += "\'Page Down\':\n";
							helpInfo += "\tDecrease Radius Active Window\n";
							helpInfo += "\n";
							helpInfo += "\'+\':\n";
							helpInfo += "\tIncrease Speed\n";
							helpInfo += "\n";
							helpInfo += "\'-\':\n";
							helpInfo += "\tDecrease Speed\n";
							helpInfo += "\n";
							helpInfo += "\'Tab\':\n";
							helpInfo += "\tChange Active Window\n";
							helpInfo += "\n";
							helpInfo += "\'Esc\':\n";
							helpInfo += "\tExit\n";
							helpInfo += "\n";
							helpInfo += "\'Home\':\n";
							helpInfo += "\tZoom In Camera\n";
							helpInfo += "\n";
							helpInfo += "\'End\':\n";
							helpInfo += "\tZoom Out Camera\n";
							helpInfo += "\n";
							helpInfo += "\'Mouse Wheel\':\n";
							helpInfo += "\tZoom Camera\n";
							helpInfo += "\n";
							helpInfo += "\'Click And Drag\':\n";
							helpInfo += "\tMove Camera\n";
							helpInfo += "\n";

							MessageBox(hwnd, helpInfo.data(), "GPG5 Generic Pager Test", MB_OK);
							break;
						}
					case VK_TAB:
						{
							GPG5_gpNode* pNode = GPG5_gpNodeManager::GetInstance().GetNextNodeOfSameType(this,true);
							if ( pNode )
							{
								Active(false);
								
								GPG5_gpNodeWindow* pNextWindow = dynamic_cast<GPG5_gpNodeWindow*> (pNode);
								if ( pNextWindow )
								{
									pNextWindow->Active(true);
								}
							}
							break;
						}
					case VK_UP:
						{
							position_.SetY( position_.GetY() + speed_);
							UpdateGPwindow();
							break;
						}
						
					case VK_DOWN:
						{
							position_.SetY( position_.GetY() - speed_);
							UpdateGPwindow();
							break;
						}
					case VK_LEFT:
						{
							position_.SetX( position_.GetX() - speed_);
							UpdateGPwindow();
							break;
						}
					case VK_RIGHT:
						{
							position_.SetX( position_.GetX() + speed_);
							UpdateGPwindow();
							break;
						}
					case VK_ADD:
						{
							speed_ += 1.0f;
							break;
						}
					case VK_SUBTRACT:
						{
							speed_ -= 1.0f;
							break;
						}
					case 34: //VK_PAGE_UP
						{
							if ( radius_ < 10)
							{
								radius_++;
								UpdateRadius(radius_);
							}
							break;
						}
					case 33: //VK_PAGE_DOWN
						{
							if ( radius_ > 0)
							{
								radius_--;
								UpdateRadius(radius_);
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
// |
// | Function        : GPG5_gpNodeWindow::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWindow::Process()
{
	if ( isActive_ )
	{
		static float incr = 0.3f;
		if ( lineWidth_ >= 10.0f )
		{
			incr = -0.3f;
		}
		else		if ( lineWidth_ <= 1.0f )
		{
			incr = 0.3f;
		}
		lineWidth_ += incr;
	}

	GPG5_gpNodeQuad::Process();
}

// +--------------------------------------------------
// |	Virtual Methods of GPworld
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWindow::Init
// | Description     : 
// |										This method calls to the GPwindow::Init, 
// |										and sets the size and position of the 
// |										window frame
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWindow::Init(const GPindex& index)
{
	GPwindow::Init(index);
	
	double winWidth  = currentIndex_.GetWidth()*((radius_*2)+1);
	double winHeight = currentIndex_.GetHeight()*((radius_*2)+1);
	
	SetSize(winWidth, winHeight);
	
	position_.SetX( 
									currentIndex_.GetX() - (currentIndex_.GetWidth()*(radius_))
								);
		
	position_.SetY( 
									currentIndex_.GetY() - (currentIndex_.GetHeight()*(radius_))
								);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWindow::UpdateRadius
// | Description     : 
// |										This method calls to the GPwindow::UpdateRadius, 
// |										and updates the size and position of the 
// |										window frame
// |
// | Parameters : 
// |   radius        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWindow::UpdateRadius(int radius)
{
	GPwindow::UpdateRadius(radius);

	double winWidth  = currentIndex_.GetWidth()*((radius_*2)+1);
	double winHeight = currentIndex_.GetHeight()*((radius_*2)+1);
	
	SetSize(winWidth, winHeight);
	
	position_.SetX( 
									currentIndex_.GetX() - (currentIndex_.GetWidth()*radius_)
								);
		
	position_.SetY( 
									currentIndex_.GetY() - (currentIndex_.GetHeight()*radius_)
								);
}

// +-------------------------------------------------------------
// |	Protected Methods
// +-------------------------------------------------------------

// +---------------------------
// |	Update GP Window
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWindow::UpdateGPwindow
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWindow::UpdateGPwindow()
{
	GPposition pos;
	pos.x_ = (GPunit) position_.GetX() + (currentIndex_.GetWidth()*radius_) + (currentIndex_.GetWidth()/2.0);
	pos.y_ = (GPunit) position_.GetY() + (currentIndex_.GetHeight()*radius_) + (currentIndex_.GetHeight()/2.0);

	Update( pos );
}
