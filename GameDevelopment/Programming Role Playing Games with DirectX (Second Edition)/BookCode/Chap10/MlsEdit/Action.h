enum Types {
  _NONE = 0,
  _TEXT,
  _BOOL,
  _INT,
  _FLOAT,
  _CHOICE
};

// Structure to store information about a single blank entry
typedef struct sEntry {
  long     Type;        // Type of blank entry (_TEXT, etc)

  // The following two unions contain the various
  // information about a single blank entry, from
  // the min/max values (for int and float values),
  // as well as the number of choices in a multiple
  // choice entry.
  union {
    long   NumChoices;  // # of choices in list
    long   lMin;        // long min. value
    float  fMin;        // float min. value
  };
  union {
    long   lMax;        // long max. value
    float  fMax;        // float max. value
    char **Choices;     // choice text array
  };

  // Structure constructor to clear to default values
  sEntry()  
  { 
    Type       = _NONE; 
    NumChoices = 0;
    Choices    = NULL; 
  }

  // Structure destructor to clean up used resources
  ~sEntry() 
  { 
    // Special case for choice types
    if(Type == _CHOICE && Choices != NULL) {
      if(NumChoices) {
        for(long i=0;i<NumChoices;i++) {
          delete [] Choices[i];
          Choices[i] = NULL;
        }
      }
      delete [] Choices;
      Choices = NULL;
    }
  }
} sEntry;

// Structure that stores a single action
typedef struct sAction {
  long      ID;          // Action ID (0 to # actions-1)
  char      Text[256];   // Action text

  short     NumEntries;  // # of entries in action
  sEntry   *Entries;     // Array of entry structures

  sAction  *Next;        // Next action in linked list

  sAction() 
  {
    ID         = 0;      // Set all data to defaults
    Text[0]    = 0;
    NumEntries = 0;
    Entries    = NULL;
    Next       = NULL;
  }

  ~sAction()
  {
    delete [] Entries; Entries = NULL;  // Free entries array
    delete Next;       Next = NULL;     // Delete next in list
  }
} sAction;

typedef struct sScriptEntry
{
  long   Type;         // Type of entry (_TEXT, _BOOL, etc.)

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
    Type = _NONE;  // Clear to default values
    IOValue = 0;
    Text = NULL;
  }

  ~sScriptEntry() { delete [] Text; Text = NULL; }  // Delete text buffer
} sScriptEntry;

typedef struct sScript
{
  long          Type;        // 0 to (number of actions-1)
  long          NumEntries;  // # entries in this action
  sScriptEntry *Entries;     // Array of entries

  sScript      *Prev;        // Prev in linked list
  sScript      *Next;        // Next in linked list

  sScript()
  { 
    Type = 0;          // Clear to defaults
    NumEntries = 0;
    Entries = NULL;
    Prev = Next = NULL;
  }

  ~sScript()
  {
    delete [] Entries; Entries = NULL; // Delete entry array
    delete Next;       Next = NULL;    // Delete next in linked list
  }
} sScript;

class cActionTemplate {
  private:
    long     m_NumActions;    // # of actions in template
    sAction *m_ActionParent;  // list of template actions

    // Functions for reading text (mainly used in actions)
    BOOL GetNextQuotedLine(char *Data, FILE *fp, long MaxSize);
    BOOL GetNextWord(char *Data, FILE *fp, long MaxSize);

  public:
    cActionTemplate();
    ~cActionTemplate();

    // Load and free an action template
    BOOL     Load(char *Filename);
    BOOL     Free();

    // Get # actions in template, action parent, 
    // and specific action structure.
    long     GetNumActions();
    sAction *GetActionParent();
    sAction *GetAction(long Num);

    // Get a specific type of sScript structure
    sScript *CreateScriptAction(long Type);

    // Get info about actions and entries
    long     GetNumEntries(long ActionNum);
    sEntry  *GetEntry(long ActionNum, long EntryNum);

    // Expand action text using min/first/TRUE choice values
    BOOL     ExpandDefaultActionText(char *Buffer, sAction *Action);

    // Expand action text using selections
    BOOL     ExpandActionText(char *Buffer, sScript *Script);
};
