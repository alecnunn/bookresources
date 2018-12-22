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

#ifndef	_DeltaMap_H
#define	_DeltaMap_H

//---------------------------------------------------------------------

#include "DeltaContainer.h"
#include <map>
#include <vector>

namespace Archive {
//---------------------------------------------------------------------
/**
	@brief An auto-synchronized map

	When the DeltaMap object invokes packDeltas, only those elements
	in the map which have changed since the last call to packDeltas will
	be included in the target Stream.

	@author Justin Randall
*/
template<class KeyType, class ValueType>
class DeltaMap : public DeltaContainer
{
public:
	DeltaMap();
	virtual ~DeltaMap();

	typedef std::map<KeyType, ValueType>::const_iterator const_iterator;

	const_iterator begin() const;
	const_iterator end() const;
	void erase(const KeyType & key);
	const_iterator find(const KeyType & key) const;
	void pack(Stream & target) const;
	void packDelta(Stream & target);
	void set(const KeyType & key, const ValueType & value);
	const unsigned int size() const;
	void unpack(Stream::ReadIterator & source);
	void unpackDelta(Stream::ReadIterator & source);

private: // methods
	DeltaMap(const DeltaMap & source);
	DeltaMap & operator = (const DeltaMap &  rhs);

private: // values
	struct Command
	{
		enum
		{
			ERASE,
			SET
		};

		unsigned char cmd;
		unsigned int  currentContainerSize;
		KeyType       key;
		ValueType     value;
	};
	std::map<KeyType, ValueType>      deltaMap;
	std::vector<Command>              commands;
};

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline DeltaMap<KeyType, ValueType>::DeltaMap() :
DeltaContainer(std::string("DeltaMap<" + TypeMap::getType((ValueType *)0) + ">")),
deltaMap(),
commands()
{
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline DeltaMap<KeyType, ValueType>::~DeltaMap()
{
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline DeltaMap<KeyType, ValueType>::const_iterator DeltaMap<KeyType, ValueType>::begin() const
{
	return deltaMap.begin();
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline DeltaMap<KeyType, ValueType>::const_iterator DeltaMap<KeyType, ValueType>::end() const
{
	return deltaMap.end();
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline void DeltaMap<KeyType, ValueType>::erase(const KeyType & key)
{
	static Command c;
	c.cmd = Command::ERASE;
	c.currentContainerSize = size();
	c.key = key;
	commands.push_back(c);
	deltaMap.erase(key);
	dirty();
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline DeltaMap<KeyType, ValueType>::const_iterator DeltaMap<KeyType, ValueType>::find(const KeyType & key) const
{
	return deltaMap.find(key);
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline void DeltaMap<KeyType, ValueType>::pack(Stream & target) const
{
	unsigned int count = size();
	Archive::put(target, count);
	std::map<KeyType, ValueType>::const_iterator i;
	for(i = deltaMap.begin(); i != deltaMap.end(); ++i)
	{
		Archive::put(target, (*i).first);
		Archive::put(target, (*i).second);
	}
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline void DeltaMap<KeyType, ValueType>::packDelta(Stream & target)
{
	unsigned int count = commands.size();
	Archive::put(target, count);
	std::vector<Command>::iterator i;
	for(i = commands.begin(); i != commands.end(); ++ i)
	{
		Archive::put(target, (*i).cmd);
		Archive::put(target, (*i).currentContainerSize);
		Archive::put(target, (*i).key);
		if((*i).cmd == Command::SET)
		{
			Archive::put(target, (*i).value);
		}
	}
	commands.clear();
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline void DeltaMap<KeyType, ValueType>::set(const KeyType & key, const ValueType & value)
{
	// avoid making deltas if the value hasn't really changed
	std::map<KeyType, ValueType>::const_iterator i;
	i = deltaMap.find(key);

	if(i == deltaMap.end() || (*i).second != value)
	{

		static Command c;
		c.cmd = Command::SET;
		c.currentContainerSize = size();
		c.key = key;
		c.value = value;

		deltaMap[key] = value;

		std::vector<Command>::iterator i;
		for(i = commands.begin(); i != commands.end(); ++i)
		{
			if((*i).cmd == Command::SET && (*i).key == key)
			{
				// just replace this command in line
				commands[std::distance(commands.begin(), i)] = c;
				return;
			}
		}
		commands.push_back(c);
		dirty();
	}
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline const unsigned int DeltaMap<KeyType, ValueType>::size() const
{
	return deltaMap.size();
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline void DeltaMap<KeyType, ValueType>::unpack(Stream::ReadIterator & source)
{
	unsigned int count = 0;
	Archive::get(source, count);
	KeyType key;
	ValueType value;
	for(unsigned int i = 0; i < count; ++ i)
	{
		Archive::get(source, key);
		Archive::get(source, value);
		deltaMap[key] = value;
	}
}

//---------------------------------------------------------------------

template<class KeyType, class ValueType>
inline void DeltaMap<KeyType, ValueType>::unpackDelta(Stream::ReadIterator & source)
{
	unsigned int count = 0;
	Archive::get(source, count);
	Command c;
	for(unsigned int i = 0; i < count; ++ i)
	{
		Archive::get(source, c.cmd);
		Archive::get(source, c.currentContainerSize);
		Archive::get(source, c.key);
		if(c.cmd == Command::SET)
		{
			Archive::get(source, c.value);
			set(c.key, c.value);
		}
		else if(c.cmd == Command::ERASE)
		{
			if(c.currentContainerSize == size())
				erase(c.key);
		}
	}
}

//---------------------------------------------------------------------
}//namespace Archive

#endif	// _DeltaMap_H
