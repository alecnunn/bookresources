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

#ifndef	_DeltaVector_H
#define	_DeltaVector_H

//---------------------------------------------------------------------

#include "Archive/DeltaContainer.h"

namespace Archive {

//---------------------------------------------------------------------
/**
	@brief a container template that tracks changes

	The DeltaVector template intercepts write operations to elements of
	the container. If any changes are made, a command list is generated
	that may be applied to another DeltaContainer to synchronize it
	with the source container.

	Operations which alter the size of the container also track the
	container's current size. Because applying deltas to the vector
	will trigger new deltas (a change), the container can forward 
	updates from a source to other synchronized containers. Since the
	current size is tracked, if a source container receives a delta
	that it originally triggered, the change can be ignored. This
	prevents delta-loops from occurring and enforces safe data
	routing from DeltaVector to DeltaVector.
*/
template<class ValueType>
class DeltaVector : public DeltaContainer
{
public:
	                    DeltaVector   ();
	                    ~DeltaVector  ();
	void                erase         (const unsigned int position);
	const ValueType &   get           (const unsigned int position) const;
	void                insert        (const unsigned int position, const ValueType & newValue);
	void                pack          (Stream & target) const;
	void                packDelta     (Stream & target);
	void                set           (const unsigned int position, const ValueType & newValue);
	const unsigned int  size          () const;
	void                unpack        (Stream::ReadIterator & source);
	void                unpackDelta   (Stream::ReadIterator & source);

private:
	struct Command
	{
		enum
		{
			ERASE,
			INSERT,
			SET
		};

		unsigned char cmd;
		unsigned int  index;
		unsigned int  currentContainerSize;
		ValueType     value;
	};

private:
	std::vector<ValueType>           v;
	std::vector<Command>             commands;
};

//---------------------------------------------------------------------

template<class ValueType>
inline DeltaVector<ValueType>::DeltaVector() :
DeltaContainer(std::string("DeltaVector<" + TypeMap::getType((ValueType *)0) + ">")),
v()
{
}

//---------------------------------------------------------------------

template<class ValueType>
inline DeltaVector<ValueType>::~DeltaVector()
{
}

//---------------------------------------------------------------------

template<class ValueType>
inline const ValueType & DeltaVector<ValueType>::get(const unsigned int position) const
{
	if(position >= size())
		return v[size() - 1];

	return v[position];
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVector<ValueType>::erase(const unsigned int position)
{
	if(position < size())
	{
		Command c;
		c.cmd = Command::ERASE;
		c.index = position;
		c.currentContainerSize = size();
		std::vector<ValueType>::iterator i = v.begin();
		std::advance(i, position);
		v.erase(i);
		commands.push_back(c);
		dirty();
	}
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVector<ValueType>::insert(const unsigned int position, const ValueType & newValue)
{
	Command c;
	c.cmd = Command::INSERT;
	c.index = position;
	c.currentContainerSize = size();
	c.value = newValue;

	std::vector<ValueType>::iterator i = v.begin();
	std::advance(i, position);
	v.insert(i, newValue);
	commands.push_back(c);
	dirty();
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVector<ValueType>::pack(Stream & target) const
{
	unsigned int count = size();
	Archive::put(target, count);
	std::vector<ValueType>::const_iterator i;
	for(i = v.begin(); i != v.end(); ++i)
	{
		Archive::put(target, (*i));
	}
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVector<ValueType>::packDelta(Stream & target) 
{
	unsigned int count = commands.size();
	Archive::put(target, count);

	std::vector<Command>::const_iterator i;
	for(i = commands.begin(); i != commands.end(); ++i)
	{
		const Command & c = (*i);
		Archive::put(target, c.cmd);
		Archive::put(target, c.index);
		switch(c.cmd)
		{
		case Command::ERASE:
			{
				Archive::put(target, c.currentContainerSize);
			}
			break;
		case Command::INSERT:
			{
				Archive::put(target, c.currentContainerSize);
			}
			// continue to SET, both insert and set require a value
		case Command::SET:
			{
				Archive::put(target, c.value);
			}
			break;
		}
	}
	commands.clear();
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVector<ValueType>::set(const unsigned int position, const ValueType & newValue)
{
	Command c;
	if(position >= size())
	{
		c.cmd = Command::SET;
		c.index = size();
		c.value = newValue;
		v[c.index] = newValue;
		commands.push_back(c);
		dirty();
	}
	else
	{
		if(v[position] != newValue)
		{
			// is there alread a set command
			// at this position? 
			std::vector<Command>::iterator i;
			unsigned int commandTarget = commands.size();

			for(i = commands.begin(); i != commands.end(); ++i)
			{
				if((*i).cmd == Command::SET && (*i).index == position)
				{
					commandTarget = std::distance(commands.begin(), i);
				}
			}
			c.cmd = Command::SET;
			c.index = position;
			c.value = newValue;
			v[c.index] = newValue;
			
			if(commandTarget < commands.size())
				commands[commandTarget] = c;
			else
				commands.push_back(c);

			dirty();
		}
	}
}

//---------------------------------------------------------------------

template<class ValueType>
inline const unsigned int DeltaVector<ValueType>::size() const
{
	return v.size();
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVector<ValueType>::unpack(Stream::ReadIterator & source)
{
	unsigned int count = 0;
	Archive::get(source, count);
	for(unsigned int i = 0; i < count; ++ i)
	{
		ValueType val;
		Archive::get(source, val);
		v.push_back(val);
	}
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVector<ValueType>::unpackDelta(Stream::ReadIterator & source)
{
	unsigned int count = 0;
	Archive::get(source, count);

	static Command c;
	for(unsigned int i = 0; i < count; ++ i)
	{
		Archive::get(source, c.cmd);
		Archive::get(source, c.index);
		switch(c.cmd)
		{
		case Command::ERASE:
			{
				Archive::get(source, c.currentContainerSize);
				if(c.currentContainerSize == size())
					erase(c.index);
			}
			break;
		case Command::INSERT:
			{
				Archive::get(source, c.currentContainerSize);
				Archive::get(source, c.value);
				if(c.currentContainerSize == size())
					insert(c.index, c.value);
			}
			break;
		case Command::SET:
			{
				Archive::get(source, c.value);
				set(c.index, c.value);
			}
			break;
		}
	}
}


//---------------------------------------------------------------------

}//namespace Archive

#endif	// _DeltaVector_H
