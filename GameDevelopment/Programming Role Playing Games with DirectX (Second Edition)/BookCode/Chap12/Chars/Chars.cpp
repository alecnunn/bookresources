#include "Core_Global.h"
#include "Frustum.h"

#include "MCL.h"
#include "MIL.h"
#include "MSL.h"

#include "Chars.h"
#include "Spell.h"

cCharacterController::cCharacterController()
{
  m_Graphics          = NULL;  // Clear cGraphics pointer
  m_Font              = NULL;  // Clear cFont pointer
  m_Frustum           = NULL;  // Clear frustum pointer

  m_MIL               = NULL;  // Clear MIL pointer
  m_MSL               = NULL;  // Clear MSL pointer

  m_CharacterParent   = NULL;  // Clear character list
  m_NumCharacters     = 0;

  m_DefinitionFile[0] = 0;     // Clear definition filename

  m_NumMeshes         = 0;     // Clear mesh data
  m_Meshes            = NULL;
  m_TexturePath[0]    = 0;

  m_NumAnimations     = 0;     // Clear animation data
  m_Animations        = NULL;

  m_SpellController   = NULL;  // Clear spell controller
}

cCharacterController::~cCharacterController()
{
  Shutdown();
}

BOOL cCharacterController::Init(                              \
                          cGraphics *Graphics,                \
                          cFont *Font, char *DefinitionFile,  \
                          sItem *MIL, sSpell *MSL,            \
                          long NumCharacterMeshes,            \
                          char **MeshNames,                   \
                          char *MeshPath, char *TexturePath,  \
                          long NumAnimations,                 \
                          sCharAnimationInfo *Anims,          \
                          cSpellController *SpellController)
{
  long i;

  Free();  // Free prior init

  // Get parent graphics object and error checking
  if((m_Graphics = Graphics) == NULL || MeshNames == NULL ||  \
     DefinitionFile == NULL)
    return FALSE;

  // Get font object pointer
  m_Font = Font;

  // Copy definition file name
  strcpy(m_DefinitionFile, DefinitionFile);

  // Store MIL and MSL pointer
  m_MIL = MIL;
  m_MSL = MSL;

  // Copy over mesh path (or set default)
  if(MeshPath != NULL)
    strcpy(m_MeshPath, MeshPath);
  else
    strcpy(m_MeshPath, ".\\");

  // Copy over texture path (or set default)
  if(TexturePath != NULL)
    strcpy(m_TexturePath, TexturePath);
  else
    strcpy(m_TexturePath, ".\\");

  // Get mesh names
  if((m_NumMeshes = NumCharacterMeshes)) {
    m_Meshes = new sCharacterMeshList[NumCharacterMeshes]();
    for(i=0;i<m_NumMeshes;i++)
      strcpy(m_Meshes[i].Filename, MeshNames[i]);
  }

  // Get animation data
  if((m_NumAnimations = NumAnimations)) {
    m_Animations = new sCharAnimationInfo[m_NumAnimations]();
    for(i=0;i<m_NumAnimations;i++) {
      memcpy(&m_Animations[i], &Anims[i],                     \
             sizeof(sCharAnimationInfo));
    }
  }

  // Store spell controller pointer
  m_SpellController = SpellController;

  return TRUE;
}

BOOL cCharacterController::Shutdown()
{
  Free();

  // Release mesh list
  delete [] m_Meshes;
  m_Meshes = NULL;
  m_NumMeshes = 0;

  // Release animation data
  m_NumAnimations = 0;
  delete [] m_Animations;
  m_Animations = NULL;

  // Clear graphics object
  m_Graphics = NULL;

  // Clear paths
  m_DefinitionFile[0] = 0;
  m_MeshPath[0] = 0;
  m_TexturePath[0] = 0;

  // Clear spell controller
  m_SpellController = NULL;

  return TRUE;
}

BOOL cCharacterController::Free()
{
  // Release character structures
  delete m_CharacterParent;
  m_CharacterParent = NULL;
  m_NumCharacters = 0;

  return TRUE;
}

BOOL cCharacterController::Add(                               \
             long IDNum, long Definition,                     \
             long Type, long AI,                              \
             float XPos, float YPos, float ZPos,              \
             float Direction)
{
  sCharacter *CharPtr;
  FILE *fp;
  char Path[MAX_PATH];
  long i;

  // Error checking
  if(m_Graphics==NULL || m_Meshes==NULL || !m_DefinitionFile[0])
    return FALSE;

  // Allocate a new structure
  CharPtr = new sCharacter();

  // Assign data
  CharPtr->Definition = Definition;
  CharPtr->ID         = IDNum;
  CharPtr->Type       = Type;
  CharPtr->AI         = AI;
  CharPtr->XPos       = XPos;
  CharPtr->YPos       = YPos;
  CharPtr->ZPos       = ZPos;
  CharPtr->Direction  = Direction;
  CharPtr->Enabled    = TRUE;

  // Set a random charge amount
  CharPtr->Charge = (float)(rand() % 101);

  // Load character definition
  if((fp=fopen(m_DefinitionFile, "rb"))==NULL) {
    delete CharPtr;
    return FALSE;
  }
  fseek(fp, sizeof(sCharacterDefinition)*Definition, SEEK_SET);
  fread(&CharPtr->Def, 1, sizeof(sCharacterDefinition), fp);
  fclose(fp);

  // Load character ICS
  if(CharPtr->Def.ItemFilename[0]) {
    CharPtr->CharICS = new cCharICS();
    CharPtr->CharICS->Load(CharPtr->Def.ItemFilename);
  }

  // Set character stats
  CharPtr->HealthPoints = CharPtr->Def.HealthPoints;
  CharPtr->ManaPoints   = CharPtr->Def.ManaPoints;

  // Load mesh and animation if needed
  if(!m_Meshes[CharPtr->Def.MeshNum].Count) {
    m_Meshes[CharPtr->Def.MeshNum].Mesh.Load(m_Graphics,      \
                   m_Meshes[CharPtr->Def.MeshNum].Filename,   \
                   m_TexturePath);
    m_Meshes[CharPtr->Def.MeshNum].Animation.Load(            \
                   m_Meshes[CharPtr->Def.MeshNum].Filename,   \
                   &m_Meshes[CharPtr->Def.MeshNum].Mesh);

    // Set animation loops
    if(m_NumAnimations) {
      for(i=0;i<m_NumAnimations;i++)
        m_Meshes[CharPtr->Def.MeshNum].Animation.SetLoop(     \
                                      m_Animations[i].Loop,   \
                                      m_Animations[i].Name);
    }
  }

  // Configure graphics object
  CharPtr->Object.Create(m_Graphics,                          \
                        &m_Meshes[CharPtr->Def.MeshNum].Mesh);
  m_Meshes[CharPtr->Def.MeshNum].Count++;

  // Load and configure weapon (if any)
  if(m_MIL != NULL && CharPtr->Def.Weapon != -1 &&            \
     m_MIL[CharPtr->Def.Weapon].MeshFilename[0]) {

    // Build the mesh path
    sprintf(Path, "%s%s", m_MeshPath,                         \
            m_MIL[CharPtr->Def.Weapon].MeshFilename);

    // Load the weapon mesh
    CharPtr->WeaponMesh.Load(m_Graphics, Path, m_TexturePath);

    // Create the weapon object
    CharPtr->WeaponObject.Create(m_Graphics,                  \
                                 &CharPtr->WeaponMesh);

    // Orient and attach the weapon
    CharPtr->WeaponObject.Move(0.0f, 0.0f, 0.0f);
    CharPtr->WeaponObject.Rotate(0.0f, 0.0f, 0.0f);
    CharPtr->WeaponObject.AttachToObject(&CharPtr->Object,    \
                                         "WeaponHand");
  }

  // Link in to head of list
  if(m_CharacterParent != NULL)
    m_CharacterParent->Prev = CharPtr;
  CharPtr->Next = m_CharacterParent;
  m_CharacterParent = CharPtr;

  return TRUE;
}

BOOL cCharacterController::Remove(long IDNum)
{
  return Remove(GetCharacter(IDNum));
}
 
BOOL cCharacterController::Remove(sCharacter *Character)
{
  // Error checking
  if(Character == NULL)
    return FALSE;

  // Decrease mesh count and release if no more
  m_Meshes[Character->Def.MeshNum].Count--;
  if(!m_Meshes[Character->Def.MeshNum].Count) {
    m_Meshes[Character->Def.MeshNum].Mesh.Free();
    m_Meshes[Character->Def.MeshNum].Animation.Free();
  }

  // Remove from list and free resource
  if(Character->Prev != NULL)
    Character->Prev->Next = Character->Next;
  else
    m_CharacterParent = Character->Next;

  if(Character->Next != NULL)
    Character->Next->Prev = Character->Prev;

  if(Character->Prev == NULL && Character->Next == NULL)
    m_CharacterParent = NULL;

  Character->Prev = Character->Next = NULL;
  delete Character;

  return TRUE;
}

