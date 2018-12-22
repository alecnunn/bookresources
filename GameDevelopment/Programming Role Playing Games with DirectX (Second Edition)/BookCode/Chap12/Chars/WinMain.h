#ifndef _WINMAIN_H_
#define _WINMAIN_H_

#include "CharICS.h"
#include "Chars.h"
#include "Script.h"
#include "Spell.h"

class cApp;

class cChars : public cCharacterController
{
  private:
    cApp      *m_App;

    BOOL PCUpdate(sCharacter *Character, long Elapsed,        \
                  float *XMove, float *YMove, float *ZMove);
    BOOL ValidateMove(sCharacter *Character,                  \
                   float *XMove, float *YMove, float *ZMove);

  public:
    BOOL SetData(cApp *App);

};

class cGameScript : public cScript
{
  friend cApp;

  private:
    BOOL       m_Flags[256];

    cGraphics *m_Graphics;
    cFont     *m_Font;

    cInputDevice *m_Keyboard;

    cApp      *m_App;
    cWindow    m_Window;
    cChars    *m_CharController;

    long         m_NumRoutePoints;
    sRoutePoint *m_Route;

    // The script function prototypes
    sScript *Script_End(sScript*);
    sScript *Script_IfFlagThen(sScript*);
    sScript *Script_Else(sScript*);
    sScript *Script_EndIf(sScript*);
    sScript *Script_SetFlag(sScript*);
    sScript *Script_Message(sScript*);
    sScript *Script_AddChar(sScript*);
    sScript *Script_RemoveChar(sScript*);
    sScript *Script_CharMessage(sScript*);
    sScript *Script_CharType(sScript*);
    sScript *Script_CharAI(sScript*);
    sScript *Script_CharDistance(sScript*);
    sScript *Script_CharBounds(sScript*);
    sScript *Script_TargetChar(sScript*);
    sScript *Script_NoTarget(sScript*);
    sScript *Script_CreateRoute(sScript*);
    sScript *Script_AddPoint(sScript*);
    sScript *Script_AssignRoute(sScript*);
    sScript *Script_MoveChar(sScript*);
    sScript *Script_CharScript(sScript*);

    // The overloaded processing functions
    BOOL     Prepare();
    BOOL     Release();
    sScript *Process(sScript *Script);

  public:
    cGameScript();
    ~cGameScript();

    BOOL SetData(cApp *Application, cGraphics *Graphics,      \
                 cFont *Font, cInputDevice *Keyboard,         \
                 cChars *Chars);
};

class cApp : public cApplication
{
  friend class cGameScript;
  friend class cChars;

  private:
    cGraphics       m_Graphics;
    cCamera         m_Camera;
    cFont           m_Font;

    cInput          m_Input;
    cInputDevice    m_Keyboard;
    cInputDevice    m_Mouse;

    cMesh           m_TerrainMesh;
    cObject         m_TerrainObject;

    cChars           m_CharController;
    cSpellController m_SpellController;

    cGameScript     m_Script;

    sItem           m_MIL[1024];

  public:
    cApp();

    BOOL Init();
    BOOL Shutdown();
    BOOL Frame();

    // Retrieve bit flag encoded keyboard input
    long GetInput();

    // Check if player collides with level mesh
    BOOL CheckIntersect(                                      \
                 float XStart, float YStart, float ZStart,    \
                 float XEnd,   float YEnd,   float ZEnd);
};

#endif
