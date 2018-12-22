//-----------------------------------------------------------------------------
// File: D3DUtil.h
//
// Desc: Helper functions and typing shortcuts for Direct3D programming.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#ifndef D3DUTIL_H
#define D3DUTIL_H




//-----------------------------------------------------------------------------
// Name: D3DUtil_GetCubeMapViewMatrix()
// Desc: Returns a view matrix for rendering to a face of a cubemap.
//-----------------------------------------------------------------------------
D3DXMATRIX D3DUtil_GetCubeMapViewMatrix( DWORD dwFace );




//-----------------------------------------------------------------------------
// Name: D3DUtil_GetRotationFromCursor()
// Desc: Returns a quaternion for the rotation implied by the window's cursor
//       position.
//-----------------------------------------------------------------------------
D3DXQUATERNION D3DUtil_GetRotationFromCursor( HWND hWnd,
                                              FLOAT fTrackBallRadius=1.0f );




//-----------------------------------------------------------------------------
// Name: D3DUtil_SetDeviceCursor
// Desc: Builds and sets a cursor for the D3D device based on hCursor.
//-----------------------------------------------------------------------------
HRESULT D3DUtil_SetDeviceCursor( LPDIRECT3DDEVICE9 pd3dDevice, HCURSOR hCursor,
                                 BOOL bAddWatermark );


//-----------------------------------------------------------------------------
// Name: D3DUtil_D3DFormatToString
// Desc: Returns the string for the given D3DFORMAT.
//       bWithPrefix determines whether the string should include the "D3DFMT_"
//-----------------------------------------------------------------------------
LPCTSTR D3DUtil_D3DFormatToString( D3DFORMAT format, bool bWithPrefix = true );


//-----------------------------------------------------------------------------
// Name: class CD3DArcBall
// Desc:
//-----------------------------------------------------------------------------
class CD3DArcBall
{
public:
    CD3DArcBall();

    // Functions to change behavior
    void Reset(); 
    VOID SetTranslationRadius( FLOAT fRadiusTranslation ) { m_fRadiusTranslation = fRadiusTranslation; }
    void SetWindow( INT nWidth, INT nHeight, FLOAT fRadius = 0.9f ) { m_nWidth = nWidth; m_nHeight = nHeight; m_fRadius = fRadius; m_vCenter = D3DXVECTOR2(m_nWidth/2.0f,m_nHeight/2.0f); }

    // Call these from client and use GetRotationMatrix() to read new rotation matrix
    void OnBegin( int nX, int nY );  // start the rotation (pass current mouse position)
    void OnMove( int nX, int nY );   // continue the rotation (pass current mouse position)
    void OnEnd();                    // end the rotation 

    // Or call this to automatically handle left, middle, right buttons
    LRESULT     HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    // Functions to get/set state
    D3DXMATRIX* GetRotationMatrix() { return D3DXMatrixRotationQuaternion(&m_mRotation, &m_qNow); };
    D3DXMATRIX* GetTranslationMatrix()      { return &m_mTranslation; }
    D3DXMATRIX* GetTranslationDeltaMatrix() { return &m_mTranslationDelta; }
    BOOL        IsBeingDragged()            { return m_bDrag; }
    D3DXQUATERNION GetQuatNow()             { return m_qNow; }
    void        SetQuatNow( D3DXQUATERNION q ) { m_qNow = q; }

protected:
    D3DXMATRIXA16  m_mRotation;         // Matrix for arc ball's orientation
    D3DXMATRIXA16  m_mTranslation;      // Matrix for arc ball's position
    D3DXMATRIXA16  m_mTranslationDelta; // Matrix for arc ball's position

    INT            m_nWidth;   // arc ball's window width
    INT            m_nHeight;  // arc ball's window height
    D3DXVECTOR2    m_vCenter;  // center of arc ball 
    FLOAT          m_fRadius;  // arc ball's radius in screen coords
    FLOAT          m_fRadiusTranslation; // arc ball's radius for translating the target

