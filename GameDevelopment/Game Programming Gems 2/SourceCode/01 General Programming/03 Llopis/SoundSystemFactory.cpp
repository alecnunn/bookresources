/* Copyright (C) Noel Llopis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Noel Llopis, 2001"
 */
// In SoundSystemFactory.cpp

#include "SoundSystemSoftware.h"
#include "SoundSystemHardware.h"
#include "SoundSYstemSomethingElse.h"

ISoundSystem * SoundSystemFactory::CreateSoundSystem ( SoundSystemType
                                                       type )
{
  ISoundSystem * pSystem;

  switch ( type ) {
    case SOUND_SOFTWARE:
      pSystem = new SoundSystemSoftware();
      break;
    case SOUND_HARDWARE:
      pSystem = new SoundSystemHardware();
      break;
    case SOUND_SOMETHINGELSE:
      pSystem = new SoundSystemSomethingElse();
      break;
    default:
      pSystem = NULL;
  }

  return pSystem;
}


