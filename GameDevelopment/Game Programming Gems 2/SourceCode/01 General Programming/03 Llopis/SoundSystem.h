/* Copyright (C) Noel Llopis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Noel Llopis, 2001"
 */
// In SoundSystem.h

class ISoundSystem {
  public:
    virtual ~ISoundSystem() {};
    virtual bool PlaySound ( handle hSound ) = 0;
    virtual bool StopSound ( handle hSound ) = 0;
};
