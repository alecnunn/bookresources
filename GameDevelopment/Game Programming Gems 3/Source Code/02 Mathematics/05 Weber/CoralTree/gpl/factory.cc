#include <gpl/gpl.h>

/***********************************************************************

***********************************************************************/
GPL_ObjectFactory * GPL_ObjectFactory::factory = (GPL_ObjectFactory *) 0;

/***********************************************************************

***********************************************************************/
GPL_ObjectFactory * GPL_ObjectFactory::Instance (void)
{
	if ((GPL_ObjectFactory *) 0 == factory)
	{
		factory = new GPL_ObjectFactory;
	}
	return factory;
}

/***********************************************************************

***********************************************************************/
GPL_Boolean GPL_ObjectFactory::Add (const GPL_String & name,
									const GPL_ClassId id,
									GPL_ObjectFactory::Creator creator)
{
	// If there is not already an creator for name, insert it.
	register long success = TRUE;
	if ((success = success && !names.Contains (name)))
	{
		names.Insert (name, creator);
	}
	// If there is not already an creator for id, insert it.
	if ((success = success && !ids.Contains (id)))
	{
		ids.Insert (id, creator);
	}
	return success;
}

/***********************************************************************

***********************************************************************/
GPL_ObjectFactory::Creator GPL_ObjectFactory::Get (const GPL_String & name) const
{
	Creator creator = (Creator) 0;
	names.LookUp (name, creator);
	return creator;
}

/***********************************************************************

***********************************************************************/
GPL_ObjectFactory::Creator GPL_ObjectFactory::Get (const GPL_ClassId id) const
{
	Creator creator = (Creator) 0;
	ids.LookUp (id, creator);
	return creator;
}

/***********************************************************************

***********************************************************************/
GPL_Boolean GPL_ObjectFactory::Remove (const GPL_String & name)
{
	// If there is not a creator for name...
	if (!names.Contains (name))
	{
		names.Remove (name);
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************

***********************************************************************/
GPL_Boolean GPL_ObjectFactory::Remove (const GPL_ClassId id)
{
	// If there is not a creator for id...
	if (!ids.Contains (id))
	{
		ids.Remove (id);
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************

***********************************************************************/
GPL_Object * GPL_ObjectFactory::Create (const GPL_String & name)
{
	if (names.Contains (name))
	{
		return (GPL_Object *) names[name] ();
	}
	return (GPL_Object *) 0;
}

/***********************************************************************

***********************************************************************/
GPL_Object * GPL_ObjectFactory::Create (const GPL_ClassId id)
{
	if (ids.Contains (id))
	{
		return (GPL_Object *) ids[id] ();
	}
	return (GPL_Object *) 0;
}

