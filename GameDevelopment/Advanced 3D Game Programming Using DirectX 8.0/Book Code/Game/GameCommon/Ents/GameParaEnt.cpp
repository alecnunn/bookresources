/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameParaEnt.cpp
 *    Desc: Parametric object, such as a rocket or particle
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

/**
 * This file isn't used in the project, but was at one point
 * so it might prove useful to you.
 */

#include "stdafx.h"
#include "GameParaEnt.h"

objID GetNextTempParaID()
{
	static ushort nextOffset = 0;
	return MakeID( c_tempSegment, nextOffset++ );
}


objID GetNextParaEntID()
{
	static ushort nextOffset = 0;
	return MakeID( c_paraSegment, nextOffset++ );
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cGameParaEnt::cGameParaEnt() 
: cGameEnt()
{

}

bool cGameParaEnt::RespondToCollision( cGameEnt* pOther )
{
	if( pOther == NULL )
	{
		// object hit a wall
	}
	else
	{
	}
	return true;
}

void cGameParaEnt::Tick( float tickLength )
{
	m_t += tickLength;
	SetLoc( m_x0 + m_t * m_v0 + (m_t * m_t) * m_a / 2.f ); 

	point3 v = m_v0 + m_t * m_a;
	// use the velocity to find the yaw and pitch
	float vMag = v.Mag();
	if( vMag < 0.1f )
	{
		v = point3::k;
		vMag = 1.f;
	}
	v /= vMag;
	

	// tempPt is the velocity sitting on the x-z plane
	point3 tempPt( v.x, 0, v.z );
	float tempMag = tempPt.Mag();
	if( tempMag < 0.1f )
	{
		tempPt = point3(0,0,1);
		tempMag = 1.f;
	}
	tempPt /= tempMag;
	SetYaw( atan2( tempPt.x, tempPt.z ) );
	SetPitch( acos( v * tempPt ) );
}


void SpawnSparks( const plane3& plane, const point3& pt )
{
	int numSparks = 15;

	point3 vec1;
	point3 vec2;

	if( plane.n == point3::j || plane.n == (-1*point3::j) )
	{
		vec1 = point3::i ^ plane.n;
	}
	else
	{
		vec1 = point3::j ^ plane.n;
	}

	vec1.Normalize();

	vec2 = vec1 ^ plane.n;

	for( int i=0; i<numSparks; i++ )
	{
		point3 vel = 
			RandFloat( -1.f, 1.f ) * vec1 +
			RandFloat( -1.f, 1.f ) * vec2 +
			RandFloat( 0.5, 1.f ) * plane.n;

		vel.Normalize();

		point3 loc = pt + 0.1f * plane.n;

		cGameParaEnt::CreateParaEnt(
			petSpark,
			GetNextTempParaID(),
			loc,
			0.f,
			vel * RandFloat( 1.8f, 3.2f ),
			point3(0,-3.5f,0) );
	}
}



uint cGameParaEnt::ProcMsg( const sMsg& msg )
{
	switch( msg.m_type )
	{
	case msgNotifyPlaneCollision:
		switch( m_type )
		{
		case petRocket:
			DP0( "KABOOM! A rocket exploded.\n");

			/**
			 * Play the explosion sound, while we're at it.
			 */
			{
				cSoundWrapper* pSoundWrap = (cSoundWrapper*)Resources()->Get( 201 );
				pSoundWrap->Play();
			}

			SpawnSparks( msg.m_plane, GetLoc() );

			//delete this;
			return -1;
			break;
		case petSpark:
			m_nBounces++;
			if( m_nBounces > 3 )
			{
				//delete this;
				return -1;
			}
			else
			{
				// reflect the velocity
				float dist = m_v0*msg.m_plane.n;
				m_v0 = m_v0 - msg.m_plane.n * (dist*2);
				m_t = 0.f;
				m_x0 = GetLoc() + msg.m_plane.n * 0.1f;
			}
			break;
		default:
			break;
		}
		break;
	default:
		return cGameEnt::ProcMsg( msg );
	}
	return 0;
}


cGameParaEnt* cGameParaEnt::CreateParaEnt(
	eParaEntType type,
	objID id,
	const point3& x0,
	float tInitial,
	const point3& v0,
	const point3& a )
{
	DP0("Spawning Parametric entity\n");
	cGameParaEnt* pOut = new cGameParaEnt();
	pOut->m_type = type;
	pOut->m_nBounces = 0;
	switch( type )
	{
	case petRocket:
		{
			pOut->m_x0 = x0;
			pOut->m_v0 = v0;
			pOut->m_a = a;
			pOut->m_t = tInitial;
			// init the other variables			
			pOut->Tick( 0 ); 

			pOut->m_bSphere.m_radius = 0.3f;
			pOut->SetID( id );
		}
		break;
	case petSpark:
		{
			pOut->m_x0 = x0;
			pOut->m_v0 = v0;
			pOut->m_a = a;
			pOut->m_t = tInitial;
			// init the other variables			
			pOut->Tick( 0 ); 

			pOut->m_bSphere.m_radius = 0.1f;
			pOut->SetID( id );		
		}
		break;
	default:
		DP0("Bad parametric type!");
		assert( false );
	}

	/**
	 * Register the object
	 */
	MsgDaemon()->RegObject( pOut->GetID(), (iGameObject*)pOut );

	/**
	 * Find the parent cell
	 */
	sMsg msg( msgFindContainingCell, g_gameWorldID, 0 );
	msg.m_pt = pOut->GetLoc();
	pOut->m_parent = MsgDaemon()->DeliverMessage( msg );

	/**
	 * Notify the cell of our entrance
	 */
	msg.m_type = msgAddObjRef;
	msg.m_dest = pOut->m_parent;
	msg.m_i[0] = pOut->GetID();
	MsgDaemon()->DeliverMessage( msg );
	
	return pOut;
}

