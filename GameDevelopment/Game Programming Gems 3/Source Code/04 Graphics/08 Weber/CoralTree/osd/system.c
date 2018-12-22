
/******************************************************************************

Coral Tree osd/system.c
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

#include "osd.h"
#if OSD_OSTYPE==OSD_UNIX
#if OSD_OS!=OSD_FBSD
#include <shadow.h>
#endif
#endif

char OSD_GLOBAL_BUFFER[OSD_GLOBAL_BUFFER_COUNT][OSD_GLOBAL_BUFFER_SIZE];

/* EMBEDDED VERSION INFO */
#ifdef CORAL_BUILDID
char OSD_BUILDID[] = CORAL_BUILDID;
#else
char OSD_BUILDID[] = "CoralBuildID improper build";
#endif

#if OSD_COMPILER==OSD_KR
long osdKill(pid)
	OSD_PID pid;
#else
long osdKill(OSD_PID pid)
#endif
{
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */
	if(pid <= 0) return FALSE;
	if(!kill(pid, SIGTERM))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
	HANDLE process;

	if(pid <= 0) return FALSE;
	process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(!process) return FALSE;
	if(!TerminateProcess(process, 0)) return FALSE;
	return TRUE;
#endif
}

/******************************************************************************
int		osdSpawnve(char *path, char *argv[], char **env)
keywords: blackbox, c, process, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
OSD_PID osdSpawnve(path, argv, env)
	char *path; 
	char *argv[]; 
	char **env;
#else
OSD_PID osdSpawnve(char *path, char *argv[], char **env)
#endif
{
	return osdSpawnveAsUser(NULL, NULL, path, argv, env);
}

#if OSD_COMPILER==OSD_KR
OSD_PID osdSpawnveAsUser(username, password, path, argv, env)
	char *username;
	char *password;
	char *path; 
	char *argv[]; 
	char **env;
#else
OSD_PID osdSpawnveAsUser(char *username, char *password, char *path, char *argv[], char **env)
#endif
{
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */
	OSD_PID pid;
	int n;
	char *tmp_argv[1];
	char *tmp_env[1];

	if(username)
	{
		if(!osdAuthenticateUser(username, password))
		{
			osdPrint(OSD_SYSTEM,OSD_ERROR,"could not authenticate user");
			return -1;
		}
	}

	pid = fork();

	if(pid < 0)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"fork failed");
		return -1;
	}

	if(pid != 0) return pid; /* parent continues on */

	if(!argv)
	{
		argv = tmp_argv;
		argv[0] = NULL;
	}

	if(!env)
	{
		env = tmp_env;
		env[0] = NULL;
	}

	if(username)
	{
		if(!osdUserSet(username, password))
		{
			exit(-1);
		}
	}

	n = execve(path, argv, env);

	if ( n == -1 )
	{
		exit(-1);
		/*return -1;*/
	}

	return 0; /* shut up compiler */
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
	PROCESS_INFORMATION p_info;
	STARTUPINFO s_info;

	char commandline[OSD_MAX_COMMAND_LINE_LEN];
	long alloc_sz;
	char **nt_envp;
	char *current_ptr;
	HANDLE user_token;

	int i,j,n;


	s_info.cb = sizeof(STARTUPINFO);
	s_info.lpReserved = NULL;
	s_info.lpDesktop = NULL;
	s_info.lpTitle = NULL;
	s_info.dwX = 0;
	s_info.dwY = 0;
	s_info.dwXSize = 0;
	s_info.dwYSize = 0;
	s_info.dwXCountChars = 0;
	s_info.dwYCountChars = 0;
	s_info.dwFillAttribute = 0;
	s_info.dwFlags = 0;
	s_info.wShowWindow = 0;
	s_info.cbReserved2 = 0;
	s_info.lpReserved2 = NULL;
	s_info.hStdInput = NULL;
	s_info.hStdOutput = NULL;
	s_info.hStdError = NULL;
	

	i = 0; j = 0;
	if ( strlen(path) > OSD_MAX_COMMAND_LINE_LEN )
	{
		osdPrint(OSD_SOFTWARE,OSD_ERROR,"command line of child too long");
		return -1;
	}
	strcpy(&commandline[0],path);
	j+=strlen(path);
	if(argv)
	{
		while( argv[i] != NULL )
		{
			if(i) /* SKIP THE FIRST ARGUMENT WHICH SHOULD BE THE FILENAME */
			{
				if ((strlen(argv[i]) + j + 1) > OSD_MAX_COMMAND_LINE_LEN )
				{
					osdPrint(OSD_SOFTWARE,OSD_ERROR,"command line of child too long");
					return -1;
				}

				if(j)
				{
					commandline[j] = ' ';
					j++;
				}

				strcpy(&commandline[j],argv[i]);
				j+=strlen(argv[i]);
			}
			i++;
		}
	}

	i = 0;
	alloc_sz = 0;
	if(env)
	{
		/* FIRST CALC SIZE */
		while ( env[i] != NULL )
		{
			alloc_sz += strlen(env[i]); // entry
			alloc_sz += sizeof(char); // NULL termination
			i++;
		}
		alloc_sz += sizeof(char); // NULL termination
		nt_envp = (char **)osdMalloc(alloc_sz+1);

		/* NOW CONSTRUCT */
		current_ptr = (char *)(&(nt_envp[0]));
		i = 0;
		while ( env[i] != NULL )
		{
			strcpy(current_ptr, env[i]);
			current_ptr += strlen(env[i]) + 1;
			i++;
		}
		current_ptr = NULL;
		current_ptr++;
		current_ptr = NULL; // extra one in case there is an env of zero args.
	}
	else
	{
		nt_envp = NULL;	
	}
	

	if(username)
	{
		if(!password)
		{
			osdPrint(OSD_SYSTEM,OSD_ERROR,"need password to change user");
			return FALSE;
		}
		if(!LogonUser(username,NULL,password,LOGON32_LOGON_INTERACTIVE,LOGON32_PROVIDER_DEFAULT,&user_token))
		{
			osdPrint(OSD_SYSTEM,OSD_ERROR,"could not logon user");
			return FALSE;
		}
		n = CreateProcessAsUser
				(
				user_token,
				(LPCTSTR)NULL,
				commandline,
				(LPSECURITY_ATTRIBUTES)NULL,
				(LPSECURITY_ATTRIBUTES)NULL,
				(BOOL)FALSE,
				(DWORD)(CREATE_NEW_PROCESS_GROUP),
				(LPVOID)nt_envp,
				(LPSTR)NULL,
				(LPSTARTUPINFO)&s_info,
				(LPPROCESS_INFORMATION)&p_info
				);
	}
	else
	{
		n = CreateProcess
				(
				(LPCTSTR)NULL,
				commandline,
				(LPSECURITY_ATTRIBUTES)NULL,
				(LPSECURITY_ATTRIBUTES)NULL,
				(BOOL)FALSE,
				(DWORD)(CREATE_NEW_PROCESS_GROUP),
				(LPVOID)nt_envp,
				(LPSTR)NULL,
				(LPSTARTUPINFO)&s_info,
				(LPPROCESS_INFORMATION)&p_info
				);
	}


	if(nt_envp)
	{
		osdFree(nt_envp);
	}

	if(!n)
	{

		osdPrint(OSD_SYSTEM,OSD_WARN,"could not start new process");
		return -1;
	}
		
	CloseHandle(p_info.hProcess);
	CloseHandle(p_info.hThread);

	return (OSD_PID)(p_info.dwProcessId);