BOOL cCharacterController::Save(long IDNum, char *Filename)
{
  char ICSFilename[MAX_PATH];
  sCharacter *CharPtr;
  FILE *fp;

  // Get pointer to character in list
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Open file
  if((fp=fopen(Filename, "wb"))==NULL)
    return FALSE;

  // Output character data
  fwrite(&CharPtr->Def, 1, sizeof(sCharacterDefinition), fp);
  fwrite(&CharPtr->HealthPoints, 1, sizeof(long), fp);
  fwrite(&CharPtr->ManaPoints, 1, sizeof(long), fp);
  fwrite(&CharPtr->Ailments, 1, sizeof(long), fp);
  fwrite(&CharPtr->XPos, 1, sizeof(float), fp);
  fwrite(&CharPtr->YPos, 1, sizeof(float), fp);
  fwrite(&CharPtr->ZPos, 1, sizeof(float), fp);
  fwrite(&CharPtr->Direction, 1, sizeof(float), fp);

  // Close file
  fclose(fp);

  // Save inventory
  if(CharPtr->CharICS != NULL) {
    sprintf(ICSFilename, "ICS%s", Filename);
    CharPtr->CharICS->Save(ICSFilename);
  }

  return TRUE;
}

BOOL cCharacterController::Load(long IDNum, char *Filename)
{
  char ICSFilename[MAX_PATH];
  sCharacter *CharPtr;
  FILE *fp;

  // Get pointer to character in list
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Open file
  if((fp=fopen(Filename, "rb"))==NULL)
    return FALSE;

  // Read in character data
  fread(&CharPtr->Def, 1, sizeof(sCharacterDefinition), fp);
  fread(&CharPtr->HealthPoints, 1, sizeof(long), fp);
  fread(&CharPtr->ManaPoints, 1, sizeof(long), fp);
  fread(&CharPtr->Ailments, 1, sizeof(long), fp);
  fread(&CharPtr->XPos, 1, sizeof(float), fp);
  fread(&CharPtr->YPos, 1, sizeof(float), fp);
  fread(&CharPtr->ZPos, 1, sizeof(float), fp);
  fread(&CharPtr->Direction, 1, sizeof(float), fp);

  // Close file
  fclose(fp);

  // Load inventory
  if(CharPtr->CharICS != NULL) {
    sprintf(ICSFilename, "ICS%s", Filename);
    CharPtr->CharICS->Load(ICSFilename);
  }

  return TRUE;
}

BOOL cCharacterController::Update(long Elapsed)
{
  sCharacter *CharPtr, *NextChar;
  float XMove, YMove, ZMove;
  static long EffectCounter = 0;
  BOOL ToProcess, DeadChar;

  // Return success if no characters to update
  if((CharPtr = m_CharacterParent) == NULL)
    return TRUE;

  // Update effect counter
  EffectCounter += Elapsed;

  // Loop through all characters
  while(CharPtr != NULL) {

    // Remember next character
    NextChar = CharPtr->Next;

    // Only update if enabled, not asleep or paralyzed
    if(CharPtr->Enabled == TRUE) {

      // Update action timer if in use
      if(CharPtr->ActionTimer != 0) {
        CharPtr->ActionTimer -= Elapsed;
        if(CharPtr->ActionTimer < 0)
          CharPtr->ActionTimer = 0;
      }

      // Update text message timer
      if(CharPtr->MessageTimer > 0)
        CharPtr->MessageTimer -= Elapsed;

      // Reset charge counter if attacking, spell, or item
      if(CharPtr->Action == CHAR_ATTACK ||                    \
         CharPtr->Action == CHAR_SPELL  ||                    \
         CharPtr->Action == CHAR_ITEM)
        CharPtr->Charge = 0.0f;

      // Kill character if no health left
      if(CharPtr->HealthPoints <= 0 &&                        \
         CharPtr->Action != CHAR_DIE) {
        SetAction(CharPtr, CHAR_DIE, 2000);
      }

      // Mark that processing can continue later on
      ToProcess = TRUE;

      // Mark character as still alive
      DeadChar = FALSE;

      // Don't allow an update if asleep or paralyzed
      if((CharPtr->Ailments & AILMENT_SLEEP) ||               \
         (CharPtr->Ailments & AILMENT_PARALYZE))
        ToProcess = FALSE;
       
      // Process non-idle, non-walk actions
      if(CharPtr->Action != CHAR_IDLE &&                      \
         CharPtr->Action != CHAR_MOVE &&                      \
         !CharPtr->ActionTimer) {
         
        switch(CharPtr->Action) {
          case CHAR_ATTACK:
            // Process attack
            if(ToProcess == TRUE)
              Attack(CharPtr, CharPtr->Victim);
            break;

          case CHAR_SPELL:
            // Manually cast a spell
            if(m_SpellController != NULL && m_MSL != NULL &&  \
               ToProcess == TRUE) {
              m_SpellController->Add(CharPtr->SpellNum,       \
                           CharPtr, CharPtr->SpellTarget,     \
                           CharPtr->XPos,                     \
                           CharPtr->YPos,                     \
                           CharPtr->ZPos,                     \
                           CharPtr->TargetX,                  \
                           CharPtr->TargetY,                  \
                           CharPtr->TargetZ);
            }
            break;

          case CHAR_ITEM:
            if(ToProcess == TRUE)
              Item(CharPtr, CharPtr,                          \
                   CharPtr->ItemNum, CharPtr->CharItem);
            break;

          case CHAR_DIE:
            Death(CharPtr->Attacker, CharPtr);
            DeadChar = TRUE;   // Mark character as dead
            ToProcess = FALSE; // Don't allow updates
            break;
        }
      }

      // Clear movement
      XMove = YMove = ZMove = 0.0f;

      // Only continue if allowed (in case character died)
      if(ToProcess == TRUE) {

        // Only allow updates if lock/timer not in use 
        if(CharPtr->Enabled == TRUE &&                        \
           !CharPtr->ActionTimer &&                           \
           CharPtr->Locked == FALSE) {

          // Reset action
          CharPtr->Action = CHAR_IDLE;

          // Get movement
          if(CharPtr->Type == CHAR_PC)
            PCUpdate(CharPtr, Elapsed, &XMove, &YMove, &ZMove);
          else
            CharUpdate(CharPtr, Elapsed, &XMove,&YMove,&ZMove);

          // Check for validity of movement (clear if invalid)
          if(CheckMove(CharPtr,&XMove,&YMove,&ZMove)==FALSE) {
            XMove = YMove = ZMove = 0.0f;
            CharPtr->Action = CHAR_IDLE;
          }
        }

        // Process movement of character
        ProcessUpdate(CharPtr, XMove, YMove, ZMove);

        // Increase action charge of character
        CharPtr->Charge += ((float)Elapsed / 1000.0f *        \
                                   GetCharge(CharPtr));
        if(CharPtr->Charge > 100.0f)
          CharPtr->Charge = 100.0f;
      }

      // Process timed ailments (only on live characters)
      if(DeadChar == FALSE && CharPtr->Ailments) {

        // Sleeping characters have 4% to wake up
        if(CharPtr->Ailments & AILMENT_SLEEP && (rand()%100)<4)
          CharPtr->Ailments &= ~AILMENT_SLEEP;

        // Paralyzed character have 2% chance to recover
        if(CharPtr->Ailments & AILMENT_PARALYZE &&            \
                                           (rand() % 100) < 2)
            CharPtr->Ailments &= ~AILMENT_PARALYZE;

        // Posion removes 2 hp every 2 seconds
        if(CharPtr->Ailments & AILMENT_POISON &&              \
                              EffectCounter >= 4000) {
          CharPtr->HealthPoints -= 2;
          SetMessage(CharPtr, "Poison -2 HP", 500,            \
                  D3DCOLOR_RGBA(0,255,64,255));
        }
      }
    }

    // Go to next character
    CharPtr = NextChar;
  }

  // Reset effect counter (after 4 seconds)
  if(EffectCounter >= 4000) 
    EffectCounter = 0;

  return TRUE;
}

