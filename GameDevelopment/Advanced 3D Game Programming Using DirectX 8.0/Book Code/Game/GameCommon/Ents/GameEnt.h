/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameEnt.h
 *    Desc: This objest started out as an interface, but enough of
 *          the functions are the same across the board I made it
 *          a class.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include "..\MsgDaemon.h"
#include "..\ResourceMgr.h"
#include "..\..\math3d\point3.h"
#include "..\..\math3d\bSphere3.h"

class cGraphicsLayer;
struct matrix4;
class cGameCell;


/**
 * States that a game entity can be in
 */
enum eEntStatus
{
	gesDead			= 0, // Hello! I am dead!
	//gesBirth		= 1, // Hello! I was just born!
	gesAlive		= 2, // Hello! I am alive and well.
	gesDying		= 3, // Ack! I am dying!
	gesUnSpawned	= 4, // hasn't been spawned yet
	gesGhost		= 5, // things like spawners are ghosts
	gesMortis		= 6  // This object can be deleted on a mortis run.
};


/**
 * The entstate is a structure that holds
 * the state (position, velocity, health)
 * of an ent.  Each entity has a current
 * state and a previous state (the state from
 * the last timeslice).
 */
struct sEntState
{
	point3		m_loc;
	point3		m_vel;
	point3		m_acc;
	float		m_yaw;
	float		m_pitch;
	eEntStatus	m_status;
	int			m_health;

	sEntState()
	: m_loc( point3::Zero )
	, m_vel( point3::Zero )
	, m_acc( point3::Zero )
	, m_yaw( 0 )
	, m_pitch( 0 )
	, m_status( gesUnSpawned )
	, m_health( 0 )
	{
		// Do Nothing.
	}

	matrix4 BuildMatrix();

};

inline bool operator!=(const sEntState& a, const sEntState& b)
{
	if( 0 == memcmp( &a, &b, sizeof(sEntState ) ) )
	{
		// they are equal
		return false;
	}
	return true;
}

// if this is true, non-mini change stuff was different, which means
// we need to send a full state change for both sides to have the
// right info.
inline bool NonMiniDifferent(const sEntState& a, const sEntState& b)
{
	// Everything except loc, vel, yaw, pitch
	if( !(a.m_acc == b.m_acc) )
	{
		return true;
	}

	if( a.m_status != b.m_status )
	{
		return true;
	}

	if( a.m_health != b.m_health )
	{
		return true;
	}

	return false;
}

// if this is true, we must send a mini state change
inline bool MiniDifferent(const sEntState& a, const sEntState& b)
{
	if( !(a.m_loc == b.m_loc) )
	{
		return true;
	}
	if( !(a.m_vel == b.m_vel) )
	{
		return true;
	}
	if( a.m_yaw != b.m_yaw )
	{
		return true;
	}
	if( a.m_pitch != b.m_pitch )
	{
		return true;
	}
	return false;
}



/**
 * cGameEnt
 * an Ent is an entitity in the world, such as an item,
 * a rocket, a player, etc.
 */
class cGameEnt 
: public iGameObject
{

protected:

	objID		m_id;

	resID		m_modelID;
	resID		m_textureID;

	bool		m_bGhost; // 1 if the object can pass through other objects (but not walls)
	bool		m_bNoClip; // 1 if the object can pass through everything
	float		m_mass;
	matrix4		m_mat;
	bSphere3	m_bSphere;

	sEntState	m_currState;
	sEntState	m_prevState;

	// each entity has a parent object (The cell it's in)
	objID	m_parent;

	cGameEnt();

	/**
	 * Move the object such that it is
	 * /just/ touching the plane
	 */
	void AbutPlane( const plane3& plane );

public:

	virtual ~cGameEnt();

	void RebuildMatrix();

	//==========--------------------------  Processing functions
	virtual void	Tick( float tickLength );
	//virtual void	Draw() = 0;

	//==========--------------------------  Object-Info functions
	 
	// return true if we exist after the collision, false otherwise.
	virtual bool	RespondToCollision( cGameEnt* pOther ) = 0;
	virtual bool	CheckState();

	//==========--------------------------  iGameObject
	virtual objID GetID();
	virtual void SetID( objID id );

	virtual uint ProcMsg( const sMsg& msg ) = 0;

	virtual bool HitsObjects() = 0;

	//==========--------------------------  Function versions of all the messages
	const bSphere3& GetSphere(){ return m_bSphere; }
	const matrix4&	GetMatrix(){ return m_mat; }
	const point3&	GetLoc(){ return m_currState.m_loc; }
	const point3&	GetVel(){ return m_currState.m_vel; }
	const float		GetYaw(){ return m_currState.m_yaw; }
	const float		GetPitch(){ return m_currState.m_pitch; }
	const float		GetMass(){ return m_mass; }

	const sEntState& GetState(){ return m_currState; }

	int		GetHealth(){ return m_currState.m_health; }
	objID	GetParent(){ return m_parent; }

	void SetLoc( const point3& loc );
	void SetVel( const point3& vel );
	void SetYaw( float yaw );
	void SetPitch( float pitch );
	void SetParent( objID parent );
	void SetHealth( int newHealth );

	void SetState( const sEntState& state )
	{
		m_currState = state;
		m_bSphere.m_loc = state.m_loc;
	}

	eEntStatus GetStatus()
	{
		return m_currState.m_status;
	}

	void SetStatus( eEntStatus status )
	{
		m_currState.m_status = status;
	}

	friend bool ResetState( cGameEnt* pEnt );
	friend bool BroadcastStateChanges( cGameEnt* pEnt );

	/**
	 * Called on object-object collisions.
	 * moves the object back to the position
	 * it was in at the last frame.
	 */
	void LastGoodLoc();
};


/**
 * for_each things
 */
bool ResetState( cGameEnt* pEnt );
bool BroadcastStateChanges( cGameEnt* pEnt );
bool MortisCheck( cGameEnt* pEnt );


#endif //_GAMEOBJECT_H
