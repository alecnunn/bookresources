#ifndef GPL_FACTORY
#define GPL_FACTORY

#include <gpl/typedefs.h>
#include <gpl/map.h>
#include <gpl/str.h>

class GPL_Object;

/***********************************************************************
class GPL_ObjectFactory
***********************************************************************/
class GPL_ObjectFactory {

	public:
		typedef	GPL_Object * (*Creator) (void);

		static GPL_ObjectFactory * Instance (void);

		GPL_Boolean Add (const GPL_String & name, const GPL_ClassId id,
							Creator creator);
		Creator Get (const GPL_String & name) const;
		Creator Get (const GPL_ClassId id) const;
		GPL_Boolean Remove (const GPL_String & name);
		GPL_Boolean Remove (const GPL_ClassId id);
		GPL_Object * Create (const GPL_String & name);
		GPL_Object * Create (const GPL_ClassId id);

	protected:
		static GPL_ObjectFactory * factory;
		GPL_Map<GPL_String, Creator> names;
		GPL_Map<GPL_ClassId, Creator> ids;

		GPL_ObjectFactory (void) : names (GPL_String::HashFunction),
								ids (HashFunction) {}

		static unsigned long HashFunction (const GPL_ClassId & id) { return id; }
};

#endif /* GPL_FACTORY */
