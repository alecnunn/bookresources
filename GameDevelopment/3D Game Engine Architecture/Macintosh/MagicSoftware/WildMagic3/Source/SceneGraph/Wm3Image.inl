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
inline int Image::GetFormat () const
{
    return m_eFormat;
}
//----------------------------------------------------------------------------
inline int Image::GetBytesPerPixel () const
{
    return ms_aiBytesPerPixel[m_eFormat];
}
//----------------------------------------------------------------------------
inline int Image::GetWidth () const
{
    return m_iWidth;
}
//----------------------------------------------------------------------------
inline int Image::GetHeight () const
{
    return m_iHeight;
}
//----------------------------------------------------------------------------
inline int Image::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline unsigned char* Image::GetData () const
{
    return m_aucData;
}
//----------------------------------------------------------------------------
inline unsigned char* Image::operator() (int i)
{
    return m_aucData + i*ms_aiBytesPerPixel[m_eFormat];
}
//----------------------------------------------------------------------------
