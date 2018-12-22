

#include "wds.h"



/******************************************************************************
int		main(int argc,char **argv,char **envp)

Keywords:
******************************************************************************/
int main(int argc,char **argv,char **envp)
	{
	GPL_TableEntry default_entry;
	GPL_Tables tables;

	default_entry.SetLong(WDS_ELEMENT_DEFAULT_INIT);

	tables.SetFilename("win32.cfg");
	tables.SetDefault(&default_entry);
	tables.SetNumberRows(4);
	tables.SetRowLength(20);
	tables.LoadFromFile();

	return FALSE;
	}
