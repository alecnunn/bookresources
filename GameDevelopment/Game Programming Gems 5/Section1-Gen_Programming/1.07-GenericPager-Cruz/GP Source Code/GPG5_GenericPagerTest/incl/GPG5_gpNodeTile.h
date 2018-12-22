// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeTile.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeTile Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPNODETILE_H
#define GPG5_GPNODETILE_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpNodeQuad.h>
#include <GPtile.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpNodeTile
// |  Description : 
// |								This class implements the methods:
// |										virtual void Load();
// |										virtual void Unload();
// |								of GPtile.
// |								These methods are called from GPworld when a GPwindow
// |								requires it.
// |
// +-----------------------------------------------------------------------
class GPG5_gpNodeTile : public GPG5_gpNodeQuad, public GPtile
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpNodeTile();
		virtual ~GPG5_gpNodeTile();

		// +--------------------------------------------------
		// |	Virtual Methods of GPtile
		// +--------------------------------------------------
		virtual void Load();
		virtual void Unload();

	protected:
	
	private:

};
#endif //GPG5_GPNODETILE_H