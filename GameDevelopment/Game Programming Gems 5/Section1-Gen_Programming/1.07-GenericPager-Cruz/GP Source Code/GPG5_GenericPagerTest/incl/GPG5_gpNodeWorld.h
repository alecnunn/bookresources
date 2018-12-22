// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeWorld.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeWorld Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_gpNODEWORLD_H
#define GPG5_gpNODEWORLD_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include<GPG5_gpNode.h>
#include<GPworld.h>

#include <GPG5_gpNodeGrid.h>
#include <GPG5_gpNodeQuad.h>
#include <GPG5_gpNodeTexture.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpNodeWorld
// |  Description : 
// |								This class implements the methods:
// |										virtual bool IsValidIndex(const GPindex& index);
// |										virtual GPtile* BuildTile(const GPindex& index);
// |								of GPworld.
// |								These methods are called automaically when Generic Pager
// |								requires it.
// |
// +-----------------------------------------------------------------------
class GPG5_gpNodeWorld: public GPG5_gpNode, public GPworld
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpNodeWorld();
		virtual ~GPG5_gpNodeWorld();

		// +---------------------------
		// |	World Limits
		// +---------------------------
		void SetWorldLimits(int left, int right, int bottom, int top, int tilesX, int tilesY);

		// +---------------------------
		// |	Texture
		// +---------------------------
		void SetTexture(const std::string& path, int numComponents, int totalWidth, int totalHeight);
		void ShowTexture(bool showTexture);
		void ShowGrid(bool showGrid);

		// +--------------------------------------------------
		// |	Virtual Methods of GPG5_gpNode
		// +--------------------------------------------------
		virtual void Process();

		// +--------------------------------------------------
		// |	Virtual Methods of GPworld
		// +--------------------------------------------------
		virtual bool IsValidIndex(const GPindex& index);
		virtual GPtile* BuildTile(const GPindex& index);

	protected:
		// +---------------------------
		// |	World Grid
		// +---------------------------
		GPG5_gpNodeGrid grid_;
		bool showGrid_;

		// +---------------------------
		// |	World Quad
		// +---------------------------
		GPG5_gpNodeQuad quad_;
		
		// +---------------------------
		// |	Texture
		// +---------------------------
		GPG5_gpNodeTexture texture_;
		std::string texturePath_;
		bool showTexture_;
		
		// +---------------------------
		// |	Limits
		// +---------------------------
		int left_;
		int right_;
		int bottom_;
		int top_;

	private:
		
};

#endif //GPG5_gpNODEWORLD_H