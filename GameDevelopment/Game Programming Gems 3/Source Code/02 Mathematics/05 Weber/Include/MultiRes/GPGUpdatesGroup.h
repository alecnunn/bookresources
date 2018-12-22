/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/

#ifndef GPGUpdatesGroup_h
#define GPGUpdatesGroup_h


#include "GPGUpdates.h"
#include "memory.h"


// This groups the multiresolution updates together.
// The update group is typically associated with a mesh group in parallel array fashion.

class GPGUpdatesGroup
{
public:

    U32      GetNumUpdates() 
    { 
		return size; 
	};
	
    // May return null.
    GPGUpdates* GetUpdates(U32 index) { return updates[index]; };
	// returns updates that we just passed in.
	GPGUpdates* SetUpdates(U32 index, GPGUpdates* pUpdates);
	
	// data that allows resolution of each material to synchronize.
	U32* GetSyncTable(U32 index) { return synchTables[index]; };
    U32* SetSyncTable(U32 index, U32* pTable);
    
	GPGRESULT Allocate(U32 numUpdates)
	{   
		GPGRESULT returnCode = GPG_OK;
		GPGASSERT(updates == NULL  &&  synchTables == NULL);

        if (updates)
        {
            delete [] updates;
            updates = NULL;
        }

        if (synchTables)
        {
            delete [] synchTables;
            synchTables = NULL;
        }

		updates     = new GPGUpdates*[numUpdates];
		synchTables = new U32*[numUpdates];

		if(updates && synchTables)
		{	
			memset(updates,0,numUpdates*sizeof(GPGUpdates*));
			memset(synchTables,0,numUpdates*sizeof(U32*));
			size = numUpdates;
		} 
		else 
		{
			if (updates)
			{
				delete [] updates;
				updates = NULL;
			}
			
			if (synchTables)
			{
				delete [] synchTables;
				synchTables = NULL;
			}
			returnCode = GPG_E_OUT_OF_MEMORY; 
		}
		return returnCode;
	};
	
	// perhaps neighbor mesh will need to add to this.
    //GPGRESULT GetInterface(U32 interfaceIdentifier, void** pInterface) { *pInterface = NULL; return GPG_FAIL; };

	//// SUBMITTER: Chris Gorman  PEER: Tom O'Rourke  FIX_FOR_DEFECT: 432, 857
	GPGUpdatesGroup()
	{
		size                    = 0;
		updates                 = NULL;
		synchTables             = NULL;
		m_maxResolution         = 0;
		m_finalMaxResolution    = 0;
	};
	
	// think about deep unallocate
	//~GPGUpdatesGroup() { delete [] updates; delete [] synchTables; };
	~GPGUpdatesGroup() 
    { 
		if(updates) {
			for(U32 i=0;i<size;i++) {
				if(updates[i])
                {
					delete updates[i];
                    updates[i] = NULL;
                }
			}
			delete [] updates;
            updates = NULL;
		}
		
        if(synchTables) 
        {
			for(U32 i=0;i<size;i++) {
				if(synchTables[i]) 
                {
					delete synchTables[i];
                    synchTables[i] = NULL;
				}
			}
			delete [] synchTables;
            synchTables = NULL;
		}
	};
		
	U32		GetMaxResolution		();
	U32		GetFinalMaxResolution	();
	void	SetMaxResolution		(U32 max);
	void	SetFinalMaxResolution	(U32 max);

private:

	U32 size;
	GPGUpdates **updates;  // array of pointers to GPGUpdate 
	U32 **synchTables;

	U32 m_maxResolution;	// Maximum resolution allowed given the amount
						// of update records we currently have.  This
						// value increases as update records are streamed
						// progressively down the internet.

	U32 m_finalMaxResolution;	// Maximum resolution when file is 
								// completely downloaded.  Used for
								// allocation purposes.

};


GPGINLINE U32 GPGUpdatesGroup::GetMaxResolution()
{
	return m_maxResolution;
}


GPGINLINE U32 GPGUpdatesGroup::GetFinalMaxResolution()
{
	return m_finalMaxResolution;
}


GPGINLINE void GPGUpdatesGroup::SetMaxResolution(U32 max)
{
	m_maxResolution = max;
}


GPGINLINE void GPGUpdatesGroup::SetFinalMaxResolution(U32 max)
{
	m_finalMaxResolution = max;
}


GPGINLINE GPGUpdates* GPGUpdatesGroup::SetUpdates(U32 index, GPGUpdates* pUpdates)
{ 
    // Delete existing updates:
    if (updates[index] && (pUpdates != updates[index]))
    {
        delete updates[index];
        updates[index] = NULL;
    }

    return updates[index] = pUpdates; 
};


GPGINLINE U32* GPGUpdatesGroup::SetSyncTable(U32 index, U32* pTable) 
{
	if( synchTables[index] && (synchTables[index] != pTable) )
	{
		delete [] ( synchTables[index] );
		synchTables[index] = NULL;
	}
    return synchTables[index] = pTable; 
};

#endif

