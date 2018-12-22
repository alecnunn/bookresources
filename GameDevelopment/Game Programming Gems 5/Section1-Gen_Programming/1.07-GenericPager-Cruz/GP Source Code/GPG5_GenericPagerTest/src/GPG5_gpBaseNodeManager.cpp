// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpBaseNodeManager.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpBaseNodeManager Class Implemantation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpBaseNodeManager.h>

#include <GPG5_gpNode.h>
#include <typeinfo>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::GPG5_gpBaseNodeManager
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpBaseNodeManager::GPG5_gpBaseNodeManager()
{
	LockMe();

	Clean();

	CleanDestroyList();
	
	UnLockMe();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::~GPG5_gpBaseNodeManager
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpBaseNodeManager::~GPG5_gpBaseNodeManager()
{
	LockMe();

	CleanDestroyList();

	CleanAndDestroy();

	UnLockMe();
}

// +---------------------------
// |	Process List
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::AddNode
// | Description     : 
// |
// | Parameters : 
// |   node         : 
// | 
// +-------------------------------------------------------------
void GPG5_gpBaseNodeManager::AddNode(GPG5_gpNode* node)
{
	LockMe();

	GPG5_gpNodeList::iterator it		= std::find(nodeList_.begin(), nodeList_.end(), node);
	if ( it == nodeList_.end() )
	{
		nodeList_.push_back(node);
	}

	UnLockMe();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::RemoveNode
// | Description     : 
// |
// | Parameters : 
// |   node         : 
// | 
// +-------------------------------------------------------------
void GPG5_gpBaseNodeManager::RemoveNode(GPG5_gpNode* node)
{
	LockMe();

	GPG5_gpNodeList::iterator it		= std::find(nodeList_.begin(), nodeList_.end(), node);
	if ( it != nodeList_.end() )
	{
		nodeList_.erase(it);
	}

	UnLockMe();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::GetNextNode
// | Description     : 
// |
// | Parameters : 
// |   node          : 
// |   loop          : 
// | 
// +-------------------------------------------------------------
GPG5_gpNode* GPG5_gpBaseNodeManager::GetNextNode(GPG5_gpNode* node, bool loop)
{
	GPG5_gpNode* returnNode = NULL;

	LockMe();

	if ( nodeList_.size() )
	{
		GPG5_gpNodeList::iterator it		= std::find(nodeList_.begin(), nodeList_.end(), node);
		if ( it != nodeList_.end() )
		{
			it++;
			if ( it == nodeList_.end() )
			{
				if ( loop )
				{
					returnNode = (*nodeList_.begin());
				}
			}
			else
			{
				returnNode = (*it);
			}
		}
	}
	
	UnLockMe();

	return returnNode;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::GetNextNodeOfSameType
// | Description     : 
// |
// | Parameters : 
// |   node          : 
// |   loop          : 
// | 
// +-------------------------------------------------------------
GPG5_gpNode* GPG5_gpBaseNodeManager::GetNextNodeOfSameType(GPG5_gpNode* node, bool loop)
{
	GPG5_gpNode* returnNode = NULL;

	LockMe();

	bool found = false;
	bool end   = false;

	returnNode = GetNextNode(node, loop);

	while ( returnNode && ( !found ) && ( !end ) )
	{
		if ( returnNode == node)
		{
			returnNode = NULL;
			end = true;
		}
		else
		{
			if ( typeid(*returnNode) == typeid(*node) )
			{
				found = true;
			}
			else
			{
				returnNode = GetNextNode(returnNode, loop);
			}
		}
	}

	UnLockMe();

	return returnNode;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::GetNode
// | Description     : 
// |
// | Parameters : 
// |   nodeName      : 
// | 
// +-------------------------------------------------------------
GPG5_gpNode* GPG5_gpBaseNodeManager::GetNode(const std::string& nodeName)
{
	GPG5_gpNode* returnNode = NULL;

	LockMe();

	GPG5_gpNodeList::iterator it		= nodeList_.begin();
	GPG5_gpNodeList::iterator itEnd = nodeList_.end();
	bool found = false;
	while( (it != itEnd) && (!found) )
	{
		if ( (*it)->GetName() == nodeName )
		{
			found = true;
			returnNode = (*it);
		}
		else
		{
			it++;
		}
	}

	UnLockMe();

	return returnNode;
}
// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::Clean
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpBaseNodeManager::Clean()
{
	LockMe();

	nodeList_.clear();

	UnLockMe();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::CleanAndDestroy
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpBaseNodeManager::CleanAndDestroy()
{
	LockMe();
	
	while ( nodeList_.begin() != nodeList_.end() )
	{
		GPG5_gpNode* node = nodeList_.front();
		nodeList_.pop_front();
		delete( node );
	}

	nodeList_.clear();

	UnLockMe();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::GetNodeList
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPG5_gpNodeList& GPG5_gpBaseNodeManager::GetNodeList() const
{
	return nodeList_;
}

// +---------------------------
// |	Destroy List
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::AddToDestroyList
// | Description     : 
// |
// | Parameters : 
// |   node          : 
// | 
// +-------------------------------------------------------------
void GPG5_gpBaseNodeManager::AddToDestroyList(GPG5_gpNode* node)
{
	LockMe();

	GPG5_gpNodeList::iterator it = std::find(destroyNodeList_.begin(), destroyNodeList_.end(), node);
	if ( it == destroyNodeList_.end() )
	{
		destroyNodeList_.push_back(node);
	}

	UnLockMe();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::RemoveFromDestroyList
// | Description     : 
// |
// | Parameters : 
// |   node          : 
// | 
// +-------------------------------------------------------------
void GPG5_gpBaseNodeManager::RemoveFromDestroyList(GPG5_gpNode* node)
{
	LockMe();

	GPG5_gpNodeList::iterator it = std::find(destroyNodeList_.begin(), destroyNodeList_.end(), node);
	if ( it != destroyNodeList_.end() )
	{
		destroyNodeList_.erase(it);
	}

	UnLockMe();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::GetDestroyNodeList
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPG5_gpNodeList& GPG5_gpBaseNodeManager::GetDestroyNodeList() const
{
	return destroyNodeList_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::CleanDestroyList
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpBaseNodeManager::CleanDestroyList()
{
	LockMe();
	
	destroyNodeList_.clear();

	UnLockMe();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::DestroyNodes
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpBaseNodeManager::DestroyNodes()
{
	LockMe();
	
	while ( destroyNodeList_.begin() != destroyNodeList_.end() )
	{
		GPG5_gpNode* node = destroyNodeList_.front();
		destroyNodeList_.pop_front();
		delete( node );
	}

	destroyNodeList_.clear();

	UnLockMe();
}

// +---------------------------
// |	Synchronization
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::LockMe
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpBaseNodeManager::LockMe()
{
	lockMe_.Lock();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::UnLockMe
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpBaseNodeManager::UnLockMe()
{
	lockMe_.UnLock();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpBaseNodeManager::GetCriticalSection
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
CRITICAL_SECTION& GPG5_gpBaseNodeManager::GetCriticalSection() const
{
	return lockMe_.GetCriticalSection();
}
