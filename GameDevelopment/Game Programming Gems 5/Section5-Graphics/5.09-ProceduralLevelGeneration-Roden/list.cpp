/*___________________________________________________________________
|
|	File: list.cpp
|
| Description: Functions for a doubly linked list.
|
| Functions:  list_Init
|             list_Free
|             list_Copy
|             list_Empty
|             list_Flush
|             list_Size
|             list_AddStart
|             list_AddEnd
|             list_RemoveStart
|             list_RemoveStart
|             list_RemoveEnd
|             list_RemoveEnd
|             list_ContainsItem
|
| (C) Copyright 2004-2005 Timothy E. Roden
|___________________________________________________________________*/

/*___________________
|
| Include Files
|__________________*/

#include <windows.h>
#include <winbase.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "list.h"

/*___________________
|
| Type definitions
|__________________*/

struct Node {
  void *item;
  Node *next, *previous;
};

struct ListData {
  Node *start, *end;
  int count;
  unsigned item_size;
};

/*___________________
|
| Macros
|__________________*/

#define LIST ((ListData *)list)

/*____________________________________________________________________
|
| Function: list_Init
|
| Output: Initializes a list.  Returns true if initialized, else false 
|   on any error.
|___________________________________________________________________*/

List list_Init (unsigned item_size)
{
  ListData *listdata = 0;

  listdata = (ListData *) malloc (sizeof(ListData));
  if (listdata) {
    listdata->start = 0;
    listdata->end   = 0;
    listdata->count = 0;
    listdata->item_size = item_size;
  }

  return ((List)listdata);
}

/*____________________________________________________________________
|
| Function: list_Free
|
| Output: Frees a list.
|___________________________________________________________________*/

void list_Free (List list)
{
  list_Flush (list);
  free (LIST);
}

/*____________________________________________________________________
|
| Function: list_Copy
|
| Output: Performs deep copy from src list to dst list.
|___________________________________________________________________*/

void list_Copy (List dst, List src)
{
  Node *np;

  if (dst && src) {
    list_Flush (dst);
    for (np=((ListData *)src)->start; np; np=np->next)
      list_AddEnd (dst, np->item);
  }
}

/*____________________________________________________________________
|
| Function: list_Empty
|
| Output: Returns true if list is empty.
|___________________________________________________________________*/

bool list_Empty (List list)
{
  return (LIST->count == 0);
}

/*____________________________________________________________________
|
| Function: list_Flush
|
| Output: Flushes all data in list.
|___________________________________________________________________*/

void list_Flush (List list)
{
  Node *temp;

  while (LIST->start) {
    temp = LIST->start;
    LIST->start = LIST->start->next;
    free (temp->item);
    free (temp);
  }
  LIST->end = 0;
  LIST->count = 0;
}

/*____________________________________________________________________
|
| Function: list_Size
|
| Output: Returns # of items in list.
|___________________________________________________________________*/

int list_Size (List list)
{
  return (LIST->count);
}

/*____________________________________________________________________
|
| Function: list_AddEnd
|
| Output: Adds an item to the end of the list.
|___________________________________________________________________*/

void list_AddEnd (List list, void *item)
{
  Node *newnode;

  newnode = (Node *) malloc (sizeof(Node));
  newnode->item = (byte *) malloc (LIST->item_size * sizeof(byte));
  memcpy (newnode->item, item, LIST->item_size);
  newnode->next     = 0;
  newnode->previous = 0;

  if (LIST->start == 0)
    LIST->start = newnode;
  else {
    LIST->end->next = newnode;
    newnode->previous = LIST->end;
  }
  LIST->end = newnode;
  LIST->count++;
}

/*____________________________________________________________________
|
| Function: list_AddStart
|
| Output: Adds an item to the start of the list.
|___________________________________________________________________*/

void list_AddStart (List list, void *item)
{
  Node *newnode;

  newnode = (Node *) malloc (sizeof(Node));
  newnode->item = (byte *) malloc (LIST->item_size * sizeof(byte));
  memcpy (newnode->item, item, LIST->item_size);
  newnode->next     = LIST->start;
  newnode->previous = 0;
  LIST->start       = newnode;
  if (LIST->end == 0)
    LIST->end = newnode;
  LIST->count++;
}

/*____________________________________________________________________
|
| Function: list_RemoveStart
|
| Output: Removes an item from the start of list.  Returns true if removed,
|   else false.
|___________________________________________________________________*/

bool list_RemoveStart (List list, void *item)
{
  Node *temp;
  bool removed = false;

  if (LIST->count) {
    memcpy (item, LIST->start->item, LIST->item_size);
    temp = LIST->start;
    LIST->start = LIST->start->next;
    if (LIST->start)
      LIST->start->previous = 0;
    else
      LIST->end = 0;
    free (temp->item);
    free (temp);
    LIST->count--;
    removed = true;
  }

  return (removed);
}

/*____________________________________________________________________
|
| Function: list_RemoveStart
|
| Output: Removes an item from the start of list.  Returns true if removed,
|   else false.
|___________________________________________________________________*/

bool list_RemoveStart (List list)
{
  Node *temp;
  bool removed = false;

  if (LIST->count) {
    temp = LIST->start;
    LIST->start = LIST->start->next;
    if (LIST->start)
      LIST->start->previous = 0;
    else
      LIST->end = 0;
    free (temp->item);
    free (temp);
    LIST->count--;
    removed = true;
  }

  return (removed);
}

/*____________________________________________________________________
|
| Function: list_RemoveEnd
|
| Output: Removes an item from the end of list.  Returns true if removed,
|   else false.
|___________________________________________________________________*/

bool list_RemoveEnd (List list, void *item)
{
  Node *temp;
  bool removed = false;

  if (LIST->count) {
    memcpy (item, LIST->end->item, LIST->item_size);
    temp = LIST->end;
    LIST->end = LIST->end->previous;
    if (LIST->end)
      LIST->end->next = 0;
    else
      LIST->start = 0;
    free (temp->item);
    free (temp);
    LIST->count--;
    removed = true;
  }

  return (removed);
}

/*____________________________________________________________________
|
| Function: list_RemoveEnd
|
| Output: Removes an item from the end of list.  Returns true if removed,
|   else false.
|___________________________________________________________________*/

bool list_RemoveEnd (List list)
{
  Node *temp;
  bool removed = false;

  if (LIST->count) {
    temp = LIST->end;
    LIST->end = LIST->end->previous;
    if (LIST->end)
      LIST->end->next = 0;
    else
      LIST->start = 0;
    free (temp->item);
    free (temp);
    LIST->count--;
    removed = true;
  }

  return (removed);
}

/*____________________________________________________________________
|
| Function: list_ContainsItem
|
| Output: Returns true if list contains at least one instance of item.
|   Performs a linear search.
|___________________________________________________________________*/

bool list_ContainsItem (List list, void *item)
{
  bool found;
  Node *np;

  for (np=LIST->start, found = false; np && (! found); np=np->next)
    if (!memcmp (np->item, item, LIST->item_size))
      found = true;

  return (found);
}
