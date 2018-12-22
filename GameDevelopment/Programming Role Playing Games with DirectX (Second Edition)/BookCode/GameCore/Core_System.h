/**************************************************
Core_System.h
GameCore Component

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

class cApplication
{
  private:
    HINSTANCE     m_hInst;
    HWND          m_hWnd;

  protected:
    char          m_Class[MAX_PATH];
    char          m_Caption[MAX_PATH];

    WNDCLASSEX    m_wcex;

    DWORD         m_Style;
    DWORD         m_XPos;
    DWORD         m_YPos;
    DWORD         m_Width;
    DWORD         m_Height;

  public:
    cApplication();

    HWND      GethWnd();
    HINSTANCE GethInst();

    BOOL Run();
    BOOL Error(BOOL Fatal, char *Text, ...);

    BOOL Move(long XPos, long YPos);
    BOOL Resize(long Width, long Height);

    BOOL ShowMouse(BOOL Show = TRUE);

    virtual FAR PASCAL MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, uMsg, wParam, lParam); }
    virtual BOOL Init()       { return TRUE; }
    virtual BOOL Shutdown()   { return TRUE; }
    virtual BOOL Frame()      { return TRUE; }
};

static cApplication *g_pApplication = NULL;
static long FAR PASCAL AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

enum Purposes {
    NOPURPOSE = 0,
    INITPURPOSE,
    SHUTDOWNPURPOSE,
    FRAMEPURPOSE
  };
    
class cStateManager
{
  typedef struct sState {
    void (*Function)(void *Ptr, long Purpose);
    sState *Next;

    sState()
    {
      Function = NULL;
      Next = NULL;
    }

    ~sState()
    {
      delete Next;
    }
  } sState;

  protected:
    sState *m_StateParent;

  public:
    cStateManager();
    ~cStateManager();

    void Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr = NULL);
    BOOL Pop(void *DataPtr = NULL);
    void PopAll(void *DataPtr = NULL);
    BOOL Process(void *DataPtr = NULL);
};

class cProcessManager
{
  typedef struct sProcess {
    void  (*Function)(void *Ptr, long Purpose);
    sProcess *Next;

    sProcess()
    {
      Function = NULL;
      Next = NULL;
    }

    ~sProcess()
    {
      delete Next; Next = NULL;
    }
  } sProcess;

  protected:
    sProcess *m_ProcessParent; 

  public:
    cProcessManager();
    ~cProcessManager();

    void Push(void (*Process)(void *Ptr, long Purpose), void *DataPtr = NULL);
    BOOL Pop(void *DataPtr = NULL);
    void PopAll(void *DataPtr = NULL);
    void Process(void *Ptr = NULL);
};

class cDataPackage
{
  protected:
    void          *m_Buf;
    unsigned long  m_Size;

  public:
    cDataPackage();
    ~cDataPackage();

    void *Create(unsigned long Size);
    void Free();

    BOOL Save(char *Filename);
    void *Load(char *Filename, unsigned long *Size);

    void          *GetPtr();
    unsigned long  GetSize();
}; 

#endif
