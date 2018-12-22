#ifndef _GAME_CHARS_H_
#define _GAME_CHARS_H_

class cApp;

class cChars : public cCharacterController
{
  private:
    cApp      *m_App;

    BOOL PCUpdate(sCharacter *Character, long Elapsed,        \
                  float *XMove, float *YMove, float *ZMove);
    BOOL ValidateMove(sCharacter *Character,                  \
                   float *XMove, float *YMove, float *ZMove);

    BOOL Experience(sCharacter *Character, long Amount);

    BOOL PCTeleport(sCharacter *Character, sSpell *Spell);

    BOOL ActionSound(sCharacter *Character);

    BOOL DropMoney(float XPos, float YPos, float ZPos,        \
                   long Quantity);
    BOOL DropItem(float XPos, float YPos, float ZPos,         \
                  long Item, long Quantity);

  public:
    BOOL SetData(cApp *App) { m_App = App; return TRUE; }
};

#endif
