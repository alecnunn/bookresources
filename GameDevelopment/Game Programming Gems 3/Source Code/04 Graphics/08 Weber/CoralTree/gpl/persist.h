#ifndef GPL_PERSISTENT_H
#define GPL_PERSISTENT_H

#include <gpl/typedefs.h>
#include <gpl/file.h>
#include <gpl/message.h>
#include <gpl/object.h>

/***********************************************************************
class GPL_Persistent

Base class for persistence.
***********************************************************************/
class GPL_Persistent : public GPL_Object
{
	public:
		virtual GPL_Boolean Pack(GPL_Message & msg) const;
		virtual GPL_Boolean Unpack(GPL_Message & msg);

		static GPL_Boolean PackToFile(const GPL_String & f,
										GPL_Persistent *object);
		static GPL_Boolean PackToFile(GPL_File & f, GPL_Persistent *object);

		static GPL_Boolean UnpackFromFile(const GPL_String & f,
											GPL_Persistent *object);
		static GPL_Boolean UnpackFromFile(GPL_File & f,
											GPL_Persistent *object);

		// GPL_Object
		GPL_OBJECT_ABSTRACT_INTERFACE(GPL_Persistent);
};

#endif /* GPL_PERSISTENT_H */
