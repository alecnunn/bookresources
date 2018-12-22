
/******************************************************************************

Coral Tree osd/dir.c
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



#define OSD_SCANDIR_DEBUG		FALSE
#define OSD_DISSECT_DEBUG		FALSE



/******************************************************************************
long	osdDirectoryScan(char *dirname,char ***namelist,
				int (*select)(struct dirent *),
				int (*compare)(struct dirent **, struct dirent **) )

	returns
		>=0	for success (number of entries)
		-1	if unable to read directory

	allocates space into namelist
	can use osdFreeDirectory() to deallocate

	the select and compare functions are reserved for future use and
	should be set to NULL

Keywords: c
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdDirectoryScan(dirname,namelist,select,compare)
	char *dirname;
	char ***namelist;
	int (*select)();
	int (*compare)();
#else
long osdDirectoryScan(char *dirname,char ***namelist,
				int (*select)(struct dirent *),
				int (*compare)(struct dirent **, struct dirent **) )
#endif
	{
	char comment[100];
	long direntry=0,nameentry=0;

#if OSD_OSTYPE==OSD_UNIX

	struct dirent **dirlist;

	long direntries;


	direntries=scandir(dirname,&dirlist,NULL,alphasort);
	if(direntries<0)
		{
		sprintf(comment,"scandir() failed on \"%s\"\n",dirname);
		osdPrint(OSD_SOFTWARE,OSD_WARN,comment);
		return -1;
		}

	/* allocate number in dir list (max to be converted to name list); note extra entry for ending NULL */
	*namelist=(char **)osdMalloc( (direntries+1) * sizeof(char *));

	/* convert entries to just names */
	for(direntry=0;direntry<direntries;direntry++)
		{
		char *name=dirlist[direntry]->d_name;

		if( strcmp(name,".") && strcmp(name,"..") )
			{
			(*namelist)[nameentry]=(char *)osdMalloc(strlen(name)+1);

			strcpy((*namelist)[nameentry],name);

			nameentry++;
			}

		osdFree(dirlist[direntry]);
		}

	osdFree(dirlist);

	/* end with a NULL entry */
	(*namelist)[nameentry]=NULL;

	return nameentry;

#elif OSD_OSTYPE==OSD_WIN32

	char pattern[200];

	HANDLE filehandle;
	WIN32_FIND_DATA finddata;

	long done=FALSE;

	*namelist=NULL;

	if(dirname[0]=='/')
		strcpy(pattern,OSD_FILE_DEFAULT_DRIVE);
	else
		pattern[0]=0;

	strcat(pattern,dirname);

	if(dirname[strlen(dirname)-2]=='/')
		strcat(pattern,"*");
	else
		strcat(pattern,"/*");

#if OSD_SCANDIR_DEBUG
	sprintf(comment,"matching \"%s\"",pattern);
	osdPrint(OSD_SOFTWARE,OSD_LOG,comment);
#endif

	filehandle=FindFirstFile(pattern,&finddata);

	if(filehandle==INVALID_HANDLE_VALUE)
		{
		sprintf(comment,"FindFirstFile() couldn't match \"%s\"",pattern);
		osdPrint(OSD_SOFTWARE,OSD_ERROR,comment);
		return -1;
		}

	while(!done)
		{
		char *name=finddata.cFileName;

#if OSD_SCANDIR_DEBUG
		sprintf(comment,"found \"%s\"",name);
		osdPrint(OSD_SOFTWARE,OSD_LOG,comment);
#endif

		if( strcmp(name,".") && strcmp(name,"..") )
			{
			*namelist=(char **)osdRealloc(*namelist,(nameentry+1) * sizeof(char *));
			(*namelist)[nameentry]=(char *)osdMalloc(strlen(name)+1);

			strcpy((*namelist)[nameentry],name);
			nameentry++;
			}

		direntry++;

		if( FindNextFile(filehandle,&finddata) != TRUE)
			{
			done=TRUE;

			if(FindClose(filehandle) != TRUE)
				{
				osdPrint(OSD_SOFTWARE,OSD_ERROR,"FindClose() couldn't close handle on file pattern match");
				return -1;
				}

			if(GetLastError()!=ERROR_NO_MORE_FILES)
				{
				osdPrint(OSD_SOFTWARE,OSD_ERROR,"FindNextFile() can't");
				return -1;
				}
			}
		}

#if OSD_SCANDIR_DEBUG
	sprintf(comment,"adding NULL entry");
	osdPrint(OSD_SOFTWARE,OSD_LOG,comment);
#endif

	/* end with a NULL entry */
	*namelist=(char **)osdRealloc(*namelist,direntry * sizeof(char *));
	(*namelist)[nameentry]=NULL;

#if OSD_SCANDIR_DEBUG
	sprintf(comment,"done scanning \"%s\"",pattern);
	osdPrint(OSD_SOFTWARE,OSD_LOG,comment);
#endif

	return nameentry;

#else
	#error OSD_OSTYPE not properly defined
#endif
	}



