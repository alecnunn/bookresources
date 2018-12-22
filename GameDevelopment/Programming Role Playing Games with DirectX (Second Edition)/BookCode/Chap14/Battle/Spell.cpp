#include "Core_Global.h"
#include "Frustum.h"

#include "MCL.h"
#include "MSL.h"

#include "Chars.h"
#include "Spell.h"

cSpellController::cSpellController()
{
  m_Graphics       = NULL;  // Clear cGraphics pointer
  m_Frustum        = NULL;  // Clear frustum pointer

  m_Chars          = NULL;  // Clear character controller
  
  m_SpellParent    = NULL;

  m_NumMeshes      = 0;     // Clear mesh data
  m_Meshes         = NULL;
  m_TexturePath[0] = 0;
}

cSpellController::~cSpellController()
{
  Shutdown();
}

BOOL cSpellController::Init(cGraphics *Graphics,              \
          char *DefinitionFile,                               \
          long NumSpellMeshes, char **MeshNames,              \
          char *TexturePath,                                  \
          cCharacterController *Controller)
{
  FILE *fp;
  long i;

  Free();  // Free prior init

  // Get parent graphics object
  if((m_Graphics = Graphics) == NULL || MeshNames == NULL ||  \
     DefinitionFile == NULL)
    return FALSE;

  // Load the spells
  if((fp=fopen(DefinitionFile, "rb"))==NULL)
    return FALSE;
  for(i=0;i<NUM_SPELL_DEFINITIONS;i++)
    fread(&m_Spells[i], 1, sizeof(sSpell), fp);
  fclose(fp);

  m_Chars = Controller;  // Store controller pointer

  // Copy over texture name
  if(TexturePath != NULL)
    strcpy(m_TexturePath, TexturePath);

  // Get mesh names
  if((m_NumMeshes = NumSpellMeshes)) {
    m_Meshes = new sSpellMeshList[NumSpellMeshes]();
    for(i=0;i<m_NumMeshes;i++)
      strcpy(m_Meshes[i].Filename, MeshNames[i]);
  }

  return TRUE;
}

BOOL cSpellController::Shutdown()
{
  Free();  // Free class data

  m_Graphics = NULL;

  // Release mesh list  
  delete [] m_Meshes;
  m_Meshes = NULL;
  m_NumMeshes = 0;

  return TRUE;
}

BOOL cSpellController::Free()
{
  long i;

  // Release spell list
  delete m_SpellParent;
  m_SpellParent = NULL;

  // Release spell meshes
  if(m_NumMeshes && m_Meshes != NULL) {
    for(i=0;i<m_NumMeshes;i++) {
      m_Meshes[i].Animation.Free();
      m_Meshes[i].Mesh.Free();
      m_Meshes[i].Count = 0;
    }
  }

  return TRUE;
}

sSpell *cSpellController::GetSpell(long SpellNum)
{
  return &m_Spells[SpellNum];
}

BOOL cSpellController::Add(long SpellNum,                     \
             sCharacter *Caster, long TargetType,             \
             float SourceX, float SourceY, float SourceZ,     \
             float TargetX, float TargetY, float TargetZ)
{
  sSpellTracker *SpellPtr;

  // Error checking
  if(m_Graphics == NULL || m_Meshes == NULL)
    return FALSE;

  // Make sure character is allowed to cast spell
  if(Caster != NULL) {
    if(!(Caster->Def.MagicSpells[SpellNum/32] &               \
                                      (1 << (SpellNum & 31))))
      return FALSE;

    // Make sure caster has enough MP to cast and reduce MP
    if(Caster != NULL) {
      // Return if not enough mana
      if(Caster->ManaPoints < m_Spells[SpellNum].Cost)
        return FALSE;
    }
  }

  // Allocate a new structure and link in to head of list
  SpellPtr = new sSpellTracker();
  if(m_SpellParent != NULL)
    m_SpellParent->Prev = SpellPtr;
  SpellPtr->Next = m_SpellParent;
  m_SpellParent = SpellPtr;

  // Set structure data
  SpellPtr->SpellNum = SpellNum;
  SpellPtr->Caster   = Caster;
  SpellPtr->Type     = TargetType;
  SpellPtr->SourceX  = SourceX;
  SpellPtr->SourceY  = SourceY;
  SpellPtr->SourceZ  = SourceZ;
  SpellPtr->TargetX  = TargetX;
  SpellPtr->TargetY  = TargetY;
  SpellPtr->TargetZ  = TargetZ;

  // Setup the mesh and movement data
  SetAnimData(SpellPtr, 0);

  return TRUE;
}

