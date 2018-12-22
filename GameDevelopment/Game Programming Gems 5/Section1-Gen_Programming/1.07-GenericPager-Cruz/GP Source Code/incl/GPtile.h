// +-----------------------------------------------------------------------
// |
// |                         File Name: GPtile.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPtile Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPTILE_H
#define GPTILE_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPdefs.h>
#include <GPtile.types.h>
#include <GPwindow.types.h>

// +---------------------------
// |	Forward Declarations
// +---------------------------
class GPworld;

// +-----------------------------------------------------------------------
// |
// |  Class       : GPtile
// |  Description : 
// |								This class is the "block" of the World that must be loaded
// |								or unloaded. Its unique position and size is defined by its 
// |								GPindex.
// |
// |								The user of GP only have to implement the methods:
// |											virtual void Load();
// |											virtual void Unload();
// |
// |								In this implementation, the user can use the GPindex of the
// |								GPtile to identify the position and size that must be loaded
// |								and/or unloaded.
// |
// |								The user of GP does not have to take care of divide the world in
// |								GPtiles. The GP manages all the system for him.
// |	
// +-----------------------------------------------------------------------
class _GP GPtile
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPtile();
		virtual ~GPtile();
	
		// +---------------------------
		// |	GPworld
		// +---------------------------
		void SetWorld(GPworld* pWorld); 
		GPworld* GetWorld(); 

		// +---------------------------
		// |	GPwindows
		// +---------------------------
		void AddWindow(GPwindow* pWindow); 
		void RemoveWindow(GPwindow* pWindow); 
		const GPwindowList& GetWindowList(); 
		int GetWindowListSize(); 

		// +---------------------------
		// |	State
		// +---------------------------
		void SetState( GPtileState state);
		GPtileState GetState();

		// +---------------------------
		// |	GPindex
		// +---------------------------
		void SetIndex(const GPindex& index);
		const GPindex& GetIndex();

		// +---------------------------
		// |	Load & Unload
		// +---------------------------
		void DoLoad();
		void DoUnload();

		// +--------------------------------------------------
		// |	Virtual Methods to Implement by a derived class
		// +--------------------------------------------------
		virtual void Load();
		virtual void Unload();
		
	protected:
		// +---------------------------
		// |	GPworld
		// +---------------------------
		GPworld* pWorld_;

		// +---------------------------
		// |	GPwindows
		// +---------------------------
		GPwindowList windowList_;

		// +---------------------------
		// |	State
		// +---------------------------
		GPtileState state_;

		// +---------------------------
		// |	GPindex
		// +---------------------------
		GPindex index_;		

	private:
		// +---------------------------
		// |	Copy Constructor
		// +---------------------------
		GPtile(const GPtile& ref); //Not Implemented
};

#endif //GPTILE_H