#endif
}

/******************************************************************************
long osdExecute(char *command, int *status, char **output);

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdExecute(command, status, output, printStdout)
	char *command; 
	int *status;
	char **output;
	int printStdout;
#else
long osdExecute(char *command, int *status, char **output, int printStdout)
#endif
{
#define	OSD_EXECUTE_BUFFER_SIZE	1024
	static char buffer[OSD_EXECUTE_BUFFER_SIZE];
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */
	FILE *f;
	size_t n;

	if (NULL == (f = popen(command, "r")))
	{
		osdPrint(OSD_SYSTEM, OSD_WARN, "could not start new process");
		return FALSE;
	}

	osdMemset(buffer, 0, OSD_EXECUTE_BUFFER_SIZE);
	n = 0;
	if(output)
	{
		*output = (char *) 0;
	}
	while (!feof(f))
	{
		if ((char *) 0 != fgets(buffer, OSD_EXECUTE_BUFFER_SIZE, f))
		{
			/* Reallocating for each line is definitely slower, but */
			/* it is also simpler. */
			if ((char *) 0 == (*output = (char *) osdRealloc(*output, n + strlen(buffer) + 1)))
			{
				osdPrint(OSD_SYSTEM, OSD_ERROR, "could not allocate memory");
				return FALSE;
			}
			osdMemset((void *) ((*output) + n), 0, strlen(buffer) + 1);
			strcat(*output, buffer);
			n = strlen(*output);
			if(printStdout){osdOutputStr(FALSE, buffer);}
		}
	}

	*status = pclose(f);
	return TRUE;
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
#if 0
	FILE *f;
	size_t n;
	char *cmd_command;
