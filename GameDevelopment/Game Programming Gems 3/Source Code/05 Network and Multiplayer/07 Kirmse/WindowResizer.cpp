#include "main.h"


/***************************************************************/
static void GetWindowParentRect(HWND hwnd, RECT *r)
{
   POINT p = {0, 0};

   ClientToScreen(GetParent(hwnd), &p);
   GetWindowRect(hwnd, r);
   OffsetRect(r, -p.x, -p.y);
}


/***************************************************************/
WindowResizer::WindowResizer(HWND hParent, ResizeItem *new_items)
{
   mpItems = CopyResizeItems(new_items);
   mpResize = AllocResize(hParent, mpItems);
}
/***************************************************************/
WindowResizer::~WindowResizer()
{
   FreeResize(mpResize);
   FreeResizeItems(mpItems);
}
/***************************************************************/
Resize *WindowResizer::AllocResize(HWND hParent, ResizeItem *items)
{
   Resize *r = (Resize *) malloc(sizeof(Resize));
   r->items = mpItems;

   ResizeItem *ri;
   for (ri = items; ri->type != RA_END; ri++)
      if (ri->subitems == NULL)
	 ri->subwindow = NULL;
      else
      {
	 HWND hControl;
	 if (ri->type == RA_ID)
	    hControl = GetDlgItem(hParent, ri->u.id);
	 else 
	    hControl = ri->u.hwnd;
	 if (hControl != NULL)
	    ri->subwindow = AllocResize(hControl, ri->subitems);
      }

   GetClientRect(hParent, &r->rect);
   return r;
}
/***************************************************************/
void WindowResizer::FreeResize(Resize *resize)
{
   ResizeItem *ri;
   for (ri = mpResize->items; ri->type != RA_END; ri++)
   {
      if (ri->subwindow != NULL)
	 FreeResize(ri->subwindow);
   }
   free(resize);
}
/***************************************************************/
void WindowResizer::OnSize(HWND hwnd, UINT state, int32 width, int32 height)
{
   if (state == SIZE_MINIMIZED)
      return;
   ResizeWindow(hwnd, mpResize);
}
/***************************************************************/
void WindowResizer::SetWindow(int32 index, HWND hwnd)
{
   assert(index >= 0);
   mpItems[index].u.hwnd = hwnd;
}
/***************************************************************/
ResizeItem *WindowResizer::CopyResizeItems(ResizeItem *items)
{
   int32 size;
   
   ResizeItem *ri;
   int32 num = 1;  // Account for terminating entry
   for (ri = items; ri->type != RA_END; ri++)
      num++;
   size = num * sizeof(ResizeItem);
   ResizeItem *r = (ResizeItem *) malloc(size);
   memcpy(r, items, size);
   return r;
}
/***************************************************************/
void WindowResizer::FreeResizeItems(ResizeItem *items)
{
   ResizeItem *ri;
   int32 num = 1;  // Account for terminating entry
   for (ri = items; ri->type != RA_END; ri++)
      num++;
   free(items);
}
/***************************************************************/
void WindowResizer::ResizeWindow(HWND hParent, Resize *r)
{
   int32 dx, dy;
   HWND hControl;
   RECT parent_rect, rect;
   ResizeItem *ri;
   
   // Calculate change in width and height of parent window
   GetClientRect(hParent, &parent_rect);
   dx = parent_rect.right - r->rect.right;
   dy = parent_rect.bottom - r->rect.bottom;

   for (ri = r->items; ri->type != RA_END; ri++)
   {
      if (ri->type == RA_ID)
	 hControl = GetDlgItem(hParent, ri->u.id);
      else 
	 hControl = ri->u.hwnd;

      if (hControl == NULL)
	 continue;
      
      // Allocate space for subwindows that are added after initialization
      if (ri->subitems != NULL && ri->subwindow == NULL)
	 ri->subwindow = AllocResize(hControl, ri->subitems);

      // Get control's rectangle relative to parent
      GetWindowParentRect(hControl, &rect);

      if (!(ri->action & RA_LEFT))
	 rect.left += dx;

      if (!(ri->action & RA_TOP))
	 rect.top += dy;

      if (ri->action & RA_RIGHT)
	 rect.right += dx;

      if (ri->action & RA_BOTTOM)
	 rect.bottom += dy;
      
      MoveWindow(hControl, rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top), TRUE);

      // Take care of subcontrols, if appropriate
      if (ri->subwindow != NULL)
	 ResizeWindow(hControl, ri->subwindow);
   }
   // Record parent window's new location
   memcpy(&r->rect, &parent_rect, sizeof(RECT));
}
