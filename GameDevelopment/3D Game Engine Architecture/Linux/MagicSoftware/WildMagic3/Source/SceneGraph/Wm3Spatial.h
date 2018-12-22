// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SPATIAL_H
#define WM3SPATIAL_H

#include "Wm3Object.h"
#include "Wm3BoundingVolume.h"
#include "Wm3Effect.h"
#include "Wm3GlobalState.h"
#include "Wm3Transformation.h"

namespace Wm3
{

class Light;

class WM3_ITEM Spatial : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~Spatial ();

    // Local and world transforms.  In some situations you might need to set
    // the world transform directly and bypass the Spatial::Update()
    // mechanism.  If World is set directly, the WorldIsCurrent flag should
    // be set to true.  For example, inverse kinematic controllers and skin
    // controllers need this capability.
    Transformation Local;
    Transformation World;
    bool WorldIsCurrent;

    // World bound access and culling.  In some situations you might want to
    // set the world bound directly and bypass the Spatial::Update()
    // mechanism.  If WorldBound is set directly, the WorldBoundIsCurrent
    // flag should be set to true.
    BoundingVolumePtr WorldBound;
    bool WorldBoundIsCurrent;
    bool ForceCull;

    // Update of geometric state and controllers.  The UpdateGS function
    // computes world transformations on the downward pass and world bounding
    // volumes on the upward pass.  The UpdateBS function just computes the
    // world bounding volumes on an upward pass.  This is useful if model
    // data changes, causing the model and world bounds to change, but no
    // transformations need recomputing.
    void UpdateGS (double dAppTime = -Mathd::MAX_REAL, bool bInitiator = true);
    void UpdateBS ();

    // global state
    void SetGlobalState (GlobalState* pkState);
    GlobalState* GetGlobalState (int eType) const;
    void RemoveGlobalState (int eType);
    void RemoveAllGlobalStates ();

    // light state
    void SetLight (Light* pkLight);
    int GetLightQuantity () const;
    Light* GetLight (int i) const;
    void RemoveLight (Light* pkLight);
    void RemoveAllLights ();

    // effect state
    virtual void SetEffect (Effect* pkEffect);
    Effect* GetEffect () const;

    // update of render state
    virtual void UpdateRS (TStack<GlobalState*>* akGStack = 0,
        TStack<Light*>* pkLStack = 0);

    // parent access
    Spatial* GetParent ();

    // The picking system.  Each Spatial-derived class derives its own pick
    // record class from PickRecord and adds whatever information it wants to
    // return from the DoPick call.  The ray parameter can be used to sort
    // the intersection points after a call to DoPick.
    //
    // RTTI for the PickRecord is obtained by using the RTTI for the member
    // IObject.  Once that member's type is known, the PickRecord can be cast
    // to the appropriate PickRecord-derived class.
    class WM3_ITEM PickRecord
    {
    public:
        virtual ~PickRecord ();

        // The intersected object.
        Pointer<Spatial> IObject;

        // The world ray parameter for the point of intersection, a
        // nonnegative number.  That is, if the world ray is P+t*D, the ray
        // parameter is t >= 0.
        float T;

    protected:
        PickRecord (Spatial* pkIObject, float fT);
    };

    typedef TArray<PickRecord*> PickArray;

    // The origin and direction of the ray must be in world coordinates.  The
    // application is responsible for deleting the pick records in the array.
    virtual void DoPick (const Ray3f& rkRay, PickArray& rkResults);

    static PickRecord* GetClosest (PickArray& rkResults);

protected:
    Spatial ();

    // geometric updates
    virtual void UpdateWorldData (double dAppTime);
    virtual void UpdateWorldBound () = 0;
    void PropagateBoundToRoot ();

    // render state updates
    void PropagateStateFromRoot (TStack<GlobalState*>* akGStack,
        TStack<Light*>* pkLStack);
    void PushState (TStack<GlobalState*>* akGStack, TStack<Light*>* pkLStack);
    void PopState (TStack<GlobalState*>* akGStack, TStack<Light*>* pkLStack);
    virtual void UpdateState (TStack<GlobalState*>* akGStack,
        TStack<Light*>* pkLStack) = 0;

    // support for hierarchical scene graph
    Spatial* m_pkParent;

    // global render state
    TList<GlobalStatePtr>* m_pkGlobalList;

    // light state (Pointer used directly to avoid circular headers)
    TList<Pointer<Light> >* m_pkLightList;

    // Effect state.  If the effect is attached to a Geometry object, it
    // applies to that object alone.  If the effect is attached to a Node
    // object, it applies to all Geometry objects in the subtree rooted at
    // the Node.  However, only one such "global" effect can be active at a
    // time.  The active one is the last one encountered in a depth-first
    // traversal of the scene.
    EffectPtr m_spkEffect;

// internal use
public:
    // parent access (Node calls this during attach/detach of children)
    void SetParent (Spatial* pkParent);

    // renderer needs access to these
    void OnDraw (Renderer& rkRenderer, bool bNoCull = false);
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false) = 0;
};

WM3_REGISTER_STREAM(Spatial);
typedef Pointer<Spatial> SpatialPtr;
#include "Wm3Spatial.inl"

}

#endif
