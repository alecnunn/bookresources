/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: MsgDaemon.h
 *    Desc: Interface and constants for the inter-object messaging
 *          system.  This is the most important file in the game.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _MSGDAEMON_H
#define _MSGDAEMON_H

#include "..\math3d\point3.h"
#include "..\math3d\color3.h"
#include "..\math3d\plane3.h"

#include "..\gamelib\GameTypes.h"

#include "netmsgs\NetMessages.h"

// disable the annoying 255 character limit warning
#pragma warning( disable : 4786 )

#include <map>
using std::map;

/**
 * objID
 * this is the unique identifier that each object has.
 * no two objects can have the same ID.
 *
 * Pardon my 16-bit programming jargon, but the int is
 * composed of a high-order 16-bit number (segment) and
 * a low-order 16-bit number (offset).  The segment 
 * identifies the type of object (system objects and
 * singletons have a segment of 0, gamecells have a segment
 * of 1, and so forth).  The offsets are chosen such that
 * each created object of a certain type has a 
 * different offset.
 *
 * One extremely important point: In any game, the server
 * and clients have the same ID's for each object.  That way,
 * if a client wants to modify an object with a certain ID, 
 * it just tells the server what to modify by giving it the 
 * ID.  Almost universally (with the exception of temporary
 * entities like sparks) the server decides the objID of an
 * object.  For example, look at the static constructors for
 * the player entities (cGamePlayerEnt.cpp).  The server
 * constructor generates an ID, the client takes one given to
 * it by the server.
 */
typedef uint objID;

inline objID MakeID( ushort segment, ushort offset )
{
	return (((uint)segment)<<16) | ((uint)offset);
}

inline ushort GetIDSegment( objID id )
{
	return (ushort)(id>>16);
}

inline ushort GetIDOffset( objID id )
{
	return (ushort)(id & 0xFFFF);
}


/**
 * These segments define the types of objects
 */
const ushort c_sysSegment = 0;    // System object
const ushort c_cellSegment = 1;   // Cell object
const ushort c_playerSegment = 2; // Player object
const ushort c_spawnSegment = 3;  // Spawning object
const ushort c_projSegment = 4;   // Projectile object
const ushort c_paraSegment = 5;   // Parametric object
const ushort c_tempSegment = 6;   // Temp object


/**
 * A few global constants for system objects that
 * we know we have at all times.
 */

const objID g_invalidID =	0xFFFFFFFF; // Invalid ID
const objID g_gameID =		MakeID( c_sysSegment, 1 ); // cGame object (client)
const objID g_gameWorldID = MakeID( c_sysSegment, 2 ); // cGameWorld object
const objID g_gameServer =	MakeID( c_sysSegment, 1 ); // server object



/**
 * eMsgType
 * this is the list of all the messages that can be sent to objects
 * organized by the type of object.
 */
enum eMsgType
{
	//==========--------------------------  cGameApp (client) messages
	msgPrintString, // cGameApp, print a string (passed in m_pData)
	msgPlayerCreated, // The player (id in m_i[0]) has been created.
	msgGetServerTime, // returns the current server time.
	msgSendToServerR, // unreliable message (m_pData) to server
	msgSendToServerUR, // unreliable message (m_pData) to server

	//==========--------------------------  server messages 
	// Note: server can also receive msgPrintString and msgGetServerTime
	msgSendNetMessageR, // reliable message (m_pData) to player (m_dest)
	msgSendNetMessageUR, // unreliable message (m_pData) to player (m_dest)
	msgBroadcastR, // reliable broadcast of (m_pData) to all players
	msgBroadcastRExcept, // r. broadcast of (m_pData) to all players except m_i[1]
	msgBroadcastUR, // unreliable broadcast of (m_pData) to all players
	msgAddPlayerName, // the edit box will get m_pData
	msgRemovePlayerName, // the edit box will lose m_pData
	msgKillSocket, // kill the socket with host ID m_i[0]

	//==========--------------------------  cGameWorld messages
	msgFindContainingCell, // pt is a loc, retval is a objID for the containing cell
	msgAddPlayer, // i[0] is the player (addent must be called too)
	msgRemovePlayer, // i[0] is the player (removeent must be called too)
	msgAddEnt, // i[0] is the ent
	msgRemoveEnt, // i[0] is the ent

	//==========--------------------------  cGameCell messages
	msgAddObjRef, // sent when an object enters a cell, i[0] is the object
	msgRemObjRef, // sent when an object leaves a cell, i[0] is the object

	//==========--------------------------  cGameEnt messages
	msgQueryGhost, // returns 1 if it is a ghost
	msgRebuildMatrix, // rebuild the matrix
	msgQueryInPlane, // m_plane is a plane to test against

	msgGetYaw, // returns yaw
	msgGetPitch, // returns pitch
	msgGetLoc, // returns ptr to loc
	msgGetSphere, // returns ptr to bsphere
	msgGetParent, // returns id of parent
	msgGetMatrix, // returns ptr to matrix

	msgSetYaw, // f[0] is the new yaw
	msgSetPitch, // f[0] is the new pitch
	msgSetLoc, // pt is the new loc
	msgSetParent, // i[0] is the new parent

	// m_plane is the plane the object hit. 
	// 0 if ok, -1 if the object destroyed itself
	msgNotifyPlaneCollision, 

	// m_plane is the plane the object hit. 
	// 0 if ok, -1 if the object destroyed itself
	msgNotifyObjectCollision, 

	//==========--------------------------  cGameSpawnEnt messages
	msgSpawnObject, // i[0] is the id to spawn

