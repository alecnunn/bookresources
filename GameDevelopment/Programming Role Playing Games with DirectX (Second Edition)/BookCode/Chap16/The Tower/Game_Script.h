#ifndef _GAME_SCRIPT_H_
#define _GAME_SCRIPT_H_

// Definitions that match script's else and endif action #'s
#define SCRIPT_ELSE     1
#define SCRIPT_ENDIF    2
#define SCRIPT_GOTO     8

class cGameScript : public cScript
{
  private:
    // The internal flags and variables array
    BOOL         m_Flags[256];
    long         m_Vars[256];

    // The parent application object
    cApp        *m_App;

    // A text window for displaying messages
    cWindow      m_Window;

    // Route points for constructing a character route
    long         m_NumRoutePoints;
    sRoutePoint *m_Route;

    // Standard processing actions
    sScript *Script_End(sScript*);
    sScript *Script_Else(sScript*);
    sScript *Script_EndIf(sScript*);
    sScript *Script_IfFlagThen(sScript*);
    sScript *Script_IfVarThen(sScript*);
    sScript *Script_SetFlag(sScript*);
    sScript *Script_SetVar(sScript*);
    sScript *Script_Label(sScript*);
    sScript *Script_Goto(sScript*);
    sScript *Script_Message(sScript*);

    // Character related actions
    sScript *Script_Add(sScript*);
    sScript *Script_Remove(sScript*);
    sScript *Script_Move(sScript*);
    sScript *Script_Direction(sScript*);
    sScript *Script_Type(sScript*);
    sScript *Script_AI(sScript*);
    sScript *Script_Target(sScript*);
    sScript *Script_NoTarget(sScript*);
    sScript *Script_Bounds(sScript*);
    sScript *Script_Distance(sScript*);
    sScript *Script_Script(sScript*);
    sScript *Script_CharMessage(sScript*);
    sScript *Script_Enable(sScript*);
    sScript *Script_CreateRoute(sScript*);
    sScript *Script_AddPoint(sScript*);
    sScript *Script_AssignRoute(sScript*);
    sScript *Script_AlterHPMP(sScript*);
    sScript *Script_Ailment(sScript*);
    sScript *Script_AlterSpell(sScript*);
    sScript *Script_Teleport(sScript*);
    sScript *Script_ShortMessage(sScript*);
    sScript *Script_Action(sScript*);
    sScript *Script_IfExpLevel(sScript*);

    // Shop/barter action
    sScript *Script_Barter(sScript*);

    // Item related actions
    sScript *Script_IfItem(sScript*);
    sScript *Script_AddItem(sScript*);
    sScript *Script_RemoveItem(sScript*);

    // Barrier related actions
    sScript *Script_AddBarrier(sScript*);
    sScript *Script_EnableBarrier(sScript*);
    sScript *Script_RemoveBarrier(sScript*);

    // Trigger related actions
    sScript *Script_AddTrigger(sScript*);
    sScript *Script_EnableTrigger(sScript*);
    sScript *Script_RemoveTrigger(sScript*);

    // Sound related actions
    sScript *Script_Sound(sScript*);
    sScript *Script_Music(sScript*);
    sScript *Script_StopMusic(sScript*);

    // Win game action
    sScript *Script_WinGame(sScript*);

    // Comment and separator actions
    sScript *Script_CommentOrSeparator(sScript*);

    // Wait action
    sScript *Script_Wait(sScript*);

    // Random number generation
    sScript *Script_IfRandThen(sScript*);

    // Force a frame to render
    sScript *Script_Render(sScript*);

    // If/then processing function
    sScript *Script_IfThen(sScript *ScriptPtr, BOOL Skip);

    // The overloaded processing functions
    BOOL     Release();
    sScript *Process(sScript *Script);

  public:
    cGameScript();
    ~cGameScript();

    BOOL SetData(cApp *App);
    BOOL Reset();

    BOOL Save(char *Filename);
    BOOL Load(char *Filename);
};

#endif
