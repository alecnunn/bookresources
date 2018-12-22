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

#ifndef	_Transceiver_H
#define	_Transceiver_H

#pragma warning ( disable : 4786 ) // identifier was truncated to 255 characters in the debug information
//-----------------------------------------------------------------------

#include <set>
#include <vector>

//---------------------------------------------------------------------

namespace Dispatch {

class TransceiverBase 
{
public:
	TransceiverBase() {};
	virtual ~TransceiverBase() {};
};

template<typename MessageType, typename IdentifierType = void *>
class Transceiver : public TransceiverBase
{
public:
	Transceiver();
	virtual ~Transceiver();
	void emitMessage(MessageType source) const;
	void listenForAny();
	void listenTo(Transceiver<MessageType, IdentifierType> & source);
	virtual void receiveMessage(MessageType) {};

private:
	Transceiver(const Transceiver & source);
	Transceiver & operator = (const Transceiver &  rhs);
	static std::set<Transceiver<MessageType, IdentifierType> *> & getGlobalReceiverSet();

private:
	std::set<Transceiver<MessageType, IdentifierType> *> localReceiverSet;
	std::set<Transceiver<MessageType, IdentifierType> *> listenSet;
};

//---------------------------------------------------------------------

template<typename MessageType, typename IdentifierType>
inline Transceiver<MessageType, IdentifierType>::Transceiver() :
localReceiverSet()
{
}

//---------------------------------------------------------------------

template<typename MessageType, typename IdentifierType>
inline Transceiver<MessageType, IdentifierType>::~Transceiver()
{
	std::set<Transceiver<MessageType, IdentifierType> *>::iterator i;
	std::set<Transceiver<MessageType, IdentifierType> *>::iterator f;

	// advise receivers that this Transceiver is going away
	for(i = localReceiverSet.begin(); i != localReceiverSet.end(); ++i)
	{
		(*i)->listenSet.erase((*i)->listenSet.find(this));
	}

	for(i = listenSet.begin(); i != listenSet.end(); ++i)
	{
		f = (*i)->localReceiverSet.find(this);
		if(f != (*i)->localReceiverSet.end())
			(*i)->localReceiverSet.erase(f);
	}

	f = getGlobalReceiverSet().find(this);
	if(f != getGlobalReceiverSet().end())
		getGlobalReceiverSet().erase(f);
}

//---------------------------------------------------------------------

template<typename MessageType, typename IdentifierType>
inline void Transceiver<MessageType, IdentifierType>::emitMessage(MessageType message) const
{
	std::set<Transceiver<MessageType, IdentifierType> *>::iterator i;
	for(i = localReceiverSet.begin(); i != localReceiverSet.end(); ++i)
	{
		(*i)->receiveMessage(message);
	}

	for(i = getGlobalReceiverSet().begin(); i != getGlobalReceiverSet().end(); ++i)
	{
		(*i)->receiveMessage(message);
	}
}

//---------------------------------------------------------------------

template<typename MessageType, typename IdentifierType>
inline std::set<Transceiver<MessageType, IdentifierType> *> & Transceiver<MessageType, IdentifierType>::getGlobalReceiverSet()
{
	static std::set<Transceiver<MessageType, IdentifierType> *> globalReceiverSet;
	return globalReceiverSet;
}

//---------------------------------------------------------------------

template<typename MessageType, typename IdentifierType>
inline void Transceiver<MessageType, IdentifierType>::listenForAny()
{
	getGlobalReceiverSet().insert(this);
}

//---------------------------------------------------------------------

template<typename MessageType, typename IdentifierType>
inline void Transceiver<MessageType, IdentifierType>::listenTo(Transceiver<MessageType, IdentifierType> & source)
{
	source.localReceiverSet.insert(this);
	listenSet.insert(&source);
}

//---------------------------------------------------------------------

template<typename MessageType, typename ObjectType, typename IdentifierType>
class OwnedTransceiver : public Dispatch::Transceiver<MessageType, IdentifierType>
{
public:
	OwnedTransceiver(Transceiver<MessageType, IdentifierType> * source, ObjectType & o);
	~OwnedTransceiver();
	void receiveMessage(MessageType msg)
	{
		(owner.*callback)(msg);
	};

