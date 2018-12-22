#ifndef _GAMEGEMS3_SYNTH_H
#define _GAMEGEMS3_SYNTH_H

/**
 *	Stochastic Synthesis for Game Gems III
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

#ifndef PI
#define PI (3.1415926535897932384626433832795)
#endif

typedef float GGSynthFloat;

// calculate sample in a block for more efficient CPU usage
#define GGSYNTH_FRAMES_PER_BLOCK    (8)

#define NextLinearCongruential(previous) (((previous) * 196314165) + 907633515)


#endif /* _GAMEGEMS3_SYNTH_H */
