// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeManager.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeManager Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPNODEMANAGER_H
#define GPG5_GPNODEMANAGER_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpBaseNodeManager.h>
#include <GPG5_gpEventManager.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpNodeManager
// |  Description : 
// |								This class is the main Singleton that manages all the 
// |								GPG5 Test Engine.
// |
// +-----------------------------------------------------------------------
class GPG5_gpNodeManager:public GPG5_gpBaseNodeManager
{
	public:
		// +---------------------------
		// |	Destructor
		// +---------------------------
		virtual ~GPG5_gpNodeManager();
		
		// +---------------------------
		// |	Instance. Singleton Method
		// +---------------------------
		static GPG5_gpNodeManager& GetInstance();

		// +---------------------------
		// |	Events Manager
		// +---------------------------
		GPG5_gpEventManager& GetEventManager();

		// +---------------------------
		// |	Process Nodes
		// +---------------------------
		void ProcessNodes();
		
		// +---------------------------
		// |	Terminate
		// +---------------------------
		void Terminate();

		// +---------------------------
		// |	ReInit GPworld and GPwindows with a new Index Size
		// +---------------------------
		void ReInit(int tileWidth, int tileHeight);

	protected:
		// +---------------------------
		// |	Constructor & Copy Constructor
		// +---------------------------
		GPG5_gpNodeManager();
		GPG5_gpNodeManager(const GPG5_gpNodeManager& ref); //Not implemented

		// +---------------------------
		// |	Destroy List
		// +---------------------------
		GPG5_gpNodeList destroyNodeList_;

	private:
		// +---------------------------
		// |	Events Manager
		// +---------------------------
		GPG5_gpEventManager genericPagerEventManager_;
};

#endif //GPG5_GPNODEMANAGER_H