/******************************************************************************

Coral Tree wds/makelook.cc
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

/******************************************************************************

This program loads and interprets laf files then saves them as a C/C++ array.

******************************************************************************/



#include "wds.h"



/******************************************************************************
int		main(const int argc,const char **argv,const char **envp)

******************************************************************************/
int main(const int argc,const char **argv,const char **envp)
	{
	WDS_NullApplication	*application;
	application=new WDS_NullApplication();
	application->MakeAware(argc,argv,envp);

	WDS_Looks looks;

	GPL_Array<GPL_String> *parameter=WBA_ENV.GetParameters();

	long parameters=parameter->GetSize();
	long m;
	char lookname[256];
	char variable[256];
	char filename[256];

	for(m=0;m<parameters;m++)
		{
		strcpy(lookname,(const char *)parameter->GetElement(m));
		sprintf(variable,"wdsLAF%s",lookname);
		sprintf(filename,"%s.cc",lookname);

		EW_PRINT(EW_APP,EW_LOG,"look (%s) variable (%s) to file (%s)",lookname,variable,filename);

		looks.Initialize(WDS_LOOK_WIN32,WDS_Widget::GetDefaultLooks());
		looks.LoadFromFile(lookname);
		looks.SaveAsC(filename,variable);
		}

	application->Exit();

	return FALSE;
	}
