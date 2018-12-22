// +-----------------------------------------------------------------------
// |
// |                         File Name: GPtile.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPtile Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPpch.h>
#include <GPtile.h>

#include <GPworld.h>
#include <GPwindow.h>
#include <GPsync.h>
#include <algorithm>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPtile::GPtile
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPtile::GPtile():
pWorld_ (NULL),
state_  (GP_UNLOADED)
{
	GPsync::GetInstance().Lock();

	windowList_.clear();

	GPsync::GetInstance().UnLock();
}

// +-------------------------------------------------------------
// |
// | Function        : GPtile::~GPtile
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPtile::~GPtile()
{
	GPsync::GetInstance().Lock();

	if ( pWorld_ )
	{
		pWorld_->RemoveTile(index_);
	}

	GPwindowList::iterator it		= windowList_.begin();
	GPwindowList::iterator itEnd = windowList_.end();
	while ( it != itEnd )
	{
		(*it)->RemoveTile(index_);
		it++;
	}
	windowList_.clear();

	GPsync::GetInstance().UnLock();
}

// +---------------------------
// |	GPworld
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPtile::SetWorld
// | Description     : 
// |
// | Parameters : 
// |   pWorld        : 
// | 
// +-------------------------------------------------------------
void GPtile::SetWorld(GPworld* pWorld)
{
	pWorld_ = pWorld;
}

// +-------------------------------------------------------------
// |
// | Function        : GPtile::GetWorld
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPworld* GPtile::GetWorld()
{
	return pWorld_;
}

// +---------------------------
// |	GPwindows
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPtile::AddWindow
// | Description     : 
// |
// | Parameters : 
// |   pWindow       : 
// | 
// +-------------------------------------------------------------
void GPtile::AddWindow(GPwindow* pWindow)
{
	GPsync::GetInstance().Lock();

	GPwindowList::iterator it = std::find(windowList_.begin(), windowList_.end(), pWindow);
	
	if ( it == windowList_.end() )
	{
		windowList_.push_back( pWindow );
	}

	GPsync::GetInstance().UnLock();
}

// +-------------------------------------------------------------
// |
// | Function        : GPtile::RemoveWindow
// | Description     : 
// |
// | Parameters : 
// |   pWindow       : 
// | 
// +-------------------------------------------------------------
void GPtile::RemoveWindow(GPwindow* pWindow)
{
	GPsync::GetInstance().Lock();

	GPwindowList::iterator it = std::find(windowList_.begin(), windowList_.end(), pWindow);
	
	if ( it != windowList_.end() )
	{
		windowList_.erase( it );
	}

	GPsync::GetInstance().UnLock();
}

// +-------------------------------------------------------------
// |
// | Function        : GPtile::GetWindowList
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPwindowList& GPtile::GetWindowList()
{
	GPsync::GetInstance().Lock();
	GPsync::GetInstance().UnLock();
	return windowList_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPtile::GetWindowListSize
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
int GPtile::GetWindowListSize()
{
	GPsync::GetInstance().Lock();
	GPsync::GetInstance().UnLock();
	return windowList_.size();
}

// +---------------------------
// |	State
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPtile::SetState
// | Description     : 
// |
// | Parameters : 
// |   state         : 
// | 
// +-------------------------------------------------------------
void GPtile::SetState( GPtileState state)
{
	GPsync::GetInstance().Lock();
	GPsync::GetInstance().UnLock();
	state_ = state;
}

// +-------------------------------------------------------------
// |
// | Function        : GPtile::GetState
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPtileState GPtile::GetState()
{
	GPsync::GetInstance().Lock();
	GPsync::GetInstance().UnLock();
	return state_;
}

// +---------------------------
// |	GPindex
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPtile::SetIndex
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
void GPtile::SetIndex(const GPindex& index)
{
	GPsync::GetInstance().Lock();
	GPsync::GetInstance().UnLock();
	index_ = index;
}

// +-------------------------------------------------------------
// |
// | Function        : GPtile::GetIndex
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPindex& GPtile::GetIndex()
{
	GPsync::GetInstance().Lock();
	GPsync::GetInstance().UnLock();
	return index_;
}

// +---------------------------
// |	Load & Unload
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPtile::DoLoad
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPtile::DoLoad()
{
	GPsync::GetInstance().Lock();

	if (
			( state_ != GP_LOADED)  &&
			( state_ != GP_LOADING)
		 )
	{
		state_ = GP_LOADING;
		
		Load();
		
		state_ = GP_LOADED;
	}

	GPsync::GetInstance().UnLock();
}

// +-------------------------------------------------------------
// |
// | Function        : GPtile::DoUnload
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPtile::DoUnload()
{
	GPsync::GetInstance().Lock();

	if (
			( state_ != GP_UNLOADED)  &&
			( state_ != GP_UNLOADING)
		 )
	{
		if ( ! windowList_.size() )
		{
			state_ = GP_UNLOADING;
			
			Unload();
			
			state_ = GP_UNLOADED;
		}
	}

	GPsync::GetInstance().UnLock();
}

// +--------------------------------------------------
// |	Virtual Methods to Implement by a derived class
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPtile::Load
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPtile::Load()
{
	//To Redefine
}

// +-------------------------------------------------------------
// |
// | Function        : GPtile::Unload
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPtile::Unload()
{
	//To Redefine
}
