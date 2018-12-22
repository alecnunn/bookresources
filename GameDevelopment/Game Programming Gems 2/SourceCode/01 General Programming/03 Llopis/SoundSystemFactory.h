/* Copyright (C) Noel Llopis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Noel Llopis, 2001"
 */
// In SoundSystemFactory.h

class ISoundSystem;

class SoundSystemFactory {
public:
  enum SoundSystemType {
    SOUND_SOFTWARE,
    SOUND_HARDWARE,
    SOUND_SOMETHINGELSE
  };

  static ISoundSystem * CreateSoundSystem(SoundSystemType type);
};

