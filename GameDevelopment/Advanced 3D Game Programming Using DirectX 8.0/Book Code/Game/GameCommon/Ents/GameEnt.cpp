/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameEnt.cpp
 *    Desc: Standard implementation of the entity interface.
 *          Most child classes extend only a few functions
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "..\GameCell.h"
#include "GameEnt.h"
#include "..\GameWorld.h"
#include "..\NetMsgs\EntNM.h"


matrix4 sEntState::BuildMatrix()
{
	Snap( m_pitch, -PI/2 + EPSILON, PI/2 - EPSILON );
	matrix4 out;
	out.ToObjectLookAt( 
		m_loc, 
		m_loc + 
		point3::Spherical( m_yaw, m_pitch, 1.f ) );
	return out;
}

cGameEnt::cGameEnt()
: m_mat( matrix4::Identity )
, m_mass( 0.f )
, m_id( g_invalidID )
{
	/**
	 * Notify the world of our creation
	 */
	GetWorld()->NotifyAdd( this );
}

cGameEnt::~cGameEnt()
{
	LogPrint("[cGameEnt::~cGameEnt] Entering destructor for (%d:%d)", 
		GetIDSegment(GetID()), GetIDOffset(GetID()) );

	/**
	 * If we're the server, un-notify our parent cell.
	 */
	if( !IsClient() )
	{
		SendMessage( 
			sMsg( msgRemObjRef, m_parent, (int)GetID() )
			);
	}

	/**
	 * Unlist the object
	 */
	MsgDaemon()->UnRegObject( GetID() );

	/**
	 * Notify the world of our removal
	 */
	GetWorld()->NotifyRemove( this );
}

void cGameEnt::RebuildMatrix()
{
	Snap( m_currState.m_pitch, -PI/2 + EPSILON, PI/2 - EPSILON );
	m_mat.ToObjectLookAt( 
		GetLoc(), 
		GetLoc() + 
		point3::Spherical( GetYaw(), GetPitch(), 1.f ) );
}

void cGameEnt::Tick( float tickLength )
{
}


bool cGameEnt::CheckState()
{
	// Early-out if we're not alive.
	if( GetStatus() != gesAlive )
		return true;

	cGameCell* pParent = (cGameCell*)MsgDaemon()->Get( m_parent );

	if( !pParent )
	{
		LogPrint("CheckState: bad parent ID");
		return true;
	}

	cGameCell* pNewParent;
	
	bool bStillInParent = pParent->ContainsPoint( GetLoc() );

	if( bStillInParent )
	{
		pNewParent = pParent;
		if( pParent->ContainsSphere( m_bSphere, true ) )
		{
			/**
			 * the object is still in the same cell, 
			 * it doesn't need to be moved
			 */
		}
		else
		{
			/**
			 * See if the object is inside a portal
			 */
			int portalNum = pParent->PointInAnyPortal( GetLoc(), m_bSphere.m_radius );

			if( portalNum == -1 )
			{
				/**
				 * The object wasn't in any portal, 
				 * it's just too close to a wall.
				 * snap it back.
				 */
				//DebPrint("Ack! I have hit a wall!");
				if( -1 == pParent->SnapInside( this ) )
				{
					/**
					 * whoops... as a result of the snap, the object
					 * that called this function has committed suicide.
					 * exit without referencing itself again.
					 */
					return true;
				}

				// FIXME: return here?  or also do object tests?
				return false;
			}

			else
			{
				/**
				 * The object was in a portal.
				 * make sure it's not intersecting
				 * any portal walls, then continue
				 */
				if( -1 == pParent->SnapInsidePortal( this, portalNum ) )
				{
					/**
					 * whoops... as a result of the snap, the object
					 * that called this function has committed suicide.
					 * exit without referencing itself again.
					 */
					return true;
				}
			}
		}
	}
	else
	{
		/**
		 * Search the neighborhood
		 */
		list< cGameCell* > cellList;
		pParent->Traverse( &cellList, 2 );
		list< cGameCell* >::iterator iter;
		for( iter = cellList.begin(); iter != cellList.end(); iter++ )
		{
			if( (*iter)->ContainsPoint( GetLoc() ) )
			{
				/**
				 * Hand off the object and try again.
				 */
				pParent->RemoveObject( this );
				(*iter)->AddObject( this );
				m_parent = (*iter)->GetID();
				return CheckState();
			}
		}

		/**
		 * If we make it here, the object wasn't in any 
		 * other cell, at least any of the ones nearby.
		 * snap to the boundaries of the cell.
		 */
		if( -1 == pParent->SnapInside( this ) )
		{
			/**
			 * whoops... as a result of the snap, the object
			 * that called this function has committed suicide.
			 * exit without referencing itself again.
			 */
			return true;
		}
	}

	return true;
}



objID cGameEnt::GetID()
{
	return m_id;
}


void cGameEnt::SetID( objID id )
{
	// if we're changing the id from an invalid value, just set it.
	if( m_id == -1 )
	{
		m_id = id;
	}
	// otherwise, unregister the old object and re-register the new one.
	else
	{
		MsgDaemon()->UnRegObject( m_id );
		MsgDaemon()->RegObject( id, this );
		m_id = id;
	}
}


void cGameEnt::SetLoc( const point3& loc )
{
	m_currState.m_loc = loc;
	m_bSphere.m_loc = loc;
}

