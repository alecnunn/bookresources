// Weapon.h: A Weapon class.
// Date: 7/02/04
// Created by: Aurelio Reis

#include "Tokenizer.h"
#include "Utils.h"
using namespace AURELIO_REIS;


// This class describes a game character's Weapon.
class CWeapon
{
public:
	// Character Attributes.
	char *m_strName;
	int m_iAttack;
	int m_iDefense;
	int m_iWeight;

	// Constructor.
	CWeapon()
	{
		m_strName = ARCreateString( "Unknown" );
		m_iAttack = 0;
		m_iDefense = 0;
		m_iWeight = 0;
	}

	// Destructor.
	~CWeapon()
	{
		ARReleaseString( m_strName );
	}

	// Set the characters name.
	void SetName( const char *strName )
	{
		ARReleaseString( m_strName );
		m_strName = ARCreateString( strName );
	}

	// Parse a Weapon Entry.
	void Parse( CTokenizer &Toker );
};


// The custom field helper class is used to easily read in text key/value pairs. It works
// first by specifying a field name which is linked with a byte offset into a class's data
// members. The class objects memory position is used as the initial offset which can then be
// used to directly map values to their memory locations.
// TODO: By supplying a function pointer, we can allow for custom parsing routines.
CCustomField g_WeaponFields[] =
{
	{ "Name",				AR_FIELD_OFFSET( CWeapon, m_strName ),				CCustomField::FT_STRING },
	{ "Attack",				AR_FIELD_OFFSET( CWeapon, m_iAttack ),				CCustomField::FT_INT },
	{ "Defense",			AR_FIELD_OFFSET( CWeapon, m_iDefense ),				CCustomField::FT_INT },
	{ "Weight",				AR_FIELD_OFFSET( CWeapon, m_iWeight ),				CCustomField::FT_INT },
};
const int g_iNumWeaponFields = sizeof( g_WeaponFields ) / sizeof( g_WeaponFields[0] );


// Parse a Weapon Entry.
void CWeapon::Parse( CTokenizer &Toker )
{
	CToken *pTok = Toker.GetNextToken();

	if ( TKT_OPENBRACE != pTok->m_Type )
	{
		throw( "Error: Expecting { at beginning of weapon entry." );
	}

	pTok = Toker.GetNextToken();

	for ( ; pTok && TKT_STRING == pTok->m_Type; pTok = Toker.GetNextToken() )
	{
		CCustomField::ParseCustomFields( g_WeaponFields, g_iNumWeaponFields, (char *) this, Toker );
	}
}