// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SHADERCONSTANT
#define WM3SHADERCONSTANT

#include "Wm3Object.h"

namespace Wm3
{

class WM3_ITEM ShaderConstant : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // state constants
    enum
    {
        SC_TRANSFORM_M,         // 4x4 model-to-world matrix
        SC_TRANSFORM_P,         // 4x4 projection matrix
        SC_TRANSFORM_MV,        // 4x4 model-to-view matrix
        SC_TRANSFORM_MVP,       // 4x4 model-to-view*projection matrix

        SC_CAMERA_POSITION,     // (x,y,z,1)
        SC_CAMERA_DIRECTION,    // (x,y,z,0)
        SC_CAMERA_UP,           // (x,y,z,0)
        SC_CAMERA_RIGHT,        // (x,y,z,0)

        SC_FOG_COLOR,           // (r,g,b,a)
        SC_FOG_PARAMS,          // (start, end, density, enabled)

        SC_MATERIAL_EMISSIVE,   // (r,g,b,a)
        SC_MATERIAL_AMBIENT,    // (r,g,b,a)
        SC_MATERIAL_DIFFUSE,    // (r,g,b,a)
        SC_MATERIAL_SPECULAR,   // (r,g,b,a)
        SC_MATERIAL_SHININESS,  // (shiny, undefined, undefined, undefined)

        SC_LIGHT_POSITION,      // (r,g,b,a)
        SC_LIGHT_DIRECTION,     // (r,g,b,a)
        SC_LIGHT_AMBIENT,       // (r,g,b,a)
        SC_LIGHT_DIFFUSE,       // (r,g,b,a)
        SC_LIGHT_SPECULAR,      // (r,g,b,a)
        SC_LIGHT_SPOTCUTOFF,    // (angle, cos, sin, exponent)
        SC_LIGHT_ATTENPARAMS,   // (constant, linear, quadratic, intensity)

        // The numerical constant is defined in the program.  Not all APIs
        // and shader versions handle constants, so you may wish to
        // conditionally handle them within the renderer.
        SC_NUMERICAL_CONSTANT,  // (f0,f1,f2,f3)

        // Total number of state variable enumerations.
        SC_QUANTITY,

        // User-defined constants are all assigned this type so that the
        // renderer knows not to set them using the current render state.
        SC_USER_DEFINED         // (f0,f1,f2,f3)
    };

    // state constant options
    enum
    {
        SCO_NONE = -1,
        SCO_MATRIX = 0,
        SCO_TRANSPOSE = 1,
        SCO_INVERSE = 2,
        SCO_INVERSE_TRANSPOSE = 3,
        SCO_LIGHT0 = 0,
        SCO_LIGHT1 = 1,
        SCO_LIGHT2 = 2,
        SCO_LIGHT3 = 3,
        SCO_LIGHT4 = 4,
        SCO_LIGHT5 = 5,
        SCO_LIGHT6 = 6,
        SCO_LIGHT7 = 7,
    };

    // construction and destruction
    ShaderConstant (int iSCType, int iSCOption, int iRegister,
        int iRegisterQuantity);
    virtual ~ShaderConstant ();

    // member access
    int GetSCType () const;
    int GetSCOption () const;
    int GetRegister () const;
    int GetRegisterQuantity () const;
    float* Data ();
    float Data (int i) const;
    void SetRegisterData (int i, float fData0, float fData1 = 0.0f,
        float fData2 = 0.0f, float fData3 = 0.0f);

protected:
    // streaming
    ShaderConstant ();

    // one of the enumerations with SC_ prefix
    int m_iSCType;

    // one of the enumerations with SCO_ prefix
    int m_iSCOption;

    // register to store data in (1 register is 4 floats)
    int m_iRegister;

    // number of registers required for the constant
    int m_iRegisterQuantity;

    // the constant's data
    float* m_afData;
};

WM3_REGISTER_STREAM(ShaderConstant);
typedef Pointer<ShaderConstant> ShaderConstantPtr;
#include "Wm3ShaderConstant.inl"

}

#endif