BOOL cSpellController::Update(long Elapsed)
{
  sSpellTracker *SpellPtr, *NextSpell;
  long SpellNum, Anim;
  BOOL GotoNextAnim;
  float Speed;

  if((SpellPtr = m_SpellParent) == NULL)
    return FALSE;

  // Scan through all spells in use
  while(SpellPtr != NULL) {

    // Remember next spell in list
    NextSpell = SpellPtr->Next;

    // Get local data
    SpellNum = SpellPtr->SpellNum;
    Anim     = SpellPtr->CurrentAnimation;

    // Update/move/countdown spell object
    GotoNextAnim = FALSE;
    switch(m_Spells[SpellNum].MeshPos[Anim]) {
      case POSITION_NONE:
        // Go to next animation
        GotoNextAnim = TRUE;
        break;

      case POSITION_CASTER:
      case POSITION_TARGET:
      case POSITION_SCALE:
        SpellPtr->Time -= Elapsed;
        if(SpellPtr->Time <= 0)
          GotoNextAnim = TRUE;
        break;

      case POSITION_TOTARGET:
      case POSITION_TOCASTER:
        Speed = (float)Elapsed / 1000.0f * SpellPtr->Speed;

        // Decrease distance amount
        SpellPtr->Distance -= Speed;
        if(SpellPtr->Distance <= 0.0f)
          GotoNextAnim = TRUE;
        else {
          SpellPtr->XPos += (SpellPtr->XAdd * Speed);
          SpellPtr->YPos += (SpellPtr->YAdd * Speed);
          SpellPtr->ZPos += (SpellPtr->ZAdd * Speed);
        }
        break;
    }

    // Update to next animation
    if(GotoNextAnim == TRUE)
      SetAnimData(SpellPtr, Anim+1);
   
    // Go to next spell
    SpellPtr = NextSpell;
  }

  return TRUE;
}

