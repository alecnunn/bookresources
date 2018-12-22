//-----------------------------------------------------------------------------
// File: Gems 4.h
//
// Desc: Header file Gems 4 sample app
//-----------------------------------------------------------------------------
#pragma once

#include "GG4Model.h"


//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY       TEXT("Software\\Game Programming Gems 4\\Team Color")
#define CUBE(x)			((x) * (x) * (x))

// Struct to store the current input state
struct UserInput
{
	float	fCameraRotX;
	float	fCameraRotY;
    BOOL	bObjectRotateUp;
    BOOL	bObjectRotateDown;
    BOOL	bObjectRotateLeft;
    BOOL	bObjectRotateRight;
	BOOL	bLeftDrag;
	BOOL	bHadInput;
};


/*
 * Simple little state machine to cycle through colors.
 */
class SLSMachine
{
public:

	enum SLSMState
	{
		kslsmPause					= 0,
		kslsmChooseNewColor			= 1,
		kslsmInterpolateToNewColor	= 2,
		kslsmMax					= 3
	};

							 SLSMachine(float inPauseTime);
	virtual					~SLSMachine(void) { };

	const D3DCOLORVALUE&	GetCurrentColor(void) const
								{ return this->mCurrentColor; }

	void					Update(float inTimePassed);

protected:

	void					SetRandomColor(D3DCOLORVALUE& outColor)
								{ outColor.r = (float)rand() / (float)RAND_MAX;
								  outColor.g = (float)rand() / (float)RAND_MAX;
								  outColor.b = (float)rand() / (float)RAND_MAX; }

	void					SetNewState(SLSMState inNewState)
								{ this->mCurrentState		= inNewState;
								  this->mCurrentStateTimer	= this->mStateDurations[this->mCurrentState]; }

	D3DCOLORVALUE			mCurrentColor;
	D3DCOLORVALUE			mStartingColor;
	D3DCOLORVALUE			mEndingColor;
	SLSMState				mCurrentState;
	float					mStateDurations[kslsmMax];
	float					mCurrentStateTimer;
};


//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	GG4Model*				mTestModels[kgg4tctMax];	/* a test model */
	SLSMachine				mMajorColorStateMachine;	/* a state machine */
	SLSMachine				mMinorColorStateMachine;	/* a state machine */
	GG4TeamColorTechnique	mTeamColorTechnique;		/* the team color technique */

    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    CD3DFont*               m_pFont;                // Font for drawing text

    UserInput               m_UserInput;            // Struct for storing user input 

    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis
	FLOAT                   m_fCameraRotX;          // Camera rotation state X-axis
    FLOAT                   m_fCameraRotY;          // Camera rotation state Y-axis

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    HRESULT RenderText(bool inSuccessfulRender);

    void    UpdateKeyboardInput(UserInput* pUserInput);
	void    UpdateMouseInput(UserInput* pUserInput, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

