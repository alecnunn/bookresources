// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

//----------------------------------------------------------------------------
inline NodePtr LightWaveReader::GetObject () const
{
    return m_spkLWObject;
}
//----------------------------------------------------------------------------
inline LightWaveReader::ParseError LightWaveReader::GetError () const
{
    return m_eError;
}
//----------------------------------------------------------------------------
inline bool LightWaveReader::IsSmoothed () const
{
    return m_bSmoothed;
}
//----------------------------------------------------------------------------