#define OSD_CMD_LINE "cmd /C"
	cmd_command = (char *)osdMalloc(strlen(command)+strlen(OSD_CMD_LINE)+2);
	if(!cmd_command)
	{
		osdPrint(OSD_SYSTEM, OSD_ERROR, "could not allocate memory");
		return FALSE;
	}
	sprintf(cmd_command,"%s %s",OSD_CMD_LINE, command);

	if (NULL == (f = _popen(cmd_command, "r")))
	{
		osdPrint(OSD_SYSTEM, OSD_WARN, "could not start new process");
		return FALSE;
	}


	osdMemset(buffer, 0, OSD_EXECUTE_BUFFER_SIZE);
	n = 0;
	if(output)
	{
		*output = (char *) 0;
	}
	while (!feof(f))
	{
		if ((char *) 0 != fgets(buffer, OSD_EXECUTE_BUFFER_SIZE, f))
		{
			/* Reallocating for each line is definitely slower, but */
			/* it is also simpler. */
			if ((char *) 0 == (*output = (char *) osdRealloc(*output, n + strlen(buffer) + 1)))
			{
				osdPrint(OSD_SYSTEM, OSD_ERROR, "could not allocate memory");
				return FALSE;
			}
			osdMemset((void *) ((*output) + n), 0, strlen(buffer) + 1);
			strcat(*output, buffer);
			n = strlen(*output);
			if(printStdout){osdOutputStr(FALSE, buffer);}
		}
	}

	*status = _pclose(f);
	return TRUE;
#else
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	SECURITY_ATTRIBUTES secAttr;
	HANDLE readPipe, readPipeTmp;
	HANDLE writePipe;
	HANDLE std_out_hndl, std_err_hndl;
	DWORD exitCode;
	DWORD bytesRead,d;
	char *cmd_command;
	long m,n;


	secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAttr.bInheritHandle = TRUE;
	secAttr.lpSecurityDescriptor = NULL;

	/* Create the pipe for communication between parent and child. */
	std_out_hndl = GetStdHandle(STD_OUTPUT_HANDLE);
	std_err_hndl = GetStdHandle(STD_ERROR_HANDLE);
	if (!CreatePipe(&readPipeTmp, &writePipe, &secAttr, 0))
	{
		osdPrint(OSD_SYSTEM, OSD_WARN, "could not create pipe");
		return FALSE;
	}
	if(!SetStdHandle(STD_OUTPUT_HANDLE, writePipe) )
	{
		osdPrint(OSD_SYSTEM, OSD_WARN, "could not redirect stdout");
		return FALSE;
	}
	if(!SetStdHandle(STD_ERROR_HANDLE, writePipe) )
	{
		osdPrint(OSD_SYSTEM, OSD_WARN, "could not redirect stderr");
		return FALSE;
	}

	if(!DuplicateHandle(GetCurrentProcess(), readPipeTmp,
			GetCurrentProcess(), &readPipe, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		osdPrint(OSD_SYSTEM, OSD_WARN, "could not duplicate handle");
		return FALSE;
	}

	CloseHandle(readPipeTmp);

	/* Populate the startup info structure. */
	/* How could cb be anything *BUT* sizeof(STARTUPINFO)? */
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
#if 0
	startupInfo.lpReserved = NULL;
	startupInfo.lpDesktop = NULL;
	startupInfo.lpTitle = NULL;
	startupInfo.dwX = 0;
	startupInfo.dwY = 0;
	startupInfo.dwXSize = 0;
	startupInfo.dwYSize = 0;
	startupInfo.dwXCountChars = 0;
	startupInfo.dwYCountChars = 0;
	startupInfo.dwFillAttribute = 0;
	startupInfo.dwFlags = 0;
	startupInfo.wShowWindow = 0;
	startupInfo.cbReserved2 = 0;
	startupInfo.lpReserved2 = NULL;
	startupInfo.hStdInput = NULL;
	startupInfo.hStdOutput = NULL;
	startupInfo.hStdError = NULL;
#endif

	/* Initialize the process info structure. */
	processInfo.hProcess = INVALID_HANDLE_VALUE;
	processInfo.hThread = INVALID_HANDLE_VALUE;
	processInfo.dwProcessId = 0;
	processInfo.dwThreadId = 0;

	/* fInheritHandles is set to TRUE since we want to use a handle */
	/* from the parent to return standard output. */
	/* fdwCreate is zero since (in theory) none of its functionality */
	/* is needed.  Specifically, console data should go to the parent */
	/* through the handle and the command should handle whether or */
	/* not it has a window. */
	/* lpsiStartInfo must contain the handle for the child's standard */
	/* output, which the parent reads. */
	/* lppiProcInfo is where the parent gets a handle to the child */
	/* for determining if the child has exited. */
#define OSD_CMD_LINE "cmd /C"
	cmd_command = (char *)osdMalloc(strlen(command)+strlen(OSD_CMD_LINE)+2);
	if(!cmd_command)
	{
		osdPrint(OSD_SYSTEM, OSD_ERROR, "could not allocate memory");
		return FALSE;
	}
	sprintf(cmd_command,"%s %s",OSD_CMD_LINE, command);
	if (!CreateProcess(NULL, cmd_command, NULL, NULL, TRUE, 0,
						NULL, NULL, &startupInfo, &processInfo))
	{
		osdPrint(OSD_SYSTEM, OSD_WARN, "could not start new process");
		return FALSE;
	}
	if (INVALID_HANDLE_VALUE == processInfo.hProcess)
	{
		osdPrint(OSD_SYSTEM, OSD_WARN, "could not connect to new process");
		return FALSE;
	}

	if(!SetStdHandle(STD_OUTPUT_HANDLE, std_out_hndl))
	{
		osdPrint(OSD_SYSTEM, OSD_WARN, "could reconnect stdout in parent");
		return FALSE;
	}
	if(!SetStdHandle(STD_ERROR_HANDLE, std_err_hndl))
	{
		osdPrint(OSD_SYSTEM, OSD_WARN, "could reconnect stdout in parent");
		return FALSE;
	}
	CloseHandle(writePipe);

	/* Initialize the output. */
	osdMemset(buffer, 0, OSD_EXECUTE_BUFFER_SIZE);
	n = 0;
	*output = (char *) 0;
	bytesRead = 0;

	m=1;
	exitCode = STILL_ACTIVE;
	while(exitCode == STILL_ACTIVE)
	{
		if(m)
		{
			while((m=ReadFile(readPipe, buffer, OSD_EXECUTE_BUFFER_SIZE, &bytesRead, NULL)) && bytesRead!=0)
			{
				/* Reallocating for each line is definitely slower, but */
				/* it is also simpler. */
				if ((char *) 0 == (*output = osdRealloc(*output, n + bytesRead + 1)))
				{
					osdPrint(OSD_SYSTEM, OSD_ERROR, "could not allocate memory");
					return FALSE;
				}
				osdMemset((*output) + n, 0, bytesRead + 1);
				strncat(*output, buffer, bytesRead);
				n = strlen(*output);
				if(printStdout)
				{
//					WriteFile(std_out_hndl,buffer,bytesRead,&d,NULL);
					osdOutputRaw(FALSE,buffer,bytesRead);
				}
					/*	{osdOutputStr(FALSE, buffer);}*/
				osdMemset(buffer, 0, OSD_EXECUTE_BUFFER_SIZE);
			}
			if(!m)
			{
				/* If the failure is not due to the child dying... */
				if (ERROR_BROKEN_PIPE != GetLastError())
				{
					osdPrint(OSD_SYSTEM, OSD_WARN, "could not read from new process");
				}
			}
		}
		if(!GetExitCodeProcess(processInfo.hProcess, &exitCode))
		{
			osdPrint(OSD_SYSTEM, OSD_WARN, "could not get exit code for dead process");
			return FALSE;
		}
	}

	/* Close the pipe. */
	CloseHandle(readPipe);
	osdFree(cmd_command);

	/* Return the exit code. */
	*status = (int) exitCode;

	return TRUE;
#endif
#endif /* OSD_OSTYPE==OSD_WIN32 */
}