	//==========--------------------------  cGamePlayerEnt messages
	msgGetSpawnLoc, // pt is the location to spawn projectiles
	msgGetSpawnDir, // pt is the direction to spawn projectiles

	//==========--------------------------  Misc messages
	msgForceDword = 0xFFFFFFFF
};



/**
 * The message structure that gets
 * passed around.  7 DWORDS in size.
 * a few constructors are defined to
 * help inline message sending
 */
struct sMsg
{
	eMsgType	m_type;
	objID		m_dest;
	union
	{
		struct
		{
			point3 m_pt;
		};
		struct
		{
			plane3 m_plane;
		};
		struct
		{
			color3 m_col;
		};
		struct
		{
			int m_i[4];
		};
		struct
		{
			float m_f[4];
		};
		struct
		{
			void* m_pData;
		};
	};

	sMsg( eMsgType type = msgForceDword, objID dest = 0 ) 
	: m_type( type )
	, m_dest( dest )
	{
	}

	sMsg( eMsgType type, objID dest, float f )
	: m_type( type )
	, m_dest( dest )
	{
		m_f[0] = f;
	}

	sMsg( eMsgType type, objID dest, int i ) 
	: m_type( type )
	, m_dest( dest )
	{
		m_i[0] = i;
	}

	sMsg( eMsgType type, objID dest, const point3& pt ) 
	: m_type( type )
	, m_dest( dest )
	, m_pt(pt)
	{
	}

	sMsg( eMsgType type, objID dest, const plane3& plane ) 
	: m_type( type )
	, m_dest( dest )
	, m_plane(plane)
	{
	}

	sMsg( eMsgType type, objID dest, void* pData ) 
	: m_type( type )
	, m_dest( dest )
	, m_pData( pData )
	{
	}
}; 


/**
 * iGameObject
 * This is the interface that allows the game to work.
 * each class in the game that is of any concern to the
 * networking components must implement this interface.
 * It allows the network client to get and set the unique
 * ID for each object (That way, two instances of an object,
 * on two different machines, can talk to each other, by
 * sending messages.
 * Messages are sent to an object using the ProcMsg call.
 * in code this breaks down to a large switch statement,
 * with a case for every type of message the class can
 * deal with.
 */
typedef uint msgRet;

interface iGameObject
{
public:
	virtual objID GetID() = 0;
	virtual void SetID( objID id ) = 0;

	virtual msgRet ProcMsg( const sMsg& msg ) = 0;
};



/**
 * cMsgDaemon
 * This class ties together all of the objects.
 * DeliverMessage provides an abstraction so that
 * objects can communicate without requiring pointers
 * to each other.  Objects can even be modified by
 * remote software (talking through the network layer).
 * Each Game Object must Register itself with RegObject,
 * and UnReg itself whenever it deletes itself.  
 */
class cMsgDaemon  
{
	map< objID, iGameObject* > m_objectMap;

	static cMsgDaemon* m_pGlobalMsgDaemon;

public:
	cMsgDaemon();
	~cMsgDaemon();

	static cMsgDaemon* GetMsgDaemon()
	{
		// Accessor to the singleton
		if( !m_pGlobalMsgDaemon )
		{
			m_pGlobalMsgDaemon = new cMsgDaemon;
		}
		return m_pGlobalMsgDaemon;
	}

	void RegObject( objID id, iGameObject* pObj );
	void UnRegObject( objID id );

	iGameObject* Get( int id )
	{
		return m_objectMap[id];
	}

	/**
	 * Deliver this message to the destination
	 * marked in msg.m_dest.  Throws an exception
	 * if no such object exists.
	 */
	uint DeliverMessage( const sMsg& msg );
};

//==========--------------------------  Conversion functions

inline msgRet FloatToMRet( const float& in )
{
	return *((msgRet*)(&in));
}

inline float MRetToFloat( const msgRet& in )
{
	return *((float*)(&in));
}


inline msgRet PointToMRet( const point3& in )
{
	return (msgRet)(&in);
}

inline point3 MRetToPoint( const msgRet& in )
{
	return *((point3*)in);
}



inline msgRet MatrixToMRet( const matrix4& in )
{
	return (msgRet)(&in);
}

inline matrix4 MRetToMatrix( const msgRet& in )
{
	return *((matrix4*)in);
}



inline msgRet BSphereToMRet( const bSphere3& in )
{
	return (msgRet)(&in);
}

inline bSphere3 MRetToBSphere( const msgRet& in )
{
	return *((bSphere3*)in);
}

//==========--------------------------  Global-scope inline functions


// Easy way to access the singleton
inline cMsgDaemon* MsgDaemon()
{
	return cMsgDaemon::GetMsgDaemon();
}



inline uint SendMessage( const sMsg& msg )
{
	return MsgDaemon()->DeliverMessage( msg );
}



void DebPrint( char* first, ... ); // Print to the console
void LogPrint( char* first, ... ); // print to the log file


/**
 * Easy way to get access to the world object
 */
class cGameWorld;
cGameWorld* GetWorld();

//==========--------------------------  Network-associated stuff

void ReliableSendToPlayer( cNetMessage& nMsg, objID player );
void UnreliableSendToPlayer( cNetMessage& nMsg, objID player );

void UnreliableBroadcast( cNetMessage& nMsg );
void ReliableBroadcast( cNetMessage& nMsg );
void ReliableBroadcastExcept( cNetMessage& nMsg, objID exception );

bool IsClient();
objID GetClientID();

#endif // _MSGDAEMON_H

