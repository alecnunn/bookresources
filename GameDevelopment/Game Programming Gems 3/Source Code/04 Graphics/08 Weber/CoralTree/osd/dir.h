
/******************************************************************************

Coral Tree osd/dir.h
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



#ifdef __cplusplus
    extern "C"
    {
#endif


#if OSD_COMPILER==OSD_KR

long osdDirectoryScan();
void osdDirectoryListFree();
long osdHierarchyNameDissect();

long osdDirectoryCreate();

#else

long osdDirectoryScan(char *dirname,char ***namelist,
						int (*select)(struct dirent *),
						int (*compare)(struct dirent **, struct dirent **) );
void osdDirectoryListFree(char ***namelist);
long osdHierarchyNameDissect(char *string,char *a_user,char *a_host,char *a_type,char *a_space,char *a_path,char *a_basename);

long osdDirectoryCreate(char *path);
#endif                                                                                                          


#ifdef __cplusplus
    }
#endif
