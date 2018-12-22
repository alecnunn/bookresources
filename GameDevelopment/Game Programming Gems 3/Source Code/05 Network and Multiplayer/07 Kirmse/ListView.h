/*
 * ListView
 *
 * Encapsulates some of the behavior of a Windows ListView control.
 *
 */

#ifndef _LISTVIEW_H_
#define _LISTVIEW_H_

struct ListViewColumn
{
   char *name;   // NULL to indicate end of list
   int32 width;
};

class CListView
{
public:

   CListView(HWND new_list);
   ~CListView();

   void SetColumns(const ListViewColumn *columns);

   // Add the item to the end of the list view.  item is the "lParam" value.
   void AddItem(void *data);

   // Add the string to the end of the list view and return its row index.
   int32 AddNewRow(char *str);

   // Add the string to the given row and column
   void AddString(char *str, int32 row, int32 col);

   // Return the currently selected index, or NULL if none.
   int32 GetSelectedIndex() const;

   // Return the lParam value for the currently selected item, or NULL if none.
   void *GetSelectedItem() const;

   // Remove the item with the given index and return its lParam value,
   // or NULL if removal failed.
   void *RemoveItem(int32 index);

   // Make sure the last row is in view
   void  ScrollToBottom();

   // Make the item with the given index selected
   void  SetSelectedItem(int32 index);

   // Make the list view empty
   void  Clear();

   int32 GetNumColumns() const;
   int32 GetColumnWidth(int32 index) const;
   void  SetColumnWidth(int32 index, int32 width);
   
private:
   
   HWND mhList;
};

#endif
