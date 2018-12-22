// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpBaseNodeManager.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpBaseNodeManager Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPBASENODEMANAGER_H
#define GPG5_GPBASENODEMANAGER_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpNode.types.h>
#include <GPlock.h>
	
// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpBaseNodeManager
// |  Description : 
// |								This class is the base class of GPG5_gpNodeManager and
// |								GPG5_gpEventManager. This is used for manage the 
// |								GPG5_gpNode objects.
// |
// +-----------------------------------------------------------------------
class GPG5_gpBaseNodeManager
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpBaseNodeManager();
		virtual ~GPG5_gpBaseNodeManager();

		// +---------------------------
		// |	Process List
		// +---------------------------
		void AddNode(GPG5_gpNode* node);
		void RemoveNode(GPG5_gpNode* node);
		GPG5_gpNode* GetNextNode(GPG5_gpNode* node, bool loop = false);
		GPG5_gpNode* GetNextNodeOfSameType(GPG5_gpNode* node, bool loop = false);
		GPG5_gpNode* GetNode(const std::string& nodeName);
		void Clean();
		void CleanAndDestroy();
		const GPG5_gpNodeList& GetNodeList() const;

		// +---------------------------
		// |	Destroy List
		// +---------------------------
		void AddToDestroyList(GPG5_gpNode* node);
		void RemoveFromDestroyList(GPG5_gpNode* node);
		const GPG5_gpNodeList& GetDestroyNodeList() const;
		void CleanDestroyList();
		void DestroyNodes();

		// +---------------------------
		// |	Synchronization
		// +---------------------------
		void LockMe();
		void UnLockMe();
		CRITICAL_SECTION& GetCriticalSection() const;
		

	protected:
		// +---------------------------
		// |	Process List
		// +---------------------------
		GPG5_gpNodeList nodeList_;

		// +---------------------------
		// |	Destroy List
		// +---------------------------
		GPG5_gpNodeList destroyNodeList_;

		// +---------------------------
		// |	Lock & UnLock
		// +---------------------------
		GPlock lockMe_;
};

#endif //GPG5_GPBASENODEMANAGER_H