/******************************************************************************
void	osdGetPID()

******************************************************************************/
OSD_PID osdGetPID()
{
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */
	return getpid();
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
	return GetCurrentProcessId();
#endif
}

/******************************************************************************

******************************************************************************/
#if OSD_COMPILER==OSD_KR
char *osdBasename(path)
	char *path;
#else
char *osdBasename(char *path)
#endif
{
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */
#define OSD_FS_SLASH '/'
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
#define OSD_FS_SLASH '\\'
#endif
	static char dot[2] = ".";
	long i;
	i = strlen(path);
	/* IF NO PATH RETURN DOT */
	if(!path) return (char *)dot;
	/* REMOVE TRAILING SLASHES */
	while((i > 0) && (path[--i] == OSD_FS_SLASH)) path[i] = 0;
	/* MOVE POINTER */
	while((i > 0) && (path[i-1] != OSD_FS_SLASH)) i--;
	/* IF NO PATH RETURN DOT */
	if(path[i] == '\0') return (char *)dot;

	return &path[i];
}



/******************************************************************************
void	osdPathnameConvert(char *out_name, char *in_name)
keywords: blackbox, c, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
void osdPathnameConvert(out_name, in_name)
	char *out_name; 
	char *in_name;
#else
void osdPathnameConvert(char *out_name, char *in_name)
#endif
{
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */
	char c;

	int length;
	int inpt, outpt;

	inpt = 0;outpt = 0;
	
	length = strlen(in_name);
	
	/* STRIP THE DRIVE LETTER IF THERE IS ONE */
	if( length >= 2 )
		if( in_name[1] == ':' )
			inpt = 2;

	while(inpt < length)
	{
		switch(in_name[inpt])
		{
			case '/':
			case '\\':
				out_name[outpt] = '/';
				break;
			default:
				c=in_name[inpt];

#ifdef OSD_FORCE_PATHNAME_LOWER_CASE
				/* force lower case */
				if(c>='A' && c<='Z')
					c='a'+ c-'A';
#endif

				out_name[outpt] = c;
				break;
		}
		inpt++;
		outpt++;
	}
	out_name[outpt] = '\0'; 
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
	int length;
	int inpt, outpt;

	inpt = 0;outpt = 0;
	
	length = strlen(in_name);
	
	while(inpt < length)
	{
		switch(in_name[inpt])
		{
			case '/':
			case '\\':
				out_name[outpt] = '\\';
				break;
			default:
				out_name[outpt] = in_name[inpt];
				break;
		}
		inpt++;
		outpt++;
	}

	out_name[outpt] = '\0'; 
