#ifndef _MSL_H_
#define _MSL_H_

typedef struct sSpell
{
  char  Name[32];           // Name of character
  char  Description[128];   // Description of spell
  
  long  DmgClass;           // Class that spell does 2x dmg
  long  CureClass;          // Class that spell aids

  long  Cost;               // Spell casting cost in MP

  float Distance;           // Max. distance to target

  long  Effect;             // Spell effect
  long  Chance;             // % of effect occurring
  float Value[4];           // Misc. values

  long  Target;             // Target of spell
  float Range;              // Range (in game units)

  long  MeshNum[3];         // Mesh # to use
  long  MeshPos[3];         // Positioning of mesh
  float MeshSpeed[3];       // Speed of mesh movement
  long  MeshSound[3];       // Sound effect # to play
  BOOL  MeshLoop[3];        // Loop animation
} sSpell;

#endif
