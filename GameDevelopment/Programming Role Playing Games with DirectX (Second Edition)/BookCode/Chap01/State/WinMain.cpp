/**************************************************
WinMain.cpp
Chapter 1 State-based processing Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

// Include files
#include <windows.h>
#include <stdio.h>

class cStateManager
{
  // A structure that stores a function pointer and linked list
  typedef struct sState {
    void  (*Function)();
    sState *Next;
  } sState;

  protected:
    sState *m_StateParent; // The top state in the stack
                           // (the head of the stack)

  public:
    cStateManager() { m_StateParent = NULL; }

    ~cStateManager() 
    {
      sState *StatePtr;

      // Remove all states from the stack
      while((StatePtr = m_StateParent) != NULL) {
        m_StateParent = StatePtr->Next;
        delete StatePtr;
      }
    }

    // Push a function on to the stack
    void Push(void (*Function)())
    {
      // Don't push a NULL value
      if(Function != NULL) {
        // Allocate a new state and push it on stack
        sState *StatePtr = new sState;
        StatePtr->Next = m_StateParent;
        m_StateParent = StatePtr;
        StatePtr->Function = Function;
      }
    }

    BOOL Pop()
    {
      sState *StatePtr = m_StateParent;

      // Remove the head of stack (if any)
      if(StatePtr != NULL) {
        m_StateParent = StatePtr->Next;
        delete StatePtr;
      }

      // return TRUE if more states exist, FALSE otherwise
      if(m_StateParent == NULL)
        return FALSE;
      return TRUE;
    }

    BOOL Process()
    { 
      // return an error if no more states
      if(m_StateParent == NULL)
        return FALSE;
      // Process the top-most state (if any)
      m_StateParent->Function(); 
      return TRUE;
    }
};

cStateManager g_StateManager;

// Function prototypes
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow);

// Macro to ease the use of MessageBox function
#define MB(s) MessageBox(NULL, s, s, MB_OK);

// State function prototypes - must follow this prototype!
void Func1() { MB("1"); g_StateManager.Pop(); }
void Func2() { MB("2"); g_StateManager.Pop(); }
void Func3() { MB("3"); g_StateManager.Pop(); }

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow)
{
  g_StateManager.Push(Func1);
  g_StateManager.Push(Func2);
  g_StateManager.Push(Func3);
  while(g_StateManager.Process() == TRUE);

  return 0;
}