#endif
}


/******************************************************************************
long	osdUserSet(char *username)
TODO: there is some allocations not matched with frees in here...fix
keywords: blackbox, c, system, user

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdUserSet(username, password)
	char *username;
	char *password;
#else
long osdUserSet(char *username, char *password)
#endif
{
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */

	struct passwd *passwd_ptr;
#if OSD_OS!=OSD_FBSD
	struct spwd *spwd_ptr;
#endif
	passwd_ptr = getpwnam(username);
	if(!passwd_ptr)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not find user name");
		return FALSE;
	}
	if(password != NULL)
	{
/******************************************************************************
Free BSD appears to have rolled shadow password access into getpwnam so
getspnam isnt necessary (it doesnt exist anyway).
******************************************************************************/
#if OSD_OS==OSD_FBSD
		if(strcmp(passwd_ptr->pw_passwd,crypt(password,passwd_ptr->pw_passwd)))
		{
			osdPrint(OSD_SYSTEM,OSD_ERROR,"invalid password");
			return FALSE;
		}
#else
		spwd_ptr = getspnam(username);
		if(!spwd_ptr)
		{
			if(strcmp(passwd_ptr->pw_passwd,crypt(password,passwd_ptr->pw_passwd)))
			{
				osdPrint(OSD_SYSTEM,OSD_ERROR,"invalid password");
				return FALSE;
			}
		}
		else
		{
			if(strcmp(spwd_ptr->sp_pwdp,crypt(password,spwd_ptr->sp_pwdp)))
			{
				osdPrint(OSD_SYSTEM,OSD_ERROR,"invalid password");
				return FALSE;
			}
		}
#endif
	}
	if(setuid(passwd_ptr->pw_uid) == -1)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not change uid");
		return FALSE;
	}
	return TRUE;

#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
#if OSD_OSVER==4
	HANDLE user_token;

	if(!password)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"need password to change user");
		return FALSE;
	}
	if(!LogonUser(username,NULL,password,LOGON32_LOGON_INTERACTIVE,LOGON32_PROVIDER_DEFAULT,&user_token))
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not logon user");
		return FALSE;
	}
	if(!ImpersonateLoggedOnUser(user_token))
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not become user");
		CloseHandle(user_token);
		return FALSE;
	}

	return TRUE;
#else
	osdPrint(OSD_SYSTEM,OSD_ERROR,"sorry, osdUserSet not implemented");
	return FALSE;
#endif
#endif
}

#if OSD_COMPILER==OSD_KR
long osdAuthenticateUser(username, password)
	char *username;
	char *password;
#else
long osdAuthenticateUser(char *username, char *password)
#endif
{
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */
/*TODO: THIS IS BASICLY CUT AND PASTE FROM osdUserSet(),at least macroize it */
	struct passwd *passwd_ptr;
#if OSD_OS!=OSD_FBSD
	struct spwd *spwd_ptr;
#endif
	passwd_ptr = getpwnam(username);
	if(!passwd_ptr)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not find user name");
		return FALSE;
	}
	if(password != NULL)
	{
#if OSD_OS==OSD_FBSD
		if(strcmp(passwd_ptr->pw_passwd,crypt(password,passwd_ptr->pw_passwd)))
		{
			osdPrint(OSD_SYSTEM,OSD_ERROR,"invalid password");
			return FALSE;
		}
#else
		spwd_ptr = getspnam(username);
		if(!spwd_ptr)
		{
			if(strcmp(passwd_ptr->pw_passwd,crypt(password,passwd_ptr->pw_passwd)))
			{
				osdPrint(OSD_SYSTEM,OSD_ERROR,"invalid password");
				return FALSE;
			}
		}
		else
		{
			if(strcmp(spwd_ptr->sp_pwdp,crypt(password,spwd_ptr->sp_pwdp)))
			{
				osdPrint(OSD_SYSTEM,OSD_ERROR,"invalid password");
				return FALSE;
			}
		}
#endif
	}
	return TRUE;
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
	return FALSE;
#else
	osdPrint(OSD_SYSTEM,OSD_ERROR,"sorry, osdAuthenticateUser not implemented");
	return FALSE;
#endif
}

/******************************************************************************
long osdUserGet(username, n);
keywords: blackbox, system, user, c

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdUserGet(username, n)
	char *username;
	long n;
#else
long osdUserGet(char *username, long n)
#endif
{
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */
	struct passwd *passwd_ptr;
	passwd_ptr = getpwuid(geteuid());
	if(!passwd_ptr)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not find user name");
		return FALSE;
	}
	strncpy(username,passwd_ptr->pw_name,n);
	return TRUE;
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
	DWORD nn;
	nn = n;
	if(!GetUserName(username,(LPDWORD)&nn))
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not get login name");
		return FALSE;
	}
	return TRUE;
#endif
}


