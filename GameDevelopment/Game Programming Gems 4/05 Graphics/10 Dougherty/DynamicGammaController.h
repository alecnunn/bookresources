//-----------------------------------------------------------------------------
// File: DynamicGammaController.h
// Desc: Contains the declaration of the dynamic gamma post processing
//       technique controller class
//
// Copyright © 2003 Michael Dougherty (mdougher@hotmail.com).
// All rights reserved worldwide.
//
// Permission to copy, modify, reproduce or redistribute this source code is
// granted provided the above copyright notice is retained in the resulting 
// source code.
// 
// This software is provided "as is" and without any express or implied
// warranties.
//
//-----------------------------------------------------------------------------


#ifndef DYNAMIC_GAMMA_CONTROLLER_H
#define DYNAMIC_GAMMA_CONTROLLER_H


//-----------------------------------------------------------------------------
// Name: struct Histogram
// Desc: Contains the data generated from the luminance texture
//-----------------------------------------------------------------------------
struct Histogram  
{
    FLOAT Lums[256]; // Normalized number of samples present at each luminance level
    FLOAT fMin;      // Smallest luminance value above min/max threshold
    FLOAT fMax;      // Largest luminance value below min/max threshold
    FLOAT fAvg;      // Average luminance value
};


//-----------------------------------------------------------------------------
// Name: class DynamicGammaController
// Desc: Handles luminance sampling and gamma updates
//-----------------------------------------------------------------------------
class DynamicGammaController
{
    IDirect3DPixelShader9*  m_pLuminancePixelShader; // Luminance pixel shader
    FLOAT m_fMin; // Current min
    FLOAT m_fMax; // Current max
    FLOAT m_fAvg; // Current average
    
    Histogram m_Histogram;

    D3DGAMMARAMP m_Ramp;  // used for debug drawing

    IDirect3DSurface9* m_pLuminanceSurface; // Down sampled luminance surface


    IDirect3DPixelShader9* m_pGammaPixelShader;  // pixel shader that implements a gamma
                                                 // transform
    
    IDirect3DTexture9* m_pGammaRampLUT;          // gamma texture LUT
    
    IDirect3DTexture9* m_pInvGammaRampLUT;       // inv gamma texture LUT

public:
    VOID        Initialize();   // Initialize values and resources   
    VOID        Finalize();     // Free resources, reset gamma ramp to linear

    // In order to dynamically update the gamma ramp based on scene luminance,
    // call the following functions in this order:
    // 1) GetLuminance
    // 2) GetHistogram
    // 3) UpdateGammaValues
    // 4) SetGammaRamp
    // 
    // UpdateGammaValues can be called more than once per frame for smoother
    // transitions if you luminance sampling update is not done very often.
    //
    // Note that for performance reasons, the functions may not be called all
    // in the same frame

    // Get luminance texture from source texture
    VOID        GetLuminance( IDirect3DTexture9* pTextureSrc,
                              UINT nSuperSampleX,
                              UINT nSuperSampleY );

    // Calculate histogram from luminance texture
    VOID        GetHistogram();
    
    // Update current gamma ramp min, max, and average
    VOID        UpdateGammaValues( FLOAT fDt );

    // Update RGB gamma ramps
    VOID        SetGammaRamp( BOOL bUseHardwareGammaRamp );
    
    // Draw histogram and gamma ramps in the supplied screen space rectangles
    VOID        DrawHistogram( FLOAT fX1, FLOAT fY1, FLOAT fX2, FLOAT fY2 );
    VOID        DrawGammaRamps( FLOAT fX1, FLOAT fY1, FLOAT fX2, FLOAT fY2 );

    // Sets up the pixel shader and textures required to draw a HUD
    // element with constant gamma
    VOID        SetInvGammaCorrection();

    // Sets up the pixel shader and textures required to draw a a
    // gamma ajusted texture
    VOID        SetGammaCorrection();

    // sets the ramp used to draw the debug gamma ramp
    VOID        SetDebugdrawRamp( D3DGAMMARAMP* pRamp );

    // Effect adjustments

    // Gamma ramp bias
    FLOAT       m_fBias;             

    // Maximum gamma ramp adjustment for dark scene compensation
    FLOAT       m_fDarkClamp;  

    // Maximum gamma ramp adjustment for bright scene compensation
    FLOAT       m_fLightClamp;       

    // Gamma ramp darkness adjustment with respect to time
    FLOAT       m_fDarkAdjustDt;   

    // Gamma ramp brightness adjustment with respect to time
    FLOAT       m_fLightAdjustDt; 

    // Gamma ramp contrast adjustment with respect to time
    FLOAT       m_fContrastAdjustDt; 

    // Area underneath the dark side of the normalized histogram where
    // luminance values are considered 0.  Used to filter out dark outliners.
    FLOAT       m_fDarkThreshold;    

    // Area underneath the bright side of the normalized histogram where
    // luminance values are considered 0.  Used to filter out bright outliners.
    FLOAT       m_fLightThreshold;   
};


#endif // DYNAMIC_GAMMA_CONTROLLER_H
