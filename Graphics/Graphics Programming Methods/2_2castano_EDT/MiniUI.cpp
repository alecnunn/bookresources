

/*---------------------------------------------------------------------------
	Includes:
---------------------------------------------------------------------------*/

#include "EDT.h"
//#include "LinkList.h"

// From LinkList

/// @hideinitializer Add a node at the beginning of the double linked list.
#define LL_Add( rootnode, newnode, next, prev ) 				\
	{                                              				\
		(newnode)->prev = (rootnode);                  			\
		(newnode)->next = (rootnode)->next;					   	\
		(rootnode)->next->prev = (newnode);                		\
		(rootnode)->next = (newnode);                      		\
	}

/// @hideinitializer Remove a node from the linked list.
#define LL_Remove(node,next,prev)								\
	{															\
		node->prev->next = node->next;							\
		node->next->prev = node->prev;							\
		node->next = node;										\
		node->prev = node;										\
	}

/// @hideinitializer Reset node pointers.
#define LL_Reset(list,next,prev)	(list)->next = (list)->prev = (list)



/*---------------------------------------------------------------------------
	Structs:
---------------------------------------------------------------------------*/

struct MiniWindow {
	HWND			hwnd;
	HDC				wnddc;
	HBITMAP			dib;
	HDC				dibdc;
	const Image *	img;

	uint8 *			buffer;

	MiniWindow *	next;
	MiniWindow *	prev;
};



/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern HINSTANCE GInstance;

MiniWindow	windows;

static ATOM atom;


/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/

static void MiniWindowBlit( MiniWindow * wnd ) {

	RECT wndrect;
	GetClientRect( wnd->hwnd, &wndrect );
	int	w = wndrect.right - wndrect.left;
	int	h = wndrect.bottom - wndrect.top;
//	StretchDIBits(window_hdc, wndrect.left, wndrect.top, w, h, 0, 0, screen->w, screen->h, screen->buffer, bmi, DIB_RGB_COLORS, SRCCOPY);
//	StretchBlt( window_hdc, wndrect.left, wndrect.top, w, h, dib_hdc, 0, 0, screen->w, screen->h, SRCCOPY );
	BitBlt( wnd->wnddc, wndrect.left, wndrect.top, w, h, wnd->dibdc, 0, 0, SRCCOPY );
}


static LONG WINAPI MiniWindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam ) {
	MiniWindow * wnd = (MiniWindow *) GetWindowLong( hwnd, GWL_USERDATA );

	switch( msg ) {
		case WM_CREATE:
			wnd = (MiniWindow *)((CREATESTRUCT *)lparam)->lpCreateParams;
			SetWindowLong( hwnd, GWL_USERDATA, (LONG)wnd );
			return 0;
		break;
		case WM_DESTROY:
			LL_Remove( wnd, next, prev );
			if( wnd->dib ) DeleteObject( wnd->dib );
			if( wnd->dibdc ) DeleteDC( wnd->dibdc );
			delete wnd;
			return 0;
		break;
		case WM_PAINT:
			ValidateRect( hwnd, NULL );
			MiniWindowBlit( wnd );
			return 0;
		break;
		case WM_KEYDOWN:
			if( wparam == VK_ESCAPE ) {
				DestroyWindow( hwnd );
				return 0;
			}
		break;
	}

	// default action
	return DefWindowProc( hwnd, msg, wparam, lparam );
}






/** Init the mini windows. */
void MiniUI::Init( void ) {
	LL_Reset( &windows, next, prev );

	// Create window class
	WNDCLASS cls = { 
		CS_VREDRAW | CS_HREDRAW | CS_OWNDC, 
		(WNDPROC)MiniWindowProc,
        0, 
		0, 
		GInstance, 
		NULL,
		NULL, 
		(HBRUSH)GetStockObject(BLACK_BRUSH),
		0, 
		"MiniWindow" 
	};

	atom = RegisterClass(&cls);
}


/** Shut the mini gui. */
void MiniUI::Shut( void ) {
	UnregisterClass( (const char *)atom, GInstance );
}





/** Add a window. */
void MiniUI::AddWindow( const Image * img, const char * title, int wnd_x, int wnd_y ) {

	MiniWindow * wnd = new MiniWindow;
	wnd->img = img;
	LL_Add( &windows, wnd, next, prev );


	RECT rect = { 0, 0, img->w, img->h };
	AdjustWindowRectEx( &rect, WS_CAPTION|WS_SYSMENU, 0, WS_EX_TOOLWINDOW );

	int wnd_w = rect.right - rect.left;
	int wnd_h = rect.bottom - rect.top;

	if( wnd_x==-1 ) wnd_x = CW_USEDEFAULT;
	if( wnd_y==-1 ) wnd_y = CW_USEDEFAULT;


	wnd->hwnd = CreateWindowEx( WS_EX_TOOLWINDOW|WS_EX_TOPMOST, "MiniWindow", title, WS_CAPTION|WS_SYSMENU, 
					wnd_x, wnd_y, wnd_w, wnd_h,
					NULL, NULL, GInstance, wnd );

	wnd->wnddc = GetDC( wnd->hwnd );

	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biXPelsPerMeter = 72;
	bmi.bmiHeader.biYPelsPerMeter = 72;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiHeader.biWidth = img->w;
	bmi.bmiHeader.biHeight = -img->h;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biSizeImage = img->w * img->h * 3;

	wnd->dibdc = CreateCompatibleDC( NULL );
	if( !wnd->dibdc ) {
		piDebug( "*** Error: CreateCompatibleDC failed.\n" );
		return;
	}

	wnd->dib = CreateDIBSection( wnd->dibdc, &bmi, DIB_RGB_COLORS, (void **)&wnd->buffer, NULL, 0 );
	if( !wnd->dib ) {
		piDebug( "*** Error: CreateDIBSection failed.\n" );
		return;
	}

	SelectObject( wnd->dibdc, wnd->dib );

	for( int i=0; i<img->w*img->h; i++ ) {
		wnd->buffer[3*i+0] = img->buffer[i].b;	// swap rgb
		wnd->buffer[3*i+1] = img->buffer[i].g;
		wnd->buffer[3*i+2] = img->buffer[i].r;
	}

	ShowWindow( wnd->hwnd, SW_SHOWDEFAULT );

}


/** Remove any window with that title. */
void MiniUI::RemoveWindow( const Image * img ) {

	MiniWindow * w, * wnext;
	for( w = windows.next; w!=&windows; w=wnext ) {
		wnext = w->next;
		if( w->img == img ) {
			LL_Remove( w, next, prev );
		}
	}

}


/** Update the windows. */
bool MiniUI::Update( void ) {

	MiniWindow * w;
	for( w = windows.next; w!=&windows; w=w->next ) {
		// update this window.
	}


	if( windows.next == &windows ) PostQuitMessage(0);


	MSG msg;
	if( GetMessage( &msg, NULL, 0, 0 ) ) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);	
	}

	if( WM_QUIT == msg.message ) 
		return false;

	return true;
}
