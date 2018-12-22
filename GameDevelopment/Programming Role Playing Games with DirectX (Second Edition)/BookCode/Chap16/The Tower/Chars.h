#ifndef _CHARS_H_
#define _CHARS_H_

#include "Frustum.h"
#include "CharICS.h"
#include "MCL.h"
#include "MIL.h"
#include "MSL.h"

// Spell references
class  cSpellController;
struct sSpellTracker;

// Number of characters in file
#define NUM_CHARACTER_DEFINITIONS   256

// Character types
#define CHAR_PC          0
#define CHAR_NPC         1
#define CHAR_MONSTER     2

// AI types
#define CHAR_STAND       0
#define CHAR_WANDER      1
#define CHAR_ROUTE       2
#define CHAR_FOLLOW      3
#define CHAR_EVADE       4

// Action/Animation types
#define CHAR_IDLE        0
#define CHAR_MOVE        1
#define CHAR_ATTACK      2
#define CHAR_SPELL       3
#define CHAR_ITEM        4
#define CHAR_HURT        5
#define CHAR_DIE         6
#define CHAR_TALK        7

// Status ailments
#define AILMENT_POISON          1
#define AILMENT_SLEEP           2
#define AILMENT_PARALYZE        4
#define AILMENT_WEAK            8
#define AILMENT_STRONG         16
#define AILMENT_ENCHANTED      32
#define AILMENT_BARRIER        64
#define AILMENT_DUMBFOUNDED   128
#define AILMENT_CLUMSY        256
#define AILMENT_SUREFOOTED    512 
#define AILMENT_SLOW         1024
#define AILMENT_FAST         2048
#define AILMENT_BLIND        4096
#define AILMENT_HAWKEYE      8192
#define AILMENT_SILENCED    16384

// Animation info
typedef struct {
  char Name[32];    // Name of animation
  BOOL Loop;        // To loop flag
} sCharAnimationInfo;

// A mesh list
typedef struct sCharacterMeshList {
  char       Filename[MAX_PATH];  // Filename of mesh/anim
  long       Count;               // # characters using mesh
  cMesh      Mesh;                // Mesh object
  cAnimation Animation;           // Animation object

  sCharacterMeshList()  { Count = 0;                     }
  ~sCharacterMeshList() { Mesh.Free(); Animation.Free(); }
} sCharacterMeshList;

// Path/Route structure
typedef struct {
  float XPos, YPos, ZPos;   // Target position
} sRoutePoint;

typedef struct sCharacter
{
  long  Definition;         // Character definition #
  long  ID;                 // ID # of character

  long  Type;               // PC, NPC, or MONSTER
  long  AI;                 // STAND, WANDER, etc

  BOOL  Enabled;            // Enabled flag (for updates)

  sCharacterDefinition Def; // Loaded definition
  cCharICS *CharICS;        // PC character's ICS

  char ScriptFilename[MAX_PATH]; // Associated script

  long  HealthPoints;       // Current health points
  long  ManaPoints;         // Current mana points
  long  Ailments;           // Ailments against character
  float Charge;             // Attack charge

  long  Action;             // Current action
  float XPos, YPos, ZPos;   // Current coordinates
  float Direction;          // Angle character is facing
  long  LastAnim;           // Last animation
  long  LastAnimTime;       // Last animation time

  BOOL  Locked;             // Specific action lock
  long  ActionTimer;        // Lock action countdown timer

  sCharacter *Attacker;     // Attacking character (if any)
  sCharacter *Victim;       // Character to attack

  long  SpellNum;           // Spell to cast when ready
  long  SpellTarget;        // Target type of spell
  float TargetX, TargetY, TargetZ; // Spell target coords

  long ItemNum;             // Item to use when ready
  sCharItem *CharItem;      // Item to remove from inventory

  float Distance;           // Follow/Evade distance
  sCharacter *TargetChar;   // Character to follow
  float MinX, MinY, MinZ;   // Min bounding coordinates
  float MaxX, MaxY, MaxZ;   // Max bounding coordiantes

  long  NumPoints;          // # points in route
  long  CurrentPoint;       // Current route point
  sRoutePoint *Route;       // Route points

  char  Message[128];       // Text message
  long  MessageTimer;       // Text message timer
  D3DCOLOR MessageColor;    // Color of text message

  cObject Object;           // Character object class
  cMesh   WeaponMesh;       // Weapon mesh
  cObject WeaponObject;     // Weapon object

  sCharacter *Prev, *Next;  // Linked list of characters

  sCharacter()
  {
    Definition = 0;         // Set to definition #0
    ID = -1;                // Set to no ID
    Type = CHAR_NPC;        // Set to NPC character
    Enabled = FALSE;        // Set to not enabled
    
    Ailments = 0;           // Set no ailments
    Charge = 0.0f;          // Set no charge

    // Clear definition
    ZeroMemory(&Def, sizeof(sCharacterDefinition));
    CharICS = NULL;         // Set no ICS

    ScriptFilename[0] = 0;  // Set no script

    Action = CHAR_IDLE;     // Set default animation
    LastAnim = -1;          // Reset animation

    Locked = FALSE;         // Set no lock
    ActionTimer = 0;        // Set no action timer

    Attacker = NULL;        // Set no attacker
    Victim = NULL;          // Set no victim

    ItemNum = 0;            // Set no item to use
    CharItem = NULL;        // Set no item to decrease

    Distance = 0.0f;        // Set distance
    TargetChar = NULL;      // Set no target character

    // Clear bounding box (for limiting movement)
    MinX = MinY = MinZ = MaxX = MaxY = MaxZ = 0.0f;

    NumPoints = 0;          // Set no route points
    Route = NULL;           // Set no route

    Message[0] = 0;         // Clear message
    MessageTimer = 0;       // Set no message timer

    Prev = Next = NULL;     // Clear linked list pointers
  }

  ~sCharacter()
  {
    if(CharICS != NULL) {  // Release character ICS
      CharICS->Free();
      delete CharICS;
    }

    delete [] Route;       // Release route

    WeaponObject.Free();   // Release weapon object
    WeaponMesh.Free();     // Release weapon mesh
    Object.Free();         // Release character object

    delete Next;           // Delete next character in list
  }
} sCharacter;

