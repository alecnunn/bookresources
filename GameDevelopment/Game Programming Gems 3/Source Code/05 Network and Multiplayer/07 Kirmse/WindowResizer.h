// A class that resizes a window's subcontrols.
// To use this class, create a WindowResizer object as a result of the
// WM_CREATE or WM_INITDIALOG message, and delete it on WM_DESTROY.
// Then pass WM_SIZE messages to the OnSize method of the
// WindowResizer object.

#ifndef _WINDOWRESIZER_H
#define _WINDOWRESIZER_H

// Which edges of a control are anchored to the corresponding side of the parent window
enum
{
   RA_LEFT   = 0x01,
   RA_RIGHT  = 0x02,
   RA_TOP    = 0x04,
   RA_BOTTOM = 0x08,
   RA_ALL    = 0x0F,
};

enum
{
   RA_END  = 0,
   RA_ID   = 1,
   RA_HWND = 2,
};

struct Resize;

struct ResizeItem
{
   int32 type;             // Either RA_ID or RA_HWND, to indicate which member of
                           // u is used; or RA_END to signify last entry.
   union                   // Can identify a child window by control id or by handle 
   {                 
      int32        id;     // Window control id
      HWND         hwnd;   // Window handle   
   } u;
   uint32       action;    // What to do when control is resized
   ResizeItem  *subitems;  // How to resize subwindows; NULL if none 
   Resize      *subwindow; // Points to Resize structure; NULL if no subwindows (internal use only)
};

struct Resize
{
   RECT rect;              // Screen rectangle of this window
   ResizeItem *items;      // How to resize subitems
};

class WindowResizer
{
public:
   // Makes an internal copy of new_items
   WindowResizer(HWND hParent, ResizeItem *new_items);
   ~WindowResizer();
 
   // Handles WM_SIZE messages
   void OnSize(HWND hwnd, UINT state, int32 width, int32 height);

   // Set the index-th entry in the ResizeItem structure to have the given window.
   void SetWindow(int32 index, HWND hwnd);
   
private:  
   // Return a newly allocated copy of items.
   ResizeItem *CopyResizeItems(ResizeItem *items);
   void FreeResizeItems(ResizeItem *items);
   
   // Set up/free resizing structures
   Resize *AllocResize(HWND hParent, ResizeItem *items);
   void FreeResize(Resize *resize);

   // Recursively resizes hParent's subwindows, according to r.
   void ResizeWindow(HWND hParent, Resize *r);

   Resize     *mpResize;
   ResizeItem *mpItems;
};

#endif
