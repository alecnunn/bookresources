#ifndef _WINMAIN_H_
#define _WINMAIN_H_

class cApp : public cApplication
{
  friend class cSpells;
  friend class cChars;
  friend class cGameScript;

  private:
    // Graphics device, camera, and font
    cGraphics       m_Graphics;
    cCamera         m_Camera;
    cFont           m_Font;

    // Input devices
    cInput          m_Input;
    cInputDevice    m_Keyboard;
    cInputDevice    m_Mouse;

    // Sound device and channels
    cSound          m_Sound;
    cSoundChannel   m_SoundChannel;
    cMusicChannel   m_MusicChannel;
    cSoundData      m_SoundData;

    // The scene textures, mesh, and object
    long            m_SceneNum;
    cTexture        m_SceneTextures[6];
    cMesh           m_SceneMesh;
    cObject         m_SceneObject;

    // Character and spell controllers
    cChars          m_CharController;
    cSpells         m_SpellController;

    // The options bitmap texture image
    cTexture        m_Options;

    // Text windows
    cWindow         m_Stats;
    cWindow         m_Window;
    cWindow         m_Header;

    // A script parser object
    cGameScript     m_Script;

    // Map to teleport to on next frame (-1 = none)
    long            m_TeleportMap;

    // Flag if monsters during last frame
    BOOL            m_MonstersLastFrame;

    // Combat booty to reward at end of combat
    long            m_CombatExp;
    long            m_CombatMoney;
    long            m_CombatItems[1024];

    // The trigger and barrier classes
    cTrigger        m_Trigger;
    cBarrier        m_Barrier;

    // The master item list
    sItem           m_MIL[1024];

    // Intersection functions
    float GetHeightBelow(float XPos, float YPos, float ZPos);
    BOOL  CheckIntersect(                                     \
                  float XStart, float YStart, float ZStart,   \
                  float XEnd,   float YEnd,   float ZEnd,     \
                  float *Length);

    // Read in a text floating point number
    float GetNextFloat(FILE *fp);

    // Setup to teleport
    BOOL SetupTeleport(long Level,                            \
                         float XPos, float YPos, float ZPos);

    // Setup to barter
    BOOL SetupBarter(sCharacter *Character, char *ICSFilename);

    // Functions load and free 3Din2D background images
    BOOL LoadLevel(long Num);
    BOOL FreeLevel();

    // Play a sound or music
    BOOL PlaySound(long Num);
    BOOL PlayMusic(long Num);
    BOOL StopMusic();

    // Process win-game scenerio
    BOOL WinGame();

    // Called at start and end of combat
    BOOL StartOfCombat();
    BOOL EndOfCombat();

    // Function to render a single game frame w/out updates
    // and using elapsed animation timer.
    BOOL RenderFrame(long Elapsed=0);

    // Get the character where mouse is pointed
    sCharacter *GetCharacterAt(long XPos, long YPos);

    // Check if last point on route has been reached
    BOOL LastPointReached(sCharacter *Character);

    cStateManager m_StateManager;
    static void MenuFrame(void *Ptr, long Purpose);
    static void GameFrame(void *Ptr, long Purpose);
    static void StatusFrame(void *Ptr, long Purpose);
    static void BarterFrame(void *Ptr, long Purpose);

  public:
    cApp();

    // Overridden functions
    BOOL Init();
    BOOL Shutdown();
    BOOL Frame();
};

#endif
