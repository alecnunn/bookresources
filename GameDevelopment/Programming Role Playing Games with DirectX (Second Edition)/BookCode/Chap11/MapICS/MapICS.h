#ifndef _MAPICS_H_
#define _MAPICS_H_

typedef struct sMapItem
{
  long      ItemNum;          // MIL item number
  long      Quantity;         // Quantity of item (ie coins)
  float     XPos, YPos, ZPos; // Map coordinates

  sMapItem  *Prev, *Next;     // linked list pointers

  long       Index;           // This items index #
  long       Owner;           // Owner index #
  sMapItem  *Parent;          // Parent of a contained item

  sMapItem()  
  { 
    Prev = Next = Parent = NULL;
    Index = 0; Owner = -1;
   }
   
  ~sMapItem() { delete Next; Next = NULL; } 
} sMapItem;

class cMapICS
{
  private:
    long      m_NumItems;    // # items in map
    sMapItem *m_ItemParent;  // Linked list parent map item
    
    // Functions to read in next long or float # in file
    long  GetNextLong(FILE *fp);
    float GetNextFloat(FILE *fp);

  public:
    cMapICS();   // Constructor
    ~cMapICS();  // Destructor

    // Load, save, and free a list of map items
    BOOL Load(char *Filename);
    BOOL Save(char *Filename);
    BOOL Free();

    // Add and remove an item on map
    BOOL Add(long ItemNum, long Quantity,                     \
             float XPos, float YPos, float ZPos,              \
             sMapItem *OwnerItem = NULL);
    BOOL Remove(sMapItem *Item);

    // Retrieve # items or parent linked list object
    long      GetNumItems();
    sMapItem *GetParentItem();
    sMapItem *GetItem(long Num);
};

#endif