    D3DXQUATERNION m_qDown;             // Quaternion before button down
    D3DXQUATERNION m_qNow;              // Composite quaternion for current drag
    BOOL           m_bDrag;             // Whether user is dragging arc ball

    POINT          m_ptLastMouse;      // position of last mouse point
    D3DXVECTOR3    m_vDownPt;           // starting point of rotation arc
    D3DXVECTOR3    m_vCurrentPt;        // current point of rotation arc

    D3DXQUATERNION QuatFromBallPoints( const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo );
    D3DXVECTOR3    ScreenToVector( float fScreenPtX, float fScreenPtY );
};




//-----------------------------------------------------------------------------
// Name: enum D3DUtil_CameraKeys 
// Desc: used by CCamera to map WM_KEYDOWN keys
//-----------------------------------------------------------------------------
enum D3DUtil_CameraKeys
{
    CAM_STRAFE_LEFT = 0,
    CAM_STRAFE_RIGHT,
    CAM_MOVE_FORWARD,
    CAM_MOVE_BACKWARD,
    CAM_MOVE_UP,
    CAM_MOVE_DOWN,
    CAM_RESET,
    CAM_MAX_KEYS,
    CAM_UNKNOWN = 0xFF
};

#define KEY_WAS_DOWN_MASK 0x80
#define KEY_IS_DOWN_MASK  0x01

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08



//-----------------------------------------------------------------------------
// Name: class CBaseCamera
// Desc: Simple base camera class that moves and rotates.  The base class
//       records mouse and keyboard input for use by a derieved class, and 
//       keeps common state.
//-----------------------------------------------------------------------------
class CBaseCamera
{
public:
    CBaseCamera();

    // Call these from client and use Get*Matrix() to read new matrices
    virtual LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual VOID    FrameMove( FLOAT fElapsedTime ) = 0;

    // Functions to change camera matrices
    virtual void Reset(); 
    VOID SetViewParams( D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt );
    VOID SetProjParams( FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane );

    // Functions to change behavior
    VOID SetInvertPitch( bool bInvertPitch ) { m_bInvertPitch = bInvertPitch; }
    VOID SetDrag( bool bMovementDrag, FLOAT fTotalDragTimeToZero = 0.25f ) { m_bMovementDrag = bMovementDrag; m_fTotalDragTimeToZero = fTotalDragTimeToZero; }
    VOID SetEnableYAxisMovement( bool bEnableYAxisMovement ) { m_bEnableYAxisMovement = bEnableYAxisMovement; }
    VOID SetEnablePositionMovement( bool bEnablePositionMovement ) { m_bEnablePositionMovement = bEnablePositionMovement; }
    VOID SetClipToBoundary( bool bClipToBoundary, D3DXVECTOR3* pvMinBoundary, D3DXVECTOR3* pvMaxBoundary ) { m_bClipToBoundary = bClipToBoundary; if( pvMinBoundary ) m_vMinBoundary = *pvMinBoundary; if( pvMaxBoundary ) m_vMaxBoundary = *pvMaxBoundary; }
    VOID SetScalers( FLOAT fRotationScaler = 0.01f, FLOAT fMoveScaler = 5.0f )  { m_fRotationScaler = fRotationScaler; m_fMoveScaler = fMoveScaler; }
    VOID SetNumberOfFramesToSmoothMouseData( int nFrames ) { if( nFrames > 0 ) m_fFramesToSmoothMouseData = (float)nFrames; }
    VOID SetResetCursorAfterMove( bool bResetCursorAfterMove ) { m_bResetCursorAfterMove = bResetCursorAfterMove; }

