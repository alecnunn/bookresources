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
#include "DeltaArchive.h"
#include "DeltaVariable.h"

namespace Archive {

//---------------------------------------------------------------------
/**
	@brief DeltaArchive ctor

	@author Justin Randall
*/
DeltaArchive::DeltaArchive() :
AutoArchive(),
dirtyVariables()
{
}

//---------------------------------------------------------------------
/**
	@brief destroy a DeltaArchive object

	The DeltaArchive destructor will iterate through the AutoArchive::members
	list and advise watched variables that they no longer have an owner.

	@author Justin Randall
*/
DeltaArchive::~DeltaArchive()
{
	std::vector<AutoVariableBase *>::iterator i;
	for(i = members.begin(); i != members.end(); ++ i)
	{
		DeltaVariableBase * v = dynamic_cast<DeltaVariableBase *>((*i));
		if(v)
			v->removeOwner(this);
	}
}

//---------------------------------------------------------------------
/**
	@brief add a watched variable

	In addition to invokeing AutoArchive::addVariable(), this method
	will check to see if the new variable is a DeltaVariableBase object. 
	If it is, then the owner and index values of the watched variable will
	be set to reflect the owner (this DeltaArchive object) and index (the
	location in the AutoArchive::members variable) of this DeltaArchive
	object.

	@param newVariable  the target variable that is being watched

	@see AutoArchive

	@author Justin Randall
*/
void DeltaArchive::addVariable(AutoVariableBase & newVariable)
{
	const unsigned int newIndex = members.size();
	AutoArchive::addVariable(newVariable);
	DeltaVariableBase * v = dynamic_cast<DeltaVariableBase *>(&newVariable);
	if(v)
	{
		v->setOwner(this, newIndex);
	}
}

//---------------------------------------------------------------------
/**
	@brief pack watched variables that have changed since the last call to
	packDeltas

	packDetals will invoke packDelta on any DeltaVariableBase object
	that has been placed on the dirtyVariables list since the last call
	to packDeltas.

	@param target    An Archive Stream object that will receive delta data
	                 at its current write position.

	@author Justin Randall
*/
void DeltaArchive::packDeltas(Stream & target)
{
	unsigned int count = dirtyVariables.size();
	char id = 'D';
	Archive::put(target, id);
	Archive::put(target, count);
	std::set<DeltaVariableBase *>::iterator i;
	for(i = dirtyVariables.begin(); i != dirtyVariables.end(); ++i)
	{
		Archive::put(target, (*i)->getIndex(this));
		(*i)->packDelta(target);
	}
	dirtyVariables.clear();
}

//---------------------------------------------------------------------
/**
	@brief receive a Stream and apply values as appropriate

	The unpack method is overridden from AutoArchive to determine if
	the supplied stream is an AutoArchive or a DeltaArchive. If the 
	magic byte in the stream indicates that the stream reflects an
	AutoArchive, then AutoArchive::unpack() is invoked with the source
	stream argument. If the magic byte indicates that the source stream
	is a DeltaArchive, then the changes reflected in the stream will
	be applied to this DeltaArchive.

	@param source  an Archive Stream object containing relevent Archive
	               data to rebuild or synchronize the data in this local
	               instance of a DeltaArchive.

	@see AutoArchive

	@author Justin Randall
*/
void DeltaArchive::unpack(Stream::ReadIterator & source)
{
	// what kind of stream is it? Auto or Delta?
	Stream::ReadIterator r = source;
	signed char id;
	Archive::get(r, id);
	switch(id)
	{
	case 'D':
		unpackDeltas(r);
		break;
	case 'A':
		AutoArchive::unpack(source);
		break;
	default:
		assert(false); // garbage in!
	}
}

//---------------------------------------------------------------------
/**
	@brief read the contents of a Stream containing delta info about
	this DeltaArchive.

	@param source  A Stream object describing delta values which will
	               be applied to this instance of a DeltaArchive.

	@see AutoArchive

	@author Justin Randall
*/
void DeltaArchive::unpackDeltas(Stream::ReadIterator & source)
{
	unsigned int count;
	Archive::get(source, count);
	unsigned int index;
	for(unsigned int i = 0; i < count; ++i)
	{
		Archive::get(source, index);
		DeltaVariableBase * v = dynamic_cast<DeltaVariableBase *>(members[index]);
		if(v)
		{
			v->unpackDelta(source);
		}
	}
}

//---------------------------------------------------------------------

}//namespace Archive
