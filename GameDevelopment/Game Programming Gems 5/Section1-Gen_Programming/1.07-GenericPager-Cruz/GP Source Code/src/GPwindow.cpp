// +-----------------------------------------------------------------------
// |
// |                         File Name: GPwindow.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This class contains the GPwindow Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPpch.h>
#include <GPwindow.h>

#include <GPworld.h>
#include <GPtile.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::GPwindow
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPwindow::GPwindow():
pWorld_	(NULL),
radius_ (1)
{
	tilesMap_.clear();
}

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::~GPwindow
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPwindow::~GPwindow()
{
	GPtileMap::iterator it		 = tilesMap_.begin();
	GPtileMap::iterator itEnd = tilesMap_.end();

	while ( it != itEnd )
	{
		GPtile* pTile = (it->second);
		it++;
		if ( pTile )
		{
			pTile->RemoveWindow( this );
		}
	}

	tilesMap_.clear();
}

// +---------------------------
// |	GPindex
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::Init
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
void GPwindow::Init(const GPindex& index)
{
	currentIndex_ = index;

	winPosition_.x_ = index.GetX();
	winPosition_.y_ = index.GetY();

	UpdateRadius(radius_);
}

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::ChangeIndex
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// |                 : 
// |   updateWindowPosition : 
// | 
// +-------------------------------------------------------------
void GPwindow::ChangeIndex(const GPindex& index, bool updateWindowPosition)
{
	Clean();
	currentIndex_ = index;

	if ( updateWindowPosition )
	{
		winPosition_.x_ = index.GetX();
		winPosition_.y_ = index.GetY();
	}
	
	UpdateRadius(radius_);
}

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::GetCurrentIndex
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPindex& GPwindow::GetCurrentIndex() const
{
	return currentIndex_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::IsInside
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
bool GPwindow::IsInside(const GPindex& index) const
{
	GPtileMap::const_iterator it = tilesMap_.find(index);
	
	if ( it != tilesMap_.end() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

// +---------------------------
// |	GPworld
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::SetWorld
// | Description     : 
// |
// | Parameters : 
// |   pWorld        : 
// | 
// +-------------------------------------------------------------
void GPwindow::SetWorld( GPworld* pWorld )
{
	pWorld_ = pWorld;
}

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::GetWorld
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPworld* GPwindow::GetWorld() const 
{
	return pWorld_;
}

// +---------------------------
// |	GPtiles
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::RemoveTile
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
void GPwindow::RemoveTile(const GPindex& index)
{
	GPtileMap::iterator it = tilesMap_.find( index );
	if ( it != tilesMap_.end() )
	{
		tilesMap_.erase(it);
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::GetTilesMap
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPtileMap& GPwindow::GetTilesMap() const
{
	return tilesMap_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::Clean
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPwindow::Clean()
{
	//Unload Unneeded Tiles
	GPtileMap::const_iterator itPrev		= tilesMap_.begin();
	GPtileMap::const_iterator itPrevEnd = tilesMap_.end();
	while ( itPrev != itPrevEnd )
	{
		GPtile* pTile = itPrev->second;
		pTile->RemoveWindow( this );
		pWorld_->UnloadTile( pTile );

		itPrev++;
	}

	tilesMap_.clear();
}

// +---------------------------
// |	Update
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::Update
// | Description     : 
// |
// | Parameters : 
// |   position      : 
// |   checkMovement : 
// | 
// +-------------------------------------------------------------
void GPwindow::Update(const GPposition& position, bool checkMovement)
{
	if ( pWorld_ )
	{
		winPosition_ = position;
	
		GPindex newIndex = currentIndex_.GetIndex( position.x_, position.y_);

		if ( 
				( checkMovement ) &&
				( newIndex == currentIndex_ ) 
			 )
		{
			//The new position is still in the Current Index, so do nothing
			return;
		}
		else
		{
			//Update the Current Index
			currentIndex_ = newIndex;
		}
		
		//Update Tiles Map
		GPtileMap previousTilesMap = tilesMap_;
		UpdateTilesMap();

		//Unload Unneeded Tiles
		GPtileMap::const_iterator itPrev		= previousTilesMap.begin();
		GPtileMap::const_iterator itPrevEnd = previousTilesMap.end();
		while ( itPrev != itPrevEnd )
		{
			GPtile* pTile = itPrev->second;
			if ( pTile )
			{
				if ( ! IsInside( pTile->GetIndex() ) )
				{
					pTile->RemoveWindow( this );
					pWorld_->UnloadTile( pTile );
				}
			}
			itPrev++;
		}

		//Check New Window
		GPtileMap::const_iterator it		= tilesMap_.begin();
		GPtileMap::const_iterator itEnd = tilesMap_.end();
		while ( it != itEnd )
		{
			GPtile* pTile = it->second;
			if ( pTile )
			{
				pWorld_->LoadTile( pTile );
			}
			it++;
		}
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::GetPosition
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPposition& GPwindow::GetPosition() const
{
	return winPosition_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::UpdateRadius
// | Description     : 
// |
// | Parameters : 
// |   radius          : 
// | 
// +-------------------------------------------------------------
void GPwindow::UpdateRadius(int radius)
{
	if ( radius < 0 )
	{
		radius_ = 0;
	}
	else
	{
		radius_ = radius;
	}
	Update( winPosition_ , false);
}

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::GetRadius
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
int GPwindow::GetRadius() const
{
	return radius_;
}

// +------------------------------------
// |	Protected Methods
// +------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPwindow::UpdateTilesMap
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPwindow::UpdateTilesMap( )
{
	if ( pWorld_ )
	{
		tilesMap_.clear();

		GPunit xFirstCorner = currentIndex_.PreviousX( radius_ );
		GPunit yFirstCorner = currentIndex_.PreviousY( radius_ );

		int sizeY = (radius_*2)+1;
		int sizeX = (radius_*2)+1;

		GPindex workIndex( currentIndex_ );
		workIndex.SetX(xFirstCorner);
		workIndex.SetY(yFirstCorner);

		while ( sizeY-- )
		{
			int workSizeX = sizeX;
			while ( workSizeX-- )
			{
				GPtile* pTile = pWorld_->GetTile( workIndex );
				if ( pTile )
				{
					pTile->AddWindow(this);
					tilesMap_.insert(GPtileMap::value_type(workIndex, pTile) );
				}
				workIndex.SetX(workIndex.NextX());
				
			}
			workIndex.SetX(xFirstCorner);
			workIndex.SetY(workIndex.NextY());
		}
	}
}