BOOL cCharacterController::Render(                            \
               long       Elapsed,                            \
               cFrustum  *Frustum,                            \
               float      ZDistance)
{
  cFrustum     ViewFrustum;  // Local viewing frustum
  float        Radius;       // Bounding radius
  sCharacter  *CharPtr; 
  DWORD        Time;

  // Variables for printing messages
  BOOL         GotMatrices = FALSE;
  D3DXMATRIX   matWorld, matView, matProj;
  D3DXVECTOR3  vecPos;
  D3DVIEWPORT9 vpScreen;
  float        MaxY;

  // Error checking
  if(m_Graphics == NULL)
    return FALSE;

  // Return success if no character to draw
  if((CharPtr = m_CharacterParent) == NULL)
    return TRUE;

  // Construct the viewing frustum (if none passed)
  if((m_Frustum = Frustum) == NULL) {
    ViewFrustum.Construct(m_Graphics, ZDistance);
    m_Frustum = &ViewFrustum;
  }

  // Get time to update animations (30fps) if
  // elapsed value passed == -1
  if(Elapsed == -1)
    Time = timeGetTime() / 30;
 
  // Loop through each character and draw
  while(CharPtr != NULL) {
    // Update animation based on elapsed time passed
    if(Elapsed != -1) {
      CharPtr->LastAnimTime += (Elapsed/30);
      Time = CharPtr->LastAnimTime;
    }

    CharPtr->Object.GetBounds(NULL,NULL,NULL,                 \
                              NULL,&MaxY,NULL,&Radius);

    // Draw character if in viewing frustum
    if(m_Frustum->CheckSphere(CharPtr->Object.GetXPos(),      \
                              CharPtr->Object.GetYPos(),      \
                              CharPtr->Object.GetZPos(),      \
                              Radius) == TRUE) {
      CharPtr->Object.UpdateAnimation(Time, TRUE);
      CharPtr->Object.Render();

      // Draw character's weapon
      if(CharPtr->Def.Weapon != -1)
        CharPtr->WeaponObject.Render();

      // Draw message if needed
      if(CharPtr->MessageTimer > 0) {

        // Get the matrices and viewport if not done already
        if(GotMatrices == FALSE) {
          GotMatrices = TRUE;

          // Get the world, projection, and view transformations
          D3DXMatrixIdentity(&matWorld);
          m_Graphics->GetDeviceCOM()->GetTransform(           \
                                  D3DTS_VIEW, &matView);
          m_Graphics->GetDeviceCOM()->GetTransform(           \
                                  D3DTS_PROJECTION, &matProj);

          // Get viewport
          m_Graphics->GetDeviceCOM()->GetViewport(&vpScreen);
        }

        // Project coordinates to screen  
        D3DXVec3Project(&vecPos,                              \
                  &D3DXVECTOR3(CharPtr->XPos,                 \
                               CharPtr->YPos+(MaxY*0.5f),     \
                               CharPtr->ZPos),                \
                  &vpScreen, &matProj, &matView, &matWorld);

        // Print message
        m_Font->Print(CharPtr->Message,                       \
                      (long)vecPos.x, (long)vecPos.y,         \
                      0, 0, CharPtr->MessageColor);
      }
    }

    // go to next character    
    CharPtr = CharPtr->Next;
  }

  return TRUE;
}

float cCharacterController::GetXZRadius(sCharacter *Character)
{
  float MinX, MaxX, MinZ, MaxZ;
  float x, z;

  // Error checking
  if(Character == NULL)
    return 0.0f;

  Character->Object.GetBounds(&MinX, NULL, &MinZ,             \
                              &MaxX, NULL, &MaxZ, NULL);
  x = (float)max(fabs(MinX), fabs(MaxX));
  z = (float)max(fabs(MinZ), fabs(MaxZ));

  return max(x, z);
}

///////////////////////////////////////////////////////////
// Set/Get Functions
///////////////////////////////////////////////////////////
sCharacter *cCharacterController::GetParentCharacter()
{
  return m_CharacterParent;
}

sCharacter *cCharacterController::GetCharacter(long IDNum)
{
  sCharacter *CharPtr;

  // Scan through all characters
  if((CharPtr = m_CharacterParent) != NULL) {
    while(CharPtr != NULL) {
    
      // Return character
      if(IDNum == CharPtr->ID)
        return CharPtr;
   
      // Go to next character
      CharPtr = CharPtr->Next;
    }
  }

  return NULL;
}

float cCharacterController::GetSpeed(sCharacter *Character)
{
  float Speed;
  
  // Error checking
  if(Character == NULL)
    return 0.0f;

  // Calculate adjusted speed
  Speed = Character->Def.Speed;

  if(Character->Ailments & AILMENT_SLOW)
    Speed *= 0.5f;
  if(Character->Ailments & AILMENT_FAST)
    Speed *= 1.5f;

  // Bounds check value
  if(Speed < 1.0f)
    Speed = 1.0f;

  return Speed;
}

long cCharacterController::GetAttack(sCharacter *Character)
{
  long Attack;
  
  // Error checking
  if(Character == NULL)
    return 0;

  // Calculate adjusted attack
  Attack = Character->Def.Attack;

  // Adjust attack based on item value (in %(Value/100)+1)
  if(Character->Def.Weapon != -1 && m_MIL != NULL) {
    Attack = (long)((float)Attack *                           \
             (((float)m_MIL[Character->Def.Weapon].Value /    \
             100.0f) + 1.0f));
  }

  if(Character->Ailments & AILMENT_WEAK)
    Attack = (long)((float)Attack * 0.5f);
  if(Character->Ailments & AILMENT_STRONG)
    Attack = (long)((float)Attack * 1.5f);

  return Attack;
}

long cCharacterController::GetDefense(sCharacter *Character)
{
  long Defense;
  
  // Error checking
  if(Character == NULL)
    return 0;

  // Calculate adjusted defense
  Defense = Character->Def.Defense;
  
  if(Character->Def.Armor != -1 && m_MIL != NULL)
    Defense = (long)((float)Defense *                         \
             (((float)m_MIL[Character->Def.Armor].Value /     \
             100.0f) + 1.0f));

  if(Character->Def.Shield != -1 && m_MIL != NULL)
    Defense = (long)((float)Defense *                         \
             (((float)m_MIL[Character->Def.Shield].Value /    \
             100.0f) + 1.0f));

  if(Character->Ailments & AILMENT_WEAK)
    Defense = (long)((float)Defense * 0.5f);
  if(Character->Ailments & AILMENT_STRONG)
    Defense = (long)((float)Defense * 1.5f);

  // Bounds check value
  if(Defense < 0)
    Defense = 0;

  return Defense;
}

long cCharacterController::GetAgility(sCharacter *Character)
{
  long Agility;
  
  // Error checking
  if(Character == NULL)
    return 0;

  // Calculate adjusted agility
  Agility = Character->Def.Agility;
  
  if(Character->Ailments & AILMENT_CLUMSY)
    Agility = (long)((float)Agility * 0.75f);
  if(Character->Ailments & AILMENT_SUREFOOTED)
    Agility = (long)((float)Agility * 1.5f);

  return Agility;
}

long cCharacterController::GetResistance(sCharacter *Character)
{
  long Resistance;
  
  // Error checking
  if(Character == NULL)
    return 0;

  // Calculate adjusted resistance
  Resistance = Character->Def.Resistance;
  
  if(Character->Ailments & AILMENT_ENCHANTED)
    Resistance = (long)((float)Resistance * 0.5f);
  if(Character->Ailments & AILMENT_BARRIER)
    Resistance = (long)((float)Resistance * 1.5f);

  return Resistance;
}

long cCharacterController::GetMental(sCharacter *Character)
{
  long Mental;
  
  // Error checking
  if(Character == NULL)
    return 0;

  // Calculate adjusted mental
  Mental = Character->Def.Mental;
  
  if(Character->Ailments & AILMENT_DUMBFOUNDED)
    Mental = (long)((float)Mental * 0.5f);

  return Mental;
}

long cCharacterController::GetToHit(sCharacter *Character)
{
  long ToHit;
  
  // Error checking
  if(Character == NULL)
    return 0;

  // Calculate adjusted to hit
  ToHit = Character->Def.ToHit;
  
  if(Character->Ailments & AILMENT_BLIND)
    ToHit = (long)((float)ToHit * 0.75f);
  if(Character->Ailments & AILMENT_HAWKEYE)
    ToHit = (long)((float)ToHit * 1.5f);

  return ToHit;
}

float cCharacterController::GetCharge(sCharacter *Character)
{
  float Charge;
  
  // Error checking
  if(Character == NULL)
    return 0;

  // Calculate adjusted charge
  Charge = Character->Def.ChargeRate;
  
  if(Character->Ailments & AILMENT_SLOW)
    Charge = Charge * 0.75f;
  if(Character->Ailments & AILMENT_FAST)
    Charge = Charge * 1.5f;

  return Charge;
}

cCharICS *cCharacterController::GetICS(long IDNum)
{
  sCharacter *CharPtr;

  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return NULL;
  return CharPtr->CharICS;
}

BOOL cCharacterController::SetLock(long IDNum, BOOL State)
{
  sCharacter *CharPtr;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Set new value
  CharPtr->Locked = State;
  return TRUE;
}

BOOL cCharacterController::SetActionTimer(long IDNum, long Timer)
{
  sCharacter *CharPtr;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Set new value
  CharPtr->ActionTimer = Timer;
  return TRUE;
}

