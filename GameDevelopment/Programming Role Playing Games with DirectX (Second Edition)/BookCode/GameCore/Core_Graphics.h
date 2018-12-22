/**************************************************
Core_Graphics.h
GameCore Component

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

#ifndef _CORE_GRAPHICS_H_
#define _CORE_GRAPHICS_H_

#define ReleaseCOM(x) if(x) { x->Release(); x = NULL; }

class cGraphics;

class cTexture;
class cMaterial;
class cLight;
class cFont;
class cVertexBuffer;

class cWorldPosition;
class cCamera;
class cMesh;
class cObject;
class cAnimation;

struct sMesh;
struct sFrame;
struct sAnimation;
struct sAnimationSet;

class cGraphics
{
  protected:
    HWND              m_hWnd;
    IDirect3D9       *m_pD3D;
    IDirect3DDevice9 *m_pD3DDevice;
    ID3DXSprite      *m_pSprite;

    D3DDISPLAYMODE    m_d3ddm;

    BOOL              m_Windowed;
    BOOL              m_ZBuffer;
    BOOL              m_HAL;

    long              m_Width;
    long              m_Height;
    char              m_BPP;

    char              m_AmbientRed;
    char              m_AmbientGreen;
    char              m_AmbientBlue;

  public:
    cGraphics();
    ~cGraphics();

    IDirect3D9       *GetDirect3DCOM();
    IDirect3DDevice9 *GetDeviceCOM();
    ID3DXSprite      *GetSpriteCOM();

    BOOL Init();
    BOOL Shutdown();

    BOOL SetMode(HWND hWnd, BOOL Windowed = TRUE, BOOL UseZBuffer = FALSE, long Width = 0, long Height = 0, char BPP = 0);

    long GetNumDisplayModes(D3DFORMAT Format);
    BOOL GetDisplayModeInfo(long Num, D3DDISPLAYMODE *Mode, D3DFORMAT Format);

    char GetFormatBPP(D3DFORMAT Format);
    BOOL CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL);

    BOOL Display();

    BOOL BeginScene();
    BOOL EndScene();

    BOOL BeginSprite();
    BOOL EndSprite();

    BOOL Clear(long Color = 0, float ZBuffer = 1.0f);
    BOOL ClearDisplay(long Color = 0);
    BOOL ClearZBuffer(float ZBuffer = 1.0f);

    long GetWidth();
    long GetHeight();
    char GetBPP();
    BOOL GetHAL();
    BOOL GetZBuffer();

    BOOL SetPerspective(float FOV=D3DX_PI / 4.0f, float Aspect=1.3333f, float Near=1.0f, float Far=10000.0f);
    
    BOOL SetWorldPosition(cWorldPosition *WorldPos);
    BOOL SetCamera(cCamera *Camera);
    BOOL SetLight(long Num, cLight *Light);
    BOOL SetMaterial(cMaterial *Material);
    BOOL SetTexture(short Num, cTexture *Texture);
    
    BOOL SetAmbientLight(char Red, char Green, char Blue);
    BOOL GetAmbientLight(char *Red, char *Green, char *Blue);

    BOOL EnableLight(long Num, BOOL Enable = TRUE);
    BOOL EnableLighting(BOOL Enable = TRUE);
    BOOL EnableZBuffer(BOOL Enable = TRUE);
    BOOL EnableAlphaBlending(BOOL Enable = TRUE, DWORD Src = D3DBLEND_SRCALPHA, DWORD Dest = D3DBLEND_INVSRCALPHA);
    BOOL EnableAlphaTesting(BOOL Enable = TRUE);
};

class cTexture
{
  protected:
    cGraphics         *m_Graphics;    // Parent cGraphics
    IDirect3DTexture9 *m_Texture;     // Texture COM
    unsigned long m_Width, m_Height;  // Dimensions of
                                      // texture image.

  public:
    cTexture(); // Constructor
    ~cTexture(); // Destructor

    IDirect3DTexture9 *GetTextureCOM(); // Return texture COM

    // Load a texture from file
    BOOL Load(cGraphics *Graphics, char *Filename,             \
              DWORD Transparent = 0,                           \
              D3DFORMAT Format = D3DFMT_UNKNOWN);

    // Create a texture using specific dimensions and format
    BOOL Create(cGraphics *Graphics,                           \
                DWORD Width, DWORD Height, D3DFORMAT Format);

    // Configure a cTexture class from an existing
    // IDirect3DTexture9 object instance.
    BOOL Create(cGraphics *Graphics,                           \
                IDirect3DTexture9 *Texture);

    BOOL Free(); // Free texture object

    BOOL IsLoaded();  // Returns TRUE if texture is loaded

    long GetWidth();  // Return width (pitch) of texture
    long GetHeight(); // Return height of texture
    D3DFORMAT GetFormat(); // Return texture storage format

    // Draw a 2-D portion of texture to device
    BOOL Blit(long DestX, long DestY,                          \
              long SrcX = 0, long SrcY = 0,                    \
              long Width = 0, long Height = 0,                 \
              float XScale = 1.0f, float YScale = 1.0f,        \
              D3DCOLOR Color = 0xFFFFFFFF);
};


class cMaterial
{
  protected:
    D3DMATERIAL9  m_Material;

  public:
    cMaterial();

    D3DMATERIAL9 *GetMaterial();

    BOOL SetDiffuseColor(char Red, char Green, char Blue);
    BOOL GetDiffuseColor(char *Red, char *Green, char *Blue);

    BOOL SetAmbientColor(char Red, char Green, char Blue);
    BOOL GetAmbientColor(char *Red, char *Green, char *Blue);
    
    BOOL SetSpecularColor(char Red, char Green, char Blue);
    BOOL GetSpecularColor(char *Red, char *Green, char *Blue);
    
    BOOL SetEmissiveColor(char Red, char Green, char Blue);
    BOOL GetEmissiveColor(char *Red, char *Green, char *Blue);

    BOOL  SetPower(float Power);
    float GetPower(float Power);
};

class cLight
{
  protected:
    D3DLIGHT9 m_Light;

  public:
    cLight();

    D3DLIGHT9 *GetLight();

    BOOL SetType(D3DLIGHTTYPE Type);

    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XPos, float YPos, float ZPos);
    BOOL GetPos(float *XPos, float *YPos, float *ZPos);

    BOOL Point(float XFrom, float YFrom, float ZFrom,
               float XAt,   float YAt,   float ZAt);
    BOOL GetDirection(float *XDir, float *YDir, float *ZDir);

    BOOL SetDiffuseColor(char Red, char Green, char Blue);
    BOOL GetDiffuseColor(char *Red, char *Green, char *Blue);

    BOOL SetSpecularColor(char Red, char Green, char Blue);
    BOOL GetSpecularColor(char *Red, char *Green, char *Blue);
    
    BOOL SetAmbientColor(char Red, char Green, char Blue);
    BOOL GetAmbientColor(char *Red, char *Green, char *Blue);

    BOOL SetRange(float Range);
    float GetRange();
    
    BOOL SetFalloff(float Falloff);
    float GetFalloff();
    
    BOOL SetAttenuation0(float Attenuation);
    float GetAttenuation0();

    BOOL SetAttenuation1(float Attenuation);
    float GetAttenuation1();

    BOOL SetAttenuation2(float Attenuation);
    float GetAttenuation2();
    
    BOOL SetTheta(float Theta);
    float GetTheta();
    
    BOOL SetPhi(float Phi);
    float GetPhi();
};

class cWorldPosition
{
  protected:
    BOOL  m_Billboard;

    float m_XPos,      m_YPos,      m_ZPos;
    float m_XRotation, m_YRotation, m_ZRotation;
    float m_XScale,    m_YScale,    m_ZScale;
    
    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matScale;
    D3DXMATRIX m_matRotation;
    D3DXMATRIX m_matTranslation;
    D3DXMATRIX *m_matCombine1;
    D3DXMATRIX *m_matCombine2;

  public:
    cWorldPosition();

    D3DXMATRIX *GetMatrix(cGraphics *Graphics = NULL);
    BOOL SetCombineMatrix1(D3DXMATRIX *Matrix = NULL);
    BOOL SetCombineMatrix2(D3DXMATRIX *Matrix = NULL);

    BOOL Copy(cWorldPosition *DestPos);

    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XAdd, float YAdd, float ZAdd);

    BOOL Rotate(float XRot, float YRot, float ZRot);
    BOOL RotateRel(float XAdd, float YAdd, float ZAdd);

    BOOL Scale(float XScale, float YScale, float ZScale);
    BOOL ScaleRel(float XAdd, float YAdd, float ZAdd);

    BOOL Update(cGraphics *Graphics = NULL);
    BOOL EnableBillboard(BOOL Enable = TRUE);

    float GetXPos();
    float GetYPos();
    float GetZPos();
    float GetXRotation();
    float GetYRotation();
    float GetZRotation();
    float GetXScale();
    float GetYScale();
    float GetZScale();
};

class cCamera
{
  protected:
    float m_XPos, m_YPos, m_ZPos;
    float m_XRot, m_YRot, m_ZRot;

    float m_StartXPos, m_StartYPos, m_StartZPos;
    float m_StartXRot, m_StartYRot, m_StartZRot;

    float m_EndXPos, m_EndYPos, m_EndZPos;
    float m_EndXRot, m_EndYRot, m_EndZRot;
    
    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matTranslation;
    D3DXMATRIX m_matRotation;

  public:
    cCamera();

    D3DXMATRIX *GetMatrix(); // Get view transformation matrix
    BOOL Update();           // Update transformation matrix

    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XAdd, float YAdd, float ZAdd);

    BOOL Rotate(float XRot, float YRot, float ZRot);
    BOOL RotateRel(float XAdd, float YAdd, float ZAdd);

    BOOL Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt);

    BOOL SetStartTrack();
    BOOL SetEndTrack();
    BOOL Track(float Time, float Length);

    float GetXPos();
    float GetYPos();
    float GetZPos();
    float GetXRotation();
    float GetYRotation();
    float GetZRotation();
};

class cFont
{
  private:
    ID3DXFont *m_Font;

  public:
    cFont();
    ~cFont();

    ID3DXFont *GetFontCOM();

    BOOL Create(cGraphics *Graphics, char *Name, long Size = 16, BOOL Bold = FALSE, BOOL Italic = FALSE, BOOL Underline = FALSE, BOOL Strikeout = FALSE);
    BOOL Free();

    BOOL Begin();
    BOOL End();
    BOOL Print(char *Text, long XPos, long YPos, long Width = 0, long Height = 0, D3DCOLOR Color = 0xFFFFFFFF, DWORD Format = 0);
};

typedef struct sMesh
{
  char               *m_Name;            // Name of mesh

  ID3DXMesh          *m_Mesh;            // Mesh object
  ID3DXMesh          *m_SkinMesh;        // Skin mesh object
  ID3DXSkinInfo      *m_SkinInfo;        // Skin mesh info

  DWORD               m_NumMaterials;    // # materials in mesh
  D3DMATERIAL9       *m_Materials;       // Array of materials
  IDirect3DTexture9 **m_Textures;        // Array of textures

  DWORD               m_NumBones;        // # of bones
  D3DXMATRIX         *m_Matrices;        // Bone matrices
  D3DXMATRIX        **m_FrameMatrices;   // Pointers to frame matrices

  D3DXVECTOR3         m_Min, m_Max;      // Bounding box
  float               m_Radius;          // Bounding sphere

  sMesh              *m_Next;            // Next mesh in list

  sMesh()
  { 
    m_Name           = NULL;
    m_Mesh           = NULL;
    m_SkinMesh       = NULL;
    m_SkinInfo       = NULL;
    m_NumMaterials   = 0;
    m_Materials      = NULL;
    m_Textures       = NULL;
    m_NumBones       = 0;
    m_Matrices       = NULL;
    m_FrameMatrices  = NULL;
    m_Min.x = m_Min.y = m_Min.z = m_Max.x = m_Max.y = m_Max.z = 0.0f;
    m_Radius         = 0.0f;
    m_Next           = NULL;
  }

  ~sMesh()
  {
    delete [] m_Name;
    ReleaseCOM(m_Mesh);
    ReleaseCOM(m_SkinMesh);
    ReleaseCOM(m_SkinInfo);
    delete [] m_Materials;
    if(m_Textures != NULL) {
      for(DWORD i=0;i<m_NumMaterials;i++) {
        ReleaseCOM(m_Textures[i]);
      }
      delete [] m_Textures;
    }
    delete [] m_Matrices;
    delete [] m_FrameMatrices;
    delete m_Next;
  }

  sMesh *FindMesh(char *Name)
  {
    sMesh *Mesh;

    // Return first instance if name == NULL
    if(Name == NULL)
      return this;

    // Compare names and return if exact match
    if(m_Name != NULL && !strcmp(Name, m_Name))
      return this;

    // Search next in list
    if(m_Next != NULL) {
      if((Mesh = m_Next->FindMesh(Name)) != NULL)
        return Mesh;
    }

    return NULL;
  }

  DWORD GetNumMaterials()
  {
    return m_NumMaterials;
  }

  D3DMATERIAL9 *GetMaterial(unsigned long Num)
  {
    if(Num >= m_NumMaterials || m_Materials == NULL)
      return NULL;
    return &m_Materials[Num];
  }

  IDirect3DTexture9 *GetTexture(unsigned long Num)
  {
    if(Num >= m_NumMaterials || m_Textures == NULL)
      return NULL;
    return m_Textures[Num];
  }

  void CopyFrameToBoneMatrices()
  {
    DWORD i;

    // Copy all matrices from frames to local bone matrix array
    if(m_NumBones && m_Matrices != NULL && m_FrameMatrices != NULL) {
      for(i=0;i<m_NumBones;i++) {
        if(m_FrameMatrices[i] != NULL)
          D3DXMatrixMultiply(&m_Matrices[i], m_SkinInfo->GetBoneOffsetMatrix(i), m_FrameMatrices[i]);
        else
          D3DXMatrixIdentity(&m_Matrices[i]);
      }
    }

    // Process next in list
    if(m_Next != NULL)
      m_Next->CopyFrameToBoneMatrices();
  }
} sMesh;

typedef struct sFrameMeshList
{
  sMesh          *m_Mesh;
  sFrameMeshList *m_Next;

  sFrameMeshList()
  {
    m_Mesh = NULL;
    m_Next = NULL;
  }

  ~sFrameMeshList()
  {
    delete m_Next;
  }
} sFrameMeshList;

typedef struct sFrame
{
  char       *m_Name;

  sFrameMeshList *m_MeshList;   // List of meshes attached to this frame

  D3DXMATRIX  m_matCombined;    // Combined transformation matrix
  D3DXMATRIX  m_matTransformed; // Currently transformed matrix
  D3DXMATRIX  m_matOriginal;    // Original .X file matrix
  
  sFrame     *m_Parent;         // Parent Frame
  sFrame     *m_Child;          // Child frame
  sFrame     *m_Sibling;        // Sibling frame

  sFrame() 
  {
    m_Name = NULL;
    m_MeshList = NULL;
    D3DXMatrixIdentity(&m_matCombined);
    D3DXMatrixIdentity(&m_matTransformed);
    D3DXMatrixIdentity(&m_matOriginal);
    m_Parent = m_Sibling = m_Child = NULL;
  }

  ~sFrame()
  {
    delete m_Name;
    delete m_MeshList;
    delete m_Child;
    delete m_Sibling;
  }

  sFrame *FindFrame(char *Name)
  {
    sFrame *Frame;

    // Return this instance if name == NULL
    if(Name == NULL)
      return this;
    
    // Compare names and return if exact match
    if(m_Name != NULL && !strcmp(Name, m_Name))
      return this;

    // Search child lists
    if(m_Child != NULL) {
      if((Frame = m_Child->FindFrame(Name)) != NULL)
        return Frame;
    }

    // Search sibling lists
    if(m_Sibling != NULL) {
      if((Frame = m_Sibling->FindFrame(Name)) != NULL)
        return Frame;
    }

    return NULL;
  }

  void ResetMatrices()
  {
    m_matTransformed = m_matOriginal;

    if(m_Child != NULL)
      m_Child->ResetMatrices();
    if(m_Sibling != NULL)
      m_Sibling->ResetMatrices();      
  }

  void AddMesh(sMesh *Mesh)
  {
    sFrameMeshList *List;

    List = new sFrameMeshList();
    List->m_Mesh = Mesh;
    List->m_Next = m_MeshList;
    m_MeshList = List;
  }
} sFrame;

class cMesh
{
  private:
    cGraphics *m_Graphics;

    long       m_NumMeshes;
    sMesh     *m_Meshes;

    long       m_NumFrames;
    sFrame    *m_Frames;

    D3DXVECTOR3    m_Min, m_Max;
    float          m_Radius;

    void ParseXFileData(IDirectXFileData *pData, sFrame *ParentFrame, char *TexturePath);
    void MapFramesToBones(sFrame *Frame);

  public:
    cMesh();
    ~cMesh();

    BOOL    IsLoaded();

    long    GetNumFrames();
    sFrame *GetParentFrame();
    sFrame *GetFrame(char *Name);

    long    GetNumMeshes();
    sMesh  *GetParentMesh();
    sMesh  *GetMesh(char *Name);

    BOOL    GetBounds(float *MinX, float *MinY, float *MinZ, float *MaxX, float *MaxY, float *MaxZ, float *Radius);

    BOOL    Load(cGraphics *Graphics, char *Filename, char *TexturePath = ".\\");
    BOOL    Free();
};

class cObject
{
  protected:
    cGraphics     *m_Graphics;
    cMesh         *m_Mesh;
    sAnimationSet *m_AnimationSet;
    cWorldPosition m_Pos;

    BOOL           m_Billboard;

    unsigned long  m_StartTime;

    void UpdateFrame(sFrame *Frame, D3DXMATRIX *Matrix);
    void DrawFrame(sFrame *Frame);
    void DrawMesh(sMesh *Mesh);

  public:
    cObject();
    ~cObject();

    BOOL Create(cGraphics *Graphics, cMesh *Mesh = NULL);
    BOOL Free();

    BOOL EnableBillboard(BOOL Enable = TRUE);
    BOOL AttachToObject(cObject *Object, char *FrameName = NULL);

    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XAdd, float YAdd, float ZAdd);

    BOOL Rotate(float XRot, float YRot, float ZRot);
    BOOL RotateRel(float XAdd, float YAdd, float ZAdd);

    BOOL Scale(float XScale, float YScale, float ZScale);
    BOOL ScaleRel(float XAdd, float YAdd, float ZAdd);

    D3DXMATRIX *GetMatrix();

    float GetXPos();
    float GetYPos();
    float GetZPos();
    float GetXRotation();
    float GetYRotation();
    float GetZRotation();
    float GetXScale();
    float GetYScale();
    float GetZScale();

    BOOL  GetBounds(float *MinX, float *MinY, float *MinZ, float *MaxX, float *MaxY, float *MaxZ, float *Radius);

    BOOL SetMesh(cMesh *Mesh);
    cMesh *GetMesh();
    BOOL SetAnimation(cAnimation *Animation, char *Name = NULL, unsigned long StartTime = 0);
    char *GetAnimation();
    BOOL ResetAnimation(unsigned long StartTime = 0);

    BOOL UpdateAnimation(unsigned long Time, BOOL Smooth = TRUE);
    BOOL AnimationComplete(unsigned long Time);
 
    BOOL Update();
    BOOL Render();
};

typedef struct 
{
  DWORD Time;
  DWORD Floats;
  float w;
  float x;
  float y;
  float z;
} sXFileRotateKey;

typedef struct 
{
  DWORD       Time;
  DWORD       Floats;	
  D3DXVECTOR3 Scale;	
} sXFileScaleKey;

typedef struct
{
  DWORD       Time;
  DWORD       Floats;	
  D3DXVECTOR3 Pos;	
} sXFilePositionKey;

typedef struct
{
  DWORD      Time;
  DWORD      Floats;	
  D3DXMATRIX Matrix;
} sXFileMatrixKey;

typedef struct
{
  DWORD          Time;
  D3DXQUATERNION Quaternion;
} sRotateKey;

typedef struct
{
  DWORD       Time;
  D3DXVECTOR3 Pos;
  D3DXVECTOR3 PosInterpolation;
} sPositionKey;

typedef struct
{
  DWORD	      Time;
  D3DXVECTOR3 Scale;
  D3DXVECTOR3 ScaleInterpolation;
} sScaleKey;

typedef struct
{
  DWORD	     Time;
  D3DXMATRIX Matrix;
  D3DXMATRIX MatInterpolation;
} sMatrixKey;

typedef struct sAnimation
{
  char         *m_Name;
  char         *m_FrameName;
  sFrame       *m_Frame;

  BOOL          m_Loop;
  BOOL          m_Linear;

  DWORD           m_NumPositionKeys;
  sPositionKey *m_PositionKeys;

  DWORD           m_NumRotateKeys;
  sRotateKey   *m_RotateKeys;

  DWORD           m_NumScaleKeys;
  sScaleKey    *m_ScaleKeys;

  DWORD           m_NumMatrixKeys;
  sMatrixKey     *m_MatrixKeys;

  sAnimation   *m_Next;

  sAnimation()
  {
    m_Name            = NULL;
    m_FrameName       = NULL;
    m_Frame           = NULL;
    m_Loop            = FALSE;
    m_Linear          = TRUE;
    m_NumPositionKeys = 0;
    m_PositionKeys    = NULL;
    m_NumRotateKeys   = 0;
    m_RotateKeys      = NULL;
    m_NumScaleKeys    = 0;
    m_ScaleKeys       = NULL;
    m_NumMatrixKeys   = 0;
    m_MatrixKeys      = NULL;
    m_Next            = NULL;
  }

  ~sAnimation()
  {
    delete [] m_Name;         m_Name         = NULL;
    delete [] m_FrameName;    m_FrameName    = NULL;
    delete [] m_PositionKeys; m_PositionKeys = NULL;
    delete [] m_RotateKeys;   m_RotateKeys   = NULL;
    delete [] m_ScaleKeys;    m_ScaleKeys    = NULL;
    delete [] m_MatrixKeys;   m_MatrixKeys   = NULL;
    delete m_Next;            m_Next         = NULL;
  }

  void Update(DWORD Time, BOOL Smooth)
  {
    unsigned long i, Key;
    DWORD TimeDiff, IntTime;
    D3DXMATRIX     Matrix, MatTemp;
    D3DXVECTOR3    Vector;
    D3DXQUATERNION Quat;
    
    if(m_Frame == NULL)
      return;

    // update rotation, scale, and position keys
    if(m_NumRotateKeys || m_NumScaleKeys || m_NumPositionKeys) {
      D3DXMatrixIdentity(&Matrix);

      if(m_NumRotateKeys && m_RotateKeys != NULL) {
        // Find the key that fits this time
        Key = 0;
        for(i=0;i<m_NumRotateKeys;i++) {
          if(m_RotateKeys[i].Time <= Time)
            Key = i;
          else
            break;
        }

        // If it's the last key or non-smooth animation,
        // then just set the key value.
        if(Key == (m_NumRotateKeys-1) || Smooth == FALSE) {
          Quat = m_RotateKeys[Key].Quaternion;
        } else {
          // Calculate the time difference and 
          // interpolate time.
          TimeDiff = m_RotateKeys[Key+1].Time - m_RotateKeys[Key].Time;
          IntTime  = Time - m_RotateKeys[Key].Time;

          // Get the quaternion value
          D3DXQuaternionSlerp(&Quat, &m_RotateKeys[Key].Quaternion, &m_RotateKeys[Key+1].Quaternion, ((float)IntTime / (float)TimeDiff));
        }

        // Combine with the new matrix
        D3DXMatrixRotationQuaternion(&MatTemp, &Quat);
        D3DXMatrixMultiply(&Matrix, &Matrix, &MatTemp);
      }

      if(m_NumScaleKeys && m_ScaleKeys != NULL) {
        // Find the key that fits this time
        Key = 0;
        for(i=0;i<m_NumScaleKeys;i++) {
          if(m_ScaleKeys[i].Time <= Time)
            Key = i;
          else
            break;
        }

        // If it's the last key or non-smooth animation,
        // then just set the key value.
        if(Key == (m_NumScaleKeys-1) || Smooth == FALSE) {
          Vector = m_ScaleKeys[Key].Scale;
        } else {
          // Calculate the time difference and 
          // interpolate time.
          IntTime  = Time - m_ScaleKeys[Key].Time;

          // Get the interpolated vector value
          Vector = m_ScaleKeys[Key].Scale + m_ScaleKeys[Key].ScaleInterpolation * (float)IntTime;
        }

        // Combine with the new matrix
        D3DXMatrixScaling(&MatTemp, Vector.x, Vector.y, Vector.z);
        D3DXMatrixMultiply(&Matrix, &Matrix, &MatTemp);
      }

      if(m_NumPositionKeys && m_PositionKeys != NULL) {
        // Find the key that fits this time
        Key = 0;
        for(i=0;i<m_NumPositionKeys;i++) {
          if(m_PositionKeys[i].Time <= Time)
            Key = i;
          else
            break;
        }

        // If it's the last key or non-smooth animation,
        // then just set the key value.
        if(Key == (m_NumPositionKeys-1) || Smooth == FALSE) {
          Vector = m_PositionKeys[Key].Pos;
        } else {
          // Calculate the time difference and 
          // interpolate time.
          IntTime  = Time - m_PositionKeys[Key].Time;

          // Get the interpolated vector value
          Vector = m_PositionKeys[Key].Pos + m_PositionKeys[Key].PosInterpolation * (float)IntTime;
        }

        // Combine with the new matrix
        D3DXMatrixTranslation(&MatTemp, Vector.x, Vector.y, Vector.z);
        D3DXMatrixMultiply(&Matrix, &Matrix, &MatTemp);
      }

      // Set the new matrix
      m_Frame->m_matTransformed = Matrix;
    }

    // update matrix keys
    if(m_NumMatrixKeys && m_MatrixKeys != NULL) {
      // Find the key that fits this time
      Key = 0;
      for(i=0;i<m_NumMatrixKeys;i++) {
        if(m_MatrixKeys[i].Time <= Time)
          Key = i;
        else
          break;
      }

      // If it's the last key or non-smooth animation,
      // then just set the matrix.
      if(Key == (m_NumMatrixKeys-1) || Smooth == FALSE) {
        m_Frame->m_matTransformed = m_MatrixKeys[Key].Matrix;
      } else {
        // Calculate the time difference and 
        // interpolate time.
        IntTime  = Time - m_MatrixKeys[Key].Time;

        // Set the new interpolation matrix
        Matrix = m_MatrixKeys[Key].MatInterpolation * (float)IntTime;
        m_Frame->m_matTransformed = Matrix + m_MatrixKeys[Key].Matrix;
      }
    }
  }
} sAnimation;

typedef struct sAnimationSet
{
  char          *m_Name;
  sAnimation    *m_Animation;

  unsigned long  m_Length;

  sAnimationSet *m_Next;

  sAnimationSet()
  {
    m_Name      = NULL;
    m_Animation = NULL;
    m_Length    = 0;
    m_Next      = NULL;
  }

  ~sAnimationSet()
  {
    delete [] m_Name;    m_Name = NULL;
    delete m_Animation;  m_Animation = NULL; 
    delete m_Next;       m_Next = NULL;
  }

  sAnimationSet *FindSet(char *Name)
  {
    sAnimationSet *AnimSet;

    // return first instance if name=NULL
    if(Name == NULL)
      return this;

    // Compare names and return if exact match
    if(m_Name != NULL && !strcmp(Name, m_Name))
      return this;

    // Search next in list
    if(m_Next != NULL) {
      if((AnimSet = m_Next->FindSet(Name)) != NULL)
        return AnimSet;
    }

    return NULL;
  }

  void Update(DWORD Time, BOOL Smooth)
  {
    sAnimation *Anim;

    Anim = m_Animation;
    while(Anim != NULL) {
      if(!m_Length)
        Anim->Update(0, FALSE);
      else
      if(Time >= m_Length && Anim->m_Loop == FALSE)
        Anim->Update(Time, FALSE);
      else
        Anim->Update((Time % m_Length), Smooth);

      Anim = Anim->m_Next;
    }
  }

} sAnimationSet;

class cAnimation
{
  protected:
    long           m_NumAnimations;
    sAnimationSet *m_AnimationSet;

    void ParseXFileData(IDirectXFileData *DataObj, sAnimationSet *ParentAnim, sAnimation *CurrentAnim);

  public:
    cAnimation();
    ~cAnimation();

    BOOL           IsLoaded();

    long           GetNumAnimations();
    sAnimationSet *GetAnimationSet(char *Name = NULL);
    unsigned long  GetLength(char *Name = NULL);

    BOOL Load(char *Filename, cMesh *MapMesh = NULL);
    BOOL Free();

    BOOL MapToMesh(cMesh *Mesh);

    BOOL SetLoop(BOOL ToLoop, char *Name = NULL);
};

class cVertexBuffer
{
  private:
    cGraphics              *m_Graphics;
    IDirect3DVertexBuffer9 *m_pVB;

    DWORD                   m_NumVertices;
    DWORD                   m_VertexSize;
    DWORD                   m_FVF;

    BOOL                    m_Locked;
    char                   *m_Ptr;

  public:
    cVertexBuffer();
    ~cVertexBuffer();

    IDirect3DVertexBuffer9 *GetVertexBufferCOM();
    unsigned long           GetVertexSize();
    unsigned long           GetVertexFVF();
    unsigned long           GetNumVertices();

    BOOL Create(cGraphics *Graphics, unsigned long NumVertices, DWORD Descriptor, long VertexSize);
    BOOL Free();

    BOOL IsLoaded();

    BOOL Set(unsigned long FirstVertex, unsigned long NumVertices, void *VertexList);
    BOOL Render(unsigned long FirstVertex, unsigned long NumPrimitives, DWORD Type);

    BOOL  Lock(unsigned long FirstVertex = 0, unsigned long NumVertices = 0);
    BOOL  Unlock();
    void *GetPtr();
};

#endif
