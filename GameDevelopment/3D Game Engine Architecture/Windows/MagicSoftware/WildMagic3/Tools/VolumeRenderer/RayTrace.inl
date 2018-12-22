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
inline Ergb8 RayTrace::GetRendered (int iX, int iY)
{
    return (*m_pkRender)(iX,iY);
}
//----------------------------------------------------------------------------
inline Ergb8 RayTrace::GetRendered (int i)
{
    return (*m_pkRender)[i];
}
//----------------------------------------------------------------------------
inline float& RayTrace::Frame (int iY, int iX)
{
    return m_aafFrame[iY][iX];
}
//----------------------------------------------------------------------------
