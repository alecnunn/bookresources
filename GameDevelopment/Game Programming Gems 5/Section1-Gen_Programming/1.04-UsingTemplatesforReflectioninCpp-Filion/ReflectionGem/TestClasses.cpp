//
// TestClass.cpp
//

#include "stdafx.h"
#include "PropertySystem.h"
#include "TestClasses.h"

CWeapon::CWeapon()
{
	m_szName = "Big Ass Whooping Weapon";
	m_iMinDmg = 10;
	m_iMaxDmg = 140;
	m_fAreaOfEffect = 4.5f;
}

void CWeapon::RegisterReflection()
{
	RegisterProperty<char*>( "Name", GetName, SetName );
	RegisterProperty<int>( "MinDmg", GetMinDmg, SetMinDmg );
	RegisterProperty<int>( "MaxDmg", GetMaxDmg, SetMaxDmg );
	RegisterProperty<float>( "AreaOfEffect", GetAreaOfEffect, SetAreaOfEffect );
}

CBaseMonsterStats::CBaseMonsterStats()
{
	m_szName = "Four-legged furry zombie giant";
}

void CBaseMonsterStats::RegisterReflection()
{
	RegisterProperty<char*>( "Name", GetName, SetName );
}

CMonsterStats::CMonsterStats()
{
	m_iLevel = 15;
	m_iHitPoints = 4000;
	m_fSpeed = 2.75;
	m_bFlatulent = true;
}

void CMonsterStats::RegisterReflection()
{
	RegisterProperty<int>( "Level", GetLevel, SetLevel );
	RegisterProperty<int>( "HitPoints", GetHitPoints, SetHitPoints );
	RegisterProperty<bool>( "Flatulent", GetFlatulent, SetFlatulent );
	RegisterProperty<float>( "Speed", GetSpeed, SetSpeed );
	RegisterProperty<CWeapon*>( "Weapon", GetWeapon, NULL );
}