BOOL cCharacterController::SetAction(sCharacter *Character,   \
                                     long Action,             \
                                     long AddTime)
{
  long MeshNum;

  // Error checking
  if(Character == NULL)
    return FALSE;

  // Make sure attack, spell, and item has supporting charge
  if(Action == CHAR_ATTACK || Action == CHAR_SPELL ||
     Action == CHAR_ITEM) {
    if(Character->Charge < 100.0f)
      return FALSE;
  }

  // Set action
  Character->Action = Action;

  // Play sound effect
  ActionSound(Character);

  // Get mesh number
  MeshNum = Character->Def.MeshNum;

  // Set action time (or set to 1 is addtime = -1)
  if(AddTime == -1)
    Character->ActionTimer = 1;
  else
    Character->ActionTimer = AddTime +                        \
                m_Meshes[MeshNum].Animation.GetLength(        \
                m_Animations[Action].Name) * 30;

  return TRUE;
}

BOOL cCharacterController::SetDistance(                       \
               long IDNum, float Distance)
{
  sCharacter *CharPtr;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Set new value
  CharPtr->Distance = Distance;
  return TRUE;
}

float cCharacterController::GetDistance(long IDNum)
{
  sCharacter *CharPtr;

  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return 0.0f;
  return CharPtr->Distance;
}

BOOL cCharacterController::SetRoute(long IDNum,               \
                                    long NumPoints,           \
                                    sRoutePoint *Route)
{
  sCharacter *CharPtr;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Free old route
  delete [] CharPtr->Route;
  CharPtr->Route = NULL;

  // Set new route
  if((CharPtr->NumPoints = NumPoints) != NULL) {
    CharPtr->Route = new sRoutePoint[NumPoints];
    memcpy(CharPtr->Route,Route,NumPoints*sizeof(sRoutePoint));
    CharPtr->CurrentPoint = 0;
  }

  return TRUE;
}

BOOL cCharacterController::SetScript(long IDNum,              \
                                     char *ScriptFilename)
{
  sCharacter *CharPtr;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Set new script
  strcpy(CharPtr->ScriptFilename, ScriptFilename);
  return TRUE;
}

char *cCharacterController::GetScript(long IDNum)
{
  sCharacter *CharPtr;

  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return NULL;
  return CharPtr->ScriptFilename;
}

BOOL cCharacterController::SetEnable(long IDNum, BOOL Enabled)
{
  sCharacter *CharPtr;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Set new value
  CharPtr->Enabled = Enabled;
  return TRUE;
}

BOOL cCharacterController::GetEnable(long IDNum)
{
  sCharacter *CharPtr;

  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;
  return CharPtr->Enabled;
}

BOOL cCharacterController::SetMessage(sCharacter *Character,  \
                                   char *Text, long Timer,    \
                                   D3DCOLOR Color)
{
  strcpy(Character->Message, Text);
  Character->MessageTimer = Timer;
  Character->MessageColor = Color;

  return TRUE;
}

BOOL cCharacterController::Move(                              \
               long IDNum,                                    \
               float XPos, float YPos, float ZPos)
{
  sCharacter *CharPtr;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Set new values
  CharPtr->XPos = XPos;
  CharPtr->YPos = YPos;
  CharPtr->ZPos = ZPos;
  return TRUE;
}

BOOL cCharacterController::GetPosition(                       \
               long IDNum,                                    \
               float *XPos, float *YPos, float *ZPos)
{
  sCharacter *CharPtr;

  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  if(XPos != NULL)
    *XPos = CharPtr->XPos;
  if(YPos != NULL)
    *YPos = CharPtr->YPos;
  if(ZPos != NULL)
    *ZPos = CharPtr->ZPos;

  return TRUE;
}

BOOL cCharacterController::SetBounds(                         \
               long IDNum,                                    \
               float MinX, float MinY, float MinZ,            \
               float MaxX, float MaxY, float MaxZ)
{
  sCharacter *CharPtr;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Set new values
  CharPtr->MinX = min(MinX, MaxX);
  CharPtr->MinY = min(MinY, MaxY);
  CharPtr->MinZ = min(MinZ, MaxZ);
  CharPtr->MaxX = max(MinX, MaxX);
  CharPtr->MaxY = max(MinY, MaxY);
  CharPtr->MaxZ = max(MinZ, MaxZ);

  return TRUE;
}   

BOOL cCharacterController::GetBounds(                         \
               long IDNum,                                    \
               float *MinX, float *MinY, float *MinZ,         \
               float *MaxX, float *MaxY, float *MaxZ)
{
  sCharacter *CharPtr;

  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  if(MinX != NULL)
    *MinX = CharPtr->MinX;
  if(MinY != NULL)
    *MinY = CharPtr->MinY;
  if(MinZ != NULL)
    *MinZ = CharPtr->MinZ;
  if(MaxX != NULL)
    *MaxX = CharPtr->MaxX;
  if(MaxY != NULL)
    *MaxY = CharPtr->MaxY;
  if(MaxZ != NULL)
    *MaxZ = CharPtr->MaxZ;

  return TRUE;
}

BOOL cCharacterController::SetType(long IDNum, long Type)
{
  sCharacter *CharPtr;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Set new value
  CharPtr->Type = Type;
  return TRUE;
}

long cCharacterController::GetType(long IDNum)
{
  sCharacter *CharPtr;

  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return 0;
  return CharPtr->Type;
}

BOOL cCharacterController::SetAI(long IDNum, long Type)
{
  sCharacter *CharPtr;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Set new value
  CharPtr->AI = Type;
  return TRUE;
}

long cCharacterController::GetAI(long IDNum)
{
  sCharacter *CharPtr;

  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return 0;
  return CharPtr->AI;
}

BOOL cCharacterController::SetTargetCharacter(long IDNum,     \
                                              long TargetNum)
{
  sCharacter *CharPtr, *CharTarget;

  // Get pointer to character
  if((CharPtr = GetCharacter(IDNum)) == NULL)
    return FALSE;

  // Clear if TargetNum == -1
  if(TargetNum == -1)
    CharPtr->TargetChar = NULL;
  else {
    // Scan through list and target 1st TargetNum found
    CharTarget = m_CharacterParent;
    while(CharTarget != NULL) {
      if(CharTarget->ID == TargetNum) {
        CharPtr->TargetChar = CharTarget;
        break;
      }
      CharTarget = CharTarget->Next;
    }

    // Clear target if not found in list
    if(CharTarget == NULL)
      CharPtr->TargetChar = NULL;
  }
  return TRUE;
}

