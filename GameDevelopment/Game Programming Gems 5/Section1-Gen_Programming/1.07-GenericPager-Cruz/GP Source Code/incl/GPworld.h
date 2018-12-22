// +-----------------------------------------------------------------------
// |
// |                         File Name: GPworld.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This class contains the GPworld Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPWORLD_H
#define GPWORLD_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPdefs.h>
#include <GPtile.types.h>
#include <GPthreadObject.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPworld
// |  Description : 
// |								This class is the "World" of GP which contains all the
// |								GPtiles that are been used by all the GPwindows in the 
// |								system.
// |							
// |								The GPworld loads and unloads the GPtiles that the
// |								GPwindows needs. When a GPtile is unloaded and no other
// |								GPwindow needs it, the GPworld automatically destroys it
// |								in order to get a better performance and an optimal use of
// |								the system memory ( and other resources ).
// |
// |								The user of GP only has to implement the method:
// |
// |										virtual GPtile* BuildTile(const GPindex& index)
// |
// |								This method can be implemented so easy as:
// |
// |									 "return new GPuserDerivedTile();"
// |					
// |								or so complicated as the system requirements specify it.
// |
// |								The method:
// |						
// |										virtual bool IsValidIndex(const GPindex& index);
// |
// |								can also be implemented. This methods must returns true if an index 
// |								its valid in the current GPworld or false if not. If it is not valid,
// |								all the GPwindows will ignore it and the GPworld never will
// |								try to load or unload a GPtile specified by that GPindex.
// |
// +-----------------------------------------------------------------------
class _GP GPworld: public GPthreadObject
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPworld();
		virtual ~GPworld();

		// +---------------------------
		// |	GPtiles & GPindexes
		// +---------------------------
		GPindex GetIndex(const GPposition& position, const GPindex& index) const;
		const GPtileMap& GetTilesMap() const;
		GPtile* GetTile(const GPindex& index);
		void RemoveTile(const GPindex& index);
		virtual void DestroyTile(const GPindex& index);
		GPtile* ExistTile(const GPindex& index) const;
		void Clean();

		// +---------------------------
		// |	Load & UnLoad
		// +---------------------------
		void LoadTile(GPtile* pTile);
		void UnloadTile(GPtile* pTile);

		// +---------------------------
		// |	Update
		// +---------------------------
		void Update();

		// +--------------------------------------
		// |	Virtual Methods From GPthreadObject
		// +--------------------------------------
		virtual void Loop();
 
		// +--------------------------------------------------
		// |	Virtual Methods to Implement by a derived class
		// +--------------------------------------------------
		virtual bool IsValidIndex(const GPindex& index);
		virtual GPtile* BuildTile(const GPindex& index);

	protected:
		// +---------------------------
		// |	GPtiles
		// +---------------------------
		GPtileMap tilesMap_;

		// +---------------------------
		// |	Load & UnLoad
		// +---------------------------
		std::deque<GPtileListData> loadUnloadTileList_;
		
	private:
		// +---------------------------
		// |	Copy Constructor
		// +---------------------------
		GPworld(const GPworld& ref); //Not Implemented
};

#endif //GPWORLD_H