BOOL cSpellController::SetAnimData(sSpellTracker *SpellPtr,   \
                                  long Num)
{
  float XDiff, YDiff, ZDiff, Dist, Length;
  float XAngle = 0.0f, YAngle = 0.0f;
  float Scale = 1.0f;
  long  i;
  long  SpellNum, MeshNum;
  float SourceX, SourceY, SourceZ;
  float TargetX, TargetY, TargetZ;

  // Process spell effect if no more animations left
  // while storing the current animation number.
  if((SpellPtr->CurrentAnimation = Num) >= 3) {
    if(m_Chars != NULL)
      m_Chars->Spell(SpellPtr->Caster, SpellPtr, m_Spells);

    // Remove any mesh references
    for(i=0;i<3;i++) {
      if(m_Spells[SpellPtr->SpellNum].MeshPos[i] !=           \
                                             POSITION_NONE) {
        MeshNum = m_Spells[SpellPtr->SpellNum].MeshNum[i];

        // Decrease count and remove if needed
        m_Meshes[MeshNum].Count--;
        if(!m_Meshes[MeshNum].Count) {
          m_Meshes[MeshNum].Mesh.Free();
          m_Meshes[MeshNum].Animation.Free();
        }
      }
    }

    // Remove spell from list
    if(SpellPtr->Prev != NULL)
      SpellPtr->Prev->Next = SpellPtr->Next;
    else
      m_SpellParent = SpellPtr->Next;
    if(SpellPtr->Next != NULL)
      SpellPtr->Next->Prev = SpellPtr->Prev;
    SpellPtr->Prev = SpellPtr->Next = NULL;
    delete SpellPtr;

    return TRUE;
  }

  // Setup local data
  SourceX  = SpellPtr->SourceX;
  SourceY  = SpellPtr->SourceY;
  SourceZ  = SpellPtr->SourceZ;
  TargetX  = SpellPtr->TargetX;
  TargetY  = SpellPtr->TargetY;
  TargetZ  = SpellPtr->TargetZ;
  SpellNum = SpellPtr->SpellNum;

  // Go to next animation if no mesh to use
  if(m_Spells[SpellNum].MeshPos[Num] == POSITION_NONE)
    return SetAnimData(SpellPtr,Num+1);

  // Get mesh # to use
  MeshNum = m_Spells[SpellNum].MeshNum[Num];

  // Load mesh and animation if needed
  if(!m_Meshes[MeshNum].Count) {
    m_Meshes[MeshNum].Mesh.Load(m_Graphics,                   \
                   m_Meshes[MeshNum].Filename,                \
                   m_TexturePath);
    m_Meshes[MeshNum].Animation.Load(                         \
                   m_Meshes[MeshNum].Filename,                \
                   &m_Meshes[MeshNum].Mesh);

    // Set animation loop
    m_Meshes[MeshNum].Animation.SetLoop(                      \
             m_Spells[SpellNum].MeshLoop[Num], "Anim");
  }

  // Configure graphics object
  SpellPtr->Object.Create(m_Graphics,&m_Meshes[MeshNum].Mesh);
  m_Meshes[MeshNum].Count++;

  // Setup mesh movements
  switch(m_Spells[SpellNum].MeshPos[Num]) {
    case POSITION_CASTER:
      SpellPtr->XPos = SourceX;
      SpellPtr->YPos = SourceY;
      SpellPtr->ZPos = SourceZ;
      SpellPtr->Time = (long)m_Spells[SpellNum].MeshSpeed[Num];

      if(SpellPtr->Caster != NULL)
        YAngle = SpellPtr->Caster->Direction;
      break;

    case POSITION_TOTARGET:
      // Store position and speed info
      SpellPtr->XPos  = SourceX;
      SpellPtr->YPos  = SourceY;
      SpellPtr->ZPos  = SourceZ;
      SpellPtr->Speed = m_Spells[SpellNum].MeshSpeed[Num];

      // Calculate movement
      XDiff = (float)fabs(TargetX - SourceX);
      YDiff = (float)fabs(TargetY - SourceY);
      ZDiff = (float)fabs(TargetZ - SourceZ);
      Dist  = (float)sqrt(XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff);

      if((SpellPtr->Distance = Dist) != 0.0f) {
        SpellPtr->XAdd = (TargetX - SourceX) / Dist;
        SpellPtr->YAdd = (TargetY - SourceY) / Dist;
        SpellPtr->ZAdd = (TargetZ - SourceZ) / Dist;

        // Calculate angles
        XAngle = -(float)atan(SpellPtr->YAdd);
        YAngle =  (float)atan2(SpellPtr->XAdd, SpellPtr->ZAdd);
      }
      break;

    case POSITION_TARGET:
      SpellPtr->XPos = TargetX;
      SpellPtr->YPos = TargetY;
      SpellPtr->ZPos = TargetZ;
      SpellPtr->Time = (long)m_Spells[SpellNum].MeshSpeed[Num];

      // Calculate distance from source to target
      XDiff = (float)fabs(TargetX - SourceX);
      ZDiff = (float)fabs(TargetZ - SourceZ);
      Dist  = (float)sqrt(XDiff*XDiff+ZDiff*ZDiff);

      SpellPtr->XAdd = (TargetX - SourceX) / Dist;
      SpellPtr->ZAdd = (TargetZ - SourceZ) / Dist;

      // Calculate angle
      YAngle =  (float)atan2(SpellPtr->XAdd, SpellPtr->ZAdd);
      break;

    case POSITION_TOCASTER:
      // Store position and speed info
      SpellPtr->XPos  = TargetX;
      SpellPtr->YPos  = TargetY;
      SpellPtr->ZPos  = TargetZ;
      SpellPtr->Speed = m_Spells[SpellNum].MeshSpeed[Num];

      // Calculate movement
      XDiff = (float)fabs(SourceX - TargetX);
      YDiff = (float)fabs(SourceY - TargetY);
      ZDiff = (float)fabs(SourceZ - TargetZ);
      Dist  = (float)sqrt(XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff);

      if((SpellPtr->Distance = Dist) != 0.0f) {
        SpellPtr->XAdd = (SourceX - TargetX) / Dist;
        SpellPtr->YAdd = (SourceY - TargetY) / Dist;
        SpellPtr->ZAdd = (SourceZ - TargetZ) / Dist;

        // Calculate angles
        XAngle = -(float)atan(SpellPtr->YAdd);
        YAngle =  (float)atan2(SpellPtr->XAdd, SpellPtr->ZAdd);
      }
      break;

    case POSITION_SCALE:
      // Store position and speed info
      SpellPtr->XPos  = SourceX;
      SpellPtr->YPos  = SourceY;
      SpellPtr->ZPos  = SourceZ;
      SpellPtr->Time = (long)m_Spells[SpellNum].MeshSpeed[Num];

      // Get distance from source to target and size of mesh
      XDiff = (float)fabs(TargetX - SourceX);
      YDiff = (float)fabs(TargetY - SourceY);
      ZDiff = (float)fabs(TargetZ - SourceZ);
      Dist  = (float)sqrt(XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff);
      SpellPtr->Object.GetBounds(NULL,NULL,NULL,              \
                                 NULL,NULL,&Length,NULL);

      // Calculate scale
      Scale = Dist / Length;
      
      // Calculate angles
      SpellPtr->XAdd = (TargetX - SourceX) / Dist;
      SpellPtr->YAdd = (TargetY - SourceY) / Dist;
      SpellPtr->ZAdd = (TargetZ - SourceZ) / Dist;
      XAngle = -(float)atan(SpellPtr->YAdd);
      YAngle =  (float)atan2(SpellPtr->XAdd, SpellPtr->ZAdd);

      break;
  }

  // Rotate object to points towards target
  SpellPtr->Object.Rotate(XAngle, YAngle, 0.0f);

  // Scale object
  SpellPtr->Object.Scale(1.0f, 1.0f, Scale);

  // Set the animation
  SpellPtr->Object.SetAnimation(                              \
      &m_Meshes[MeshNum].Animation, "Anim", timeGetTime()/30);

  // Play the sound
  if(m_Spells[SpellNum].MeshSound[Num] != -1)
    SpellSound(m_Spells[SpellNum].MeshSound[Num]);

  return TRUE;
}