/******************************************************************************
void osdSetLastError(OSD_ERROR_NUMBER errno)
keywords: blackbox, system, error, c

******************************************************************************/
#if OSD_COMPILER==OSD_KR
void osdSetLastError(errno)
	OSD_ERROR_NUMBER errno;
#else
void osdSetLastError(OSD_ERROR_NUMBER err)
#endif
{
#if OSD_OSTYPE==OSD_UNIX
	errno = err;
#elif OSD_OSTYPE==OSD_WIN32
	SetLastError(err);
#endif /* OSD_OSTYPE==OSD_WIN32 */
}


/******************************************************************************
OSD_ERROR_NUMBER	osdGetLastError(void)
keywords: blackbox, system, error, c

******************************************************************************/
#if OSD_COMPILER==OSD_KR
OSD_ERROR_NUMBER osdGetLastError()
#else
OSD_ERROR_NUMBER osdGetLastError(void)
#endif
{
#if OSD_OSTYPE==OSD_UNIX
	return errno;
#elif OSD_OSTYPE==OSD_WIN32
	return GetLastError();
#endif /* OSD_OSTYPE==OSD_WIN32 */
}


/******************************************************************************
long	osdCWDSet(char *path)
keywords: blackbox, system, c

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdCWDSet(path)
	char *path;
#else
long osdCWDSet(char *path)
#endif
{
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */
	int i;
	i = chdir(path);

	if(!i)
		return TRUE;
	else
		return FALSE;
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
	int i;
	i = SetCurrentDirectory(path);

	return i;
#endif
}


/******************************************************************************
long osdTimer(void)
keywords: blackbox, time, system, c

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdTimer()
#else
long osdTimer(void)
#endif
{
#if OSD_OSTYPE==OSD_UNIX /* begin Unix-specific section */
	static struct timeval timer;

#if OSD_OS==OSD_SUNOS || OSD_OS==OSD_HPUX || OSD_OS==OSD_LINUX || OSD_OS==OSD_LYNX || OSD_OS==OSD_SCO || OSD_OS==OSD_FBSD
	if( gettimeofday(&timer,NULL) )
#else
	if( gettimeofday(&timer) )
#endif
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not get time of day");
		return -1;
	}
	else
	{
		/* deresolve seconds to 20 bits */
		return ( (timer.tv_sec & 0x000FFFFF)*1000 + (timer.tv_usec/1000));
	}
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
	return timeGetTime();
#endif
}



/******************************************************************************
void	osdMilliSleep(long milliseconds)
Keywords: blackbox

******************************************************************************/
#if OSD_COMPILER==OSD_KR
void osdMilliSleep(milliseconds)
	long milliseconds;
#else
void osdMilliSleep(long milliseconds)
#endif
	{

#if OSD_OS==OSD_WINNT

/*	millisecond sleep
 *	Sleep(0) will just yield the processor
 */

	Sleep(milliseconds);

#elif OSD_OS==OSD_IRIX

/*	SGI's default to CLK_TCK=100 per second
 *	sginap(0) will just yield the processor
 */

	sginap(milliseconds * CLK_TCK/1000 );

#elif OSD_OS==OSD_SUNOS && OSD_OSVER==4

/*	microsecond sleep
 *	does usleep(0) yield?
 */

	usleep( (unsigned)(milliseconds * 1000) );

#elif OSD_OSTYPE==OSD_UNIX

/*	generic UNIX pause
 */

	static struct timeval timeout;

	timeout.tv_sec=milliseconds/1000;
	timeout.tv_usec=(milliseconds-timeout.tv_sec*1000)*1000;

#if OSD_OS==OSD_HPUX && OSD_OSVER==9
	if(select(0,(int *)NULL,(int *)NULL,(int *)NULL,&timeout) < 0)
		osdPrint(OSD_SYSTEM,OSD_ERROR,"osdMilliSleep(): failed using select() for pause");
#else
	if(select(0,(fd_set *)NULL,(fd_set *)NULL,(fd_set *)NULL,&timeout) < 0)
		osdPrint(OSD_SYSTEM,OSD_ERROR,"osdMilliSleep(): failed using select() for pause");
#endif

#else
	#error  osdMilliSleep() Neither OSD_WINNT nor OSD_UNIX used
#endif
	}

void *osdMallocDefault(size_t size)
{
	return osdMallocImpl(size);
}

void *osdCallocDefault(size_t x, size_t y)
{
	return osdCallocImpl(x,y);
}

void *osdReallocDefault(void *x, size_t y)
{
	return osdReallocImpl(x,y);
}

void  osdFreeDefault(void *x)
{
	osdFreeImpl(x);
}

void *osdMemsetDefault(void *x, int y, size_t z)
{
	return osdMemsetImpl(x,y,z);
}

void *osdMemcpyDefault(void *x, const void *y, size_t z)
{
	return osdMemcpyImpl(x,y,z);
}

int osdMemcmpDefault(const void *x, const void *y, size_t z)
{
	return osdMemcmpImpl(x,y,z);
}

void *osdMemmoveDefault(void *x, const void *y, size_t z)
{
	return osdMemmoveImpl(x,y,z);
}

