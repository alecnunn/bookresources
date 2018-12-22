// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONTROLLER_H
#define WM3CONTROLLER_H

#include "Wm3Object.h"

namespace Wm3
{

class WM3_ITEM Controller : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // destruction (abstract base class)
    virtual ~Controller ();

    // the controlled object
    Object* GetObject () const;

    // animation update
    virtual bool Update (double dAppTime);

    // time management

    enum // RepeatType
    {
        RT_CLAMP,
        RT_WRAP,
        RT_CYCLE,
        RT_QUANTITY
    };

    int RepeatType;    // default = RT_CLAMP
    double MinTime;    // default = 0.0
    double MaxTime;    // default = 0.0
    double Phase;      // default = 0.0
    double Frequency;  // default = 1.0
    bool Active;       // default = true

protected:
    // construction (abstract base class)
    Controller ();

    // the controlled object
    friend class Object;
    virtual void SetObject (Object* pkObject);

    // Conversion from application time units to controller time units.
    // Derived classes may use this in their update routines.
    double GetControlTime (double dAppTime);

    // Regular pointer used for controlled object to avoid circular smart
    // pointers between controller and object.
    Object* m_pkObject;

    double m_dLastAppTime;
};

WM3_REGISTER_STREAM(Controller);
typedef Pointer<Controller> ControllerPtr;
#include "Wm3Controller.inl"

}

#endif