/******************************************************************************
void	osdDirectoryListFree(char ***namelist)

	frees the buffers allocated by osdScanDirectory()

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
void osdDirectoryListFree(namelist)
	char ***namelist;
#else
void osdDirectoryListFree(char ***namelist)
#endif
	{
	long name=0;

	while((*namelist)[name])
		osdFree( (*namelist)[name++] );

	osdFree(*namelist);
	}



/******************************************************************************
long osdHierarchyNameDissect(char *string,char *a_user,char *a_host,
                    char *a_type,char *a_space,char *a_path,char *a_basename)

    takes a 'string' of format:

            [user@][host:][[(type)]space][[/path]...]/basename

    such as jason@aahz.stt3.com:s/tmp/readme.doc
    or      aj@titan.stt3.com:(harness)aj/root
    and extracts the components into the appropriate arguments

    'space' of default type "file" refers to a drive device name (letter)
        and is not relevant under UNIX
    'path' includes the full compound path not including any basename

    the basename is the last component in the full name
    this function doesn't determine if it's a directory or file

    buffer space is assumed to already be allocated to large enough size

    all pathnames are absolute, never relative
    a trailing '/' will cause an empty basename

    NOTE: using a '@', ':', '(', ')', or space in the name except as stated
    above is bound to cause problems.

    Any arguments except 'string' may be NULL, in which case they are not set

    returns zero if successful

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdHierarchyNameDissect(string,a_user,a_host,a_type,a_space,a_path,a_basename)
	char *string,*a_user,*a_host,*a_type,*a_space,*a_path,*a_basename;
#else
long osdHierarchyNameDissect(char *string,char *a_user,char *a_host,char *a_type,char *a_space,char *a_path,char *a_basename)
#endif
    {
    /* buffers for when an a_* argument pointer is NULL */
    char b_user[128];
    char b_host[128];
    char b_type[128];
    char b_space[128];
    char b_path[1024];
    char b_basename[128];

    char *user=     (a_user)?       a_user:     b_user;
    char *host=     (a_host)?       a_host:     b_host;
    char *type=     (a_type)?       a_type:     b_type;
    char *space=    (a_space)?      a_space:    b_space;
    char *path=     (a_path)?       a_path:     b_path;
    char *basename= (a_basename)?   a_basename: b_basename;

    char part[128];

    char *s=string;

    long v,len;

    type[0]=0;
    space[0]=0;
    basename[0]=0;
    strcpy(path,"/");

    if( (v=sscanf(s,"%[^@]",user)) && s[len=strlen(user)]=='@')
        s+=len+1;
    else
        user[0]=0;

#if OSD_DISSECT_DEBUG
    printf("user %d %d %s + %s (%d)\n",v,len,user,s,*s);
#endif

    if( (v=sscanf(s,"%[^:]",host)) && s[len=strlen(host)]==':' )
        s+=len+1;
    else
        host[0]=0;

#if OSD_DISSECT_DEBUG
    printf("host %d %d %s + %s (%d)\n",v,len,host,s,*s);
#endif

    if( (v=sscanf(s,"%[^/]",part)) && s[len=strlen(part)]=='/' )
        {
        s+=len+1;

        if(part[0]=='(' && (v=sscanf(&part[1],"%[^)]",type)) && part[1+(len=strlen(type))]==')' )
            strcpy(space,&part[2+len]);
        else
            {
            strcpy(space,part);
            type[0]=0;
            }
        }
    else
        {
        if(*s=='/')
            s++;
        }
#if OSD_DISSECT_DEBUG
    printf("(type)space %d %d (%s)%s + %s (%d)\n",v,len,type,space,s,*s);
#endif

    while( (v=sscanf(s,"%[^/]",part))>0 && s[len=strlen(part)]=='/' )
        {
        strcat(path,part);
        strcat(path,"/");

        s+=len+1;

#if OSD_DISSECT_DEBUG
        printf("part %d %d %s + %s (%d)\n",v,len,part,s,*s);
#endif
        }

#if OSD_DISSECT_DEBUG
    printf("last %d %d %s + %s (%d)\n",v,len,part,s,*s);
#endif

    strcpy(basename,s);

#if OSD_DISSECT_DEBUG
    printf("\"%s\" dissected to:\n user:        \"%s\"\n host:      \"%s\"\n type:      \"%s\"\n space:     \"%s\"\n path:      \"%s\"\n basename:  \"%s\"\n",
            string,user,host,type,space,path,basename);
#endif

    return 0;
    }

/******************************************************************************

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdDirectoryCreate(path)
	char *path;
#else
long osdDirectoryCreate(char *path)
#endif
{
#if OSD_OSTYPE==OSD_UNIX
	if(mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
	{
		osdPrint(OSD_SYSTEM,OSD_WARN,"could not make directory");
		return FALSE;
	}
	else
	{
		return TRUE;
	}
#elif OSD_OSTYPE==OSD_WIN32
	return CreateDirectory(path,NULL);
#else
	#error OSD_OSTYPE not properly defined
#endif

}


