// +-----------------------------------------------------------------------
// |
// |                         File Name: GPwindow.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This class contains the GPwindow Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPWINDOW_H
#define GPWINDOW_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPdefs.h>
#include <GPtile.types.h>

// +---------------------------
// |	Forward Declarations
// +---------------------------
class GPworld;

// +-----------------------------------------------------------------------
// |
// |  Class       : GPwindow
// |  Description : 
// |								This class is the "Navigator" of the GP. It uses the
// |								GPindex to navigate through the GPworld. This class also
// |								tells to the GPworld which GPtiles must be loaded and which
// |								of them must be unloaded.
// |		
// |								There can be several GPwindows in the world at the same 
// |								time, each of them loading and unloading any GPtile ( free
// |								or not). The GP automatically manages all of them. The user
// |								of GP does not have to take care of anything.
// |
// +-----------------------------------------------------------------------
class _GP GPwindow
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPwindow();
		virtual ~GPwindow();
			
		// +---------------------------
		// |	GPindex
		// +---------------------------
		virtual void Init(const GPindex& index);
		virtual void ChangeIndex(const GPindex& index, bool updateWindowPosition=true);
		const GPindex& GetCurrentIndex() const;
		bool IsInside(const GPindex& index) const;

		// +---------------------------
		// |	GPworld
		// +---------------------------
		void SetWorld( GPworld* pWorld );
		GPworld* GetWorld() const;

		// +---------------------------
		// |	GPtiles
		// +---------------------------
		void RemoveTile(const GPindex& index);
		const GPtileMap& GetTilesMap() const;
		void Clean();

		// +---------------------------
		// |	Update
		// +---------------------------
		virtual void Update(const GPposition& position, bool checkMovement = true);
		const GPposition& GetPosition() const;
		virtual void UpdateRadius(int radius);
		int GetRadius() const;
			
	protected:
		// +---------------------------
		// |	Update
		// +---------------------------
		void UpdateTilesMap();

		// +---------------------------
		// |	GPworld
		// +---------------------------
		GPworld* pWorld_;

		// +---------------------------
		// |	GPindex
		// +---------------------------
		GPtileMap	tilesMap_;
		GPindex currentIndex_;

		// +---------------------------
		// |	Update
		// +---------------------------
		GPposition winPosition_;
		int radius_;

	private:
		// +---------------------------
		// |	Copy Constructor
		// +---------------------------
		GPwindow(const GPwindow& ref); //Not Implemented

};

#endif //GPWINDOW_H