class cCharacterController
{
  private:
    cGraphics  *m_Graphics;            // Parent graphics object
    cFont      *m_Font;                // Font object to use
    cFrustum   *m_Frustum;             // Viewing frustum

    char m_DefinitionFile[MAX_PATH];   // Filename of def. file

    sItem      *m_MIL;                 // Master item list
    sSpell     *m_MSL;                 // Master spell list

    cSpellController *m_SpellController; // Spell controller

    long        m_NumCharacters;       // # characters in list
    sCharacter *m_CharacterParent;     // List of characters

    long        m_NumMeshes;           // # meshes in use
    sCharacterMeshList *m_Meshes;      // Meshes list
    char m_MeshPath[MAX_PATH];         // Weapon mesh path
    char m_TexturePath[MAX_PATH];      // Mesh texture path

    long        m_NumAnimations;       // # animations
    sCharAnimationInfo *m_Animations;  // Animation data

    // Function to overide for playing sounds
    virtual BOOL ActionSound(sCharacter *Character)
            { return TRUE; }

    // Move function for player characters (need to override)
    virtual BOOL PCUpdate(                                    \
                   sCharacter *Character, long Elapsed,       \
                   float *XMove, float *YMove, float *ZMove)  \
            { return TRUE; }

    // Character update function for all non-PC characters
    BOOL CharUpdate(sCharacter *Character, long Elapsed,      \
                    float *XMove, float *YMove, float *ZMove);

    // Check for valid movements. Bounds check to other 
    // characters and call ValidateMove (overidden).
    BOOL CheckMove(sCharacter *Character,                     \
                   float *XMove, float *YMove, float *ZMove);

    // Virtual ValidateMove for outside bounds checking
    // against character movements.
    virtual BOOL ValidateMove(sCharacter *Character,          \
                   float *XMove, float *YMove, float *ZMove)  \
                   { return TRUE; }

    // Finish movement by setting direction, animation, etc
    BOOL ProcessUpdate(sCharacter *Character,                 \
                   float XMove, float YMove, float ZMove);

    // Process death of a player character
    virtual BOOL PCDeath(sCharacter *Character)               \
                 { return TRUE; }

    // Process death of a non-player character
    virtual BOOL NPCDeath(sCharacter *Character)              \
                 { return TRUE; }

    // Functions to drop money and item when character dies
    virtual BOOL DropMoney(float XPos,float YPos,float ZPos,  \
                           long Quantity)
                 { return TRUE; }

    virtual BOOL DropItem(float XPos, float YPos, float ZPos, \
                          long ItemNum)
                 { return TRUE; }

  public:
    cCharacterController();   // Constructor
    ~cCharacterController();  // Destructor

    // Functions to init/shutdown controller class
    BOOL Init(cGraphics *Graphics, cFont *Font,               \
              char *DefinitionFile, sItem *MIL, sSpell *MSL,  \
              long NumCharacterMeshes, char **MeshNames,      \
              char *MeshPath, char *TexturePath,              \
              long NumAnimations, sCharAnimationInfo *Anims,  \
              cSpellController *SpellController);
    BOOL Shutdown();

    // Free class
    BOOL Free();

    // Add a character to the list
    BOOL Add(long IDNum, long Definition, long Type, long AI, \
             float XPos, float YPos, float ZPos,              \
             float Direction = 0.0f);

    // Remove a character from list
    BOOL Remove(long IDNum);
    BOOL Remove(sCharacter *Character);

