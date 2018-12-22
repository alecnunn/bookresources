/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: EntNM.h
 *    Desc: Entity-style net-messages.  Grok NetMessages.h before
 *          you try to grok this.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _ENTNM_H
#define _ENTNM_H

#include "NetMessages.h"

//=================================================================

/**
 * Message: Entity Addition
 * ID: 3
 * Desc: When a client connects to the server, when a rocket is
 * fired, etc, etc, etc, entities are added.  This message is
 * sent exclusively by the server, only it has the authority to
 * add objects to the world (clients request entity additions)
 */
class cNM_EntAdd 
: public cNetMessage
{
	int m_entID; // ID of the object that has been added.
	sEntState m_state;

public:

	cNM_EntAdd( 
		int entID, 
		sEntState state );
	cNM_EntAdd(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};


//=================================================================

/**
 * Message: Player Addition
 * ID: 12
 * Desc: Very similar to cNM_EntAdd.  The difference is that 
 * players have names and ent's don't, so they need their own
 * messages
 */
class cNM_PlayerAdd 
: public cNetMessage
{
	int m_playerID; // ID of the player that has been added.
	sEntState m_state;
	char m_name[32];

public:

	cNM_PlayerAdd( int playerID );
	cNM_PlayerAdd(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};


//=================================================================

/**
 * Message: Entity State Change
 * ID: 4
 * Desc: Sent by the server to clients.  Whenever a player moves
 * around or changes their state in any way, this message notifies
 * clients of the new state.
 */
class cNM_EntStateChange 
: public cNetMessage
{
	int m_entID; // ID of the object that has changed
	sEntState m_state;

public:

	cNM_EntStateChange( 
		int entID, 
		sEntState state );
	cNM_EntStateChange(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};


//=================================================================

/**
 * Message: Spawn Request
 * ID: 5
 * Desc: Sent by dead or unspawned clients that wish to be created.
 */
class cNM_SpawnReq 
: public cNetMessage
{
	int m_entID; // ID of the object that wants to be spawned

public:

	cNM_SpawnReq( int entID );
	cNM_SpawnReq(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};


//=================================================================

/**
 * Message: Entity State Change Request
 * ID: 6
 * Desc: Sent by the clients to servers.  Sent generally
 * after input is received.
 */
class cNM_EntStateChangeRequest 
: public cNetMessage
{
	int m_entID; // ID of the object that wants to change
	sEntState m_state;

public:

	cNM_EntStateChangeRequest( 
		int entID, 
		sEntState state );
	cNM_EntStateChangeRequest(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};

//=================================================================

/**
 * Message: Mini State Change
 * ID: 9
 * Desc: Sent by the server to clients.  This is for just changing
 * position and orientation, which happens often enough to justify
 * a special case.
 */
class cNM_MiniStateChange 
: public cNetMessage
{
	int m_entID; // ID of the object that has changed
	int m_time; // Time of the state change
	point3 m_loc; // location
	point3 m_vel; // velocity
	float m_yaw, m_pitch; // yaw and pitch

public:

	cNM_MiniStateChange( 
		int entID,
		sEntState& state );
	cNM_MiniStateChange(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};

//=================================================================

/**
 * Message: Mini State Change Request
 * ID: 10
 * Desc: Sent by the clients to servers.  Sent generally
 * after input is received.
 */
class cNM_MiniStateChangeRequest 
: public cNetMessage
{
	int m_entID; // ID of the object that wants to change
	int m_time; // Time of the state change
	point3 m_loc; // location
	point3 m_vel; // velocity
	float m_yaw, m_pitch; // yaw and pitch

public:

	cNM_MiniStateChangeRequest( 
		int entID,
		sEntState& state );
	cNM_MiniStateChangeRequest(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};

//=================================================================

/**
 * Message: Projectile fire request
 * ID: 11
 * Desc: Sent by a client to a server when the fire key is pressed.
 */
class cNM_ProjFireRequest 
: public cNetMessage
{
	int m_entID; // ID of the object that wants to fire
	float m_yaw, m_pitch; // yaw and pitch

public:

	// Input: the id of the object that wants to fire
	cNM_ProjFireRequest( objID id );
	cNM_ProjFireRequest(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};



#endif //_ENTNM_H