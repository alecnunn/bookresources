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
inline int StringTree::GetStringQuantity () const
{
    return m_kStrings.GetQuantity();
}
//----------------------------------------------------------------------------
inline char* StringTree::GetString (int i)
{
    if ( 0 <= i && i < m_kStrings.GetQuantity() )
        return m_kStrings[i];

    return 0;
}
//----------------------------------------------------------------------------
inline int StringTree::GetChildQuantity () const
{
    return m_kChildren.GetQuantity();
}
//----------------------------------------------------------------------------
inline StringTree* StringTree::GetChild (int i)
{
    if ( 0 <= i && i < m_kChildren.GetQuantity() )
        return m_kChildren[i];

    return 0;
}
//----------------------------------------------------------------------------
template <class T>
StringTree* StringTree::Format (const char* acTitle, int iQuantity,
    const T* atValue)
{
    StringTree* pkTree = new StringTree(1,iQuantity+1);
    pkTree->SetString(0,StringTree::Format(acTitle));

    StringTree* pkSubtree = new StringTree(iQuantity+1,0);
    pkSubtree->SetString(0,StringTree::Format("quantity =",iQuantity));
    for (int i = 0; i < iQuantity; i++)
    {
        char acPrefix[256];
        sprintf(acPrefix,"%d: ",i);
        T tValue = atValue[i];
        pkSubtree->SetString(i+1,StringTree::Format(acPrefix,tValue));
    }

    pkTree->SetChild(0,pkSubtree);

    return pkTree;
}
//----------------------------------------------------------------------------
