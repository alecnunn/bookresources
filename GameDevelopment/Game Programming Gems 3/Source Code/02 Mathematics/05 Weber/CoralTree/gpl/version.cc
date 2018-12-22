/******************************************************************************

Coral Tree gpl/version.cc
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

#include <gpl/gpl.h>


const GPL_Version gplCoralVersion
	(CORAL_MAJOR, CORAL_MINOR, CORAL_PATCH, (GPL_Availability)(CORAL_AVAILABILITY), CORAL_CODENAME);

GPL_Version::GPL_Version(void)
{
	SetMajor(GPL_VERSION_ANY);
	SetMinor(GPL_VERSION_ANY);
	SetPatch(GPL_VERSION_ANY);
	SetAvailability(GPL_AVAILABILITY_GENERAL);
}

GPL_Version::GPL_Version(const GPL_Integer maj, const GPL_Integer min,
							const GPL_Integer p, const GPL_Availability a,
							const GPL_String & n)
{
	SetMajor(maj);
	SetMinor(min);
	SetPatch(p);
	SetAvailability(a);
	SetCodeName(n);
}

GPL_Version::~GPL_Version(void)
{
}

GPL_Boolean GPL_Version::operator==(const GPL_Version & v) const
{
	return major == v.major && minor == v.minor && patch == v.patch &&
		availability == v.availability && codeName == v.codeName;
}

GPL_Boolean GPL_Version::operator<(const GPL_Version & v) const
{
	if (major == v.major)
	{
		if (minor == v.minor)
		{
			if (patch == v.patch)
			{
				return (int) availability < (int) v.availability;
			}
			return GPL_VERSION_ANY == v.patch || GPL_VERSION_ANY == v.patch ||
				patch < v.minor;
		}
		return GPL_VERSION_ANY == v.minor || GPL_VERSION_ANY == v.minor ||
			minor < v.minor;
	}
	return GPL_VERSION_ANY == major || GPL_VERSION_ANY == v.major ||
		major < v.major;
}

GPL_Boolean GPL_Version::operator>(const GPL_Version & v) const
{
	if (major == v.major)
	{
		if (minor == v.minor)
		{
			if (patch == v.patch)
			{
				return (int) availability > (int) v.availability;
			}
			return GPL_VERSION_ANY == v.patch || GPL_VERSION_ANY == v.patch ||
				patch > v.minor;
		}
		return GPL_VERSION_ANY == v.minor || GPL_VERSION_ANY == v.minor ||
			minor > v.minor;
	}
	return GPL_VERSION_ANY == major || GPL_VERSION_ANY == v.major ||
		major > v.major;
}

GPL_Boolean GPL_Version::Print(GPL_String & string,
								const GPL_Verbosity verbosity) const
{
	GPL_String image = "";
	if (GPL_VERBOSITY_VERBOSE == verbosity)
	{
		if (GPL_VERSION_ANY != major)
		{
			image.Printf("%d", major);
		}
		else
		{
			image = "x";
		}
		string.Append(image);
		if (GPL_VERSION_ANY != minor)
		{
			image.Printf("%d", minor);
		}
		else
		{
			image = "x";
		}
		string.Join(".", image);
		if (GPL_VERSION_ANY != patch)
		{
			image.Printf("%d", patch);
		}
		else
		{
			image = "x";
		}
		string.Join(".", image);
	}
	else
	{
		if (GPL_VERSION_ANY != major)
		{
			if (GPL_VERSION_ANY != minor)
			{
				if (GPL_VERSION_ANY != patch)
				{
					image.Printf("%d.%d.%d", major, minor, patch);
				}
				else
				{
					image.Printf("%d.%d", major, minor);
				}
			}
			else
			{
				image.Printf("%d", major);
			}
		}
		string.Append(image);
	}
	if (GPL_VERBOSITY_VERBOSE == verbosity || GPL_VERSION_ANY != major)
	{
		switch (availability)
		{
		case GPL_AVAILABILITY_ALPHA:
			string.Join(" ", "Alpha");
			break;
		case GPL_AVAILABILITY_BETA:
			string.Join(" ", "Beta");
			break;
		default:
			string.Join (" ", "GA");
			break;
		}
	}
	if (0 < codeName.GetLength())
	{
		image.Printf("Code name: %s", codeName.GetData());
		string.Join(" ", image);
	}
	return TRUE;
}

GPL_Boolean GPL_Version::Parse(const GPL_String & string)
{
	static GPL_RegularExpression versionExpression("^([0-9]+)(\\.([0-9]+)(\\.([0-9]+))?)?([ 	]+(Alpha|Beta|GA))?([ 	]+Code name: (.*))?");
	if (versionExpression.Matches(string))
	{
		GPL_String s;
		if (!(s = versionExpression.GetMatch(1)).IsNull())
		{
			major = atoi(s.GetData());
		}
		else
		{
			major = GPL_VERSION_ANY;
		}
		if (!(s = versionExpression.GetMatch(3)).IsNull())
		{
			minor = atoi(s.GetData());
		}
		else
		{
			minor = GPL_VERSION_ANY;
		}
		if (!(s = versionExpression.GetMatch(5)).IsNull())
		{
			patch = atoi(s.GetData());
		}
		else
		{
			patch = GPL_VERSION_ANY;
		}
		if (!(s = versionExpression.GetMatch(7)).IsNull())
		{
			if (s == "Alpha")
			{
				availability = GPL_AVAILABILITY_ALPHA;
			}
			else if (s == "Beta")
			{
				availability = GPL_AVAILABILITY_BETA;
			}
			else
			{
				availability = GPL_AVAILABILITY_GENERAL;
			}
		}
		else
		{
			availability = GPL_AVAILABILITY_GENERAL;
		}
		codeName = versionExpression.GetMatch(9);
		return TRUE;
	}
	return FALSE;
}

GPL_Boolean GPL_Version::Pack(GPL_Message & msg) const
{
	msg.AppendLong(major);
	msg.AppendLong(minor);
	msg.AppendLong(patch);
	msg.AppendUnsignedChar(OSD_HELP_CAST(const unsigned long) availability);
	msg.AppendString(codeName);
	return TRUE;
}

GPL_Boolean GPL_Version::Unpack(GPL_Message & msg)
{
	major = msg.ReadLong();
	minor = msg.ReadLong();
	patch = msg.ReadLong();
	availability = (GPL_Availability) msg.ReadUnsignedChar();
	codeName = msg.ReadString();
	return TRUE;
}

GPL_OBJECT_IMPLEMENTATION(GPL_Version, 0x40000001, "GPL_Version");

