// System Window Listener class

#pragma once


namespace System
{
    /// Implement this interface and register with a display to recieve window events.
    /// See Display::addWindowListener for details.

	class WindowListener
	{
	public:

        /// called when window is activated and deactivated.

		virtual void onActivate(bool active) = 0;

        /// called when the window is about to close.
        /// you can return false to indicate that you do not want the window to close.

        virtual bool onClose() = 0;
	};
}
