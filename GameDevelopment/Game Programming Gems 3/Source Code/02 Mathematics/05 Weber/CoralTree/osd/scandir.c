
/******************************************************************************

Coral Tree osd/scandir.c
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



#include"osd/osd.h"


#define DIR_DEBUG	FALSE
#define SCAN_DEBUG	FALSE

#define MAXENTRIES	1024



#ifdef OSD_REPLACE_SCANDIR

/******************************************************************************
int		scandir(const char *dirname,struct dirent **namelist[],
							int (*select)(struct dirent *),
							int (*compar)(struct dirent **,struct dirent **))

	This should follow System V spec's.

******************************************************************************/
/*PROTOTYPE*/
int scandir(const char *dirname,struct dirent **namelist[],
							int (*select)(struct dirent *),
							int (*compar)(struct dirent **,struct dirent **))
	{
	DIR *directory;

	struct dirent *entry;

	int entries=0;

	*namelist=(struct dirent **)malloc(MAXENTRIES*sizeof(struct dirent *));
	if(*namelist==NULL)
		{
		if(DIR_DEBUG)
			printf("Error allocating *namelist\n");

		return -1;
		}

	directory=opendir(dirname);
	if(directory==NULL)
		{
		if(DIR_DEBUG)
			printf("Error opening \"%s\"\n",dirname);

		free(*namelist);
		return -1;
		}

	/* read all entries into namelist */
	do
		{
		entry=readdir(directory);
		if(entry)
			{
			(*namelist)[entries]=(struct dirent *)malloc(entry->d_reclen);
			if((*namelist)[entries]==NULL)
				{
				if(DIR_DEBUG)
					printf("Error allocating (*namelist)[%d]\n",entries);

				/* free previous entries */
				entries--;
				while(entries>=0)
					free((*namelist)[entries--]);

				free(*namelist);
				return -1;
				}

			memcpy((*namelist)[entries],entry,entry->d_reclen);

			if(DIR_DEBUG)
				printf("Read \"%s\" \"%s\" %d\n",entry->d_name,(*namelist)[entries]->d_name, sizeof(struct dirent));

			entries++;
			}

		} while(entry!=NULL);

	closedir(directory);

	if(DIR_DEBUG)
		printf("qsort()\n");

	if(compar)
		qsort(*namelist,entries,sizeof(struct dirent *),(int (*)(const void *, const void *))compar);

	if(DIR_DEBUG)
		printf("done\n");

	return entries;
	}



/******************************************************************************
int		alphasort(struct dirent **d1,struct dirent **d2)

	This should follow System V spec's.

	SAS/C didn't bother to include it.

******************************************************************************/
/*PROTOTYPE*/
int alphasort(struct dirent **d1,struct dirent **d2)
{
#if SCAN_DEBUG

	printf("\"%s\" \"%s\"\n",(*d1)->d_name,(*d2)->d_name);

#endif

	return (int)strcmp((*d1)->d_name,(*d2)->d_name);
}

#endif
