#ifndef _SCRIPT_H_
#define _SCRIPT_H_

// Script structures ///////////////////////////////
enum Types { _NONE = 0, _TEXT, _BOOL, _INT, _FLOAT, _CHOICE };

typedef struct sScriptEntry
{
  long   Type;         // Type of entry

  union {
    long   IOValue;    // Used for saving/loading
    long   Length;     // Length of text (w/0 terminator)
    long   Selection;  // Selection in choice
    BOOL   bValue;     // BOOL value
    long   lValue;     // long value
    float  fValue;     // float value
  };
  char    *Text;       // Text buffer

  sScriptEntry()
  {
    Type = _NONE;
    IOValue = 0;
    Text = NULL;
  }

  ~sScriptEntry()
  {
    delete [] Text;
  }
} sScriptEntry;

typedef struct sScript
{
  long          Type;        // 0-(#actions)-1
  long          NumEntries;  // # entries
  sScriptEntry *Entries;     // Array of entries

  sScript      *Prev;        // Prev in list
  sScript      *Next;        // Next in a list

  sScript()
  { 
    Type = 0;
    NumEntries = 0;
    Entries = NULL;
    Prev = Next = NULL;
  }

  ~sScript()
  {
    delete [] Entries;
    delete Next;
  }
} sScript;

class cScript 
{
  private:
    long     m_NumActions;
    sScript *m_ScriptParent;

    virtual BOOL Prepare() { return TRUE; }
    virtual BOOL Release() { return TRUE; }

    virtual sScript *Process(sScript *Script) 
            { return Script->Next; }

  public:
    cScript();
    ~cScript();

    BOOL Load(char *Filename);
    BOOL Free();
    BOOL Execute(char *Filename);

    sScript *GetParentScript();
};

#endif
