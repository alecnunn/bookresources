#ifndef GPL_VERSION_H
#define GPL_VERSION_H

/******************************************************************************

Coral Tree gpl/version.h
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#include <gpl/typedefs.h>
#include <gpl/persist.h>
#include <gpl/depict.h>

#define	GPL_VERSION_ANY	((GPL_Integer) -1)
#define	GPL_VERSION_ALPHA	(GPL_AVAILABILITY_ALPHA)
#define	GPL_VERSION_BETA	(GPL_AVAILABILITY_BETA)
#define	GPL_VERSION_GENERAL	(GPL_AVAILABILITY_GENERAL)

/***********************************************************************
enum GPL_Availability
***********************************************************************/
enum GPL_Availability
{
	GPL_AVAILABILITY_ALPHA		=	0,
	GPL_AVAILABILITY_BETA		=	1,
	GPL_AVAILABILITY_GENERAL	=	2
};

/***********************************************************************
class GPL_Version
***********************************************************************/
class GPL_Version : public GPL_Persistent, public GPL_Depictable
{
	public:
		GPL_Version(void);
		GPL_Version(const GPL_Integer maj, const GPL_Integer min,
					const GPL_Integer p, const GPL_Availability a,
					const GPL_String & n);
		virtual ~GPL_Version(void);

		GPL_Boolean operator==(const GPL_Version & v) const;
		GPL_Boolean operator!=(const GPL_Version & v) const
		{
			return !(operator==(v));
		}
		GPL_Boolean operator<(const GPL_Version & v) const;
		GPL_Boolean operator<=(const GPL_Version & v) const
		{
			return operator<(v) || operator==(v);
		}
		GPL_Boolean operator>(const GPL_Version & v) const;
		GPL_Boolean operator>=(const GPL_Version & v) const
		{
			return operator>(v) || operator==(v);
		}

		inline GPL_Integer GetMajor(void) const { return major; }
		inline GPL_Integer GetMinor(void) const { return minor; }
		inline GPL_Integer GetPatch(void) const { return patch; }
		inline GPL_Availability GetAvailability(void) const { return availability; }
		inline const GPL_String & GetCodeName(void) const { return codeName; }

		inline void SetMajor(const GPL_Integer maj) { major = maj; }
		inline void SetMinor(const GPL_Integer min) { minor = min; }
		inline void SetPatch(const GPL_Integer p) { patch = p; }
		inline void SetAvailability(const GPL_Availability a) { availability = a; }
		inline void SetCodeName(const GPL_String & n) { codeName = n; }

		// GPL_Depictable
		virtual GPL_Boolean Print(GPL_String & string,
									const GPL_Verbosity verbosity) const;
		virtual GPL_Boolean Parse(const GPL_String & string);

		// GPL_Persistent
		virtual GPL_Boolean Pack(GPL_Message & msg) const;
		virtual GPL_Boolean Unpack(GPL_Message & msg);

		// GPL_Object
		GPL_OBJECT_INTERFACE(GPL_Version);

	protected:
		GPL_Integer major;
		GPL_Integer minor;
		GPL_Integer patch;
		GPL_Availability availability;
		GPL_String codeName;
};

extern const GPL_Version gplCoralVersion;

#endif /* GPL_VERSION_H */