    // Functions to get state
    D3DXMATRIX*  GetViewMatrix()            { return &m_mView; }
    D3DXMATRIX*  GetProjMatrix()            { return &m_mProj; }
    bool IsBeingDragged() { return (m_bMouseLButtonDown || m_bMouseMButtonDown || m_bMouseRButtonDown); }
    bool IsMouseLButtonDown() { return m_bMouseLButtonDown; } 
    bool IsMouseMButtonDown() { return m_bMouseMButtonDown; } 
    bool IsMouseRButtonDown() { return m_bMouseRButtonDown; } 

protected:
    // Functions to map a WM_KEYDOWN key to a D3DUtil_CameraKeys enum
    virtual D3DUtil_CameraKeys MapKey( UINT nKey );    
    BOOL IsKeyDown( BYTE key )  { return( (key & KEY_IS_DOWN_MASK) == KEY_IS_DOWN_MASK ); }
    BOOL WasKeyDown( BYTE key ) { return( (key & KEY_WAS_DOWN_MASK) == KEY_WAS_DOWN_MASK ); }

    void ConstrainToBoundary( D3DXVECTOR3* pV );
    void UpdateMouseDelta( float fElapsedTime );
    void UpdateVelocity( float fElapsedTime );

    D3DXMATRIX            m_mView;              // View matrix 
    D3DXMATRIX            m_mProj;              // Projection matrix

    BYTE                  m_aKeys[CAM_MAX_KEYS];  // State of input - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK
    POINT                 m_ptLastMousePosition;  // Last absolute postion of mouse cursor
    bool                  m_bMouseLButtonDown;    // True if left button is down 
    bool                  m_bMouseMButtonDown;    // True if middle button is down 
    bool                  m_bMouseRButtonDown;    // True if right button is down 
    int                   m_nCurrentButtonMask;   // mask of which buttons are down
    int                   m_nMouseWheelDelta;     // Amount of middle wheel scroll (+/-) 
    D3DXVECTOR2           m_vMouseDelta;          // Mouse relative delta smoothed over a few frames
    float                 m_fFramesToSmoothMouseData; // Number of frames to smooth mouse data over

    D3DXVECTOR3           m_vDefaultEye;          // Default camera eye position
    D3DXVECTOR3           m_vDefaultLookAt;       // Default LookAt position
    D3DXVECTOR3           m_vEye;                 // Camera eye position
    D3DXVECTOR3           m_vLookAt;              // LookAt position
    float                 m_fCameraYawAngle;      // Yaw angle of camera
    float                 m_fCameraPitchAngle;    // Pitch angle of camera

    D3DXVECTOR3           m_vVelocity;            // Velocity of camera
    bool                  m_bMovementDrag;        // If true, then camera movement will slow to a stop otherwise movement is instant
    D3DXVECTOR3           m_vVelocityDrag;        // Velocity drag force
    FLOAT                 m_fDragTimer;           // Countdown timer to apply drag
    FLOAT                 m_fTotalDragTimeToZero; // Time it takes for velocity to go from full to 0
    D3DXVECTOR2           m_vRotVelocity;         // Velocity of camera

    float                 m_fFOV;                 // Field of view
    float                 m_fAspect;              // Aspect ratio
    float                 m_fNearPlane;           // Near plane
    float                 m_fFarPlane;            // Far plane

    float                 m_fRotationScaler;      // Scaler for rotation
    float                 m_fMoveScaler;          // Scaler for movement

    bool                  m_bInvertPitch;         // Invert the pitch axis
    bool                  m_bEnablePositionMovement; // If true, then the user can translate the camera/model 
    bool                  m_bEnableYAxisMovement; // If true, then camera can move in the y-axis

    bool                  m_bClipToBoundary;      // If true, then the camera will be clipped to the boundary
    D3DXVECTOR3           m_vMinBoundary;         // Min point in clip boundary
    D3DXVECTOR3           m_vMaxBoundary;         // Max point in clip boundary

    bool                  m_bResetCursorAfterMove;// If true, the class will reset the cursor position so that the cursor always has space to move 
};




//-----------------------------------------------------------------------------
// Name: class CFirstPersonCamera
// Desc: Simple first person camera class that moves and rotates.
//       It allows yaw and pitch but not roll.  It uses WM_KEYDOWN and 
//       GetCursorPos() to respond to keyboard and mouse input and updates the 
//       view matrix based on input.  
//-----------------------------------------------------------------------------
class CFirstPersonCamera : public CBaseCamera
{
public:
    CFirstPersonCamera();

