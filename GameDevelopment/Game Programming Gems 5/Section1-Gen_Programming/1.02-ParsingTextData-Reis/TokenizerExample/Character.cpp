// Character.cpp: A Character class.
// Date: 7/02/04
// Created by: Aurelio Reis

#include "Character.h"
#include "Weapon.h"
using namespace AURELIO_REIS;

#include <iostream>
#include <conio.h>
using namespace std;


// The custom field helper class is used to easily read in text key/value pairs. It works
// first by specifying a field name which is linked with a byte offset into a class's data
// members. The class objects memory position is used as the initial offset which can then be
// used to directly map values to their memory locations.
// TODO: By supplying a function pointer, we can allow for custom parsing routines.
CCustomField g_CharacterFields[] =
{
	{ "Name",				AR_FIELD_OFFSET( CCharacter, m_strName ),				CCustomField::FT_STRING },
	{ "Strength",			AR_FIELD_OFFSET( CCharacter, m_iStrength ),				CCustomField::FT_INT },
	{ "Dexterity",			AR_FIELD_OFFSET( CCharacter, m_iDexterity ),			CCustomField::FT_INT },
	{ "Constitution",		AR_FIELD_OFFSET( CCharacter, m_iConstitution ),			CCustomField::FT_INT },
	{ "Intelligence",		AR_FIELD_OFFSET( CCharacter, m_iIntelligence ),			CCustomField::FT_INT },
	{ "Wisdom",				AR_FIELD_OFFSET( CCharacter, m_iWisdom ),				CCustomField::FT_INT },
	{ "Charisma",			AR_FIELD_OFFSET( CCharacter, m_iCharisma ),				CCustomField::FT_INT },
	{ "HitPoints",			AR_FIELD_OFFSET( CCharacter, m_iHitPoints ),			CCustomField::FT_INT },
};
const int g_iNumCharacterFields = sizeof( g_CharacterFields ) / sizeof( g_CharacterFields[0] );


// Constructor.
CCharacter::CCharacter()
{
	m_strName = ARCreateString( "Unknown" );
	m_iStrength = 0;
	m_iDexterity = 0;
	m_iConstitution = 0;
	m_iIntelligence = 0;
	m_iWisdom = 0;
	m_iCharisma = 0;
	m_iHitPoints = 0;
}

// Destructor.
CCharacter::~CCharacter()
{
	ARReleaseString( m_strName );

/*	if ( m_pWeapon )
	{
		delete m_pWeapon;		m_pWeapon = NULL;
	}*/
}

// Parse the Token List.
void CCharacter::Parse( CTokenizer &Toker )
{
	CToken *pTok = Toker.GetNextToken();

	// Expecting:
	// Character
	// {
	//		<values>
	// }

	// First token MUST be the declaration.
	if ( !Toker.ExpectString( pTok, "Character" ) )
	{
		throw( "Error: Expecting \"Character\" declaration" );
	}

	pTok = Toker.GetNextToken();

	if ( !Toker.ExpectTokenType( pTok, TKT_OPENBRACE ) )
	{
		throw( "Error: Expecting {" );
	}

	pTok = Toker.GetNextToken();

	// Get the class byte offset.
	char *byOffset = (char *) this;

	// Read the character declaration body.
	while ( pTok )
	{
		// If closed brace, we're done.
		if ( Toker.ExpectTokenType( pTok, TKT_CLOSEDBRACE ) )
		{
			break;
		}

		// Otherwise parse the fields if we have a valid string.
		if ( !Toker.ExpectTokenType( pTok, TKT_STRING ) )
		{
			pTok = Toker.GetNextToken();
			continue;
		}

		// Special case weapon entries.
		const char *strVal = ((CTokenString *) pTok)->m_strValue;
		if ( strcmp( "Weapon", strVal ) == 0 )
		{
			// Parse out the weapon entry.
			if ( !m_pWeapon )
			{
				m_pWeapon = new CWeapon();
				m_pWeapon->Parse( Toker );
			}
			pTok = Toker.GetNextToken();
			continue;
		}

		CCustomField::ParseCustomFields( g_CharacterFields, g_iNumCharacterFields, (char *) this, Toker );

		pTok = Toker.GetNextToken();
	}
}

// Load the characters stats from a file.
bool CCharacter::LoadStats( const char *strFile )
{
	cout << "Tokenizing \"" << strFile << "\"...\n";

	// Tokenize the character file.
	CTokenizer Toker;
	if ( !Toker.TokenizeFile( strFile ) )
	{
		cout << "Error: File could not be read!" << endl << endl;
		return false;
	}

	cout << "Parsing...\n";

	// Parse out the character file.
	try
	{
		Parse( Toker );
	}
	catch( const char *strErr )
	{
		cout << strErr << endl << endl;
		return false;
	}

	return true;
}

// Show the characters stats.
void CCharacter::ShowStats()
{
	cout << endl << "<Character>" << endl;
	cout << "Name: " << m_strName << endl;
	cout << "Stength: " << m_iStrength << endl;
	cout << "Dexterity: " << m_iDexterity << endl;
	cout << "Constitution: " << m_iConstitution << endl;
	cout << "Intelligence: " << m_iIntelligence << endl;
	cout << "Wisdom: " << m_iWisdom << endl;
	cout << "Charisma: " << m_iCharisma << endl;
	cout << "HitPoints: " << m_iHitPoints << endl;

	if ( !m_pWeapon )
	{
		return;
	}

	cout << endl << "<Weapon>" << endl;
	cout << "Name: " << m_pWeapon->m_strName << endl;
	cout << "Attack: " << m_pWeapon->m_iAttack << endl;
	cout << "Defense: " << m_pWeapon->m_iDefense << endl;
	cout << "Weight: " << m_pWeapon->m_iWeight << endl;
}