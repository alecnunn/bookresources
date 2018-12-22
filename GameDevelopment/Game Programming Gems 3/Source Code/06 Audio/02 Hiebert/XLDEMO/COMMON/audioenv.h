/****************************************************************************

   FILE: audioenv.h

   DESCRIPTION: header for audio environment class

****************************************************************************/

#include "switches.h"
#include "xlheader.h"

#if !defined (AUDIOENV)
#define AUDIOENV

class AudioEnv
{
private:
   ALuint buffer[256];
   ALuint source[256];
   ALuint nextBuffer, nextSource;
   unsigned int EAXlevel;
public:
   AudioEnv ();
   ~AudioEnv ();
   void Init ();
   void ListenerPosition (ALfloat*, ALfloat*);
   void UpdateObstruction (int);
   int LoadFile (char *, bool);
   int PlayFile (char *, bool);
   void SetSourcePosition (int, float *);
   void SetSourceVelocity (int, float *);
   void Play (int);
   void Stop (int);
   int IncrementEnv();
};

typedef AudioEnv * AudioEnvPtr;

#endif // AUDIOENV

