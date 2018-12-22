#ifndef _WINMAIN_H_
#define _WINMAIN_H_

#include "CharICS.h"
#include "Chars.h"
#include "Spell.h"

class cApp : public cApplication
{
  friend class cChars;

  private:
    cGraphics       m_Graphics;  // cGraphics object
    cCamera         m_Camera;    // cCamera object
    cFont           m_Font;      // cFont object

    cWindow         m_Stats;     // Window for HP/MP stats
    cWindow         m_Options;   // Window for spells

    cInput          m_Input;     // cInput object
    cInputDevice    m_Keyboard;  // Keyboard input object
    cInputDevice    m_Mouse;     // Mouse input object

    cMesh           m_TerrainMesh;    // Terrain mesh
    cObject         m_TerrainObject;  // Terrain object

    cVertexBuffer   m_Target;    // Target object
    cTexture        m_Buttons;   // Buttons and more pics.

    // Character and spell animation controllers
    cCharacterController m_CharController;
    cSpellController     m_SpellController;

    sItem           m_MIL[1024]; // The master item list

    // See which character mouse is pointing at
    long GetCharacterAt(long XPos, long YPos);

  public:
    cApp();

    BOOL Init();
    BOOL Shutdown();
    BOOL Frame();
};

#endif
