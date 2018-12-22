
/******************************************************************************

Coral Tree wds/help.h
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


#ifndef	WDS_WINHELP
#define WDS_WINHELP	TRUE
#endif

#ifndef HELP_WM_HELP
#define	HELP_WM_HELP		0x000c
#endif



/******************************************************************************
class	WDS_Help

******************************************************************************/
class WDS_Help
	{
	private:

		char			lphelpfile[256];
		unsigned short	wcommand;
		unsigned long	dwdata;

	public:

						WDS_Help(void)
							{
							SetHelpFile("");
							SetData(0);
							SetCommand(0);
							};

		long			Spawn(void *hwnd)
							{
							if(WDS_WINHELP)
								return WinHelp((EW_DISPLAY_TYPE)hwnd,
												lphelpfile,wcommand,dwdata);
							else
								return FALSE;
							};

		void			SetHelpFile(char *set)	{ strcpy(lphelpfile,set); };
		char			*GetHelpFile(void)		{ return lphelpfile; };

		void			SetCommand(unsigned short set)	{ wcommand=set; };
		unsigned short	GetCommand(void)				{ return wcommand; };

		void			SetData(unsigned long set)		{ dwdata=set; };
		unsigned long	GetData(void)					{ return dwdata; };

		// convenience
		void			SetFCD(char *filename,unsigned short command,
														unsigned long data)
							{
							SetHelpFile(filename);
							SetCommand(command);
							SetData(data);
							};
	};
