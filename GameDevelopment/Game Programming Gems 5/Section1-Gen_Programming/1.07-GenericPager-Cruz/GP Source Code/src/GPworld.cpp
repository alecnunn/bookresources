// +-----------------------------------------------------------------------
// |
// |                         File Name: GPworld.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This class contains the GPworld Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPpch.h>
#include <GPworld.h>

#include <GPsync.h>
#include <GPtile.h>
#include <assert.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPworld::GPworld
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPworld::GPworld():
GPthreadObject()
{
	GPsync::GetInstance().Lock();
	
	tilesMap_.clear();

	loadUnloadTileList_.clear();

	Start();

	GPsync::GetInstance().UnLock();
}

// +-------------------------------------------------------------
// |
// | Function        : GPworld::~GPworld
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPworld::~GPworld()
{
	GPsync::GetInstance().Lock();

	Finish();

	Clean();

	GPsync::GetInstance().UnLock();
}

// +---------------------------
// |	GPtiles & GPindexes
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPworld::GetIndex
// | Description     : 
// |
// | Parameters : 
// |   position      : 
// |   index         : 
// | 
// +-------------------------------------------------------------
GPindex GPworld::GetIndex(const GPposition& position, const GPindex& index) const
{
	GPindex indexPos = index.GetIndex( position.x_, position.y_);
	
	return indexPos;
}

