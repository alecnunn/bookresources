/**************************************************
Core_System.cpp
GameCore Component

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

#include "Core_Global.h"

cApplication::cApplication()
{
  // Save instance handle
  g_pApplication = this;

  // Get the instance handle
  m_hInst = GetModuleHandle(NULL);

  // Set a default window class and caption
  strcpy(m_Class, "AppClass");
  strcpy(m_Caption, "Application Caption");

  // Set default window style, position, width, height
  m_Style  = WS_OVERLAPPEDWINDOW;
  m_XPos   = 0;
  m_YPos   = 0;
  m_Width  = 256;
  m_Height = 256;

  // Set default WNDCLASSEX structure
  m_wcex.cbSize        = sizeof(WNDCLASSEX);
  m_wcex.style         = CS_CLASSDC;
  m_wcex.lpfnWndProc   = AppWindowProc;
  m_wcex.cbClsExtra    = 0;
  m_wcex.cbWndExtra    = 0;
  m_wcex.hInstance     = m_hInst;
  m_wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  m_wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  m_wcex.hbrBackground = NULL;
  m_wcex.lpszMenuName  = NULL;
  m_wcex.lpszClassName = m_Class;
  m_wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
}

HWND cApplication::GethWnd()
{
  return m_hWnd;
}

HINSTANCE cApplication::GethInst()
{
  return m_hInst;
}

BOOL cApplication::Run()
{
  MSG Msg;

  // Register window class
  if(!RegisterClassEx(&m_wcex))
    return FALSE;

  // Create the Main Window
  m_hWnd = CreateWindow(m_Class, m_Caption,
        m_Style, 
        m_XPos, m_YPos,
        m_Width, m_Height,
        NULL, NULL, m_hInst, NULL);
  if(!m_hWnd)
    return FALSE;

  // Show and update the window
  ShowWindow(m_hWnd, SW_NORMAL);
  UpdateWindow(m_hWnd);

  // Make sure client area is correct size
  Resize(m_Width, m_Height);

  // Initialize COM
  CoInitialize(NULL);

  if(Init() == TRUE) {
     // Enter the message pump
    ZeroMemory(&Msg, sizeof(MSG));
    while(Msg.message != WM_QUIT) {

      // Handle Windows messages (if any)
      if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
      } else {
        // Do per-frame processing, break on FALSE return value
        if(Frame() == FALSE)
          break;
      }
    }
  }

  Shutdown();

  // Shutdown COM
  CoUninitialize();

  // Unregister the window class
  UnregisterClass(m_Class, m_hInst);

  return TRUE;
}

BOOL cApplication::Error(BOOL Fatal, char *Text, ...)
{
  char CaptionText[12];
  char ErrorText[2048];
  va_list valist;

  // Build the message box caption based on fatal flag
  if(Fatal == FALSE)
    strcpy(CaptionText, "Error");
  else 
    strcpy(CaptionText, "Fatal Error");

  // Build variable text buffer
  va_start(valist, Text);
  vsprintf(ErrorText, Text, valist);
  va_end(valist);

  // Display the message box
  MessageBox(NULL, ErrorText, CaptionText, MB_OK | MB_ICONEXCLAMATION);

  // Post a quit message if error was fatal
  if(Fatal == TRUE)
    PostQuitMessage(0);

  return TRUE;
}

BOOL cApplication::Move(long XPos, long YPos)
{
  RECT ClientRect;

  GetClientRect(m_hWnd, &ClientRect);
  MoveWindow(m_hWnd, XPos, YPos, ClientRect.right, ClientRect.bottom, TRUE);

  return TRUE;
}

BOOL cApplication::Resize(long Width, long Height)
{
  RECT WndRect, ClientRect;
  long WndWidth, WndHeight;

  GetWindowRect(m_hWnd, &WndRect);
  GetClientRect(m_hWnd, &ClientRect);

  WndWidth  = (WndRect.right  - (ClientRect.right  - Width))  - WndRect.left;
  WndHeight = (WndRect.bottom - (ClientRect.bottom - Height)) - WndRect.top;

  MoveWindow(m_hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);

  return TRUE;
}

BOOL cApplication::ShowMouse(BOOL Show)
{
  ShowCursor(Show);
  return TRUE;
}

// The message procedure - empty except for destroy message
long FAR PASCAL AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    default: return g_pApplication->MsgProc(hWnd, uMsg, wParam, lParam);
  }
}

cStateManager::cStateManager() 
{ 
  m_StateParent = NULL; 
}

cStateManager::~cStateManager() 
{
  PopAll();
}

// Push a function on to the stack
void cStateManager::Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr)
{
  sState *StatePtr;

  // Don't push a NULL value
  if(Function != NULL) {
    // Allocate a new state and push it on stack
    StatePtr = new sState();

    StatePtr->Function = Function;
    StatePtr->Next = m_StateParent;

    m_StateParent = StatePtr;

    // Call state with init purpose
    StatePtr->Function(DataPtr, INITPURPOSE);
  }
}

BOOL cStateManager::Pop(void *DataPtr)
{
  sState *StatePtr;

  // Remove the head of stack (if any)
  if((StatePtr = m_StateParent) != NULL) {
    // First call with shutdown purpose
    m_StateParent->Function(DataPtr, SHUTDOWNPURPOSE);

    m_StateParent = StatePtr->Next;
    StatePtr->Next = NULL;
    delete StatePtr;
  }

  // return TRUE if more states exist, FALSE otherwise
  if(m_StateParent == NULL)
    return FALSE;
  return TRUE;
}

void cStateManager::PopAll(void *DataPtr)
{
  while(Pop(DataPtr) == TRUE);
}

BOOL cStateManager::Process(void *DataPtr)
{ 
  // return an error if no more states
  if(m_StateParent == NULL)
    return FALSE;

  // Process the top-most state
  m_StateParent->Function(DataPtr, FRAMEPURPOSE); 

  return TRUE;
}

cProcessManager::cProcessManager() 
{ 
  m_ProcessParent = NULL; 
}

cProcessManager::~cProcessManager() 
{
  // Pop each process
  while(Pop()==TRUE);
}

// Push a function on to the stack
void cProcessManager::Push(void (*Process)(void *Ptr, long Purpose), void *DataPtr)
{
  // Don't push a NULL value
  if(Process != NULL) {
    // Allocate a new process and push it on stack
    sProcess *ProcessPtr = new sProcess();
    ProcessPtr->Function = Process;
    ProcessPtr->Next = m_ProcessParent;
    m_ProcessParent = ProcessPtr;

    // Call process with init purpose
    ProcessPtr->Function(DataPtr, INITPURPOSE);
  }
}

// Pop top process from stack
BOOL cProcessManager::Pop(void *DataPtr)
{
  sProcess *ProcessPtr;

  // Remove the head of stack (if any)
  if((ProcessPtr = m_ProcessParent) != NULL) {
    // First call with shutdown purpose
    m_ProcessParent->Function(DataPtr, SHUTDOWNPURPOSE);

    m_ProcessParent = ProcessPtr->Next;
    ProcessPtr->Next = NULL;
    delete ProcessPtr;
  }

  // return TRUE if more processes exist, FALSE otherwise
  if(m_ProcessParent == NULL)
    return FALSE;
  return TRUE;
}

void cProcessManager::PopAll(void *DataPtr)
{
  while(Pop(DataPtr) == TRUE);
}

// Process all functions
void cProcessManager::Process(void *DataPtr)
{ 
  sProcess *ProcessPtr = m_ProcessParent;

  while(ProcessPtr != NULL) {
    ProcessPtr->Function(DataPtr, FRAMEPURPOSE);
    ProcessPtr = ProcessPtr->Next;
  }
}

cDataPackage::cDataPackage()  
{  
  m_Buf = NULL; 
  m_Size = 0; 
}

cDataPackage::~cDataPackage() 
{ 
  Free(); 
}

void *cDataPackage::Create(unsigned long Size)
{
  // Free a previously created buffer
  Free();

  // Allocate some memory and return a pointer
  return (m_Buf = (void*)new char[(m_Size = Size)]);
}

// Free the allocated memory
void cDataPackage::Free() 
{ 
  delete m_Buf; 
  m_Buf = NULL; 
  m_Size = 0; 
}

BOOL cDataPackage::Save(char *Filename)
{
  FILE *fp;

  // Make sure there's something to write
  if(m_Buf != NULL && m_Size) {
    // Open file, write size and data
    if((fp=fopen(Filename, "wb")) != NULL) {
      fwrite(&m_Size, 1, 4, fp);
      fwrite(m_Buf, 1, m_Size, fp);
      fclose(fp);
      return TRUE;
    }
  }

  return FALSE;
}

void *cDataPackage::Load(char *Filename, unsigned long *Size)
{
  FILE *fp;

  // Free a prior buffer
  Free();

  if((fp=fopen(Filename, "rb"))!=NULL) {
    // Read in size and data
    fread(&m_Size, 1, 4, fp);
    if((m_Buf = (void*)new char[m_Size]) != NULL)
      fread(m_Buf, 1, m_Size, fp);
    fclose(fp);

    // Store size to return
    if(Size != NULL)
      *Size = m_Size;

    // return pointer
    return m_Buf;
  }

  return NULL;
}

void *cDataPackage::GetPtr()
{
  return m_Buf;
}

unsigned long cDataPackage::GetSize()
{
  return m_Size;
}