    // Call these from client and use Get*Matrix() to read new matrices
    virtual VOID FrameMove( FLOAT fElapsedTime );

    // Functions to get state
    D3DXVECTOR3  GetWorldRight()            { return D3DXVECTOR3( m_mCameraWorld._11, m_mCameraWorld._12, m_mCameraWorld._13 ); } 
    D3DXVECTOR3  GetWorldUp()               { return D3DXVECTOR3( m_mCameraWorld._21, m_mCameraWorld._22, m_mCameraWorld._23 ); }
    D3DXVECTOR3  GetWorldAhead()            { return D3DXVECTOR3( m_mCameraWorld._31, m_mCameraWorld._32, m_mCameraWorld._33 ); }
    D3DXVECTOR3  GetEyePt()                 { return D3DXVECTOR3( m_mCameraWorld._41, m_mCameraWorld._42, m_mCameraWorld._43 ); }

protected:
    D3DXMATRIX m_mCameraWorld;       // World matrix of the camera (inverse of the view matrix)

};




//-----------------------------------------------------------------------------
// Name: class CModelViewerCamera
// Desc: 
//-----------------------------------------------------------------------------
class CModelViewerCamera : public CBaseCamera
{
public:
    CModelViewerCamera();

    // Call these from client and use Get*Matrix() to read new matrices
    virtual LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual VOID FrameMove( FLOAT fElapsedTime );

    // Functions to change behavior
    void Reset(); 
    VOID SetButtonMasks( int nRotateModelButtonMask = MOUSE_LEFT_BUTTON, int nZoomButtonMask = MOUSE_WHEEL, int nRotateCameraButtonMask = MOUSE_RIGHT_BUTTON ) { m_nRotateModelButtonMask = nRotateModelButtonMask, m_nZoomButtonMask = nZoomButtonMask; m_nRotateCameraButtonMask = nRotateCameraButtonMask; }
    VOID SetWindow( int nWidth, int nHeight, float fArcballRadius=0.9f ) { m_WorldArcBall.SetWindow( nWidth, nHeight, fArcballRadius ); m_ViewArcBall.SetWindow( nWidth, nHeight, fArcballRadius ); }
    VOID SetRadius( float fDefaultRadius=5.0f, float fMinRadius=1.0f, float fMaxRadius=FLT_MAX  ) { m_fDefaultRadius = m_fRadius = fDefaultRadius; m_fMinRadius = fMinRadius; m_fMaxRadius = fMaxRadius; }
    VOID SetModelCenter( D3DXVECTOR3 vModelCenter ) { m_vModelCenter = vModelCenter; }
    VOID SetLimitPitch( bool bLimitPitch ) { m_bLimitPitch = bLimitPitch; }
    VOID SetViewQuat( D3DXQUATERNION q ) { m_ViewArcBall.SetQuatNow( q ); }
    VOID SetWorldQuat( D3DXQUATERNION q ) { m_WorldArcBall.SetQuatNow( q ); }

    // Functions to get state
    D3DXMATRIX* GetWorldMatrix() { return &m_mWorld; }

protected:
    CD3DArcBall  m_WorldArcBall;
    CD3DArcBall  m_ViewArcBall;
    D3DXVECTOR3  m_vModelCenter;
    D3DXMATRIX   m_mModelLastRot;        // Last arcball rotation matrix for model 
    D3DXMATRIX   m_mModelRot;            // Rotation matrix of model
    D3DXMATRIX   m_mWorld;               // World matrix of model

    int          m_nRotateModelButtonMask;
    int          m_nZoomButtonMask;
    int          m_nRotateCameraButtonMask;

    bool         m_bLimitPitch;
    float        m_fRadius;              // Distance from the camera to model 
    float        m_fDefaultRadius;       // Distance from the camera to model 
    float        m_fMinRadius;           // Min radius
    float        m_fMaxRadius;           // Max radius
};





#endif