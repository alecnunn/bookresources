#ifndef _GAME_SPELLS_H_
#define _GAME_SPELLS_H_

class cSpells : public cSpellController
{
  private:
    cApp *m_App;

  public:
    BOOL SetData(cApp *App) { m_App = App; return TRUE; }
    BOOL SpellSound(long Num);
};

#endif
