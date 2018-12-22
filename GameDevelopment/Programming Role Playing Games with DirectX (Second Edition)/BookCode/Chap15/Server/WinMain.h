#ifndef _WINMAIN_H_
#define _WINMAIN_H_

///////////////////////////////////////////////////////////////////
// Player data
///////////////////////////////////////////////////////////////////
// Maximum # of players allowed to be connected at once
#define MAX_PLAYERS           8

// Player states
#define STATE_IDLE            1 
#define STATE_MOVE            2
#define STATE_SWING           3
#define STATE_HURT            4

typedef struct sPlayer {
  BOOL  Connected;        // TRUE if player connected

  char  Name[256];        // Name of player
  DPNID dpnidPlayer;      // DirectPlay Player ID #

  long  State;            // Last known state (STATE_*)
  long  Time;             // Time of last state update
  long  Latency;          // Half of round trip latency in ms

  float XPos, YPos, ZPos; // Player coordinates
  float Direction;        // Angle facing
  float Speed;            // Movement speed

  // Constructor
  sPlayer() { Connected = FALSE; Latency = 0; }
} sPlayer;

///////////////////////////////////////////////////////////////////
// Network message data
///////////////////////////////////////////////////////////////////
// # of messages to process per frame
#define MESSAGES_PER_FRAME   64

// # of message to allocate for message queue
#define NUM_MESSAGES       1024

// Types of messages
#define MSG_CREATE_PLAYER     1
#define MSG_PLAYER_INFO       2
#define MSG_DESTROY_PLAYER    3
#define MSG_STATE_CHANGE      4

// The message header structure used in all messages
typedef struct {
  long  Type;             // Type of message (MSG_*)
  long  Size;             // Size of data to send
  DPNID PlayerID;         // Player performing action
} sMessageHeader;

// The message queue message structure
typedef struct {
  sMessageHeader Header;  // Message header

  char Data[512];         // Message data
} sMessage;

// Create a player message
typedef struct {
  sMessageHeader Header;  // Message header

  float XPos, YPos, ZPos; // Create player coordinates
  float Direction;        // Direction of player
} sCreatePlayerMessage;

// Request player information message
typedef struct {
  sMessageHeader Header;  // Message header

  DPNID PlayerID;         // Which player to request
} sRequestPlayerInfoMessage;

// Destroy a player message
typedef struct {
  sMessageHeader Header;  // Message header
} sDestroyPlayerMessage;

// Change in state message
typedef struct {
  sMessageHeader Header;  // Message header

  long  State;            // State message (STATE_*)
  float XPos, YPos, ZPos; // Player coordinates
  float Direction;        // Player facing direction
  float Speed;            // Walking speed of player

  long  Latency;          // Latency value of connection
} sStateChangeMessage;

///////////////////////////////////////////////////////////////////
// Server class definition
///////////////////////////////////////////////////////////////////
class cServer : public cNetworkServer
{
  private:
    BOOL CreatePlayer(DPNMSG_CREATE_PLAYER *Msg);
    BOOL DestroyPlayer(DPNMSG_DESTROY_PLAYER *Msg);
    BOOL Receive(DPNMSG_RECEIVE *Msg);
};

///////////////////////////////////////////////////////////////////
// Application class definition
///////////////////////////////////////////////////////////////////
class cApp : public cApplication
{
  private:
    HWND             m_Controls[3];  // The app window handles

    CRITICAL_SECTION m_UpdateCS;     // Critical sections
    CRITICAL_SECTION m_MessageCS;

    GUID            *m_guidAdapter;  // Adapter used to connect
    cNetworkAdapter  m_Adapters;     // List of adapters
    cServer          m_Server;       // Derived server class

    cGraphics        m_Graphics;     // Graphics Core interface
    cMesh            m_LevelMesh;    // Mesh of world

    long             m_NumPlayers;   // # players connected
    sPlayer         *m_Players;      // Player data

    sMessage        *m_Messages;     // Message queue
    long             m_MsgHead;      // Head of message list
    long             m_MsgTail;      // Tail of message list
    
    BOOL SelectAdapter();            // Select network adapter
    void SetupApplicationWindow();   // Set application window
    BOOL InitializeGame();           // Initialize game
    BOOL HostGame();                 // Begin hosting game

    void ListPlayers();              // List connected players

    void ProcessQueuedMessages();    // Process waiting messages
    void UpdatePlayers();            // Move all players
    void UpdateNetwork();            // Send updates to players
    void UpdateLatency();            // Update latency values

    // Send a network message to client(s)
    BOOL SendNetworkMessage(void *Msg, long SendFlags = 0,    \
                            int To = -1);

    //  Functions to process queued network messages
    BOOL QueueMessage(void *Msg);  
    BOOL PlayerInfo(sMessage *Msg, DPNID To);
    BOOL AddPlayer(sMessage *Msg);
    BOOL RemovePlayer(sMessage *Msg);
    BOOL PlayerStateChange(sMessage *Msg);

    // Check if player collides with level mesh
    BOOL CheckIntersect(cMesh *Mesh,                          \
                 float XStart, float YStart, float ZStart,    \
                 float XEnd,   float YEnd,   float ZEnd);

  public:
    // Overloaded class functions
    cApp();

    BOOL Init();
    BOOL Shutdown();
    BOOL Frame();
    FAR PASCAL MsgProc(HWND hWnd, UINT uMsg,                  \
                       WPARAM wParam, LPARAM lParam);

    void SetAdapter(GUID *Adapter); // Select adapter GUID

    // Functions to siphon network messages to application
    BOOL CreatePlayer(DPNMSG_CREATE_PLAYER *Msg);
    BOOL DestroyPlayer(DPNMSG_DESTROY_PLAYER *Msg);
    BOOL Receive(DPNMSG_RECEIVE *Msg);
};

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow);
BOOL CALLBACK ConfigDialogProc(HWND hWnd, UINT uMsg,          \
                               WPARAM wParam, LPARAM lParam);

#endif
