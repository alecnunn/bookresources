// System Mouse class

#pragma once


namespace System
{
    /// Stores data about a mouse event.
    /// Essentially a wrapper class around a mouse button code.
    /// May be extended in the future to include key modifiers etc.
    /// See MouseListener::onMouseDown for an example of use.
	
    class Mouse
	{
	public:

        /// mouse buttons

		enum Button { NONE, LEFT, RIGHT, MIDDLE };

        /// default constructor.
        /// clears button to none.

		Mouse()
		{
			_button = NONE;
		}

        /// construct mouse object with specified button down.

		Mouse(Button button)
		{
			_button = button;
		}

        /// get button down state.

		Button button() const
		{
			return _button;
		}

	private:

		Button _button;     ///< button down state.
	};
}