BOOL cSpellController::Render(cFrustum *Frustum,              \
                              float ZDistance)
{
  cFrustum    ViewFrustum;  // Local viewing frustum
  float       Radius;       // Bounding radius
  sSpellTracker *SpellPtr;
  DWORD       Time;

  // Error checking
  if(m_Graphics == NULL)
    return FALSE;

  // Return success if no character to draw
  if((SpellPtr = m_SpellParent) == NULL)
    return TRUE;

  // Construct the viewing frustum (if none passed)
  if((m_Frustum = Frustum) == NULL) {
    ViewFrustum.Construct(m_Graphics, ZDistance);
    m_Frustum = &ViewFrustum;
  }

  // Get time to update animations (30fps)
  Time = timeGetTime() / 30;

  // Loop through each spell and draw
  while(SpellPtr != NULL) {
    // Draw spell if in viewing frustum
    SpellPtr->Object.GetBounds(NULL,NULL,NULL,NULL,           \
                              NULL,NULL,&Radius);

    if(m_Frustum->CheckSphere(SpellPtr->XPos,                 \
                              SpellPtr->YPos,                 \
                              SpellPtr->ZPos,                 \
                              Radius) == TRUE) {

      // Position object
      SpellPtr->Object.Move(SpellPtr->XPos,                   \
                            SpellPtr->YPos,                   \
                            SpellPtr->ZPos);

      SpellPtr->Object.UpdateAnimation(Time, TRUE);
      SpellPtr->Object.Render();
    }

    // Go to next spell
    SpellPtr = SpellPtr->Next;
  }

  return TRUE;
}
