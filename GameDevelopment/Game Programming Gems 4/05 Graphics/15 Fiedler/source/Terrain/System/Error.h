// System Error class

#pragma once

#include <string>


namespace System
{
    /// An exception class with a text title and description

	class Error
	{
        std::string _message;       ///< error message string.

	public:

        /// construct error with given message.

		Error(const char message[])
		{
			_message = message;
		}

        /// get error title.
        /// if you want to define a new error type, derive from this class
        /// and return an identifying string in the override of this method.

		virtual const char* title() const
		{
			return "System Error";
		}

        /// get error message.

		virtual const char* message() const
		{
			return _message.c_str();
		}

        /// report error and shutdown.
        /// we may change this method to simply report the error and not shutdown...

        virtual int report()
        {
            printf("%s - %s", title(), message());
            exit(1);
            return 1;
        }
	};
}
