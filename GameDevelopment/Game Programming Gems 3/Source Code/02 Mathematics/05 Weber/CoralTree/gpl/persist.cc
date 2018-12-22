#include <gpl/gpl.h>

GPL_Boolean GPL_Persistent::Pack(GPL_Message & msg) const
{
	msg.AppendUnsignedShort(GetClassId());
	return TRUE;
}

GPL_Boolean GPL_Persistent::Unpack(GPL_Message & msg)
{
	register const GPL_ClassId id = msg.ReadUnsignedShort();
	return GetClassId() == id;
}

GPL_Boolean GPL_Persistent::PackToFile(const GPL_String & f, GPL_Persistent *object)
{
	GPL_File file(f.GetData(), OSD_FILE_WRITE | OSD_FILE_ERASE);
	file.SetPointer(0, OSD_FILE_BEGIN);
	return PackToFile(file, object);
}

GPL_Boolean GPL_Persistent::PackToFile(GPL_File & f, GPL_Persistent *object)
{
	// Let the sub-classes do their work.
	GPL_Message msg;
	object->Pack(msg);

	// Stuff the contents of the message into the file.
	unsigned long length = 0;
	register char * buffer = msg.GetBuffer(&length);
	return length == (unsigned long) (f.Write(buffer, length));
}

GPL_Boolean GPL_Persistent::UnpackFromFile(const GPL_String & f,
											GPL_Persistent *object)
{
	GPL_File file(f.GetData(), OSD_FILE_READ);
	file.SetPointer(0, OSD_FILE_BEGIN);
	return UnpackFromFile(file, object);
}

GPL_Boolean GPL_Persistent::UnpackFromFile(GPL_File & f,
											GPL_Persistent *object)
{
	// Determine the number of bytes to be read.
	register GPL_Boolean success = FALSE;
	OSD_FileAttributes attr;
	if (f.GetAttributes(attr))
	{
		register char *buffer = (char *) osdMalloc(attr.size);
		success = (attr.size == f.Read(buffer, attr.size));
		if (success)
		{
			// Stuff the contents of the file in the message.
			GPL_Message msg;
			msg.SetBuffer(buffer, attr.size);

			// Let the sub-classes do their work, which may include
			// over-riding the internal validity.
			success = object->Unpack(msg);
		}
		osdFree(buffer);
	}
	return success;
}
