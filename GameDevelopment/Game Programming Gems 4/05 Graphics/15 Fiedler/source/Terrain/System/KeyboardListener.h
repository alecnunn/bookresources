// System Keyboard Listener class

#pragma once


namespace System
{
    /// Implement this interface and register with a display to recieve keyboard events.
    /// See Display::addKeyboardListener, Display::removeKeyboardListener for details.

	class KeyboardListener
	{
	public:

        /// called when a key is pressed down.

		virtual void onKeyDown(const Key &key) = 0;

        /// called when a key that was pressed down is released.

		virtual void onKeyUp(const Key &key) = 0;
	};
}