	void setCallback(void (ObjectType::*cb)(MessageType) )
	{
		callback = cb;
	};

private:
	friend class Callback;
	OwnedTransceiver<MessageType, ObjectType, IdentifierType> & operator = (const OwnedTransceiver<MessageType, ObjectType, IdentifierType> &);
	OwnedTransceiver(const OwnedTransceiver<MessageType, ObjectType, IdentifierType> &);
	Transceiver<MessageType, IdentifierType> * source;
	ObjectType & owner;
	void (ObjectType::*callback)(MessageType);
};

//---------------------------------------------------------------------

template<typename MessageType, typename ObjectType, typename IdentifierType>
inline OwnedTransceiver<MessageType, ObjectType, IdentifierType>::OwnedTransceiver(Transceiver<MessageType, IdentifierType> * s, ObjectType & o) :
source(s),
owner(o),
callback(0)
{
}

//-----------------------------------------------------------------------

template<typename MessageType, typename ObjectType, typename IdentifierType>
inline OwnedTransceiver<MessageType, ObjectType, IdentifierType>::~OwnedTransceiver()
{
}

//-----------------------------------------------------------------------
class Callback
{
public:
	Callback();
	~Callback();

	template<typename MessageType, typename ObjectType>
		void connect(
		             Dispatch::Transceiver<MessageType, void *> & source, 
		             ObjectType & object,
		             void (ObjectType::*callback)(MessageType)
		            )
	{
		OwnedTransceiver<MessageType, ObjectType, void *> * target = new OwnedTransceiver<MessageType, ObjectType, void *>(&source, object);
		receivers.push_back(target);
		
		target->setCallback(callback);
		target->listenTo(source);
	}

	template<typename MessageType, typename ObjectType, typename IdentifierType>
		void connect(
		             Callback & cb,
		             Dispatch::Transceiver<MessageType, IdentifierType> & source, 
		             ObjectType & object,
		             void (ObjectType::*callback)(MessageType),
		             const IdentifierType *
		            )
	{
		OwnedTransceiver<MessageType, ObjectType, IdentifierType> * target = new OwnedTransceiver<MessageType, ObjectType, IdentifierType>(&source, object);
		receivers.push_back(target);
		
		target->setCallback(callback);
		target->listenTo(source);
	}


	template<typename MessageType, typename ObjectType>
		inline void connect(ObjectType & object, void (ObjectType::*callback)(MessageType))
	{
		OwnedTransceiver<MessageType, ObjectType, void *> * target = new OwnedTransceiver<MessageType, ObjectType, void *>(0, object);
		receivers.push_back(target);
		
		target->setCallback(callback);
		target->listenForAny();
	}

	template<typename MessageType, typename ObjectType, typename IdentifierType>
		inline void connect(ObjectType & object, void (ObjectType::*callback)(MessageType), const IdentifierType *)
	{
		OwnedTransceiver<MessageType, ObjectType, IdentifierType> * target = new OwnedTransceiver<MessageType, ObjectType, IdentifierType>(0, object);
		receivers.push_back(target);
		
		target->setCallback(callback);
		target->listenForAny();
	}

	template<typename MessageType, typename ObjectType>
		void disconnect(
		                Transceiver<MessageType, void *> & source,
						ObjectType & object,
						void (ObjectType::*callback)(MessageType)
		               )
	{
		removeTarget(&source, object, callback);
	}

	template<typename MessageType, typename ObjectType, typename IdentifierType>
		void disconnectByType(
		                Transceiver<MessageType, IdentifierType> & source,
						ObjectType & object,
						void (ObjectType::*callback)(MessageType),
	                    const IdentifierType *
		               )
	{
		removeTarget(&source, object, callback);
	}

	template<typename MessageType, typename ObjectType>
		void disconnect(ObjectType & object, void (ObjectType::*callback)(MessageType) )
	{
		removeTarget(static_cast<Transceiver<MessageType, void *> *>(0), object, callback);
	}

	template<typename MessageType, typename ObjectType, typename IdentifierType>
		void disconnect(ObjectType & object, void (ObjectType::*callback)(MessageType), const IdentifierType *)
	{
		removeTarget(static_cast<Transceiver<MessageType, IdentifierType> *>(0), object, callback);
	}

private:
	template<typename MessageType, typename ObjectType, typename IdentifierType>
		void removeTarget(
		                  Transceiver<MessageType, IdentifierType> * source,
		                  ObjectType & object,
		                  void (ObjectType::*callback)(MessageType)
		                 )
	{
		std::vector<TransceiverBase *>::iterator i;
		OwnedTransceiver<MessageType, ObjectType, IdentifierType> * target;
		
		for(i = receivers.begin(); i != receivers.end(); ++i)
		{
			target = dynamic_cast<OwnedTransceiver<MessageType, ObjectType, IdentifierType> *>((*i));
			if(target)
			{
				if(&target->owner == &object && target->callback == callback && target->source == source)
				{
					delete target;
					receivers.erase(i);
					break;
				}
			}
		}
	}

	std::vector<TransceiverBase *> receivers;
};
	
//-----------------------------------------------------------------------
}//namespace Dispatch

#endif	// _Transceiver_H
