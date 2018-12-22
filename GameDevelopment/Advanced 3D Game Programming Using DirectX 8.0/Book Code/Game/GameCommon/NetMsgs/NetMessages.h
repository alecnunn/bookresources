/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: NetMessages.h
 *    Desc: All of the network messages the game uses
 *          gives abstract defenitions, hides ugly details 
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _NETMESSAGES_H
#define _NETMESSAGES_H

// Include STL stuff...
#include <map>
using namespace std;

// And other game headers...
#include "..\..\gamelib\GameTypes.h"
#include "..\MsgDaemon.h"

#define MAX_MESSAGE_SIZE 1024

typedef DWORD netID; // machine ID.

/**
 * Generic Message
 * Every message class derives from this one.
 */
class cNetMessage
{
public:

	/**
	 * Write out a bitstream to be sent over
	 * the wire that encapsulates the data of
	 * the message.
	 */
	virtual int SerializeTo( uchar* pOutput ) 
	{ 
		return 0; 
	} 

	/**
	 * Take a bitstream as input (coming in over
	 * the wire) and convert it into a message 
	 */
	virtual void SerializeFrom( uchar *pFromData, int datasize )
	{ 
	}

	/**
	 * This is called on a newly constructed message.
	 * The message in essence executes itself.  This
	 * works because of the objID system; the message
	 * object can communicate it's desired changes to
	 * the other objects in the system.
	 */
	virtual void Exec() = 0;

	netID GetFrom() 
	{ 
		return m_from; 
	}
	netID GetTo()
	{ 
		return m_to;
	}

	void SetFrom( netID id )
	{ 
	  m_from = id; 
	}

	void SetTo( netID id )
	{ 
	  m_to = id; 
	}

protected:

	netID m_from;
	netID m_to;
};


/**
 * Generic Message Maker
 * Based off the article "Why Pluggable Factories Rock my
 * Multiplayer World" as seen on www.gamedev.net.
 * 
 * The basic idea:
 * Messages register themselves (using DEFINE_MAKER) to have
 * a certain first byte; essentially an ID tag identifying the
 * bitstream.  ConstructMessage() looks up in m_registry to see
 * which derived Maker class (there is one for each message) should
 * receive the bitstream.  The maker for a particular message in
 * turn just calls SerializeFrom for the particular message it's
 * associated with.
 */
class cNetMessageMaker
{
public:

	cNetMessageMaker( unsigned long int type ) 
	{ 
		//m_registry.insert( std::make_pair( type, this ) ); 
		m_registry[ type ] = this;
	}
	static cNetMessage *ConstructMessage( uchar *pData, int dataSize);

protected:
	typedef std::map<unsigned long int, cNetMessageMaker *> tNetMessageMakerMap;
	static tNetMessageMakerMap m_registry;
	virtual cNetMessage *MakeMessage( uchar *pData, int datasize ) const = 0;
};



/**
 * This is a generic way to create a maker.
 * takes in a maker name, a mesasge name, 
 * and a net ID number.
 * A DEFINE_MAKER call should only be put in
 * NetMessages.cpp
 */
#define DEFINE_MAKER( cMakerName, cMsgName, msgID )  \
class cMakerName : public cNetMessageMaker \
{ \
public: \
	cMakerName() \
	: cNetMessageMaker( msgID ) \
	{ \
	} \
	static cMakerName m_registerthis; \
private: \
	cNetMessage* MakeMessage( uchar* pData, int datasize ) const \
	{ \
		cMsgName* pMsg = NULL; \
		try \
		{ \
			pMsg = new cMsgName; \
			pMsg->SerializeFrom( pData, datasize ); \
		} \
		catch( ... ) \
		{ \
			delete pMsg; \
			throw; \
		} \
		return pMsg; \
	} \
};\
cMakerName cMakerName::m_registerthis

#endif //_NETMESSAGES_H