// System Mouse Listener class

#pragma once


namespace System
{
    /// Implement this interface and register with a display to recieve mouse events.
    /// See Display::addMouseListener for details.

	class MouseListener
	{
	public:

        /// called when a mouse button is pressed down.

		virtual void onMouseDown(const Mouse &mouse, int x, int y) = 0;

        /// called when the mouse cursor is moved.

		virtual void onMouseMove(const Mouse &mouse, int x, int y) = 0;

        /// called when a pressed mouse button is released.

		virtual void onMouseUp(const Mouse &mouse, int x, int y) = 0;
	};
}
