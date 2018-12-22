/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// MIDIDef.h - MIDI defines

#ifndef MIDIDEF_H
#define MIDIDEF_H

#define MIDIDEF_MIDIEVENT       0x80
#define MIDIDEF_SYSEXEVENT      0xF0
#define MIDIDEF_METAEVENT       0xFF
#define MIDIDEF_METASEQN        0
#define MIDIDEF_METATEXT        1
#define MIDIDEF_METACOPYR       2
#define MIDIDEF_METATNAME       3
#define MIDIDEF_METAINAME       4
#define MIDIDEF_METALYRIC       5
#define MIDIDEF_METAMARKER      6
#define MIDIDEF_METACUEPT       7
#define MIDIDEF_METACHANPFX     0x20
#define MIDIDEF_METAEOT         0x2F
#define MIDIDEF_METATEMPO       0x51
#define MIDIDEF_METASMPTEOFF    0x54
#define MIDIDEF_METATIMESIG     0x58
#define MIDIDEF_METAKEYSIG      0x59
#define MIDIDEF_METASEQEVENT    0x7F

#endif