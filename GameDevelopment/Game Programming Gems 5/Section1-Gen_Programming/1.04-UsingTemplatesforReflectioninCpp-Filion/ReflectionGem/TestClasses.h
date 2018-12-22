//
// TestClass.h
//

#ifndef __TEST_CLASS_H
#define __TEST_CLASS_H

#include "BaseObject.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWeapon
//
// An example class using the reflection feature. This is accessible through a pointer property in
// CMonsterStats and will be shown in the UI as a sub-category.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CWeapon : public CRTTIClass<CWeapon, CBaseObject, 0x87feda90>
{

public :

	//----------------------------------------------------------------------------------------------
	CWeapon();

	//----------------------------------------------------------------------------------------------
	// Sample accessors.
	char*	GetName()						{ return m_szName;			};
	void	SetName( char* szValue )		{ m_szName = szValue;		};

	int		GetMinDmg()						{ return m_iMinDmg;			};
	void	SetMinDmg( int iValue )			{ m_iMinDmg = iValue;		};

	int		GetMaxDmg()						{ return m_iMaxDmg;			};
	void	SetMaxDmg( int iValue )			{ m_iMaxDmg = iValue;		};

	float	GetAreaOfEffect()				{ return m_fAreaOfEffect;	};
	void	SetAreaOfEffect( float fValue )	{ m_fAreaOfEffect = fValue;	};

	//----------------------------------------------------------------------------------------------
	// Properties are registered by implementing this method in each class.
	static	void RegisterReflection();

private :

	char*	m_szName;
	int		m_iMinDmg;
	int		m_iMaxDmg;
	float	m_fAreaOfEffect;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CBaseMonsterStats
//
// An example class using the reflection feature. This is a base class of CMonsterStats and its
// properties will be shown as part of CMonsterStats', above the child class properties.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CBaseMonsterStats : public CRTTIClass<CBaseMonsterStats, CBaseObject, 0x87456378>
{

public :

	//----------------------------------------------------------------------------------------------
	CBaseMonsterStats();

	//----------------------------------------------------------------------------------------------
	// Sample accessors.
	char*	GetName()					{ return m_szName;		};
	void	SetName( char* szValue )	{ m_szName = szValue;	};

	//----------------------------------------------------------------------------------------------
	// Properties are registered by implementing this method in each class.
	static	void RegisterReflection();

private :

	char*		m_szName;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CMonsterStats
//
// An example class using the reflection feature.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CMonsterStats : public CRTTIClass<CMonsterStats, CBaseMonsterStats, 0x2e160f7a>
{

public :

	//----------------------------------------------------------------------------------------------
	CMonsterStats();

	//----------------------------------------------------------------------------------------------
	// Sample accessors.
	int		GetHitPoints()				{ return m_iHitPoints;	};
	void	SetHitPoints( int iValue )	{ m_iHitPoints = iValue;};

	int		GetLevel()					{ return m_iLevel;		};
	void	SetLevel( int iValue )		{ m_iLevel = iValue;	};

	float	GetSpeed()					{ return m_fSpeed;		};
	void	SetSpeed( float fValue )	{ m_fSpeed = fValue;	};

	bool	GetFlatulent()				{ return m_bFlatulent;	};
	void	SetFlatulent( bool bValue )	{ m_bFlatulent = bValue;};

	CWeapon*	GetWeapon()				{ return &m_Weapon;		};

	//----------------------------------------------------------------------------------------------
	// Properties are registered by implementing this method in each class.
	static	void RegisterReflection();

private :

	int			m_iLevel;
	int			m_iHitPoints;
	float		m_fSpeed;
	bool		m_bFlatulent;
	CWeapon		m_Weapon;

};

#endif	// __TEST_CLASS_H