void * (*osdMallocImplPtr)(size_t) = &osdMallocDefault;
void * (*osdCallocImplPtr)(size_t, size_t) = &osdCallocDefault;
void * (*osdReallocImplPtr)(void *,size_t) = &osdReallocDefault;
void   (*osdFreeImplPtr)(void *) = &osdFreeDefault;
void * (*osdMemsetImplPtr)(void *,int,size_t) = &osdMemsetDefault;
void * (*osdMemcpyImplPtr)(void *,const void *,size_t) = &osdMemcpyDefault;
int    (*osdMemcmpImplPtr)(const void *,const void *,size_t)=&osdMemcmpDefault;
void * (*osdMemmoveImplPtr)(void *,const void *,size_t) = &osdMemmoveDefault;

void *osdMalloc(size_t size)
{
	return (*osdMallocImplPtr)(size);
}

void *osdCalloc(size_t x, size_t y)
{
	return (*osdCallocImplPtr)(x,y);
}

void *osdRealloc(void *x, size_t y)
{
	return (*osdReallocImplPtr)(x,y);
}

void  osdFree(void *x)
{
	(*osdFreeImplPtr)(x);
}

void *osdMemset(void *x, int y, size_t z)
{
	return (*osdMemsetImplPtr)(x,y,z);
}

void *osdMemcpy(void *x, const void *y, size_t z)
{
	return (*osdMemcpyImplPtr)(x,y,z);
}

int osdMemcmp(const void *x, const void *y, size_t z)
{
	return (*osdMemcmpImplPtr)(x,y,z);
}

void *osdMemmove(void *x, const void *y, size_t z)
{
	return (*osdMemmoveImplPtr)(x,y,z);
}



#if OSD_COMPILER==OSD_KR
void *osdReallocImpl(ptr, size)
	void *ptr; 	
	unsigned long size;
#else
void *osdReallocImpl(void *ptr, unsigned long size)
#endif
{
	if(!ptr)
	{
		return osdMalloc(size);
	}
	else
	{
#if (OSD_OS==OSD_SUNOS && OSD_OSVER==4 && OSD_COMPILER!=OSD_GNU) && defined __cplusplus
		return (void *)realloc((char *)ptr,size);
#else
		return realloc(ptr,size);
#endif
	}
}

char *osdStrdup(const char *str)
{
	char *ptr;
	long len;
	len = strlen(str) + 1;
	ptr = (char *)osdMalloc(len);
	if(ptr)
	{
		osdMemcpy((void *)ptr, (void *)str, len);
	}
	return ptr;
}

#if OSD_COMPILER==OSD_KR
long osdGetEnvironmentVariable(name, value, size)
	char *name;
	char *value;
	long size;
#else
long osdGetEnvironmentVariable(char *name, char *value, long size)
#endif
{
#if OSD_OSTYPE==OSD_UNIX
	char *cp;
	cp = getenv(name);
	if ((char *) 0 != cp)
	{
		strncpy(value, cp, size - 1);
		value[size - 1] = (char) 0;
		return 1;
	}
	else
	{
		return 0;
	}
#else
#if OSD_OSTYPE==OSD_WIN32
	unsigned long cp;
	cp = GetEnvironmentVariable(name, value, size);
	if (0 != cp)
	{
		return 1;
	}
	else
	{
		return 0;
	}
#endif /* OSD_WIN32 */
#endif /* OSD_UNIX */
}

#if OSD_COMPILER==OSD_KR
double osdRoundInteger(x)
	double x;
#else
double osdRoundInteger(double x)
#endif
{
#if OSD_OSTYPE==OSD_WIN32
	return (0.5 < fmod(x, (double) 1.0)) ? ceil(x) : floor(x);
#else
	return rint(x);
#endif /* OSD_OSTYPE!=OSD_WIN32 */
}

