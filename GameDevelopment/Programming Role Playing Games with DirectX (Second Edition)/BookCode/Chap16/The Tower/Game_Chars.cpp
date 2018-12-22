#include "global.h"

// Look for PC character from WinMain.cpp
extern sCharacter *g_PCChar;

///////////////////////////////////////////////////////////
// cChars class code
///////////////////////////////////////////////////////////
BOOL cChars::PCUpdate(sCharacter *Character, long Elapsed,    \
                    float *XMove, float *YMove, float *ZMove)
{
  float Speed;
  sCharacter *TargetChar;
  float XDiff, YDiff, ZDiff;
  float Dist, Range;
  char Filename[MAX_PATH];
  long Spell = -1;

  // Don't update if no elapsed time
  if(!Elapsed)
    return TRUE;

  // Rotate character
  if(m_App->m_Keyboard.GetKeyState(KEY_LEFT) == TRUE) {
    Character->Direction -= (float)Elapsed / 1000.0f * 4.0f;
    Character->Action = CHAR_MOVE;
  }

  if(m_App->m_Keyboard.GetKeyState(KEY_RIGHT) == TRUE) {
    Character->Direction += (float)Elapsed / 1000.0f * 4.0f;
    Character->Action = CHAR_MOVE;
  }

  if(m_App->m_Keyboard.GetKeyState(KEY_UP) == TRUE) {
    Speed = (float)Elapsed / 1000.0f *                        \
                   m_App->m_CharController.GetSpeed(Character);
    *XMove = (float)sin(Character->Direction) * Speed;
    *ZMove = (float)cos(Character->Direction) * Speed;
    Character->Action = CHAR_MOVE;
  }

  // Process attack/talk action
  if(m_App->m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE) {
    
    // See which character is being pointed at and make
    // sure it's a monster character.
    if((TargetChar = m_App->GetCharacterAt(                   \
                       m_App->m_Mouse.GetXPos(),              \
                       m_App->m_Mouse.GetYPos())) != NULL) {

      // Handle talking to NPCs
      if(TargetChar->Type == CHAR_NPC) {
        // No distance checks, just process their script
        sprintf(Filename, "..\\Data\\Char%lu.mls",            \
                TargetChar->ID);
        m_App->m_Script.Execute(Filename);

        return TRUE; // Don't process anymore
      }
                       
      // Handle attacking monsters
      if(TargetChar->Type == CHAR_MONSTER) {

        // Get distance to target
        XDiff = (float)fabs(TargetChar->XPos-Character->XPos);
        YDiff = (float)fabs(TargetChar->YPos-Character->YPos);
        ZDiff = (float)fabs(TargetChar->ZPos-Character->ZPos);
        Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;

        // Offset dist by target's radius
        Range = GetXZRadius(TargetChar);
        Dist -= (Range * Range);
      
        // Get maximum attack range
        Range = GetXZRadius(Character);
        Range += Character->Def.Range;

        // Only perform attack if target in range
        if(Dist <= (Range * Range)) {

          // Set target/victim info
          TargetChar->Attacker = Character;
          Character->Victim    = TargetChar;

          // Face victim
          XDiff = TargetChar->XPos - Character->XPos;
          ZDiff = TargetChar->ZPos - Character->ZPos;
          Character->Direction = (float)atan2(XDiff, ZDiff);

          // Set action
          m_App->m_CharController.SetAction(Character,        \
                                            CHAR_ATTACK);
        }
      }
    }
  }

  // Cast magic spell based on # pressed
  if(m_App->m_Keyboard.GetKeyState(KEY_1) == TRUE) {
    m_App->m_Keyboard.SetLock(KEY_1, TRUE);
      Spell = 0;  // Fireball
  }
  if(m_App->m_Keyboard.GetKeyState(KEY_2) == TRUE) {
    m_App->m_Keyboard.SetLock(KEY_2, TRUE);
    Spell = 1;  // Ice
  }
  if(m_App->m_Keyboard.GetKeyState(KEY_3) == TRUE) {
    m_App->m_Keyboard.SetLock(KEY_3, TRUE);
    Spell = 2;  // Heal Self
  }
  if(m_App->m_Keyboard.GetKeyState(KEY_4) == TRUE) {
    m_App->m_Keyboard.SetLock(KEY_4, TRUE);
    Spell = 3;  // Teleport
  }
  if(m_App->m_Keyboard.GetKeyState(KEY_5) == TRUE) {
    m_App->m_Keyboard.SetLock(KEY_5, TRUE);
    Spell = 4;  // Groundball
  }

  // Cast spell if commanded
  if(Spell != -1) {

    // Only cast if spell known and has enough mana points
    if(g_PCChar->Def.MagicSpells[Spell/32]&(1<<(Spell&31)) && \
            g_PCChar->ManaPoints >=                           \
            m_App->m_SpellController.GetSpell(Spell)->Cost) {

      // See which character is being pointed
      if((TargetChar = m_App->GetCharacterAt(                 \
                       m_App->m_Mouse.GetXPos(),              \
                       m_App->m_Mouse.GetYPos())) != NULL) {

        // Don't target NPC's
        if(TargetChar->Type != CHAR_NPC) {

          // Get distance to target
          XDiff = (float)fabs(TargetChar->XPos-Character->XPos);
          YDiff = (float)fabs(TargetChar->YPos-Character->YPos);
          ZDiff = (float)fabs(TargetChar->ZPos-Character->ZPos);
          Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;

          // Offset dist by target's radius
          Range = GetXZRadius(TargetChar);
          Dist -= (Range * Range);
      
          // Get maximum spell range
          Range = GetXZRadius(Character);
          Range +=                                            \
            m_App->m_SpellController.GetSpell(Spell)->Distance;

          // Only perform spell if target in range
          if(Dist <= (Range * Range)) {

            // Set spell data
            Character->SpellNum    = Spell;
            Character->SpellTarget = CHAR_MONSTER;
      
            // Store target coordinates
            Character->TargetX = TargetChar->XPos;
            Character->TargetY = TargetChar->YPos;
            Character->TargetZ = TargetChar->ZPos;

            // Clear movement
            (*XMove) = (*YMove) = (*ZMove) = 0.0f;
        
            // Perform spell action
            SetAction(Character, CHAR_SPELL);

            // Face victim
            XDiff = TargetChar->XPos - Character->XPos;
            ZDiff = TargetChar->ZPos - Character->ZPos;
            Character->Direction = (float)atan2(XDiff, ZDiff);

            // Set action
            m_App->m_CharController.SetAction(Character,      \
                                              CHAR_SPELL);
          }
        }
      }
    }
  }

  // Enter status frame if right mouse button pressed
  if(m_App->m_Mouse.GetButtonState(MOUSE_RBUTTON) == TRUE) {
    m_App->m_Mouse.SetLock(MOUSE_RBUTTON, TRUE);
    m_App->m_StateManager.Push(m_App->StatusFrame, m_App);
  }

  return TRUE;
}