BOOL cCharacterController::CharUpdate(                        \
               sCharacter *Character, long Elapsed,           \
               float *XMove, float *YMove, float *ZMove)
{
  float MoveX, MoveY, MoveZ, Speed;
  float XDiff, YDiff, ZDiff, Dist, Radius;
  float y1, y2;
  long  i, SpellNum;
  BOOL SpellCast;
  sCharacter *CharPtr;

  // Error checking
  if(Character == NULL)
    return FALSE;

  // Clear movements and action
  MoveX = MoveY = MoveZ = 0.0f;

  // Calculate movement speed
  Speed = (float)Elapsed / 1000.0f * GetSpeed(Character);

  // Move character based on their type
  switch(Character->AI) {
    case CHAR_STAND:
      break;
      
    case CHAR_WANDER:
      // Calculate new distance and direction if needed
      Character->Distance -= Elapsed;

      if(Character->Distance <= 0.0f) {

        // Calculate a new distance to travel
        Character->Distance = (float)(rand() % 2000) + 2000.0f;

        // Calculate a new direction
        Character->Direction = (float)(rand()%360)*0.01744444f;
      }

      // Process walk or stand still
      if(Character->Distance > 1000.0f) {
        MoveX = (float)sin(Character->Direction) * Speed;
        MoveZ = (float)cos(Character->Direction) * Speed;
        Character->Action = CHAR_MOVE;
      } else {
        // Stand still for one second
        Character->Action = CHAR_IDLE;
      }

      break;

    case CHAR_ROUTE:
      // Determine if character has reached point
      XDiff = (float)fabs(Character->XPos -                   \
              Character->Route[Character->CurrentPoint].XPos);
      YDiff = (float)fabs(Character->YPos -                   \
              Character->Route[Character->CurrentPoint].YPos);
      ZDiff = (float)fabs(Character->ZPos -                   \
              Character->Route[Character->CurrentPoint].ZPos);
      Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;

      Radius = GetXZRadius(Character) * 0.25f;
      
      // Go to next point if reached
      if(Dist < (Radius*Radius)) {
        Character->CurrentPoint++;
        if(Character->CurrentPoint >= Character->NumPoints)
          Character->CurrentPoint = 0;

        // Calculate new differences and distance
        XDiff = (float)fabs(Character->XPos -                 \
              Character->Route[Character->CurrentPoint].XPos);
        YDiff = (float)fabs(Character->YPos -                 \
              Character->Route[Character->CurrentPoint].YPos);
        ZDiff = (float)fabs(Character->ZPos -                 \
              Character->Route[Character->CurrentPoint].ZPos);
        Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;
      }

      // Setup movement towards target
      Dist = (float)sqrt(Dist);
      if(Speed > Dist)
        Speed = Dist;

      MoveX=(Character->Route[Character->CurrentPoint].XPos - \
              Character->XPos) / Dist * Speed;
      MoveZ=(Character->Route[Character->CurrentPoint].ZPos - \
              Character->ZPos) / Dist * Speed;

      // Set new direction
      Character->Direction = (float)atan2(MoveX, MoveZ);

      // Set new action
      Character->Action = CHAR_MOVE;

      break;

    case CHAR_FOLLOW:
      if(Character->TargetChar != NULL) {

        // Check distance between characters
        XDiff = (float)fabs(Character->XPos -                 \
                            Character->TargetChar->XPos);
        YDiff = (float)fabs(Character->YPos -                 \
                            Character->TargetChar->YPos);
        ZDiff = (float)fabs(Character->ZPos -                 \
                            Character->TargetChar->ZPos);
        Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;

        // Update if further then distance
        if(Dist > (Character->Distance*Character->Distance)) {

          // Setup movement towards target
          Dist = (float)sqrt(Dist);
          if(Speed > Dist)
            Speed = Dist;
          MoveX = (Character->TargetChar->XPos -              \
                 Character->XPos) / Dist * Speed;
          MoveZ = (Character->TargetChar->ZPos -              \
                 Character->ZPos) / Dist * Speed;

          // Set new direction
          Character->Direction = (float)atan2(MoveX, MoveZ);

          // Set new action
          Character->Action = CHAR_MOVE;
        }
      }

      break;

    case CHAR_EVADE:
      if(Character->TargetChar != NULL) {

        // Check distance between characters
        XDiff = (float)fabs(Character->XPos -                 \
                            Character->TargetChar->XPos);
        YDiff = (float)fabs(Character->YPos -                 \
                            Character->TargetChar->YPos);
        ZDiff = (float)fabs(Character->ZPos -                 \
                            Character->TargetChar->ZPos);
        Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;

        // Update if closer then distance
        if(Dist < (Character->Distance*Character->Distance)) {

          // Setup movement away from target
          Dist = (float)sqrt(Dist);
          if(Speed > Dist)
            Speed = Dist;
          MoveX = -(Character->TargetChar->XPos -             \
                 Character->XPos) / Dist * Speed;
          MoveZ = -(Character->TargetChar->ZPos -             \
                 Character->ZPos) / Dist * Speed;

          // Set new direction
          Character->Direction = (float)atan2(MoveX, MoveZ);

          // Set new action
          Character->Action = CHAR_MOVE;
        }
      }

      break;
  }

  // Process monster actions if at full charge
  if(Character->Type == CHAR_MONSTER &&                       \
                                Character->Charge >= 100.0f) {

    // Determine chance of attacking
    if((rand() % 100) <= Character->Def.ToAttack) {
      // Scan through list and pick a character
      CharPtr = m_CharacterParent;
      while(CharPtr != NULL) {
        // Randomly pick enabled target (a PC),
        // and make sure the target is not hurt or dead.
        if(CharPtr != Character && CharPtr->Type==CHAR_PC &&  \
           (rand() % 100) < 50 &&                             \
           CharPtr->Enabled == TRUE &&                        \
           CharPtr->Action != CHAR_DIE &&                     \
           CharPtr->Action != CHAR_HURT) {

          // Get distance to target
          XDiff = (float)fabs(Character->XPos - CharPtr->XPos);
          YDiff = (float)fabs(Character->YPos - CharPtr->YPos);
          ZDiff = (float)fabs(Character->ZPos - CharPtr->ZPos);
          Dist  = XDiff * XDiff + YDiff * YDiff + ZDiff * ZDiff;

          // Make sure in range to attack
          Radius = GetXZRadius(Character);
          Radius += Character->Def.Range;

          // Attack if in range
          if((Radius*Radius) >= Dist) {
            // Set attack data
            Character->Victim = CharPtr;
            CharPtr->Attacker = Character;

            // Clear movement
            MoveX = MoveY = MoveZ = 0.0f;

            // Point towards target character
            XDiff = CharPtr->XPos - Character->XPos;
            ZDiff = CharPtr->ZPos - Character->ZPos;
            Character->Direction = (float)atan2(XDiff, ZDiff);

            // Perform attack action
            SetAction(Character, CHAR_ATTACK);

            break;
          }
        }

        // Go to next character
        CharPtr = CharPtr->Next;
      }
    } else

    // Determine chance of spell casting
    if((rand() % 100) <= Character->Def.ToMagic &&            \
                 m_MSL != NULL && m_SpellController != NULL) {

      // Flag no spells cast
      SpellCast = FALSE;

      // If health is less then half, then there's a 50% chance
      // of healing (if the monster knows any heal spells)
      if(Character->HealthPoints <=                           \
                          (Character->Def.HealthPoints / 2)) {

        // Search for a known heal spell
        for(i=0;i<64;i++) {
          if(m_MSL[i].Name[0] &&                              \
             m_MSL[i].Effect == ALTER_HEALTH &&               \
             m_MSL[i].Value[0] > 0.0f &&                      \
             Character->ManaPoints >= m_MSL[i].Cost &&        \
             Character->Def.MagicSpells[i/32] & (1<<(i&31))) {

            // This is the spell, determine chance to heal
            if((rand() % 100) < 50) {
              // Set spell data
              Character->Victim      = Character;
              Character->Attacker    = Character;
              Character->SpellNum    = i;
              Character->SpellTarget = CHAR_MONSTER;
              
              // Store target coordinates
              Character->TargetX = Character->XPos;
              Character->TargetY = Character->YPos;
              Character->TargetZ = Character->ZPos;

              // Clear movement
              MoveX = MoveY = MoveZ = 0.0f;

              // Perform spell action
              SetAction(Character, CHAR_SPELL);

              // Flag spell as cast
              SpellCast = TRUE;

              break;
            }
          }
        }
      }

      // If there are bad status ailments, then there's a 
      // 50% chance of dispeling magic.
      if(Character->Ailments & AILMENT_POISON ||              \
         Character->Ailments & AILMENT_SLEEP ||               \
         Character->Ailments & AILMENT_PARALYZE ||            \
         Character->Ailments & AILMENT_ENCHANTED ||           \
         Character->Ailments & AILMENT_DUMBFOUNDED ||         \
         Character->Ailments & AILMENT_SLOW ||                \
         Character->Ailments & AILMENT_BLIND ||               \
         Character->Ailments & AILMENT_SILENCED &&            \
         SpellCast == FALSE) {
        
        // Search for a known dispel spell
        for(i=0;i<64;i++) {
          if(m_MSL[i].Name[0] &&                              \
             m_MSL[i].Effect == DISPEL_MAGIC &&               \
             Character->ManaPoints >= m_MSL[i].Cost &&        \
             Character->Def.MagicSpells[i/32] & (1<<(i&31))) {

            // This is the spell, determine chance to dispel
            if((rand() % 100) < 50) {
              // Set spell data
              Character->Victim      = Character;
              Character->Attacker    = Character;
              Character->SpellNum    = i;
              Character->SpellTarget = CHAR_MONSTER;
              
              // Store target coordinates
              Character->TargetX = Character->XPos;
              Character->TargetY = Character->YPos;
              Character->TargetZ = Character->ZPos;

              // Clear movement
              MoveX = MoveY = MoveZ = 0.0f;

              // Perform spell action
              SetAction(Character, CHAR_SPELL);

              // Flag spell as cast
              SpellCast = TRUE;

              break;
            }
          }
        }
      } 

      // If now spells already cast, then pick a random one
      if(SpellCast == FALSE) {

        // Pick a random spell to attack with
        SpellNum = rand() % 64;

        // Scan through list until a spell is found the
        // monster can cast.
        for(i=0;i<64;i++) {
          if(m_MSL[SpellNum].Name[0] &&                       \
             Character->Def.MagicSpells[SpellNum / 32] &      \
                                   (1 << (SpellNum & 31)) &&  \
             Character->ManaPoints >= m_MSL[SpellNum].Cost && \
             (rand() % 100) < 50) {

            // Scan through list and pick a character
            CharPtr = m_CharacterParent;
            while(CharPtr != NULL) {

              // Randomly pick an enabled target (a PC), 
              // and make sure the target is not hurt or dead.
              // Also, don't cast self-targeting spells here.
              if(CharPtr != Character &&                      \
                 CharPtr->Type == CHAR_PC &&                  \
                 m_MSL[SpellNum].Target != TARGET_SELF &&     \
                 (rand() % 100) < 50 &&                       \
                 CharPtr->Enabled == TRUE &&                  \
                 CharPtr->Action != CHAR_DIE &&               \
                 CharPtr->Action != CHAR_HURT) {

                // Get heights of attacker and target
                // for line of sight checking
                Character->Object.GetBounds(NULL,NULL,NULL,   \
                                            NULL,&y1,NULL,NULL);
                y1 = (y1 * 0.5f) + Character->YPos;

                CharPtr->Object.GetBounds(NULL,NULL,NULL,     \
                                          NULL,&y2,NULL,NULL);
                y2 = (y2 * 0.5f) + CharPtr->YPos;

                // Get distance to target
                XDiff = (float)fabs(Character->XPos -         \
                                    CharPtr->XPos);
                YDiff = (float)fabs(Character->YPos -         \
                                    CharPtr->YPos);
                ZDiff = (float)fabs(Character->ZPos -         \
                                    CharPtr->ZPos);
                Dist  = XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff;

                // Reduce distance by character's radius
                Radius = GetXZRadius(CharPtr);
                Dist -= (Radius*Radius);

                // Get spell radius
                Radius = m_MSL[SpellNum].Distance;

                // Make sure target is in range and in sight
                if(LineOfSight(Character, CharPtr,            \
                               Character->XPos, y1,           \
                               Character->ZPos,               \
                               CharPtr->XPos, y2,             \
                               CharPtr->ZPos) &&              \
                   Dist <= (Radius * Radius)) {

                  // Set the spell data
                  Character->Victim      = CharPtr;
                  CharPtr->Attacker      = Character;
                  Character->SpellNum    = SpellNum;
                  Character->SpellTarget = CHAR_PC;

                  // Store target coordinates
                  Character->TargetX = CharPtr->XPos;
                  Character->TargetY = CharPtr->YPos;
                  Character->TargetZ = CharPtr->ZPos;

                  // Face toward target (only if not self)
                  if(m_MSL[SpellNum].Target != TARGET_SELF) {
                    XDiff = CharPtr->XPos - Character->XPos;
                    ZDiff = CharPtr->ZPos - Character->ZPos;
                    Character->Direction =                    \
                                 (float)atan2(XDiff, ZDiff);
                  }

                  // Clear movement
                  MoveX = MoveY = MoveZ = 0.0f;

                  // Set the spell action
                  SetAction(Character, CHAR_SPELL);

                  // Flag spell as cast
                  SpellCast = TRUE;

                  break;
                }
              }

              // Go to next character
              CharPtr = CharPtr->Next;
            }
            break;
          }

          // Go to next spell
          SpellNum = (SpellNum + 1) % 64;
        }
      }

      // If still no spell cast, try casting a known
      // self-enhancing ailment-effecting spell.
      if(SpellCast == FALSE) {
        for(i=0;i<64;i++) {
          if(m_MSL[i].Name[0] &&                              \
             m_MSL[i].Effect == CAUSE_AILMENT &&              \
             Character->ManaPoints >= m_MSL[i].Cost &&        \
             Character->Def.MagicSpells[i/32]&(1<<(i&31)) &&  \
             (rand()%100) < 10) {

            // Make sure it's self-enhancing
            if((long)m_MSL[i].Value[0]&AILMENT_STRONG ||      \
               (long)m_MSL[i].Value[0]&AILMENT_BARRIER ||     \
               (long)m_MSL[i].Value[0]&AILMENT_SUREFOOTED ||  \
               (long)m_MSL[i].Value[0]&AILMENT_FAST ||        \
               (long)m_MSL[i].Value[0]&AILMENT_HAWKEYE) {

              // Make sure ailment not already set
              if(!(Character->Ailments &                      \
                                   (long)m_MSL[i].Value[0])) {

                // Set spell data
                Character->Victim      = Character;
                Character->Attacker    = Character;
                Character->SpellNum    = i;
                Character->SpellTarget = CHAR_MONSTER;
              
                // Store target coordinates
                Character->TargetX = Character->XPos;
                Character->TargetY = Character->YPos;
                Character->TargetZ = Character->ZPos;

                // Clear movement
                MoveX = MoveY = MoveZ = 0.0f;

                // Perform spell action
                SetAction(Character, CHAR_SPELL);

                break;
              }
            }
          }
        }
      }
    }
  }

  // Store movement and return
  *XMove = MoveX;
  *YMove = MoveY;
  *ZMove = MoveZ;

  return TRUE;
}

