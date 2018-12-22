#ifndef GPL_DEPICT_H
#define GPL_DEPICT_H

#include <gpl/str.h>

/***********************************************************************
enum GPL_Verbosity
***********************************************************************/
enum GPL_Verbosity
{
	GPL_VERBOSITY_CONCISE	=	0,
	GPL_VERBOSITY_VERBOSE	=	1
};

#define	GPL_DEPICTABLE_CONCISE	GPL_VERBOSITY_CONCISE
#define	GPL_DEPICTABLE_VERBOSE	GPL_VERBOSITY_VERBOSE

/***********************************************************************
class GPL_Depictable

Abstract, base class for objects that can be depicted by a string.
***********************************************************************/
class GPL_Depictable
{
	public:
		virtual GPL_Boolean Print(GPL_String & string,
									const GPL_Verbosity verbosity =
										GPL_VERBOSITY_CONCISE) const
		{
			GPL_String s = string;s = s;
			GPL_Verbosity v = verbosity;v = v;
			return FALSE;
		}
		virtual GPL_Boolean Parse(const GPL_String & string)
		{
			GPL_String s = string;s = s;
			return FALSE;
		}
		virtual GPL_String ToString(const GPL_Verbosity verbosity =
										GPL_VERBOSITY_CONCISE) const
		{
			GPL_String image;
			Print(image, verbosity);
			return image;
		}
};

#endif /* GPL_DEPICT_H */
