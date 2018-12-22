// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeTexture.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeTexture Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPNODETEXTURE_H
#define GPG5_GPNODETEXTURE_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include<GPG5_gpNode.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpNodeTexture
// |  Description : 
// |								This class manages a Texture
// |
// +-----------------------------------------------------------------------
class GPG5_gpNodeTexture: public GPG5_gpNode
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpNodeTexture();
		virtual ~GPG5_gpNodeTexture();

		// +---------------------------
		// |	Setup & Load
		// +---------------------------
		void Setup(const std::string& path, int numComponents, int totalWidth, int totalHeight);
		void Load();
		void Load(int startX, int endX, int startY, int endY );

		void Set();
		bool IsSet() const;

		// +---------------------------
		// |	Apply
		// +---------------------------
		void Apply();
		
		// +---------------------------
		// |	Data
		// +---------------------------
		unsigned char* GetData() const;
		void CleanData();

		// +--------------------------------------------------
		// |	Virtual Methods of GPG5_gpNode
		// +--------------------------------------------------
		virtual void Process();

	protected:
		// +---------------------------
		// |	Power of 2
		// +---------------------------
		int GetNearPowerOf2(int number) const;

		// +---------------------------
		// |	Setup & Load
		// +---------------------------
		std::string path_;
		unsigned int id_;
		int numComponents_;
		int totalWidth_;
		int totalHeight_;
		int startX_;
		int endX_;
		int startY_;
		int endY_;
		int loadWidth_;
		int loadHeight_;
		bool isSetup_;
		
		// +---------------------------
		// |	Data
		// +---------------------------
		unsigned char* data_;
};

#endif //GPG5_GPNODETEXTURE_H