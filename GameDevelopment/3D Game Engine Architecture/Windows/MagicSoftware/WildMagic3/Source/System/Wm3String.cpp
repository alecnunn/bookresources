// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3String.h"
using namespace Wm3;

//----------------------------------------------------------------------------
String::String (const char* acText)
{
    if ( acText )
    {
        m_iLength = (int)strlen(acText);
        m_acText = new char[m_iLength+1];
        strcpy(m_acText,acText);
    }
    else
    {
        m_iLength = 0;
        m_acText = new char[1];
        m_acText[0] = 0;
    }
}
//----------------------------------------------------------------------------
String::String (int iLength, const char* acText)
{
    if ( iLength < 0 || (iLength > 0 && !acText) )
    {
        assert( false );
        iLength = 0;
    }

#ifdef _DEBUG
    // make sure acText has as many non-null characters as specified
    if ( iLength > 0 )
    {
        int i;
        for (i = 0; i < iLength; i++)
        {
            if ( acText[i] == 0 )
                break;
        }
        assert( i == iLength );
    }
#endif

    m_iLength = iLength;
    if ( m_iLength > 0 )
    {
        m_acText = new char[m_iLength+1];
        strncpy(m_acText,acText,m_iLength);
        m_acText[m_iLength] = 0;
    }
    else
    {
        m_acText = new char[1];
        m_acText[0] = 0;
    }
}
//----------------------------------------------------------------------------
String::String (const String& rkString)
{
    m_acText = 0;
    *this = rkString;
}
//----------------------------------------------------------------------------
String::~String ()
{
    delete[] m_acText;
}
//----------------------------------------------------------------------------
String& String::operator= (const String& rkString)
{
    m_iLength = rkString.m_iLength;
    delete[] m_acText;
    m_acText = new char[m_iLength+1];
    strcpy(m_acText,rkString.m_acText);
    return *this;
}
//----------------------------------------------------------------------------
String& String::operator+= (const String& rkString)
{
    m_iLength += rkString.m_iLength;
    // Create space for the new string
    char* acNew = new char[m_iLength+1];
    // Copy old string into new space
    strcpy(acNew,m_acText);
    delete m_acText;
    // Concat new string onto old string
    m_acText = strcat(acNew,rkString.m_acText);   
    return *this;
}
//----------------------------------------------------------------------------
String String::operator+ (const String& rkString)
{
    int iLength = m_iLength + rkString.m_iLength;
    char* acNew = new char[iLength+1];
    strcpy(acNew,m_acText);
    strcat(acNew,rkString.m_acText);
    return String(iLength,acNew);
}
//----------------------------------------------------------------------------
bool String::operator== (const String& rkString) const
{
    return m_iLength == rkString.m_iLength
        && memcmp(m_acText,rkString.m_acText,m_iLength) == 0;
}
//----------------------------------------------------------------------------
bool String::operator!= (const String& rkString) const
{
    return m_iLength != rkString.m_iLength
        || memcmp(m_acText,rkString.m_acText,m_iLength) != 0;
}
//----------------------------------------------------------------------------
String::operator unsigned int () const
{
    unsigned int uiKey = 0;
    for (int i = 0; i < m_iLength; i++)
        uiKey += (uiKey << 5) + m_acText[i];
    return uiKey;
}
//----------------------------------------------------------------------------
String String::ToUpper () const
{
    char* acNew = new char[m_iLength+1];
    for (int i = 0; i < m_iLength; i++)
        acNew[i] = toupper(m_acText[i]);
    acNew[m_iLength] = 0;
    return String(m_iLength,acNew);
}
//----------------------------------------------------------------------------
String String::ToLower () const
{
    char* acNew = new char[m_iLength+1];
    for (int i = 0; i < m_iLength; i++)
        acNew[i] = tolower(m_acText[i]);
    acNew[m_iLength] = 0;
    return String(m_iLength,acNew);    
}
//----------------------------------------------------------------------------