BOOL cCharacterController::CheckMove(sCharacter *Character,   \
               float *XMove, float *YMove, float *ZMove)
{
  sCharacter *CharPtr;
  float XDiff, YDiff, ZDiff, Dist;
  float Radius1, Radius2;
  float XPos, YPos, ZPos;
  float MinX, MaxX, MinZ, MaxZ;

  // Error checking
  if(Character == NULL)
    return FALSE;  // Don't allow movement

  XPos = Character->XPos + (*XMove);
  YPos = Character->YPos + (*YMove);
  ZPos = Character->ZPos + (*ZMove);

  // Get character's X/Z radius
  Character->Object.GetBounds(&MinX, NULL, &MinZ,             \
                              &MaxX, NULL, &MaxZ, NULL);
  Radius1 = max(MaxX-MinX, MaxZ-MinZ) * 0.5f;

  // Check movement against other characters
  if((CharPtr = m_CharacterParent) != NULL) {
    while(CharPtr != NULL) {

      // Don't check against self or disabled characters
      if(Character != CharPtr && CharPtr->Enabled == TRUE) {
        // Don't check against other PC characters
        if(Character->Type == CHAR_PC &&                      \
           CharPtr->Type == CHAR_PC)
          break;
        
        // Get distance between characters
        XDiff = (float)fabs(XPos - CharPtr->XPos);
        YDiff = (float)fabs(YPos - CharPtr->YPos);
        ZDiff = (float)fabs(ZPos - CharPtr->ZPos);
        Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;

        // Get checking character's X/Z bounding radius
        CharPtr->Object.GetBounds(&MinX, NULL, &MinZ,         \
                                  &MaxX, NULL, &MaxZ, NULL);
        Radius2 = max(MaxX-MinX, MaxZ-MinZ) * 0.5f;

        // Don't allow movement if intersecting
        if(Dist <= (Radius1*Radius1 + Radius2*Radius2))
          return FALSE;
      }

      CharPtr = CharPtr->Next;
    }
  }

  // Bounds check movement if MinX != MaxX
  if(Character->MinX != Character->MaxX) {
    if(XPos < Character->MinX || XPos > Character->MaxX)
      *XMove = 0.0f;
    if(YPos < Character->MinY || YPos > Character->MaxY)
      *YMove = 0.0f;
    if(ZPos < Character->MinZ || ZPos > Character->MaxZ)
      *ZMove = 0.0f;

    // Return no movement at all
    if(!(*XMove) && !(*YMove) && !(*ZMove))
      return FALSE;
  }

  // Call overloaded check custom collisions (maps, etc)
  if(ValidateMove(Character, XMove, YMove, ZMove) == FALSE)
    return FALSE;  // Don't allow movement

  return TRUE;
}

BOOL cCharacterController::ProcessUpdate(                     \
                      sCharacter *Character,
                      float XMove, float YMove, float ZMove)
{
  // Move character
  Character->XPos += XMove;
  Character->YPos += YMove;
  Character->ZPos += ZMove;

  // Move object and point in direction
  Character->Object.Move(Character->XPos,                     \
                         Character->YPos,                     \
                         Character->ZPos);                    \
  Character->Object.Rotate(0.0f, Character->Direction, 0.0f);

  // Set new animation
  if(Character->LastAnim != Character->Action) {
    Character->LastAnim = Character->Action;

    if(m_NumAnimations && m_Animations != NULL) {
      Character->LastAnimTime = timeGetTime() / 30;
      Character->Object.SetAnimation(                         \
                 &m_Meshes[Character->Def.MeshNum].Animation, \
                 m_Animations[Character->Action].Name,        \
                 Character->LastAnimTime);
    }
  }

  return TRUE;
}

BOOL cCharacterController::Attack(sCharacter *Attacker,       \
                                  sCharacter *Victim)
{
  // Error checking
  if(Attacker == NULL || Victim == NULL)
    return FALSE;

  // Don't attack dead or hurt people
  if(Victim->Action == CHAR_DIE || Victim->Action == CHAR_HURT)
    return FALSE;

  // Set attacker and victim
  Victim->Attacker = Attacker;
  Attacker->Victim = Victim;

  // Return if hit missed
  if((rand() % 1000) > GetToHit(Attacker)) {
    SetMessage(Victim, "Missed!", 500);
    return FALSE;
  }

  // Return if hit dodged
  if((rand() % 1000) <= GetAgility(Victim)) {
    SetMessage(Victim, "Dodged!", 500);
    return FALSE;
  }

  // If character is asleep, randomly wake them up (50% chance)
  if(Victim->Ailments & AILMENT_SLEEP) {
    if((rand() % 100) < 50)
      Victim->Ailments &= ~AILMENT_SLEEP;
  }

  // Attack landed, apply damage
  Damage(Victim, TRUE, GetAttack(Attacker), -1, -1);

  return TRUE;
}

