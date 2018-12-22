//-----------------------------------------------------------------------------
// File: DynamicGamma.cpp
// Desc: main routines for the dynamic gamma test app
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


#include "stdafx.h"
#include "dynamicgammacontroller.h"
#include "helpers.h"


//-----------------------------------------------------------------------------
// Application globals for holding the file names in the texture directory
//-----------------------------------------------------------------------------
const DWORD MAX_TEXTURES = 64;
char g_aszTextures[MAX_TEXTURES][MAX_PATH];
DWORD g_dwNumTextures = 0;


//-----------------------------------------------------------------------------
// Start and warning messages
//-----------------------------------------------------------------------------
const CHAR* g_szStartMessage =
{
    "Dynamic Gamma Using Sampled Scene Luminance Test Application.\n"
    "\n"
    "Copyright © 2003 Michael Dougherty (mdougher@hotmail.com).\n"
    "All rights reserved worldwide.\n"
    "\n"
    "Notes:\n"
    "   This demo was tested on an ATI 9600 and an ATI 9700.\n"
    "       * It has not been tested on any other cards.\n"
    "\n"
    "   The demo requires pixel shader 1.1 and the DirectX 9.0 Summer Update runtime.\n"
    "       * No DirectX CAP bits are queried.\n"
    "\n"
    "   Up to 64 textures can be put in the local textures directory and viewed\n"
    "   in the application.\n"
    "\n"
    "   If you run the application full screen, the hardware gamma ramp is used by\n"
    "   default and the HUD is drawn with an inverse gamma transform.\n"
    "\n"
    "   You can toggle the usage of either hardware gamma ramp or pixel shader gamma ramp\n"
    "   in the full screen version.\n"
    "\n"
    "   The windowed version used a pixel shader to apply the gamma transform.\n"
    "\n"
    "Controls:\n"
    "   Esc:\tExit\n"
    "   N:\tNext texture\n"
    "   H:\tToggle HUD\n"
    "   D:\tToggle the gamma ramp and histogram displays\n"
    "   G:\tToggle the dynamic gamma effect\n"
    "   P:\tToggle hardware or pixel shader gamma ramp (Full Screen Mode Only)\n"
    "\n"
    "Display:\n"
    "   Upper Left:\tScene histogram\n"
    "   Center Left:\tCurrent gamma ramp transform\n"
    "   Bottom Left:\tHUD for demonstating the inverse gamma ramp pixel shader\n"
    "\n"
    "WARNING:"
    "This application has not been thoroughly tested and is for demonstration\n"
    "purposes only.\n"
    "YOU RUN THIS APPLICATION AT YOUR OWN RISK.\n"
    "\n"
    "Do you wish to continue?"
};


