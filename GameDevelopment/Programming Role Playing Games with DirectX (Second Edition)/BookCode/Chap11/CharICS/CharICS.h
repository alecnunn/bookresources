#ifndef _CHARICS_H_
#define _CHARICS_H_

typedef struct sCharItem
{
  long      ItemNum;          // MIL item number
  long      Quantity;         // Quantity of item (ie coins)

  sCharItem  *Prev, *Next;    // linked list pointers

  long       Index;           // This items index #
  long       Owner;           // Owner index #
  sCharItem  *Parent;         // Parent of a contained item

  sCharItem()
  { 
    Prev = Next = Parent = NULL;
    Index = 0; Owner = -1;
   }
   
  ~sCharItem() { delete Next; Next = NULL; } 
} sCharItem;

class cCharICS
{
  private:
    long       m_NumItems;    // # items in inventory
    sCharItem *m_ItemParent;  // Linked list parent item
    
    // Functions to read in next long or float # in file
    long  GetNextLong(FILE *fp);
    float GetNextFloat(FILE *fp);

  public:
    cCharICS();   // Constructor
    ~cCharICS();  // Destructor

    // Load, save, and free a list of items
    BOOL Load(char *Filename);
    BOOL Save(char *Filename);
    BOOL Free();

    // Add and remove an item
    BOOL Add(long ItemNum, long Quantity,    
             sCharItem *OwnerItem = NULL);
    BOOL Remove(sCharItem *Item);

    // Retrieve # items or parent linked list object
    long      GetNumItems();
    sCharItem *GetParentItem();
    sCharItem *GetItem(long Num);

    // Re-ordering functions
    BOOL Arrange();
    BOOL MoveUp(sCharItem *Item);
    BOOL MoveDown(sCharItem *Item);
};

#endif
