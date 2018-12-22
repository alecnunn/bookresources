/**************************************************
WinMain.cpp
Chapter 1 Stack Process Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

// Include files
#include <windows.h>
#include <stdio.h>

class cProcessManager
{
  // A structure that stores a function pointer and linked list
  typedef struct sProcess {
    void  (*Function)();
    sProcess *Next;
  } sProcess;

  protected:
    sProcess *m_ProcessParent; // The top state in the stack
                               // (the head of the stack)

  public:
    cProcessManager() { m_ProcessParent = NULL; }

    ~cProcessManager() 
    {
      sProcess *ProcessPtr;

      // Remove all processes from the stack
      while((ProcessPtr = m_ProcessParent) != NULL) {
        m_ProcessParent = ProcessPtr->Next;
        delete ProcessPtr;
      }
    }

    // Add function on to the stack
    void Add(void (*Process)())
    {
      // Don't push a NULL value
      if(Process != NULL) {
        // Allocate a new process and push it on stack
        sProcess *ProcessPtr = new sProcess;
        ProcessPtr->Next = m_ProcessParent;
        m_ProcessParent = ProcessPtr;
        ProcessPtr->Function = Process;
      }
    }

    // Process all functions
    void Process()
    { 
      sProcess *ProcessPtr = m_ProcessParent;

      while(ProcessPtr != NULL) {
        ProcessPtr->Function();
        ProcessPtr = ProcessPtr->Next;
      }
    }
};

cProcessManager g_ProcessManager;

// Function prototypes
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow);

// Macro to ease the use of MessageBox function
#define MB(s) MessageBox(NULL, s, s, MB_OK);

// Processfunction prototypes - must follow this prototype!
void Func1() { MB("1"); }
void Func2() { MB("2"); }
void Func3() { MB("3"); }

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow)
{
  g_ProcessManager.Add(Func1);
  g_ProcessManager.Add(Func2);
  g_ProcessManager.Add(Func3);
  g_ProcessManager.Process();
  g_ProcessManager.Process();

  return 0;
}
