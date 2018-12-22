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
inline float* PointController::PointLinearSpeed ()
{
    return m_afPointLinearSpeed;
}
//----------------------------------------------------------------------------
inline float* PointController::PointAngularSpeed ()
{
    return m_afPointAngularSpeed;
}
//----------------------------------------------------------------------------
inline Vector3f* PointController::PointLinearAxis ()
{
    return m_akPointLinearAxis;
}
//----------------------------------------------------------------------------
inline Vector3f* PointController::PointAngularAxis ()
{
    return m_akPointAngularAxis;
}
//----------------------------------------------------------------------------
