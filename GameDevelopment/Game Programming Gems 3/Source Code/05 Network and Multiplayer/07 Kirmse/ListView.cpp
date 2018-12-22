#include "main.h"

/***************************************************************/
CListView::CListView(HWND new_list)
{
   mhList = new_list;
   ListView_SetExtendedListViewStyle(mhList, LVS_EX_FULLROWSELECT);
}
/***************************************************************/
CListView::~CListView()
{
}
/***************************************************************/
void CListView::SetColumns(const ListViewColumn *columns)
{
   const ListViewColumn *c = columns;
   int32 i = 0;

   while (c->name != NULL)
   {
      LVCOLUMN lvc;
      lvc.mask     = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
      lvc.fmt      = LVCFMT_LEFT;
      lvc.cx       = c->width;
      lvc.pszText  = c->name;
      lvc.iSubItem = i;

      ListView_InsertColumn(mhList, i, &lvc);

      c++;
      i++;
   }
}
/***************************************************************/
void CListView::AddItem(void *data)
{
   LVITEM lvi;
   lvi.mask     = LVIF_PARAM | LVIF_TEXT;
   lvi.pszText  = LPSTR_TEXTCALLBACK;
   lvi.lParam   = (LPARAM) data;
   lvi.iItem    = ListView_GetItemCount(mhList);
   lvi.iSubItem = 0;
   ListView_InsertItem(mhList, &lvi);
}
/***************************************************************/
int32 CListView::AddNewRow(char *str)
{
   int32 index = ListView_GetItemCount(mhList);
   
   LVITEM lvi;
   lvi.mask     = LVIF_TEXT;
   lvi.pszText  = str;
   lvi.iItem    = index;
   lvi.iSubItem = 0;
   ListView_InsertItem(mhList, &lvi);

   return index;
}
/***************************************************************/
void CListView::AddString(char *str, int32 row, int32 col)
{
   LVITEM lvi;
   lvi.mask     = LVIF_TEXT;
   lvi.pszText  = str;
   lvi.iItem    = row;
   lvi.iSubItem = col;
   ListView_SetItem(mhList, &lvi);
}
/***************************************************************/
int32 CListView::GetSelectedIndex() const
{
   return ListView_GetNextItem(mhList, -1, LVNI_SELECTED);
}
/***************************************************************/
void *CListView::GetSelectedItem() const
{
   int32 index = ListView_GetNextItem(mhList, -1, LVNI_SELECTED);
   if (index == -1)
      return NULL;

   LVITEM lvi;
   lvi.mask     = LVIF_PARAM;
   lvi.iItem    = index;
   lvi.iSubItem = 0;
   if (!ListView_GetItem(mhList, &lvi))
      return NULL;
   
   return (void *) lvi.lParam;
}
/***************************************************************/
void *CListView::RemoveItem(int32 index)
{
   LVITEM lvi;
   lvi.mask     = LVIF_PARAM;
   lvi.iItem    = index;
   lvi.iSubItem = 0;
   if (!ListView_GetItem(mhList, &lvi))
      return NULL;
   
   void *retval = (void *) lvi.lParam;

   ListView_DeleteItem(mhList, index);

   // Set selection to next item
   SetSelectedItem(index);

   return retval;
}
/***************************************************************/
void CListView::ScrollToBottom()
{
   ListView_EnsureVisible(mhList, ListView_GetItemCount(mhList) - 1, false);
}
/***************************************************************/
void CListView::SetSelectedItem(int32 index)
{
   int32 count = ListView_GetItemCount(mhList);
   if (index >= count)
      index = max(0, count - 1);
   ListView_SetItemState(mhList, index, LVIS_SELECTED, LVIS_SELECTED);
}
/***************************************************************/
void CListView::Clear()
{
   ListView_DeleteAllItems(mhList);
}
/***************************************************************/
int32 CListView::GetNumColumns() const
{
   int num_columns = 0;
   while (true)
   {
      LVCOLUMN column;
      column.mask = LVCF_FMT;
      
      if (FALSE == ListView_GetColumn(mhList, num_columns, &column))
         return num_columns;

      ++num_columns;
   }
}
/***************************************************************/
int32 CListView::GetColumnWidth(int32 index) const
{
   return ListView_GetColumnWidth(mhList, index);
}
/***************************************************************/
void  CListView::SetColumnWidth(int32 index, int32 width)
{
   ListView_SetColumnWidth(mhList, index, width);
}
