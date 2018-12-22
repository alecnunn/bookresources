/*___________________________________________________________________
|
|	File: list.h
|
| (C) Copyright 2004-2005 Timothy E. Roden
|___________________________________________________________________*/

/*___________________
|
| Type definitions
|__________________*/

typedef void *List;

/*___________________
|
| Functions
|__________________*/

List list_Init         (unsigned item_size);
void list_Free         (List list);
void list_Copy         (List dst, List src);
bool list_Empty        (List list);
void list_Flush        (List list);
int  list_Size         (List list);
void list_AddStart     (List list, void *item);
void list_AddEnd       (List list, void *item);
bool list_RemoveStart  (List list, void *item);
bool list_RemoveStart  (List list);
bool list_RemoveEnd    (List list, void *item);
bool list_RemoveEnd    (List list);
bool list_ContainsItem (List list, void *item);
