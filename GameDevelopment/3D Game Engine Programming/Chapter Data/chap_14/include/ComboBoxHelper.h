
// HELPER FUNCTIONS TO DEAL WITH COMBOBOX CONTROLS
#include <windows.h>


// P R O T O T Y P E S /////////////////////////////////////////////

HWND  cbhCreate(HWND hWnd, HINSTANCE hInst, HMENU id, int x, int y, int dx, int dy, bool);
void  cbhAddItem(HWND hWnd, char *ch, void *pData, bool bSelect);
void  cbhRemoveSelectedItem(HWND);
void  cbhResetContent(HWND);
void  cbhSelectFirstItem(HWND hWnd);
void* cbhGetSelectedItem(HWND hWnd);
void  cbhGetSelectedText(HWND hWnd, void *pData);
UINT  cbhGetSelectedIndex(HWND hWnd);
int   cbhContainsString(HWND hWnd, char *ch);