//-----------------------------------------------------------------------------
// Name: main
// Desc: Prints the warning messages, creates the dynamic gamma controller,
//       and renders the textures in the texture directory with the 
//       dynamic gamma effect.
//-----------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
    // show message boxes
    if( MessageBox( 0, g_szStartMessage, "Dynamic Gamma",
        MB_YESNO | MB_SETFOREGROUND | MB_ICONINFORMATION ) == IDNO )
        exit( 1 );

    BOOL bWindowed = FALSE;
    if( MessageBox( 0, "Run in Windowed Mode?", "Dynamic Gamma",
        MB_YESNO | MB_SETFOREGROUND ) == IDYES )
        bWindowed = TRUE;

    g_PresentParameters.Windowed = bWindowed;
    
    // find up to 64 textures in the textures directory
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile( "textures\\*", &FindFileData );
    do
    {
        if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
        {
            strcpy( g_aszTextures[g_dwNumTextures], "textures\\" );
            strcat( g_aszTextures[g_dwNumTextures], FindFileData.cFileName );
            g_dwNumTextures++;
        }
    }
    while( FindNextFile( hFind, &FindFileData ) && g_dwNumTextures < MAX_TEXTURES );
    FindClose( hFind );
    if( g_dwNumTextures == 0 )
    {
        FatalError( "Could not find display textures in \\textures." );
    }

    // initialized D3D
    InitD3D();

    // hide the cursor
    ShowCursor( FALSE );

    // initialized the dynamic gamma controller
    DynamicGammaController* pDynamicGammaController = new DynamicGammaController;
    pDynamicGammaController->Initialize();

    // load the first display texture
    IDirect3DTexture9* pDisplayTexture = NULL;
    VERIFY( D3DXCreateTextureFromFile( g_pD3DDevice, g_aszTextures[0], &pDisplayTexture ) );
    printf("Showing Texture: %s\n", g_aszTextures[0] );
    
    // create a render target texture 
    IDirect3DTexture9* pRenderTargetTexture;
    VERIFY( g_pD3DDevice->CreateTexture( g_PresentParameters.BackBufferWidth,
                                         g_PresentParameters.BackBufferHeight,
                                         1,
                                         D3DUSAGE_RENDERTARGET,
                                         g_PresentParameters.BackBufferFormat,
                                         D3DPOOL_DEFAULT,
                                         &pRenderTargetTexture,
                                         NULL ) );
    IDirect3DSurface9* pRenderTargetSurface;
    VERIFY( pRenderTargetTexture->GetSurfaceLevel( 0, &pRenderTargetSurface ) );
    
    // load the HUD texture for demonstrating inverse gamma
    IDirect3DTexture9* pHUD = NULL;
    VERIFY( D3DXCreateTextureFromFile( g_pD3DDevice, "hud.dds", &pHUD ) );
    
    // render with dynamic gamma
    while( !GetAsyncKeyState( VK_ESCAPE ) & 0x0001 )
    {
        // skip processing windows messages
        MSG msg; 
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
		    TranslateMessage( &msg );
		    DispatchMessage( &msg );
        }

        // toggle dynamic gamma
        static BOOL bUseDynamicGamma = TRUE;
        if( GetAsyncKeyState( 'G' ) & 0x0001 )
        {
            bUseDynamicGamma = !bUseDynamicGamma;
            printf( bUseDynamicGamma ? "Dynamic Gamma On\n" : "Dynamic Gamma Off\n" );
        }

        // toggle the HUD
        static BOOL bShowHUD = TRUE;
        if( GetAsyncKeyState( 'H' ) & 0x0001 )
        {
            bShowHUD = !bShowHUD;
            printf( bShowHUD ? "HUD On\n" : "HUD Off\n" );
        }

        // toggle the gamma ramp and debug displays
        static BOOL bShowDebugGammaRampAndHistogram = TRUE;
        if( GetAsyncKeyState( 'D' ) & 0x0001 )
        {
            bShowDebugGammaRampAndHistogram = !bShowDebugGammaRampAndHistogram;
            printf( bShowDebugGammaRampAndHistogram ?
                "Debug Histogram and Gamma Ramp On\n" : "Debug Histogram and Gamma Ramp Off\n" );
        }

        // toggle the use of the hardware gamma ramp or pixel shader
        static BOOL bUseHardwareGammaRamp = TRUE;
        if( GetAsyncKeyState( 'P' ) & 0x0001 && !bWindowed )
        {
            bUseHardwareGammaRamp = !bUseHardwareGammaRamp;
            printf( bUseHardwareGammaRamp ?
                    "Using Hardware Gamma Ramp\n" : "Using pixel shader gamma transform\n" );
        }
        if( bWindowed )
        {
            bUseHardwareGammaRamp = FALSE;
        }


        // cycle through the display textures
        static DWORD dwCurrentTexture = 0;
        if( GetAsyncKeyState( 'N' ) & 0x0001 )
        {
            dwCurrentTexture++;
            if( dwCurrentTexture == g_dwNumTextures )
                dwCurrentTexture = 0;
            SAFE_RELEASE( pDisplayTexture );
            VERIFY( D3DXCreateTextureFromFile( g_pD3DDevice, g_aszTextures[dwCurrentTexture], &pDisplayTexture ) );
            printf("Showing Texture: %s\n", g_aszTextures[dwCurrentTexture] );
        }
            
        // set the render target
        IDirect3DSurface9* pCurrentRenderTargetSurface = NULL;
        if( bUseDynamicGamma )
        {
            VERIFY( g_pD3DDevice->GetRenderTarget( 0, &pCurrentRenderTargetSurface ) );
            VERIFY( g_pD3DDevice->SetRenderTarget( 0, pRenderTargetSurface ) );
        }
        
        // clear the render target (not needed for this demo)
        VERIFY( g_pD3DDevice->Clear( 0,
                                    NULL,
                                    D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
                                    0x00000000,
                                    1.0f,
                                    0x00000000 ) );

        // start rendering
        VERIFY( g_pD3DDevice->BeginScene() );
        

        // render the display texture
        RenderTexture( pDisplayTexture,
                       -0.5f,
                       -0.5f,
                       (FLOAT) g_PresentParameters.BackBufferWidth,
                       (FLOAT) g_PresentParameters.BackBufferHeight );

        // Current stage in dynamic gamma calculation and update.
        // Note that we don't get luminance and calculate gamma in the same
        // frame to avoid the lock stall when reading the luminance texture.
        enum STAGE
        {
            STAGE_GET_LUM,
            STAGE_GET_HIST,
            STAGE_SET_GAMMA,
            MAX_STAGE
        };

        static STAGE CurrentStage = STAGE_GET_LUM;
        static BOOL bGammaRampLinear = TRUE;
        if( bUseDynamicGamma )
        {
            switch( CurrentStage )
            {
                case STAGE_GET_LUM:
                {
                    // Get luminance texture
                    pDynamicGammaController->GetLuminance( pRenderTargetTexture, 16, 16 );
                    
                    // Advance to the next stage
                    CurrentStage = STAGE_GET_HIST;
                    break;
                }
            
                case STAGE_GET_HIST:
                {
                    // get the histogram form the luninance texture
                    pDynamicGammaController->GetHistogram();
                        
                    // Advance to the next stage
                    CurrentStage = STAGE_SET_GAMMA;
                    break;
                }

                case STAGE_SET_GAMMA:
                {
                    // Update gamma ramps
                    bGammaRampLinear = FALSE;
                    pDynamicGammaController->SetGammaRamp( bUseHardwareGammaRamp );
                    
                    // Advance to the next stage
                    CurrentStage = STAGE_GET_LUM;
                    break;
                }
            }
        }

        D3DGAMMARAMP Ramp;
        for( UINT i = 0; i < 256; i++ )
        {
            Ramp.red[i] = Ramp.green[i] = Ramp.blue[i] = WORD(i * 257);
        }

        // set the gamma ramp back to linear
        if( !bGammaRampLinear && (!bUseHardwareGammaRamp || !bUseDynamicGamma  ) )
        {
            g_pD3DDevice->SetGammaRamp( 0, D3DSGR_NO_CALIBRATION, &Ramp );
            bGammaRampLinear = TRUE;
        }
        if( !bUseDynamicGamma )
            pDynamicGammaController->SetDebugdrawRamp( &Ramp );
        

        // Update gamma adjustment values over time
        FLOAT fCurrent = timeGetTime()/1000.0f;
        static FLOAT fLast = fCurrent;
        FLOAT dt = fCurrent - fLast;
        fLast = fCurrent;
        pDynamicGammaController->UpdateGammaValues( dt );

        
        if( bShowDebugGammaRampAndHistogram )
        {
            // Draw the histogram
            pDynamicGammaController->DrawHistogram(  48, 75,       48+80, 75+75 );

            // Draw the gamma ramp
            pDynamicGammaController->DrawGammaRamps( 48, 75+75+60, 48+80, 75+75+60+75 );
        }

        if( bUseDynamicGamma )
        {
            // if we are using the hardware gamma ramp, we need to possibly apply an
            // inverse gamma ramp to the hud.
            if( bUseHardwareGammaRamp )
            {
                if( bShowHUD )
                {
                    if( bUseDynamicGamma )
                    {
                        // Draw the HUD with inverse gamma
                        pDynamicGammaController->SetInvGammaCorrection();
                        RenderTexture( pHUD,
                                    20.0f,
                                    g_PresentParameters.BackBufferHeight - 150.0f,
                                    20.0f + 150.0f,
                                    g_PresentParameters.BackBufferHeight - 50.0f,
                                    FALSE,
                                    TRUE );
                    }
                    else
                    {
                        RenderTexture( pHUD,
                                    20.0f,
                                    g_PresentParameters.BackBufferHeight - 150.0f,
                                    20.0f + 150.0f,
                                    g_PresentParameters.BackBufferHeight - 50.0f,
                                    TRUE,
                                    TRUE );
                    }
                }

                // reset render target and blit to back buffer
                VERIFY( g_pD3DDevice->SetRenderTarget( 0, pCurrentRenderTargetSurface ) );
                SAFE_RELEASE( pCurrentRenderTargetSurface );
                RenderTexture( pRenderTargetTexture,
                            -0.5f,
                            -0.5f,
                            (FLOAT) g_PresentParameters.BackBufferWidth,
                            (FLOAT) g_PresentParameters.BackBufferHeight
                            );
            }

            // otherwise we can draw the hud after the gamma transformed blit
            else
            {
                // set the gamma transform pixel shader
                pDynamicGammaController->SetGammaCorrection();
                 
                // reset render target and blit to back buffer
                VERIFY( g_pD3DDevice->SetRenderTarget( 0, pCurrentRenderTargetSurface ) );
                SAFE_RELEASE( pCurrentRenderTargetSurface );
                RenderTexture( pRenderTargetTexture,
                            -0.5f,
                            -0.5f,
                            (FLOAT) g_PresentParameters.BackBufferWidth,
                            (FLOAT) g_PresentParameters.BackBufferHeight,
                            FALSE,
                            FALSE
                            );

                if( bShowHUD )
                {
                    RenderTexture( pHUD,
                                   20.0f,
                                   g_PresentParameters.BackBufferHeight - 150.0f,
                                   20.0f + 150.0f,
                                   g_PresentParameters.BackBufferHeight - 50.0f,
                                   TRUE,
                                   TRUE );
                }


            }
        }
        else
        {
            if( bShowHUD )
            {
                RenderTexture( pHUD,
                            20.0f,
                            g_PresentParameters.BackBufferHeight - 150.0f,
                                20.0f + 150.0f,
                                g_PresentParameters.BackBufferHeight - 50.0f,
                                TRUE,
                                TRUE );

            }
        }



        // finished rendering
        VERIFY( g_pD3DDevice->EndScene() );
        
        VERIFY( g_pD3DDevice->Present( NULL, NULL, NULL, NULL ) );


        /*
        // output average frame rate every 2 seconds
        static DWORD Time = timeGetTime();
        static DWORD Frame = 0;
        Frame++;
        DWORD DeltaTime = timeGetTime() - Time;
        if( DeltaTime > 2000 )
        {
            float FPS = FLOAT(Frame) / ( FLOAT( DeltaTime ) / 1000.0f ) ;
            char szBuf[100];
            sprintf( szBuf, "Frame Rate: %0.2f\n", FPS );
            OutputDebugString( szBuf );
            printf( szBuf );
            Time += DeltaTime;
            Frame = 0;
        }
        */

    }

    // clean up and exit
    PostQuitMessage( 0 );
    SAFE_RELEASE( pRenderTargetTexture );
    SAFE_RELEASE( pRenderTargetSurface );
    SAFE_RELEASE( pDisplayTexture );
    SAFE_RELEASE( pHUD );
    pDynamicGammaController->Finalize();
    delete pDynamicGammaController;
	FinalizeD3D();
}
