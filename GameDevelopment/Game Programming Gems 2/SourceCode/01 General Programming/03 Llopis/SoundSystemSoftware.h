/* Copyright (C) Noel Llopis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Noel Llopis, 2001"
 */
// In SoundSystemSoftware.h

#include "SoundSystem.h"

class SoundSystemSoftware : public ISoundSystem {
  public:
    virtual ~SoundSystemSoftware();
    virtual bool PlaySound ( handle hSound );
    virtual bool StopSound ( handle hSound );

    // The rest of the functions in the implementation
};
