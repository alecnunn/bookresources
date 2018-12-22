// System Key class

#pragma once


namespace System
{
    /// Stores data about a keyboard event.
    /// Essentially a wrapper class around a key scan code.
    /// May be extended in the future to include key modifiers etc.

	class Key
	{
	public:

        /// Key codes

		enum Code
		{
			ENTER          = '\n',
			BACKSPACE      = '\b',
			TAB            = '\t',
			CANCEL         = 0x03,
			CLEAR          = 0x0C,
			SHIFT          = 0x10,
			CONTROL        = 0x11,
			ALT            = 0x12,
			PAUSE          = 0x13,
			CAPSLOCK       = 0x14,
			ESCAPE         = 0x1B,
			SPACE          = 0x20,
			PAGEUP         = 0x21,
			PAGEDOWN       = 0x22,
			END            = 0x23,
			HOME           = 0x24,
			LEFT           = 0x25,
			UP             = 0x26,
			RIGHT          = 0x27,
			DOWN           = 0x28,
			COMMA          = 0x2C,      ///< ','
			PERIOD         = 0x2E,      ///< '.'
			SLASH          = 0x2F,      ///< '/'
			ZERO           = 0x30,
			ONE            = 0x31,
			TWO            = 0x32,
			THREE          = 0x33,
			FOUR           = 0x34,
			FIVE           = 0x35,
			SIX            = 0x36,
			SEVEN          = 0x37,
			EIGHT          = 0x38,
			NINE           = 0x39,
			SEMICOLON      = 0x3B,      ///< ';'
			EQUALS         = 0x3D,      ///< '='
			A              = 0x41,
			B              = 0x42,
			C              = 0x43,
			D              = 0x44,
			E              = 0x45,
			F              = 0x46,
			G              = 0x47,
			H              = 0x48,
			I              = 0x49,
			J              = 0x4A,
			K              = 0x4B,
			L              = 0x4C,
			M              = 0x4D,
			N              = 0x4E,
			O              = 0x4F,
			P              = 0x50,
			Q              = 0x51,
			R              = 0x52,
			S              = 0x53,
			T              = 0x54,
			U              = 0x55,
			V              = 0x56,
			W              = 0x57,
			X              = 0x58,
			Y              = 0x59,
			Z              = 0x5A,
			OPENBRACKET    = 0x5B,      ///< '['
			BACKSLASH      = 0x5C,      ///< '\'
			CLOSEBRACKET   = 0x5D,      ///< ']'
			NUMPAD0        = 0x60,
			NUMPAD1        = 0x61,
			NUMPAD2        = 0x62,
			NUMPAD3        = 0x63,
			NUMPAD4        = 0x64,
			NUMPAD5        = 0x65,
			NUMPAD6        = 0x66,
			NUMPAD7        = 0x67,
			NUMPAD8        = 0x68,
			NUMPAD9        = 0x69,
			MULTIPLY       = 0x6A,      ///< numpad '*'
			ADD            = 0x6B,      ///< numpad '+'
			SEPARATOR      = 0x6C,      ///< '-' or '_'
			SUBTRACT       = 0x6D,      ///< numpad '-'
			DECIMAL        = 0x6E,      ///< numpad '.'
			DIVIDE         = 0x6F,      ///< numpad '/'
			F1             = 0x70,
			F2             = 0x71,
			F3             = 0x72,
			F4             = 0x73,
			F5             = 0x74,
			F6             = 0x75,
			F7             = 0x76,
			F8             = 0x77,
			F9             = 0x78,
			F10            = 0x79,
			F11            = 0x7A,
			F12            = 0x7B,
			// silly workaround!
			#undef DELETE
			DELETE         = 0x7F,
			NUMLOCK        = 0x90,
			SCROLLLOCK     = 0x91,
			PRINTSCREEN    = 0x9A,
			INSERT         = 0x9B,
			HELP           = 0x9C,
			META           = 0x9D,
			BACKQUOTE      = 0xC0,
			QUOTE          = 0xDE,
			FINAL          = 0x18,
			CONVERT        = 0x1C,
			NONCONVERT     = 0x1D,
			ACCEPT         = 0x1E,
			MODECHANGE     = 0x1F,
			KANA           = 0x15,
			KANJI          = 0x19,
			UNDEFINED      = 0x0
		};

        /// construct key object with given scan code.

		Key(int code)
		{
			_code = code;
		}

        /// get key code.

		int code() const
		{
			return _code;
		}

	private:

		int _code;      ///< the key code.
	};
}
