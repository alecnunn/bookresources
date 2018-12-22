// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "WSceneBuilder.h"
#include "iparamb2.h"
#include "iskin.h"
#include "modstack.h"
using namespace std;

//----------------------------------------------------------------------------
void WSceneBuilder::ApplyModifiers ()
{
    // Apply the modifier info that was collected during node traversal.  The
    // only modifier currently supported is skin.

    vector<ModifierInfo*>::iterator pkI;
    for (pkI = m_kModList.begin(); pkI < m_kModList.end(); pkI++)
    {
        ModifierInfo* pkInfo = *pkI;
        vector<Modifier*>::iterator pkJ = pkInfo->Modifiers.begin();
        for (/**/; pkJ < pkInfo->Modifiers.end(); pkJ++)
        {
            Modifier* pkModifier = *pkJ;
            Class_ID kID = pkModifier->ClassID();
            if ( kID == SKIN_CLASSID )
                ProcessSkin(pkInfo->Node,pkModifier);
        }       
    }
}
//----------------------------------------------------------------------------
void WSceneBuilder::FindModifiers (INode* pkMaxNode,
    vector<Modifier*>& rkModifiers)
{
    // Determine whether a node has any modifiers.  If so, store them for
    // later use.
    //
    // pkNode:
    //     Pointer to a Max node.
    // rkModifiers:
    //     Vector in which to store modifier pointers.

    // Get object from node. Abort if no object.
    Object* pkObj = pkMaxNode->GetObjectRef();
    if ( !pkObj ) 
        return;

    // Is derived object?
    while ( pkObj->SuperClassID() == GEN_DERIVOB_CLASS_ID )
    {
        // Yes -> Cast.
        IDerivedObject* pkDerObj = static_cast<IDerivedObject*>(pkObj);

        // iterate over all entries of the modifier stack
        int iModStackIndex = 0;
        while ( iModStackIndex < pkDerObj->NumModifiers() )
        {
            // get current modifier
            Modifier* pkMod = pkDerObj->GetModifier(iModStackIndex);
            if ( pkMod ) 
                rkModifiers.push_back(pkMod);
            iModStackIndex++;
        }
        pkObj = pkDerObj->GetObjRef();
    }
}
//----------------------------------------------------------------------------
