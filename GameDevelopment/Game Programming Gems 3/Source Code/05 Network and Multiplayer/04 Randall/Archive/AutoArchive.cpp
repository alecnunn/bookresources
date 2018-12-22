/*
Copyright (c) 2001 by Justin Randall.  All rights reserved.
This copyright applies only to this particular software distribution 
package, and does not imply a copyright on any particular module in 
the library.

Permission to use, copy, modify, and distribute this compilation for
non-commercial use is hereby granted without fee, subject to 
the following license:

1. Any copy or modification of this compilation must include the above
copyright notice and this license.

2. Users of the software included in this compilation agree to use
their best efforts to provide Justin Randall with any modifications containing
improvements or extensions and hereby grant Justin Randall a perpetual,
royalty-free license to use and distribute such modifications under
the terms of this license.

3. Justin Randall makes no warranty or representation that the operation of the
software in this compilation will be error-free, and Justin Randall is under no
obligation to provide any services, by way of maintenance, update, or
otherwise.  THE SOFTWARE AND ANY DOCUMENTATION ARE PROVIDED 'AS IS'
WITHOUT EXPRESS OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. IN NO EVENT WILL JUSTIN RANDALL OR ANY OTHER CONTRIBUTOR BE LIABLE FOR
DIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

4. Users will not use Justin Randall or any other contributor's name in any 
publicity or advertising, without prior written consent in each case.

5. Export of this software from the United States may require a
specific license from the United States Government.  It is the
responsibility of any person or organization contemplating export
to obtain such a license before exporting.

6. Certain parts of this software may be protected by patents.  It
is the users' responsibility to obtain the appropriate
licenses before using those parts.
*/

//---------------------------------------------------------------------
#include "ArchivePCH.h"

#include "AutoVariable.h"
#include "Stream.h"

namespace Archive {

//---------------------------------------------------------------------

AutoArchive::AutoArchive() :
members()
{
}

//---------------------------------------------------------------------

AutoArchive::~AutoArchive()
{
}

//---------------------------------------------------------------------
/**
	@brief Add a AutoVariable or AutoVariableWatcher to a list of
		data to be automatically packed or unpacked.

	Add the instance of a AutoVariableBase derivative to the 
	Autoarchive's list of values to be packed and unpacked 
	automatically. The AutoArchive will handle correct packing
	and unpacking order as long as the variables are added in
	order. Typically, these variables are added during construction
	of some AutoArchive derived class. 

	Once a variable has been added, it cannot be removed.

	@param newVariable  A reference to the instance of the 
	                    AutoVariableBase that will be added to the
						AutoArchive's list of tracked values.

	@see AutoArchive
	@see AutoVariableBase
	@see AutoVariable
	@see AutoVariableWatcher
	@see Stream

	@author Justin Randall
*/
void AutoArchive::addVariable(AutoVariableBase & newVariable)
{
	members.push_back(&newVariable);
}

//---------------------------------------------------------------------
/**
	@brief Pack the values (AutoVariableBase or derivatives) into the
		Stream Buffer

	When the pack method is invoked, all of the members added with
	addVariable are placed, in the order they were added, into the
	archive's buffer. The archive returns a reference to itself
	after the pack operation has completed.

	@see AutoVariableBase::pack
	@see Stream::pack
	@see Stream
	@see AutoArchive
	@see AutoVariable
	@see AutoVariableWatcher

	@return a reference to this AutoArchive

	@author Justin Randall
*/
void AutoArchive::pack(Stream & target) const
{
	std::vector<AutoVariableBase *>::const_iterator i;
	char id = 'A';
	Archive::put(target, id);
	for(i = members.begin(); i != members.end(); ++i)
	{
		(*i)->pack(target);
	}
}

//---------------------------------------------------------------------
/**
	@brief Restore values from an archive buffer

	Values are restored from the source archive's buffer in the order
	that this archive's members were added (using addVariable).

	@param source  The archive containing the source data starting at
	               the archive's current read position.

	@see AutoArchive::pack
	@see AutoArchive
	@see Stream::unpack
	@see Stream::pack
	@see Stream
	@see AutoVariableBase::unpack
	@see AutoVariableBase

	@author Justin Randall
*/
void AutoArchive::unpack(Stream::ReadIterator & source)
{
	std::vector<AutoVariableBase *>::iterator i;
	// identify the archive
	signed char id;
	Archive::get(source, id);
	if(id == 'A')
	{
		for(i = members.begin(); i != members.end(); ++i)
		{
			(*i)->unpack(source);
		}
	}
	assert(id == 'A'); // incoming garbage!
}

//---------------------------------------------------------------------

}// namespace Archive
