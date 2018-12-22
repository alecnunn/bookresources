//
// RNI Translate routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_RNI_TRANSLATE_H
#define __PTC_RNI_TRANSLATE_H

// include files
#include <windows.h>
#include <native.h>
#include "ptc.h"
#include "RNI/Key.h"
#include "RNI/Area.h"
#include "RNI/Mode.h"
#include "RNI/Color.h"
#include "RNI/Format.h"
#include "RNI/Palette.h"




// key translation
Key rni_translate_key(Hptc_Key *key);
Hptc_Key* rni_translate_key(const Key &key);

// area translation
Area rni_translate_area(Hptc_Area *area);
Hptc_Area* rni_translate_area(const Area &area);

// mode translation
Mode rni_translate_mode(Hptc_Mode *mode);
Hptc_Mode* rni_translate_mode(const Mode &mode);

// color translation
Color rni_translate_color(Hptc_Color *color);
Hptc_Color* rni_translate_color(const Color &color);

// format translation
Format rni_translate_format(Hptc_Format *format);
Hptc_Format* rni_translate_format(const Format &format);

// palette translation
Palette rni_translate_palette(Hptc_Palette *object);
Hptc_Palette* rni_translate_palette(const Palette &palette);

// string translation
const char* rni_translate_string(Hjava_lang_String *string);
Hjava_lang_String* rni_translate_string(const char string[]);




#endif
