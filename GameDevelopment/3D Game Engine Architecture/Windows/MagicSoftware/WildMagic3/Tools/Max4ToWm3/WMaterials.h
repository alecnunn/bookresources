// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WMATERIALS_H
#define WMATERIALS_H

#include <vector>
class Mtl;

class WMaterials
{
public:
    WMaterials ();

    bool Add (Mtl* pkMtl);
    void SetQuantity (int iQuantity);
    int GetQuantity () const;
    int GetID (Mtl* pkMtl) const;
    Mtl* Get (int iMtlID) const;

private:
    std::vector<Mtl*> m_kMaterial;   
};

#endif