    // Save or load an individual character
    BOOL Save(long IDNum, char *Filename);
    BOOL Load(long IDNum, char *Filename);

    // Update all characters based on elapsed time
    BOOL Update(long Elapsed);

    // Render all objects within viewing frustum
    BOOL Render(long       Elapsed = -1,                      \
                cFrustum  *Frustum = NULL,                    \
                float      ZDistance = 0.0f);

    // Retrieve an sCharacter structure
    sCharacter *GetParentCharacter();
    sCharacter *GetCharacter(long IDNum);

    // Return X/Z radius of character
    float GetXZRadius(sCharacter *Character);

    // Make sure there's a line of sight between characters
    virtual BOOL LineOfSight(                                 \
                sCharacter *Source, sCharacter *Target,       \
                float SourceX, float SourceY, float SourceZ,  \
                float TargetX, float TargetY, float TargetZ)
                { return TRUE; }

    // Function to retrieve adjusted ability/other info
    float GetSpeed(sCharacter *Character);
    long  GetAttack(sCharacter *Character);
    long  GetDefense(sCharacter *Character);
    long  GetAgility(sCharacter *Character);
    long  GetResistance(sCharacter *Character);
    long  GetMental(sCharacter *Character);
    long  GetToHit(sCharacter *Character);
    float GetCharge(sCharacter *Character);

    // Get pointer to the character ICS
    cCharICS *GetICS(long IDNum);

    // Set lock and action timer
    BOOL SetLock(long IDNum, BOOL State);
    BOOL SetActionTimer(long IDNum, long Timer);

    // Set evade/follow distance
    BOOL  SetDistance(long IDNum, float Distance);
    float GetDistance(long IDNum);

    // Set route points
    BOOL SetRoute(long IDNum,                                 \
                  long NumPoints, sRoutePoint *Route);

    // Set script 
    BOOL SetScript(long IDNum, char *ScriptFilename);
    char *GetScript(long IDNum);

    // Set enable flags
    BOOL SetEnable(long IDNum, BOOL Enable);
    BOOL GetEnable(long IDNum);

    // Functions to move and get character coordinates
    BOOL Move(long IDNum, float XPos, float YPos, float ZPos);
    BOOL GetPosition(long IDNum,                              \
                     float *XPos, float *YPos, float *ZPos);

    // Functions to Set/Get character bounds
    BOOL SetBounds(long IDNum,                                \
                   float MinX, float MinY, float MinZ,        \
                   float MaxX, float MaxY, float MaxZ);
    BOOL GetBounds(long IDNum,                                \
                   float *MinX, float *MinY, float *MinZ,     \
                   float *MaxX, float *MaxY, float *MaxZ);

    // Functions to Set/Get character type
    BOOL SetType(long IDNum, long Type);
    long GetType(long IDNum);

    // Functions to Set/Get character AI
    BOOL SetAI(long IDNum, long Type);
    long GetAI(long IDNum);

    // Set a target character
    BOOL SetTargetCharacter(long IDNum, long TargetNum);

    // Set text messages to float up from character
    BOOL SetMessage(sCharacter *Character, char *Text,        \
                long Timer, D3DCOLOR Color=0xFFFFFFFF);

    // Process attack damage from spells and physical attacks
    BOOL Damage(sCharacter *Victim,                           \
                BOOL PhysicalAttack, long Amount,             \
                long DmgClass, long CureClass);

    // Process death of an NPC/Monster
    BOOL Death(sCharacter *Attacker, sCharacter *Victim);

    // Process experience up
    virtual BOOL Experience(sCharacter *Character,            \
                            long Amount)                      \
         { Character->Def.Experience += Amount; return TRUE; }

    // Resolve a physical attack from attacker to victim
    BOOL Attack(sCharacter *Attacker, sCharacter *Victim);

    // Process spell ailments when spell completed
    BOOL Spell(sCharacter *Caster,                            \
               sSpellTracker *SpellTracker, sSpell *Spells);
    
    // Apply spell effects
    BOOL SpellEffect(sCharacter *Caster, sCharacter *Target,  \
                     sSpell *Spell);

    // Process equipping/unequipping of item
    BOOL Equip(sCharacter *Character, long ItemNum,           \
               long Type, BOOL Equip);

    // Process item being used on character
    BOOL Item(sCharacter *Owner, sCharacter *Target,          \
              long ItemNum, sCharItem *CharItem = NULL);

    // Process dropping an item
    BOOL Drop(sCharacter *Character,                          \
              sCharItem *Item, long Quantity);

    // Process a PC teleport spell
    virtual BOOL PCTeleport(sCharacter *Character,            \
                            sSpell *Spell)                    \
            { return TRUE; }

    // Set action (w/timer)
    BOOL SetAction(sCharacter *Character,                     \
                   long Action, long AddTime = 0);

};

#endif
