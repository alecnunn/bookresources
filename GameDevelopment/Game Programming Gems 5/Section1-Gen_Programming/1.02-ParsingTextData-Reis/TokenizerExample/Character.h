// Character.h: A Character class.
// Date: 7/02/04
// Created by: Aurelio Reis

#ifndef __CHARACTER__H__
#define __CHARACTER__H__

#include "Tokenizer.h"
#include "Utils.h"
using namespace AURELIO_REIS;


// Class pre-define.
class CWeapon;


// This class describes a game's character (NPC or PC).
class CCharacter
{
private:
	// This characters main weapon.
	CWeapon *m_pWeapon;

	// Parse the Token List.
	void Parse( CTokenizer &Toker );

public:
	// Character Attributes.
	char *m_strName;
	int m_iStrength;
	int m_iDexterity;
	int m_iConstitution;
	int m_iIntelligence;
	int m_iWisdom;
	int m_iCharisma;
	int m_iHitPoints;

	// Constructor.
	CCharacter();

	// Destructor.
	~CCharacter();

	// Set the characters name.
	void SetName( const char *strName )
	{
		ARReleaseString( m_strName );
		m_strName = ARCreateString( strName );
	}

	// Load the characters stats from a file.
	bool LoadStats( const char *strFile );

	// Show the characters stats.
	void ShowStats();
};

#endif