BOOL cCharacterController::Damage(sCharacter *Victim,         \
                                  BOOL PhysicalAttack,        \
                                  long Amount,                \
                                  long DmgClass,              \
                                  long CureClass)
{
  char Text[128];
  float Resist;
  float Range;
  long  DmgAmount;

  // Error checking
  if(Victim == NULL)
    return FALSE;

  // Can't attack if already dead or being hurt (or not enabled)
  if(Victim->Enabled == FALSE ||                              \
     Victim->Action==CHAR_DIE ||                              \
     Victim->Action==CHAR_HURT)
    return FALSE;

  // Adjust for defense if physical attack
  if(PhysicalAttack == TRUE) {
 
    // Random value for less/more damage (-+20%)
    Range     = (float)((rand() % 20) + 90) / 100.0f;
    DmgAmount = (long)((float)Amount * Range);

   // Subtract for defense of victim (allow -20% difference)
    Range     = (float)((rand() % 20) + 80) / 100.0f;
    DmgAmount -= (long)((float)GetDefense(Victim) * Range);
  } else {
    // Adjust for magical attack
    Resist = 1.0f - ((float)GetResistance(Victim) / 100.0f);
    DmgAmount = (long)((float)Amount * Resist);
  }

  // Bounds check value
  if(DmgAmount < 0)
    DmgAmount = 0;

  // Check for double damage
  if(Victim->Def.Class == DmgClass)
    DmgAmount *= 2;

  // Check for cure damage
  if(Victim->Def.Class == CureClass)
    DmgAmount = -(labs(DmgAmount)/2);

  // If no physical damage is dealt then randomly deal 
  // 10-20% of damage from the original amount.
  if(!DmgAmount && PhysicalAttack == TRUE) {
    Range     = (float)((rand() % 10) + 10) / 100.0f;
    DmgAmount = (long)((float)Amount * Range);
  }

  // Subtract damage amount
  Victim->HealthPoints -= DmgAmount;

  // Set hurt status and display message
  if(DmgAmount >= 0) {
    sprintf(Text, "-%lu HP", DmgAmount);
    SetMessage(Victim, Text, 500, D3DCOLOR_RGBA(255,64,0,255));

    // Only set hurt if any damage (and idle or moving)
    if(DmgAmount) {
      if(Victim->Action==CHAR_MOVE || Victim->Action==CHAR_IDLE)
        SetAction(Victim, CHAR_HURT);
    }
  } 

  // Display cure amount
  if(DmgAmount < 0) {
    sprintf(Text, "+%lu HP", -DmgAmount);
    SetMessage(Victim, Text, 500, D3DCOLOR_RGBA(0,64,255,255));
  }

  return TRUE;
}

BOOL cCharacterController::Death(sCharacter *Attacker,        \
                                 sCharacter *Victim)
{
  char Text[128];

  // If a PC or NPC dies, then don't remove from list
  if(Victim->Type == CHAR_PC) {
    // Mark character as disabled so no updates
    Victim->Enabled = FALSE;

    // Call outside death function for PC's
    if(Victim->Type == CHAR_PC)
      PCDeath(Victim);
    else
      NPCDeath(Victim);
  } else {
    // Give attacker the victim's experience
    if(Attacker != NULL) {
      if(Experience(Attacker, Victim->Def.Experience) == TRUE) {
        sprintf(Text, "+%lu exp.", Victim->Def.Experience);
        SetMessage(Attacker, Text, 500);
      }
    }

    // Drop character's money
    if(m_MIL != NULL && Victim->Def.Money)
      DropMoney(Victim->XPos, Victim->YPos, Victim->ZPos,     \
                Victim->Def.Money);

    // Randomly drop an item (as specified in definition)
    if((rand() % 100) < Victim->Def.DropChance)
      DropItem(Victim->XPos, Victim->YPos, Victim->ZPos,      \
               Victim->Def.DropItem);

    // Decrease mesh count and release if no more
    m_Meshes[Victim->Def.MeshNum].Count--;
    if(!m_Meshes[Victim->Def.MeshNum].Count) {
      m_Meshes[Victim->Def.MeshNum].Mesh.Free();
      m_Meshes[Victim->Def.MeshNum].Animation.Free();
    }

    // Remove dead character from list
    if(Victim->Prev != NULL)
      Victim->Prev->Next = Victim->Next;
    else
      m_CharacterParent = Victim->Next;

    if(Victim->Next != NULL)
      Victim->Next->Prev = Victim->Prev;

    if(Victim->Prev == NULL && Victim->Next == NULL)
      m_CharacterParent = NULL;

    Victim->Prev = Victim->Next = NULL;
    delete Victim;
  }

  return TRUE;
}

BOOL cCharacterController::Spell(sCharacter *Caster,          \
                                 sSpellTracker *SpellTracker, \
                                 sSpell *Spells)
{
  float XDiff, YDiff, ZDiff, Dist, YDist, XZDist;
  float MinX, MaxX, MinY, MaxY, MinZ, MaxZ;
  float SpellRadius, XZRadius, YRadius;
  sSpell *SpellPtr;
  sCharacter *CharPtr, *ClosestChar;
  float Closest;
  BOOL Allow;

  // Error checking
  if(Caster == NULL || SpellTracker == NULL || Spells == NULL)
    return TRUE;

  // Get pointer to spell
  SpellPtr = &Spells[SpellTracker->SpellNum];

  // Reduce magic
  Caster->ManaPoints -= SpellPtr->Cost;
  if(Caster->ManaPoints < 0)
    Caster->ManaPoints = 0;

  // Can't cast if silenced
  if(Caster->Ailments & AILMENT_SILENCED) {
    SetMessage(Caster, "Silenced!", 500);
    return FALSE;
  }

  // Get radius of spell
  SpellRadius = SpellPtr->Range * SpellPtr->Range;

  // Handle self-targeting spells instantly
  if(SpellPtr->Target == TARGET_SELF) {
    SpellEffect(Caster, Caster, SpellPtr);
    return TRUE;
  }

  // Reset closest character pointer
  ClosestChar = NULL;
  Closest = 0.0f;

  // Scan through all characters and look for hits
  if((CharPtr = m_CharacterParent) == NULL)
    return FALSE;
  while(CharPtr != NULL) {

    // Only bother with characters of allowed types
    // as well as not targeting self. Also, allow 
    // a RAISE_DEAD PC spell to affect any character.
    Allow = FALSE;
    if(CharPtr!=Caster && SpellTracker->Type==CharPtr->Type)
      Allow = TRUE;
    if(CharPtr->Type==CHAR_PC && SpellPtr->Effect==RAISE_DEAD)
      Allow = TRUE;

    // Find target(s) if allowed
    if(Allow == TRUE) {

      // Get distance from target to character
      XDiff = (float)fabs(CharPtr->XPos - SpellTracker->TargetX);
      YDiff = (float)fabs(CharPtr->YPos - SpellTracker->TargetY);
      ZDiff = (float)fabs(CharPtr->ZPos - SpellTracker->TargetZ);

      // Get X/Z and Y distances
      XZDist = (XDiff * XDiff + ZDiff * ZDiff) - SpellRadius;
      YDist  = (YDiff * YDiff) - SpellRadius;

      // Get target X/Z and Y radius
      CharPtr->Object.GetBounds(&MinX,&MinY,&MinZ,
                                &MaxX,&MaxY,&MaxZ,NULL);
      XZRadius = max(MaxX-MinX, MaxZ-MinZ) * 0.5f;
      YRadius = (MaxY-MinY) * 0.5f;

      // Check if character in range
      if(XZDist - (XZRadius * XZRadius) <= 0.0f && 
          YDist - (YRadius  * YRadius)  <= 0.0f) {
       
        // Determine what to do if in range
        if(SpellPtr->Target == TARGET_SINGLE) {
          // Record closest character in range
          Dist = XDiff * XDiff + YDiff * YDiff + ZDiff * ZDiff;
          if(ClosestChar == NULL) {
            ClosestChar = CharPtr;
            Closest = Dist;
          } else {
            if(Dist < Closest) {
              ClosestChar = CharPtr;
              Closest = Dist;
            }
          }
        } else {
          // Spell hit target if area target
          SpellEffect(Caster, CharPtr, SpellPtr);
        }
      }
    }

    // Go to next character
    CharPtr = CharPtr->Next;
  }

  // Process spell on closest character if needed
  if(SpellPtr->Target==TARGET_SINGLE && ClosestChar!=NULL)
    SpellEffect(Caster, ClosestChar, SpellPtr);

  return TRUE;
}