// +-------------------------------------------------------------
// |
// | Function        : GPworld::GetTilesMap
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPtileMap& GPworld::GetTilesMap() const
{
	return tilesMap_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPworld::GetTile
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
GPtile* GPworld::GetTile(const GPindex& index)
{
	GPsync::GetInstance().Lock();

	GPtile* pTile = NULL;

	if ( IsValidIndex(index) )
	{
		GPtileMap::const_iterator it = tilesMap_.find(index);
		if ( it == tilesMap_.end() )
		{
			pTile = BuildTile( index );
			if ( pTile )
			{
				pTile->SetIndex( index );
				pTile->SetWorld( this );
				tilesMap_.insert(GPtileMap::value_type(index, pTile) );
			}
		}
		else
		{	
			pTile = it->second;
		}
	}

	GPsync::GetInstance().UnLock();

	return pTile;
}

// +-------------------------------------------------------------
// |
// | Function        : GPworld::RemoveTile
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
void GPworld::RemoveTile(const GPindex& index)
{
	GPsync::GetInstance().Lock();

	GPtileMap::iterator it = tilesMap_.find( index );

	if ( it != tilesMap_.end() )
	{
		tilesMap_.erase( it );
	}

	GPsync::GetInstance().UnLock();
}

// +-------------------------------------------------------------
// |
// | Function        : GPworld::DestroyTile
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
void GPworld::DestroyTile(const GPindex& index)
{
	GPsync::GetInstance().Lock();

	GPtileMap::iterator itMap = tilesMap_.find( index );

	if ( itMap != tilesMap_.end() )
	{
		GPtile* pTile = itMap->second;
		pTile->SetWorld( NULL );
		

		bool found = false;
		std::deque<GPtileListData>::iterator it		= loadUnloadTileList_.begin();
		std::deque<GPtileListData>::iterator itEnd = loadUnloadTileList_.end();
		while (( it != itEnd) && (!found))
		{
			if ( pTile == (*it).pTile_ )
			{
				found = true;
			}
			else
			{
				it++;
			}
		}
		if ( found )
		{
			loadUnloadTileList_.erase( it );
		}
		
		tilesMap_.erase( itMap );
		
		pTile->DoUnload();
		delete ( pTile );
	}

	GPsync::GetInstance().UnLock();
}

// +-------------------------------------------------------------
// |
// | Function        : GPworld::ExistTile
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
GPtile* GPworld::ExistTile(const GPindex& index) const
{
	GPsync::GetInstance().Lock();

	GPtile* pTile = NULL;
	GPtileMap::const_iterator it = tilesMap_.find( index );

	if ( it != tilesMap_.end() )
	{
		pTile = it->second;
	}

	GPsync::GetInstance().UnLock();

	return pTile;
}


// +-------------------------------------------------------------
// |
// | Function        : GPworld::Clean
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPworld::Clean()
{
	GPsync::GetInstance().Lock();

	loadUnloadTileList_.clear();

	GPtileMap::iterator it		 = tilesMap_.begin();
	GPtileMap::iterator itEnd = tilesMap_.end();

	while ( it != itEnd )
	{
		GPtile* pTile = (it->second);
		it++;
		pTile->DoUnload();
		pTile->SetWorld( NULL );
		delete ( pTile ); //This destructor call to the windows
		pTile = NULL;
	}

	tilesMap_.clear();
	GPsync::GetInstance().UnLock();
}

// +---------------------------
// |	Load & UnLoad
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPworld::LoadTile
// | Description     : 
// |
// | Parameters : 
// |   pTile         : 
// | 
// +-------------------------------------------------------------
void GPworld::LoadTile(GPtile* pTile)
{
	GPsync::GetInstance().Lock();

	if ( pTile )
	{
		bool found = false;
		std::deque<GPtileListData>::iterator it		= loadUnloadTileList_.begin();
		std::deque<GPtileListData>::iterator itEnd = loadUnloadTileList_.end();
		while (( it != itEnd) && (!found))
		{
			if ( pTile == (*it).pTile_ )
			{
				found = true;
			}
			else
			{
				it++;
			}
		}
		if ( found )
		{
			if ( (*it).pTile_->GetState() == GP_LOADED)
			{
				loadUnloadTileList_.erase(it);
				//We ignore if other window had set it to unload;
			}
			else if ( (*it).state_ == GP_FOR_UNLOAD )
			{
				(*it).state_ = GP_FOR_LOAD;
			}
			else if ( (*it).state_ == GP_FOR_LOAD )
			{
				//Do Nothing
			}
		}
		else
		{
			GPtileListData tileListData;
			tileListData.state_ = GP_FOR_LOAD;
			tileListData.pTile_ = pTile;
			
			if (! loadUnloadTileList_.size())
			{
				Start();
			}

			loadUnloadTileList_.push_back( tileListData );
		}
	}

	GPsync::GetInstance().UnLock();
}

// +-------------------------------------------------------------
// |
// | Function        : GPworld::UnloadTile
// | Description     : 
// |
// | Parameters : 
// |   pTile         : 
// | 
// +-------------------------------------------------------------
void GPworld::UnloadTile(GPtile* pTile)
{
	GPsync::GetInstance().Lock();

	if ( pTile )
	{
		bool found = false;
		std::deque<GPtileListData>::iterator it		= loadUnloadTileList_.begin();
		std::deque<GPtileListData>::iterator itEnd = loadUnloadTileList_.end();
		while (( it != itEnd) && (!found))
		{
			if ( pTile == (*it).pTile_ )
			{
				found = true;
			}
			else
			{
				it++;
			}
		}
		if ( found )
		{
			if ( (*it).pTile_->GetState() == GP_UNLOADED)
			{
				if ( (*it).pTile_->GetWindowListSize() == 0)
				{
					loadUnloadTileList_.erase(it);
				}
				else
				{
					if ( (*it).state_ == GP_FOR_LOAD )
					{
						//Load -> Always priorize LOAD 
						//Do Nothing	
					}
					else if ( (*it).state_ == GP_FOR_UNLOAD )
					{
						loadUnloadTileList_.erase(it);
					}
				}
			}
			else if ( (*it).state_ == GP_FOR_LOAD )
			{
				(*it).state_ = GP_FOR_UNLOAD;
			}
			else if ( (*it).state_ == GP_FOR_UNLOAD )
			{
				//Do Nothing
			}
		}
		else
		{
			GPtileListData tileListData;
			tileListData.state_ = GP_FOR_UNLOAD;
			tileListData.pTile_ = pTile;
			
			if (! loadUnloadTileList_.size())
			{
				Start();
			}

			loadUnloadTileList_.push_back( tileListData );
		}
	}

	GPsync::GetInstance().UnLock();
}

// +---------------------------
// |	Update
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPworld::Update
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPworld::Update()
{
	if ( loadUnloadTileList_.size() )
	{
		GPsync::GetInstance().Lock();

		GPtileListData tileListData = (*loadUnloadTileList_.begin());
		loadUnloadTileList_.erase(loadUnloadTileList_.begin());

		if ( tileListData.state_ == GP_FOR_LOAD )
		{
			tileListData.pTile_->DoLoad();
		}
		else if ( tileListData.state_ == GP_FOR_UNLOAD )
		{
			tileListData.pTile_->DoUnload();
			if ( ! tileListData.pTile_->GetWindowListSize() )
			{
				DestroyTile( tileListData.pTile_->GetIndex() );
			}
		}

		GPsync::GetInstance().UnLock();
	}
}

// +--------------------------------------
// |	Virtual Methods From GPthreadObject
// +--------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPworld::Loop
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPworld::Loop()
{
	if ( ! loadUnloadTileList_.size() )
	{
		Pause();
	}
	else
	{
		Update();
	}
	

	Sleep(0);
}

// +--------------------------------------------------
// |	Virtual Methods to Implement by a derived class
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPworld::IsValidIndex
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
bool GPworld::IsValidIndex(const GPindex& index)
{
	//To Redefine if it is needed
	return true;
}

// +-------------------------------------------------------------
// |
// | Function        : GPworld::BuildTile
// | Description     : 
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
GPtile* GPworld::BuildTile(const GPindex& index)
{
	GPtile* pTile = new GPtile();
	return pTile;
}


 
