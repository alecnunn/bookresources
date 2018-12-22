// Utils.h: Some common utilities.
// Date: 7/02/04
// Created by: Aurelio Reis

#ifndef __UTILS__H__
#define __UTILS__H__

#include "Windows.h"
#include "Tokenizer.h"
using namespace AURELIO_REIS;


// Allocate a new String.
__forceinline char *ARCreateString( const char *strString )
{
	int iLen = (int) strlen( strString ) + 1;
	char *strNewString = new char[ iLen ];
	_snprintf( strNewString, iLen, strString );

	return strNewString;
}


// Release an existing String.
__forceinline void ARReleaseString( char *&strString )
{
	if ( strString )
	{
		delete [] strString;	strString = NULL;
	}
}


// Byte offset into a class's data members (starting at 0).
#define AR_FIELD_OFFSET( type, field )	int( (LONG)(LONG_PTR)&(((type *)0)->field) )

// Custom field descriptor (for reading in custom data directly into data structure fields).
class CCustomField
{
public:
	// Custom Field data types.
	enum FIELDTYPE { FT_STRING, FT_INT, FT_FLOAT, };

	// The name of the field.
	char *m_strFieldName;

	// How many bytes from the start of the class's memory this field starts at.
	int m_iByteOffset;

	// The type of field this is.
	FIELDTYPE m_FieldType;

	// Assign a value to the memory location mapped to by this custom field.
	void AssignField( char *byClassOffset, void *pValue )
	{
		// Depending on what kind of variable this is, put it in the appropriate format.
		switch ( m_FieldType )
		{
			case FT_INT:
				*(int *)(byClassOffset + m_iByteOffset) = (int)*(float *) pValue;
			return;

			case FT_FLOAT:
				*(float *)(byClassOffset + m_iByteOffset) = *(float *) pValue;
			return;

			case FT_STRING:
			{
				int iSize = (int)strlen( (char *) pValue ) + 1;
				*(char **)(byClassOffset + m_iByteOffset) = new char[iSize];
				_snprintf( *(char **)(byClassOffset + m_iByteOffset), iSize, (char *) pValue );
			}
			return;
		}
	}

	// Parse the fields looking for valid data.
	static void ParseCustomFields( CCustomField Fields[], int iNumFields, char *pByteOffset, CTokenizer &Toker )
	{
		CToken *pTok = Toker.GetCurrentToken();
		const char *strVal = ((CTokenString *) pTok)->m_strValue;

		// Find the field entry and assign the corresponding values.
		for ( int i = 0; i < iNumFields; i++ )
		{
			if ( strcmp( Fields[ i ].m_strFieldName, strVal ) == 0 )
			{
				pTok = Toker.GetNextToken();

				if ( TKT_STRING == pTok->m_Type &&
					FT_STRING == Fields[ i ].m_FieldType )
				{
					Fields[ i ].AssignField( pByteOffset, (void *)((CTokenString *) pTok)->m_strValue );
				}
				else if ( TKT_FLOAT == pTok->m_Type &&
							( FT_INT == Fields[ i ].m_FieldType ||
							FT_FLOAT == Fields[ i ].m_FieldType ) )
				{
					Fields[ i ].AssignField( pByteOffset, (void *)&((CTokenFloat *) pTok)->m_fValue );
				}
				else
				{
					char strErr[ 128 ];
					_snprintf( strErr, 128, "Error: Invalid value specified at line %d", Toker.GetPrevToken()->m_iCurLine );
					throw( strErr );
				}

				break;
			}
		}
	}
};

#endif