void cGameEnt::SetVel( const point3& vel )
{
	m_currState.m_vel = vel;
}


void cGameEnt::SetYaw( float yaw )
{
	m_currState.m_yaw = yaw;
}


void cGameEnt::SetPitch( float pitch )
{
	m_currState.m_pitch = pitch;
	Snap( m_currState.m_pitch, (-PI/2)+EPSILON, (PI/2)-EPSILON );
}


void cGameEnt::SetParent( objID parent )
{
	if( parent != m_parent )
	{
		/**
		 * Notify our old parent of removal
		 */
		m_parent = parent;

	}
}

void cGameEnt::SetHealth( int newHealth )
{
	m_currState.m_health = newHealth;

	DebPrint("Damage incurred.  new health is %d", newHealth );

	if( newHealth < 0 )
	{
		m_currState.m_health = 0;
		// TODO: A good death animation would be started here.
		// Possibly the state could be set to dying and an
		// animation would be started.  When the animation was finished 
		// the state would be set to unspawned.
		SetStatus( gesUnSpawned );
		SetLoc( point3(1000,1000,1000) );
		DebPrint("Ack!  I am slain!");
	}
}

uint cGameEnt::ProcMsg( const sMsg& msg )
{
	switch( msg.m_type )
	{
	case msgQueryGhost:
		break;

	case msgGetYaw:
		return FloatToMRet( GetYaw() );

	case msgGetPitch:
		return FloatToMRet( GetPitch() );

	case msgGetLoc:
		return PointToMRet( GetLoc() );

	case msgGetSphere:
		return BSphereToMRet( GetSphere() );

	case msgGetMatrix:
		return MatrixToMRet( GetMatrix() );

	case msgGetParent:
		return (uint)GetParent();

	case msgSetYaw:
		SetYaw( msg.m_f[0] );
		break;

	case msgSetPitch:
		SetPitch( msg.m_f[0] );
		break;

	case msgSetLoc:
		SetLoc( msg.m_pt );
		break;

	case msgSetParent:
		SetParent( msg.m_i[0] );
		break;

	case msgRebuildMatrix:
		RebuildMatrix();
		break;

	case msgQueryInPlane:
		//if( SPHERE_COPLANAR == (msg.m_plane).TestBSphere( this->m_bSphere ) )
		{
			float dp = (GetLoc() * msg.m_plane.n) + msg.m_plane.d;
			if((dp - m_bSphere.m_radius) < 0)
			{
				return 1; // yes it was
			}
		}
		return 0; // no it wasn't

	case msgNotifyPlaneCollision:
		DP0( "performing default plane collision\n");
		AbutPlane( msg.m_plane );
		return 0; // the object is still alive and well.

	default:
		DP1( "cGameEnt got unsup message [%x]", msg.m_type );
	}
	return 0;
}


void cGameEnt::AbutPlane( const plane3& plane )
{
	float dp = (GetLoc() * plane.n) + plane.d;

	/**
	 * We use a buffer region of -.05f
	 * a little safer than epsilon at
	 * avoiding infinite recursion in a
	 * couple of spots.
	 */
	if((dp - m_bSphere.m_radius) < 0)
	{
		// sphere is not in front, correct.
		float transAmt = EPSILON -dp + m_bSphere.m_radius;
		SetLoc( GetLoc() + plane.n * transAmt );
		m_bSphere.m_loc = GetLoc();
	}
	else
	{
		DP0("[ERROR!]: object got bogus AbutPlane!\n");
	}
}

void cGameEnt::LastGoodLoc()
{
	m_currState.m_loc = m_prevState.m_loc;
}



bool ResetState( cGameEnt* pEnt )
{
	/**
	 * Destroy the velocity used in the
	 * last frame.  Player objects will
	 * send their velocities over the
	 * network, rockets will re-create
	 * them when the need arises.
	 */
	pEnt->SetVel( point3::Zero );

	pEnt->m_prevState = pEnt->m_currState;

	return true;
}


bool BroadcastStateChanges( cGameEnt* pEnt )
{
	// First see if we need to send a full state change.
	if( NonMiniDifferent( pEnt->m_prevState, pEnt->m_currState) )
	{
		// Broadcast the new state
		sEntState state = pEnt->GetState();
		cNM_EntStateChange msg( pEnt->GetID(), state );
		UnreliableBroadcast( msg );
	}
	else if( MiniDifferent(pEnt->m_prevState, pEnt->m_currState) )
	{
		// Broadcast the new state
		sEntState state = pEnt->GetState();
		cNM_MiniStateChange msg( pEnt->GetID(), state );
		UnreliableBroadcast( msg );
	}
	// else don't send anything.
	return true;
}


extern void PlayGameSound( resID id );

bool MortisCheck( cGameEnt* pEnt )
{
	if( pEnt->GetStatus() == gesMortis )
	{
		DebPrint("[MortisCheck]: Removing object %d%d", 
			GetIDSegment( pEnt->GetID() ),
			GetIDOffset( pEnt->GetID() ) );
		// remove the object.

		// If we're destroying a rocket on the client , play an explosion sound.
		if( GetIDSegment( pEnt->GetID() ) == c_projSegment )
		{
			// Play the explosion sound. this is a no-op on the server.
			PlayGameSound( 201 );
		}
		delete pEnt;
	}
	return true;
}
