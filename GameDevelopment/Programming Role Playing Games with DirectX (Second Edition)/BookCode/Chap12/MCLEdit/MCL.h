#ifndef _MCL_H_
#define _MCL_H_

typedef struct sCharacterDefinition
{
  // Misc data
  char  Name[32];           // Name of character
  long  Class;              // Class # of character
  long  Money;              // Amount of money
  float Speed;              // Movement speed
  long  MagicSpells[2];     // Bit flags to mark known spells
  long  MeshNum;            // Mesh/anim # to load

  // Abilities
  long  Agility;            // Agility ability
  long  Attack;             // Attack ability
  long  Defense;            // Defend ability
  long  Resistance;         // Magic resistance ability
  long  Mental;             // Mental ability
  long  ToHit;              // Chance to hit

  // Attributes
  long  HealthPoints;       // # health points (maximum)
  long  ManaPoints;         // # mana points (maximum)
  long  Level;              // Experience level
  long  Experience;         // Experience points

  // Inventory
  char  ItemFilename[MAX_PATH]; // CharICS filename 
  long  Weapon;             // Equipped Weapon
  long  Armor;              // Equipped Armor
  long  Shield;             // Equipped Shield
  long  Accessory;          // Equipped Accessory

  // Dropping item data
  long  DropChance;         // % of dropping item when killed
  long  DropItem;           // Item # to drop when killed
  
  // Attack/Magic chances and effects
  float Range;              // Attack range
  float ChargeRate;         // Countdown rate to attack
  long  ToAttack;           // Percent to attack
  long  ToMagic;            // Percent to use magic
  long  EffectChance;       // Chance of attack effect occuring
  long  Effects;            // Bit flags of attack effects
} sCharacterDefinition;

#endif
