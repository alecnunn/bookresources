// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TLIST_H
#define WM3TLIST_H

// The class T is either native data or is class data that has the following
// member functions:
//   T::T ()
//   T::T (const T&);
//   T& T::operator= (const T&)

namespace Wm3
{

template <class T>
class TList
{
public:
    // construction
    TList ();
    TList (const T& rtItem, TList* pkNext);
    ~TList ();

    // member access
    void SetItem (const T& rtItem);
    T& Item ();
    const T& GetItem () const;
    void SetNext (TList* pkNext);
    TList*& Next ();
    const TList* GetNext () const;

    // total number of nodes in the list, including this one
    int GetQuantity () const;

private:
    T m_tItem;
    TList* m_pkNext;
};

#include "Wm3TList.inl"

}

#endif
