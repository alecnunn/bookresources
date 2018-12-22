enum Types {
  _NONE = 0,
  _TEXT,
  _BOOL,
  _INT,
  _FLOAT,
  _CHOICE
};

typedef struct sTEXTEntry {
  long  Length;  // Maximum length of the text
  sTEXTEntry()  { Length = 0; }
} sTEXTEntry;

typedef struct sBOOLEntry {
  BOOL Value;  // Boolean value (TRUE/FALSE)
  sBOOLEntry() { Value = TRUE; }
} sBOOLEntry;

typedef struct sINTEntry {
  unsigned long Min, Max;  // Min/Max values
  sINTEntry() { Min = Max = 0; }
} sINTEntry;

typedef struct sFLOATEntry {
  float Min, Max;  // Min/Max values
  sFLOATEntry() { Min = Max = 0.0f; }
} sFLOATEntry;

typedef struct sCHOICEEntry {
  long   Count;  // # choices and current selection
  char **Text;   // Array of choice's text

  sCHOICEEntry() { Count = 0; Text = NULL; }
  ~sCHOICEEntry() 
  { 
    if(Count) {
      for(long i=0;i<Count;i++)
        delete Text[i];
    }
    delete [] Text;
  }
} sCHOICEEntry;

typedef struct sEntry {
  long Type;   // Type of blank entry
  void *Data;  // Buffer to hold entry data

  sEntry()  { Type = _NONE; Data = NULL; }
  ~sEntry() { delete [] Data;            }
} sEntry;

typedef struct sAction {
  long      ID;          // Action ID
  char      Text[256];   // Action text
  short     NumEntries;  // # of entries in action
  sEntry   *Entries;     // Array of entry structures
  sAction  *Next;        // Next action in linked list

  sAction() 
  {
    ID         = 0;
    Text[0]    = 0;
    NumEntries = 0;
    Entries    = NULL;
    Next       = NULL;
  }

  ~sAction()
  {
    delete [] Entries;
    delete Next;
  }
} sAction;

typedef struct sScript {
  long   Action;
  long   NumEntries;
  long  *EntrySizes;
  char **Entries;
  sScript *Next;

  sScript() 
  { 
    Action     = 0; 
    NumEntries = 0;
    EntrySizes = NULL;
    Entries    = NULL;
    Next       = NULL;
  }

  ~sScript()
  {
    delete [] EntrySizes;
    if(NumEntries) {
      for(long i=0;i<NumEntries;i++)
        delete [] Entries[i];
    }
    delete [] Entries;
    delete Next;
  }
} sScript;

class cMLSScript {
  private:
    // Script data
    long     m_NumScriptActions; // # actions in script    
    sScript *m_ScriptParent;     // list of script actions

    // Action data
    long     m_NumActions;    // # of actions in template
    sAction *m_ActionParent;  // list of template actions

    // Functions for reading text (mainly used in actions)
    BOOL GetNextQuotedLine(char *Data, FILE *fp, long MaxSize);
    BOOL GetNextWord(char *Data, FILE *fp, long MaxSize);

  public:
    cMLSScript();
    ~cMLSScript();

    // Script functions
    BOOL     LoadScript(char *Filename);
    BOOL     SaveScript(char *Filename);
    BOOL     FreeScript();
    sAction *GetScriptActionParent();
    BOOL     AddScriptTEXTAction();
    BOOL     AddScriptBOOLAction();
    BOOL     AddScriptINTAction();
    BOOL     AddScriptFLOATAction();
    BOOL     AddScriptCHOICEAction();
    BOOL     ExpandScriptText(char *Buffer, sScript *Script);
    
    // Action functions
    BOOL     LoadActions(char *Filename);
    BOOL     FreeActions();
    long     GetNumActions();
    sAction *GetActionParent();
    sAction *GetAction(long Num);
    BOOL     ExpandActionText(char *Buffer, sAction *Action);
};
