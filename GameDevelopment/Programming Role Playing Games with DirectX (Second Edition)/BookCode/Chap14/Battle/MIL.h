#ifndef _MIL_H_
#define _MIL_H_

enum ItemCategories 
{ 
  MONEY = 0,
  WEAPON,
  ARMOR,
  SHIELD,
  ACCESSORY,
  EDIBLE,
  HEALING,
  COLLECTION,
  TRANSPORTATION,
  CONTAINER,
  OTHER 
};

#define SetUsageBit(v,c)   (v |= (1 << c))
#define ClearUsageBit(v,c) (v &= ((~(1 << c))
#define CheckUsageBit(v,c) (v & (1 << c))

enum {
  SELLABLE = 0,  // Bit 0
  CANDROP,       // Bit 1
  USEONCE,       // Bit 2
  UNKNOWN        // Bit 3
};

#define SetItemFlag(v,f)   (v |= (1 << f))
#define ClearItemFlag(v,f) (v &= ~(1 << f))
#define CheckItemFlag(v,f) (v & (1 << f))

typedef struct sItem 
{
  char  Name[32];          // A short name for the item
  char  Description[128];  // A description of item
  float Weight;            // Weight (in lbs.)
  float Size;              // Size (in cubic feet)
  long  Category;          // Category of item
  long  Value;             // Modifier, health increase, etc.
  long  Price;             // Buying price of item
  long  Flags;             // Item bit flags
  long  Usage;             // Usage restrictions

  char  ScriptFilename[16];  // .mls script filename
  char  MeshFilename[16];    // .X mesh filename
  char  ImageFilename[16];   // .bmp image filename
} sItem;

#endif