BOOL cCharacterController::SpellEffect(sCharacter *Caster,    \
                                       sCharacter *Target,    \
                                       sSpell *Spell)
{
  BOOL CanHit;
  long Chance;
  char Text[64];

  // Error checking
  if(Target == NULL || Spell == NULL)
    return FALSE;

  // Calculate chance of hitting
  if(Caster != NULL) {
    // A spell always lands if target=caster
    if(Caster == Target)
      Chance = 100;
    else
      Chance = (long)(((float)GetMental(Caster) / 100.0f +    \
                           1.0f) * (float)Spell->Chance);
  } else {
    Chance = Spell->Chance;
  }

  // Alter chance by target's resistance
  if(Caster != Target)
    Chance = (long)((1.0f - (float)GetResistance(Target) /    \
                        100.0f) * (float)Chance);

  // See if spell failed
  if(Chance != 100 && (rand() % 100) >= Chance) {
    SetMessage(Target, "Failed!", 500);
    return FALSE;
  }

  // Flag character to allow effect
  CanHit = TRUE;
  if(Target->Action==CHAR_HURT || Target->Action==CHAR_DIE)
    CanHit = FALSE;

  // Store attacker and victim
  if((Target->Attacker = Caster) != NULL)
    Caster->Victim = Target;
    
  // Process spell effect
  switch(Spell->Effect) {
    case ALTER_HEALTH:     // Alter health
      if(CanHit == FALSE)
        break;

      // Apply damage if value < 0
      if(Spell->Value[0] < 0.0f)
        Damage(Target,FALSE,-(long)Spell->Value[0],           \
               Spell->DmgClass, Spell->CureClass);

      // Cure damage if value > 0
      if(Spell->Value[0] > 0.0f) {
        Target->HealthPoints += (long)Spell->Value[0];
        if(Target->HealthPoints > Target->Def.HealthPoints)
          Target->HealthPoints = Target->Def.HealthPoints;
        
        // Display amount healed
        sprintf(Text, "+%lu HP", (long)Spell->Value[0]);
        SetMessage(Target,Text,500,D3DCOLOR_RGBA(0,64,255,255));
      }

      break;

    case ALTER_MANA:       // Alter mana
      if(CanHit == FALSE)
        break;

      // Alter mana value
      Target->ManaPoints += (long)Spell->Value[0];
      if(Target->ManaPoints < 0)
        Target->ManaPoints = 0;
      if(Target->ManaPoints > Target->Def.ManaPoints)
        Target->ManaPoints = Target->Def.ManaPoints;

      // Display remove mana message
      if(Spell->Value[0] < 0.0f) {
        sprintf(Text, "%ld MP", (long)Spell->Value[0]);
        SetMessage(Target,Text,500,D3DCOLOR_RGBA(0,128,64,255));
      }

      // Display add mana message
      if(Spell->Value[0] > 0.0f) {
        sprintf(Text, "+%lu MP", (long)Spell->Value[0]);
        SetMessage(Target,Text,500,D3DCOLOR_RGBA(0,255,0,255));
      }

      break;

    case CURE_AILMENT:      // Clear ailment flag
      if(CanHit == FALSE)
        break;

      // Apply ailment and display message
      Target->Ailments |= ~(long)Spell->Value[0];
      SetMessage(Target, "Cure", 500);

      break;

    case CAUSE_AILMENT:     // Set ailment flag
      if(CanHit == FALSE)
        break;

      // Cure ailment and display message
      Target->Ailments |= (long)Spell->Value[0];
      SetMessage(Target, "Ailment", 500);

      break;

    case RAISE_DEAD:       // Raise from dead
      if(Target->Action == CHAR_DIE) {
        Target->HealthPoints = 1;
        Target->ManaPoints   = 0;
        Target->Action       = CHAR_IDLE;
        Target->Locked       = FALSE;
        Target->ActionTimer  = 0;
        Target->Ailments      = 0;
        Target->Enabled      = TRUE;
      }
      break;

    case INSTANT_KILL:     // Kill character
      if(CanHit == FALSE)
        break;

      // Set die action
      SetAction(Target, CHAR_DIE);
      break;

    case DISPEL_MAGIC:     // Remove all ailments
      if(CanHit == FALSE)
        break;

      // Clear all ailments
      Target->Ailments = 0;
      break;

    case TELEPORT:         // Teleport PC/NPC/MONSTER
      if(CanHit == FALSE)
        break;

      // Teleport PCs from seperate function from rest
      if(Target->Type == CHAR_PC)
        PCTeleport(Caster, Spell);
      else {
        Target->XPos = Spell->Value[0];
        Target->YPos = Spell->Value[1];
        Target->ZPos = Spell->Value[2];
      }
      break;
  }

  return TRUE;
}

BOOL cCharacterController::Equip(sCharacter *Character,       \
                                 long ItemNum, long Type,     \
                                 BOOL Equip)
{
  char Path[MAX_PATH];

  // Error checking
  if(m_MIL == NULL || Character == NULL)
    return FALSE;

  // Make sure restritions allow equiping of item
  if(!CheckUsageBit(m_MIL[ItemNum].Usage,Character->Def.Class))
    return FALSE;

  // Remove current item first and equip new one
  switch(Type) {
    case WEAPON:
      // Remove current item of type
      Character->Def.Weapon = -1;
      Character->WeaponObject.Free();
      Character->WeaponMesh.Free();
      
      // Equip new item of correct type
      if(Equip==TRUE && m_MIL[ItemNum].Category == WEAPON) {
        Character->Def.Weapon = ItemNum;

        // Setup new weapon mesh and object
        if(m_MIL[ItemNum].MeshFilename[0]) {

          // Build the mesh path
          sprintf(Path, "%s%s", m_MeshPath,                   \
                  m_MIL[Character->Def.Weapon].MeshFilename);

          // Load the new mesh
          Character->WeaponMesh.Load(m_Graphics, Path,        \
                                     m_TexturePath);

          // Create the weapon object
          Character->WeaponObject.Create(m_Graphics,          \
                                       &Character->WeaponMesh);

          // Orient and attach the weapon
          Character->WeaponObject.Move(0.0f, 0.0f, 0.0f);
          Character->WeaponObject.Rotate(0.0f, 0.0f, 0.0f);
          Character->WeaponObject.AttachToObject(             \
                             &Character->Object, "WeaponHand");
        }
      }
      break;

    case ARMOR:
      // Remove current item of type
      Character->Def.Armor = -1;

      // Equip new item of correct type
      if(Equip==TRUE && m_MIL[ItemNum].Category == ARMOR)
        Character->Def.Armor = ItemNum;

      break;

    case SHIELD:
      // Remove current item of type
      Character->Def.Shield = -1;

      // Equip new item of correct type
      if(Equip==TRUE && m_MIL[ItemNum].Category == SHIELD)
        Character->Def.Shield = ItemNum;

      break;

    case ACCESSORY:
      // Remove current item of type
      Character->Def.Accessory = -1;

      // Equip new item of correct type
      if(Equip==TRUE && m_MIL[ItemNum].Category == ACCESSORY)
        Character->Def.Accessory = ItemNum;

      break;

    default: return FALSE;
  }

  return TRUE;
}

BOOL cCharacterController::Item(sCharacter *Owner,            \
                                sCharacter *Target,           \
                                long ItemNum,                 \
                                sCharItem *CharItem)
{
  sItem *Item;

  // Error checking
  if(Owner == NULL || Target == NULL || m_MIL == NULL)
    return FALSE;

  // Make sure restritions allow equiping of item
  if(!CheckUsageBit(m_MIL[ItemNum].Usage, Target->Def.Class))
    return FALSE;

  // Get pointer to item
  Item = (sItem*)&m_MIL[ItemNum];

  // Use specified item
  switch(Item->Category) {
    case EDIBLE:
    case HEALING:
      // Alter health
      Owner->HealthPoints += Item->Value;
      break;
  }

  // Decrease quanity (and remove object) if needed
  if(CheckItemFlag(Item->Flags,USEONCE) && CharItem != NULL) {
    CharItem->Quantity--;
    if(CharItem->Quantity <= 0 && Owner->CharICS != NULL)
      Owner->CharICS->Remove(CharItem);
  }

  return TRUE;
}

BOOL cCharacterController::Drop(sCharacter *Character,
                sCharItem *Item, long Quantity)
{
  // Error checking
  if(Item == NULL || m_MIL == NULL || Character == NULL)
    return FALSE;

  // Make sure item can be dropped
  if(!CheckItemFlag(m_MIL[Item->ItemNum].Flags,CANDROP))
    return FALSE;

  // Decrease quantity from item
  Item->Quantity -= Quantity;

  // Remove item from ICS if no more left
  if(Item->Quantity <= 0 && Character->CharICS != NULL)
    Character->CharICS->Remove(Item);

  return TRUE;
}
