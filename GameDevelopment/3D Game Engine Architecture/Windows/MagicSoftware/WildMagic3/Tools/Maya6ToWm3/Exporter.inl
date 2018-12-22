// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2003.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// 3D GAME ENGINE SOURCE CODE
// http://www.magic-software.com/License/3DGameEngine.pdf

//----------------------------------------------------------------------------
inline bool Exporter::Successful () const
{
    return m_bSuccess;
}
//----------------------------------------------------------------------------
inline void Exporter::DisplayInfo(MString kStr)
{
    if (!m_abFlags[FLAG_QUIET])
        MGlobal::displayInfo(kStr);
}
//----------------------------------------------------------------------------
inline void Exporter::DisplayWarning(MString kStr)
{
    if (!m_abFlags[FLAG_QUIET])
        MGlobal::displayWarning(kStr);
}
//----------------------------------------------------------------------------
inline void Exporter::DisplayError(MString kStr)
{
    // always display errors, even if being Quiet
    MGlobal::displayError(kStr);
    m_bSuccess = false;
}
//----------------------------------------------------------------------------