OSD_Boolean osdIsWin95()
{
#if OSD_OSTYPE==OSD_WIN32
	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize = sizeof(osVer);
	if (GetVersionEx (&osVer) && 
		(osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
#else
	return FALSE;
#endif
}

char *osdPlatform(const char *fields)
{
	static char deflt[] = "hor";
	unsigned long len;
	unsigned long i;
	const char *fmt;
	long havenumber;
	static char rn[OSD_MAX_PLATFORM_LEN];
	long a,b;
#if OSD_OSTYPE==OSD_UNIX
	struct utsname nm;
	uname(&nm);
#else
	char numberstring[8];
#endif

	if(fields == NULL)
	{
		fmt = deflt;
	}
	else
	{
		fmt = fields;
	}

	rn[0] = '\0';
	b = strlen(fmt);
	for(a=0;a<b;a++)
	{
		if(a!=0)
		{
			strcat(rn,"_");
		}
#define OSD_ADD_FIELD(f) if(OSD_MAX_PLATFORM_LEN <= strlen(f)+strlen(rn)) return rn; strcat(rn,f);		
		switch(fmt[a])
		{
			case 's':
#if OSD_OSTYPE==OSD_UNIX
				OSD_ADD_FIELD("unix");
#elif OSD_OSTYPE==OSD_WIN32
				OSD_ADD_FIELD("win32");
#else
				OSD_ADD_FIELD("ntype");
#endif
				break;
			case 'c':
#if OSD_COMPILER==OSD_ANSI
				OSD_ADD_FIELD("ansi");
#elif OSD_COMPILER==OSD_KR
				OSD_ADD_FIELD("kr");
#elif OSD_COMPILER==OSD_GNU
				OSD_ADD_FIELD("gnu");
#elif OSD_COMPILER==OSD_VISUAL
				OSD_ADD_FIELD("visual");
#else
				OSD_ADD_FIELD("noc");
#endif
				break;
			case 't':
#if OSD_THREADS==OSD_CTHREAD_PKG
				OSD_ADD_FIELD("cthread");
#elif OSD_THREADS==OSD_SGI_SPROC
				OSD_ADD_FIELD("sproc");
#elif OSD_THREADS==OSD_WIN32_THREADS
				OSD_ADD_FIELD("win32th");
#elif OSD_THREADS==OSD_PTHREADS
				OSD_ADD_FIELD("pthread");
#elif OSD_THREADS==OSD_NO_THREADS
				OSD_ADD_FIELD("noth");
#else
				OSD_ADD_FIELD("noth");
#endif
				break;
#ifdef OSD_PLATFORM_GRAPHICS
			case 'w':
#if EW_WL==EW_XWIN
				OSD_ADD_FIELD("xwin");
#elif EW_WL==EW_WIN32
				OSD_ADD_FIELD("win32");
#elif EW_WL==EW_MEX
				OSD_ADD_FIELD("mex");
#else
				OSD_ADD_FIELD("nowl");
#endif
				break;
			case 'g':
#if EW_GL==EW_X_GFX
				OSD_ADD_FIELD("xgfx");
#elif EW_GL==EW_OPENGL
				OSD_ADD_FIELD("opengl");
#elif EW_GL==EW_IRISGL
				OSD_ADD_FIELD("irisgl");
#elif EW_GL==EW_GDI
				OSD_ADD_FIELD("gdi");
#else
				OSD_ADD_FIELD("nogl");
#endif
				break;
#endif
			case 'h':
#if OSD_HARDWARE==OSD_MIPS
				OSD_ADD_FIELD("mips");
#elif OSD_HARDWARE==OSD_SPARC
				OSD_ADD_FIELD("sparc");
#elif OSD_HARDWARE==OSD_HPPA
				OSD_ADD_FIELD("hppa");
#elif OSD_HARDWARE==OSD_X86
				OSD_ADD_FIELD("x86");
#elif OSD_HARDWARE==OSD_AXP
				OSD_ADD_FIELD("axp");
#elif OSD_HARDWARE==OSD_68K
				OSD_ADD_FIELD("moto");
#else
				OSD_ADD_FIELD(nm.machine);
#endif
				break;
			case 'o':
#if OSD_OS==OSD_IRIX
				OSD_ADD_FIELD("irix");
#elif OSD_OS==OSD_SUNOS
				OSD_ADD_FIELD("sunos");
#elif OSD_OS==OSD_HPUX
				OSD_ADD_FIELD("hpux");
#elif OSD_OS==OSD_WINNT
				OSD_ADD_FIELD("nt");
#elif OSD_OS==OSD_WIN31
				OSD_ADD_FIELD("win31");
#elif OSD_OS==OSD_WIN95
				OSD_ADD_FIELD("win95");
#elif OSD_OS==OSD_LINUX
				OSD_ADD_FIELD("linux");
#elif OSD_OS==OSD_NECEWS
				OSD_ADD_FIELD("ews");
#elif OSD_OS==OSD_FBSD
				OSD_ADD_FIELD("fbsd");
#elif OSD_OS==OSD_LYNX
				OSD_ADD_FIELD("lynx");
#elif OSD_OS==OSD_SCO
				OSD_ADD_FIELD("sco");
#else
				OSD_ADD_FIELD(nm.sysname);
#endif
				break;
			case 'r':
#if OSD_OSTYPE==OSD_UNIX
				len = strlen(rn);
				i = 0;
				havenumber = FALSE;
				while(nm.release[i] != '\0')
				{
					if(nm.release[i] >= '0' && nm.release[i] <= '9')
					{
						havenumber = TRUE;
						rn[len++] = nm.release[i];
						if(len >= OSD_MAX_PLATFORM_LEN) return rn;
						rn[len] = '\0';
					}
					else if(havenumber && nm.release[i] == '.')
					{
						break;
					}
					i++;
				}
#else
				sprintf(numberstring,"%d",OSD_OSVER);
				strcat(rn,numberstring);
#endif
				break;
			default:
				osdPrint(OSD_SYSTEM,OSD_ERROR,"invalid fields to osdSystemType");
				return NULL;
		}
	}

	return rn;
}

