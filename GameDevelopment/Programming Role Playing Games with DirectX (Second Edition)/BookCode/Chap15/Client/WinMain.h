#ifndef _WINMAIN_H_
#define _WINMAIN_H_

///////////////////////////////////////////////////////////
// Player data
///////////////////////////////////////////////////////////
// Maximum # of players allowed to be connected at once
#define MAX_PLAYERS           8

// Player states
#define STATE_IDLE            1 
#define STATE_MOVE            2
#define STATE_SWING           3
#define STATE_HURT            4

// Animations
#define ANIM_IDLE             1
#define ANIM_WALK             2
#define ANIM_SWING            3
#define ANIM_HURT             4

typedef struct sPlayer {
  BOOL  Connected;        // TRUE if player active

  DPNID dpnidPlayer;      // DirectPlay Player ID #

  long  State;            // Last known state (STATE_*)
  long  Time;             // Time of last state update
  long  Latency;          // Half of round trip latency in ms

  float XPos, YPos, ZPos; // Player coordinates
  float Direction;        // Angle facing
  float Speed;            // Movement speed

  cObject Body;           // Character 3-D object
  cObject Weapon;         // Weapon 3-D object
  long  LastAnim;         // Last known animation

  // Constructor and destructor
  sPlayer()  
  { 
    Connected = FALSE; 
    dpnidPlayer = 0; 
    LastAnim = 0; 
    Time = 0;
  }

  ~sPlayer() { Body.Free(); Weapon.Free(); }
} sPlayer;

///////////////////////////////////////////////////////////
// Network message data
///////////////////////////////////////////////////////////
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

  char Data[1024];        // Message data
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

  long  Latency;          // Latency value from server
} sStateChangeMessage;

///////////////////////////////////////////////////////////
// Client class definition
///////////////////////////////////////////////////////////
class cClient : public cNetworkClient
{
  private:
    BOOL ConnectComplete(DPNMSG_CONNECT_COMPLETE *Msg);
    BOOL Receive(DPNMSG_RECEIVE *Msg);
};

///////////////////////////////////////////////////////////
// Application class definition
///////////////////////////////////////////////////////////
class cApp : public cApplication
{
  private:
    CRITICAL_SECTION m_UpdateCS;    // Critical sections

    cGraphics       m_Graphics;     // Graphics object
    cCamera         m_Camera;       // Camera object
    cFont           m_Font;         // Font object

    GUID           *m_guidAdapter;  // Adapter used to connect
    cNetworkAdapter m_Adapters;     // List of adapters
    cClient         m_Client;       // Derived client class

    char            m_HostIP[16];   // Host IP Address
    char            m_Name[32];     // Player name

    cInput          m_Input;        // Input object
    cInputDevice    m_Keyboard;     // Keyboard object
    cInputDevice    m_Mouse;        // Mouse object

    cMesh           m_TerrainMesh;  // Terrain object mesh
    cNodeTreeMesh   m_NodeTreeMesh; // NodeTree object

    cMesh           m_CharacterMesh; // Mesh for character
    cMesh           m_WeaponMesh;    // Mesh for weapon
    cAnimation      m_CharacterAnim; // Animation for character

    long            m_NumPlayers;   // # players connected
    sPlayer        *m_Players;      // Player data

    float           m_CamAngle;     // Viewing camera angle

    BOOL SelectAdapter();            // Select network adapter
    BOOL InitializeGame();           // Initialize game
    BOOL JoinGame();                 // Join network session

    // Get player's number in list
    long GetPlayerNum(DPNID dpnidPlayer);
    
    void UpdatePlayers();            // Update all players

    void RenderScene();              // Render the scene

    // Send a client network message to server
    BOOL SendNetworkMessage(void *Msg, long SendFlags);

    // Functions to process incoming messages
    void CreatePlayer(sMessage *Msg);
    void DestroyPlayer(sMessage *Msg);
    void ChangeState(sMessage *Msg);

  public:
    // Overloaded class functions
    cApp();

    BOOL Init();
    BOOL Shutdown();
    BOOL Frame();

    // Siphon a network receive message to application
    BOOL Receive(DPNMSG_RECEIVE *Msg);

    // Set the local client's player ID #
    BOOL SetLocalPlayerID(DPNID dpnid);

    // Set adapter to use, IP to connect to, and name to use
    void SetInfo(GUID *Adapter, char *IP, char *Name);
};

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow);
BOOL CALLBACK ConnectDialogProc(HWND hWnd, UINT uMsg,         \
                                WPARAM wParam, LPARAM lParam);

#endif
