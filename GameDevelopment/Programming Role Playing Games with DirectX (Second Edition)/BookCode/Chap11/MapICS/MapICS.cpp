#include <windows.h>
#include <stdio.h>
#include "MapICS.h"

cMapICS::cMapICS()
{
  m_NumItems = 0;
  m_ItemParent = NULL;
}

cMapICS::~cMapICS()
{
  Free();
}

BOOL cMapICS::Load(char *Filename)
{
  FILE *fp;
  long LongNum;
  sMapItem *Item, *ItemPtr = NULL;

  Free();  // Free a prior set

  // Open the file
  if((fp=fopen(Filename, "rb"))==NULL)
    return FALSE;

  // Loop forever reading in items
  while(1) {
    // Get next item number (break if no more items,
    // which is represented by a return value of -1).
    if((LongNum = GetNextLong(fp)) == -1)
      break;

    // Create a new map pointer and link it in
    Item = new sMapItem();
    if(ItemPtr == NULL)
      m_ItemParent = Item;
    else {
      Item->Prev = ItemPtr;
      ItemPtr->Next = Item;
    }
    ItemPtr = Item;

    // Store MIL item number
    Item->ItemNum = LongNum;

    // Get quantity
    Item->Quantity = GetNextLong(fp);

    // Get coordinates
    Item->XPos = GetNextFloat(fp);
    Item->YPos = GetNextFloat(fp);
    Item->ZPos = GetNextFloat(fp);

    // Get owner #
    Item->Owner = GetNextLong(fp);

    // Save index # and increase count
    Item->Index = m_NumItems++;
  }

  // Close the file
  fclose(fp);

  // Match objects that belong to others
  ItemPtr = m_ItemParent;
  while(ItemPtr != NULL) {

    // Check if this item belongs to another
    if(ItemPtr->Owner != -1) {

      // Find matching parent item
      Item = m_ItemParent;
      while(Item != NULL) {

        if(ItemPtr->Owner == Item->Index) {
          // A match, point to parent
          ItemPtr->Parent = Item;
          break; // Stop scanning for parents
        }

        Item = Item->Next;
      }
    }

    // Go to next item
    ItemPtr = ItemPtr->Next;
  }

  return TRUE;
}

BOOL cMapICS::Save(char *Filename)
{
  FILE *fp;
  sMapItem *Item;
  long Index = 0;

  // Open the file
  if((fp=fopen(Filename, "wb"))==NULL)
    return FALSE;

  // Assign index numbers to items
  if((Item = m_ItemParent) == NULL) {
    fclose(fp);
    return TRUE; // no items to save
  }
  while(Item != NULL) {
    Item->Index = Index++;
    Item = Item->Next;
  }

  // Match child items to parents
  Item = m_ItemParent;
  while(Item != NULL) {
    if(Item->Parent != NULL)
      Item->Owner = Item->Parent->Index;
    else
      Item->Owner = -1;
    Item = Item->Next;
  }

  // Save 'em out
  Item = m_ItemParent;
  while(Item != NULL) {
    // Item number
    fprintf(fp, "%lu\r\n", Item->ItemNum);

    // Quantity
    fprintf(fp, "%lu\r\n", Item->Quantity);

    // Coordinates
    fprintf(fp, "%lf\r\n%lf\r\n%lf\r\n", Item->XPos, Item->YPos, Item->ZPos);
     
    // Owner #
    fprintf(fp, "%ld\r\n", Item->Owner);

    // Next item
    Item = Item->Next;
  }

  fclose(fp); // Close the file

  return TRUE; // Return success!
}

BOOL cMapICS::Free()
{
  m_NumItems = 0;
  delete m_ItemParent;
  m_ItemParent = NULL;
  return TRUE;
}

BOOL cMapICS::Add(long ItemNum, long Quantity,                \
                  float XPos, float YPos, float ZPos,         \
                  sMapItem *OwnerItem)
{
  sMapItem *Item;

  // Create a new item structure
  Item = new sMapItem();

  // Insert into top of list
  Item->Next = m_ItemParent;
  if(m_ItemParent != NULL)
    m_ItemParent->Prev = Item;
  m_ItemParent = Item;

  // Fill the item structure
  Item->ItemNum  = ItemNum;
  Item->Quantity = Quantity;
  Item->XPos     = XPos;
  Item->YPos     = YPos;
  Item->ZPos     = ZPos;
  Item->Parent   = OwnerItem;
  
  return TRUE;
}

BOOL cMapICS::Remove(sMapItem *Item)
{
  sMapItem *ItemPtr, *NextItem;
  
  // Remove child objects first
  if((ItemPtr = m_ItemParent) != NULL) {
    while(ItemPtr != NULL) {
      NextItem = ItemPtr->Next;
      if(ItemPtr->Parent == Item)
        Remove(ItemPtr);
      ItemPtr = NextItem;
    }
  }

  // Remove from linked list and reset root
  // if it's the current head of list.
  if(Item->Prev != NULL)
    Item->Prev->Next = Item->Next;
  else
    m_ItemParent = Item->Next;
  if(Item->Next != NULL)
    Item->Next->Prev = Item->Prev;

  // Clear link list
  Item->Prev = Item->Next = NULL;

  // Free memory
  delete Item;

  return TRUE;
}

long cMapICS::GetNumItems()
{
  return m_NumItems;
}

sMapItem *cMapICS::GetParentItem()
{
  return m_ItemParent;
}

sMapItem *cMapICS::GetItem(long Num)
{
  sMapItem *Item;

  Item = m_ItemParent;
  while(Num--—) { // Loop until reached item num
    if(Item == NULL)
      return NULL;
    Item = Item->Next;
  }
  return Item;
}

long cMapICS::GetNextLong(FILE *fp)
{
  char Buf[1024];
  long Pos = 0;
  int c;

  // Read until EOF or EOL
  while(1) {
    if((c = fgetc(fp)) == EOF)
      break;
    if(c == 0x0a)
      break;
    if((c >= '0' && c <= '9') || c == '.' || c == '-')
      Buf[Pos++] = c;
  }
  if(!Pos)
    return -1;
  Buf[Pos] = 0;

  return atol(Buf);
}

float cMapICS::GetNextFloat(FILE *fp)
{
  char Buf[1024];
  long Pos = 0;
  int c;

  // Read until EOF or EOL
  while(1) {
    if((c = fgetc(fp)) == EOF)
      break;
    if(c == 0x0a)
      break;
    if((c >= '0' && c <= '9') || c == '.' || c == '-')
      Buf[Pos++] = c;
  }
  Buf[Pos] = 0;

  return (float)atof(Buf);
}
