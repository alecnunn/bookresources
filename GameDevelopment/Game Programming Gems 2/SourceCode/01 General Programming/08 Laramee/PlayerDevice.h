/* Copyright (C) François Dominic Laramée, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) François Dominic Laramée, 2001"
 */
class PlayerDevice : public Entity
{
  // ...
  void UpdateYourself();
  void QueryInputDevice() = 0;  // No implementation in PlayerDevice
};

class JoystickPlayerDevice : public PlayerDevice
{
  // ...
  void QueryInputDevice();
};

