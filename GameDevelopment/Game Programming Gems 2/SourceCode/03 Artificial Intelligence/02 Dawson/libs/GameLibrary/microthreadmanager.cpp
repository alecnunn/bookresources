/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#include "priv_precompiled.h"

#include "microthreads.h"

#include <list>
#include <algorithm>
using namespace std;

using namespace MicroThread;

//#define	ITERATOR_TO_SELF

namespace MicroThreadManager
{
	typedef list<ManagedMicroThreadInfo> ScriptList;

	class ManagedMicroThreadInfo
	{
	public:
		ManagedMicroThreadInfo()
			: m_pMicroThread(0)	// Initialize to zero, to avoid copy constructor
								// problems as we insert into STL containers.
		{
		}
		~ManagedMicroThreadInfo()
		{
			if (m_pMicroThread)
				DeleteMicroThread(m_pMicroThread);
		}
		bool operator==(const MicroThreadInfo* pThread)
		{
			return m_pMicroThread == pThread;
		}
#ifdef	ITERATOR_TO_SELF
		// Each MicroThread object contains an iterator that points
		// to itself. This allows for more efficient deleting. This
		// does restrict the choice of containers to those that don't
		// invalidate iterators when other objects are deleted.
		ScriptList::iterator m_iterator;
#endif
		MicroThreadInfo* m_pMicroThread;
	};

	static ScriptList	s_scriptList;

	void DeleteManagedMicroThread(ManagedMicroThreadInfo* pManagedThread)
	{
		MicroThreadInfo* pThread = pManagedThread->m_pMicroThread;
		//pManagedThread->m_pMicroThread = 0;
#ifdef	ITERATOR_TO_SELF
		s_scriptList.erase(pManagedThread->m_iterator);
#else
		ScriptList::iterator p = find(s_scriptList.begin(), s_scriptList.end(), pThread);
		assert(p != s_scriptList.end());
		s_scriptList.erase(p);
#endif
	}
	void DeleteAllManagedMicroThreads()
	{
		s_scriptList.clear();
		//for (ScriptList::iterator p = s_scriptList.begin(); p != s_scriptList.end(); ++p)
		//{
		//}
	}

	ManagedMicroThreadInfo* CreateManagedMicroThread(const tMicroThreadFunction scriptFunction, void* const pData /*= 0*/)
	{
		assert(!GetActiveMicroThread());
		MicroThreadInfo* pThread = CreateMicroThread(scriptFunction, pData);
		s_scriptList.push_back(ManagedMicroThreadInfo());
		ScriptList::iterator p = --s_scriptList.end();	// Get an iterator to our newly added thread.
		p->m_pMicroThread = pThread;
#ifdef	ITERATOR_TO_SELF
		p->m_iterator = p;	// Point the thread info at itself!
#endif
		return &(*p);
	}

	int GetNumManagedMicroThreads()
	{
		return s_scriptList.size();
	}

	void RunManagedMicroThreads(const bool deleteDeadScripts /*= true*/)
	{
		for (ScriptList::iterator p = s_scriptList.begin(); p != s_scriptList.end(); /**/)
		{
			MicroThreadInfo* pScript = p->m_pMicroThread;
			if (!pScript->IsDead())
			{
				bool result = SwitchToMicroThread(pScript);
				assert(result != pScript->IsDead());
			}
			if (deleteDeadScripts && pScript->IsDead())
			{
				DeleteManagedMicroThread(&(*p++));
			}
			else
				++p;
		}
	}

	MicroThreadInfo* pGetMicroThreadInfo(ManagedMicroThreadInfo* pManagedThread)
	{
		return pManagedThread->m_pMicroThread;
	}

	size_t ManagedMicroThreadStackSize()
	{
		size_t totalSize = 0;
		for (ScriptList::iterator p = s_scriptList.begin(); p != s_scriptList.end(); ++p)
		{
			totalSize += p->m_pMicroThread->MicroThreadStackSize();
		}
		return totalSize;
	}

}; // End MicroThreadManager namespace