BOOL cChars::ValidateMove(sCharacter *Character,              \
                    float *XMove, float *YMove, float *ZMove)
{
  float Height;

  // Check against terrain mesh for collision
  if(m_App != NULL) {
    if(m_App->CheckIntersect(                                 \
         Character->XPos,                                     \
         Character->YPos + 8.0f,                              \
         Character->ZPos,                                     \
         *XMove + Character->XPos,                            \
         *YMove + Character->YPos + 8.0f,                     \
         *ZMove + Character->ZPos, NULL) == TRUE)
      return FALSE;
  }

  // Get new height
  Height = m_App->GetHeightBelow(*XMove + Character->XPos,    \
                                 32.0f  + Character->YPos,  \
                                 *ZMove + Character->ZPos);
  *YMove = Height - Character->YPos;

  // Check against barriers and clear movement if any
  if(m_App->m_Barrier.GetBarrier(*XMove + Character->XPos,    \
                                 *YMove + Character->YPos,    \
                                 *ZMove + Character->ZPos)!=0)
    (*XMove) = (*YMove) = (*ZMove) = 0.0f;

  return TRUE;
}

BOOL cChars::Experience(sCharacter *Character, long Amount)
{
  m_App->m_CombatExp += Amount;

  return FALSE;  // Don't display message
}

BOOL cChars::PCTeleport(sCharacter *Character, sSpell *Spell)
{
  // Teleport player to town
  m_App->SetupTeleport(1, 100.0f, 0.0f, -170.0f);

  return TRUE;
}

BOOL cChars::ActionSound(sCharacter *Character)
{
  if(m_App == NULL || Character == NULL)
    return TRUE;

  // Play sound based on character type and action
  switch(Character->Action) {
    case CHAR_ATTACK:
      if(!Character->ID)
        m_App->PlaySound(0);
      else
        m_App->PlaySound(1);
      break;

    case CHAR_SPELL:
      m_App->PlaySound(2);
      break;

    case CHAR_HURT:
      if(!Character->ID)
        m_App->PlaySound(4);
      else
        m_App->PlaySound(5);
      break;

    case CHAR_DIE:
      if(!Character->ID)
        m_App->PlaySound(6);
      else
        m_App->PlaySound(7);
      break;
  }

  return TRUE;
}

BOOL cChars::DropMoney(float XPos, float YPos, float ZPos,    \
                       long Quantity)
{
  m_App->m_CombatMoney += Quantity;

  return TRUE;
}

BOOL cChars::DropItem(float XPos, float YPos, float ZPos,     \
                      long Item, long Quantity)
{
  m_App->m_CombatItems[Item] += Quantity;

  return TRUE;
}
