// System Display Adapter class

#pragma once

#include "Display.h"
#include "KeyboardListener.h"
#include "MouseListener.h"
#include "WindowListener.h"
#include "Mathematics/Color.h"
#include "Mathematics/Matrix.h"

#include <string>
#include <vector>
#include <algorithm>


namespace System
{
    using namespace Mathematics;

    /// This adapter class implements common display functionality.
    /// Handling object management, garbage collection and listener management
    /// here saves implementing it from scratch in each driver display class...

	class DisplayAdapter : public Display
	{
        std::string _title;         ///< display title
        int _width;                 ///< display mode x resolution or width of display window
        int _height;                ///< display mode y resolution or width of display window
        bool _windowed;             ///< true if display output is in a window, false if fullscreen

        typedef std::vector<KeyboardListener*> KeyboardListeners;       ///< convenient array of keyboard listeners type
		typedef std::vector<MouseListener*> MouseListeners;             ///< convenient array of mouse listeners type
		typedef std::vector<WindowListener*> WindowListeners;           ///< convenient array of window listeners type

		KeyboardListeners _keyboardListeners;          ///< array of keyboard listener objects
		MouseListeners _mouseListeners;                ///< array of mouse listener objects
		WindowListeners _windowListeners;              ///< array of window listener objects

		typedef std::vector<Object*> Objects;          ///< convenient array of objects type

		Objects _objects;                              ///< array of managed objects. these objects are garbage collected each update.

	protected:

        /// notify all keyboard listeners that a key has been pressed down

		void notifyKeyDown(const Key &key)
		{
			for (KeyboardListeners::iterator i = _keyboardListeners.begin(); i != _keyboardListeners.end(); i++)
				(*i)->onKeyDown(key);
		}

        /// notify all keyboard listeners that a key has been released

		void notifyKeyUp(const Key &key)
		{
			for (KeyboardListeners::iterator i = _keyboardListeners.begin(); i != _keyboardListeners.end(); i++)
				(*i)->onKeyUp(key);
		}

        /// notify all mouse listeners that a mouse button has been pressed down

		void notifyMouseDown(const Mouse &mouse, int x, int y)
		{
			for (MouseListeners::iterator i = _mouseListeners.begin(); i != _mouseListeners.end(); i++)
				(*i)->onMouseDown(mouse, x, y);
		}

        /// notify all mouse listeners that the mouse has moved

		void notifyMouseMove(const Mouse &mouse, int x, int y)
		{
			for (MouseListeners::iterator i = _mouseListeners.begin(); i != _mouseListeners.end(); i++)
				(*i)->onMouseMove(mouse, x, y);
		}

        /// notify all mouse listeners that a mouse button has been released

		void notifyMouseUp(const Mouse &mouse, int x, int y)
		{
			for (MouseListeners::iterator i = _mouseListeners.begin(); i != _mouseListeners.end(); i++)
				(*i)->onMouseUp(mouse, x, y);
		}

        /// notify all window listeners that the window has been activated

        void notifyActivate(bool active)
		{
			for (WindowListeners::iterator i = _windowListeners.begin(); i != _windowListeners.end(); i++)
				(*i)->onActivate(active);
		}

        /// notify all window listeners that the window is about to be closed.
        /// returns false if any of the window listeners do not want the window to be closed.

		bool notifyClose()
		{
            bool result = true;

			for (WindowListeners::iterator i = _windowListeners.begin(); i != _windowListeners.end(); i++)
				if (!(*i)->onClose())
                    result = false;

            return result;
		}

        /// add a managed object.
        /// from now on this object pointer is owned by the display, it is reference counted
        /// and automatically deleted when its reference count is zero.

		void addObject(Object *object)
		{
			assert(object);
			_objects.push_back(object);
		}

	public:

        /// destructor.
        /// destroys all managed objects.

		virtual ~DisplayAdapter()
		{
			// destroy all objects

			Objects::iterator i = _objects.begin();
			while (i != _objects.end())
			{
				delete *i;
				i = _objects.erase(i);
			}
		}

        /// open display.
        /// sets all data fields: title, width, height, windowed.

        virtual void open(const char title[], int width, int height, bool windowed)
        {
            _title = title;
            _width = width;
            _height = height;
            _windowed = windowed;
        }

        /// update display.
        /// performs garbage collection on managed objects.

		virtual void update()
		{
			// object garbage collection

			Objects::iterator i = _objects.begin();
			while (i != _objects.end())
			{
				Object *object = *i;
				if (!object->references())
				{
					delete object;
					i = _objects.erase(i);
				}
				else
					++i;
			}
		}

        virtual const char* title() const
        {
            return _title.c_str();
        }

        virtual int width() const
        {
            return _width;
        }

        virtual int height() const
        {
            return _height;
        }

        virtual bool windowed() const
        {
            return _windowed;
        }

        virtual void addKeyboardListener(KeyboardListener &listener)
		{
			KeyboardListeners::iterator i = std::find(_keyboardListeners.begin(), _keyboardListeners.end(), &listener);
			if (i==_keyboardListeners.end())
				_keyboardListeners.push_back(&listener);
		}

		virtual void addMouseListener(MouseListener &listener)
		{
			MouseListeners::iterator i = std::find(_mouseListeners.begin(), _mouseListeners.end(), &listener);
			if (i==_mouseListeners.end())
				_mouseListeners.push_back(&listener);
		}

		virtual void addWindowListener(WindowListener &listener)
		{
			WindowListeners::iterator i = std::find(_windowListeners.begin(), _windowListeners.end(), &listener);
			if (i==_windowListeners.end())
				_windowListeners.push_back(&listener);
		}

		virtual void removeKeyboardListener(KeyboardListener &listener)
		{
			KeyboardListeners::iterator i = std::find(_keyboardListeners.begin(), _keyboardListeners.end(), &listener);
			if (i==_keyboardListeners.end())
				_keyboardListeners.erase(i);
		}

		virtual void removeMouseListener(MouseListener &listener)
		{
			MouseListeners::iterator i = std::find(_mouseListeners.begin(), _mouseListeners.end(), &listener);
			if (i==_mouseListeners.end())
				_mouseListeners.erase(i);
		}

		virtual void removeWindowListener(WindowListener &listener)
		{
			WindowListeners::iterator i = std::find(_windowListeners.begin(), _windowListeners.end(), &listener);
			if (i==_windowListeners.end())
				_windowListeners.erase(i);
		}
	};
}
