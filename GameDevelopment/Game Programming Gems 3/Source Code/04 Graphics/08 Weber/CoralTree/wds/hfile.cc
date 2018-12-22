
/******************************************************************************

Coral Tree wds/hfile.cc
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



#include "wds.h"


#define HIER_DIR_DEBUG		FALSE
#define HIER_ALLOC_GRAIN	1024


/******************************************************************************
	WDS_HierarchyFile::WDS_HierarchyFile(void)

******************************************************************************/
WDS_HierarchyFile::WDS_HierarchyFile(void)
	{
	SetName("HierarchyFile");
	SetFileHandler(&WDS_HierarchyFile::DefaultFileHandler);

//	GetHierForm()->SetSelectability(WDS_SELECT_SINGLE|WDS_SELECT_EXCLUSIVE);
	}


/******************************************************************************
void	WDS_HierarchyFile::ShowEntry(void)

	build the child widgets

	in this context, 'file' refers generically to directories and links as well

Keywords:
******************************************************************************/
void WDS_HierarchyFile::ShowEntry(void)
	{
	char message[256];
	char request[1024];		// farol server request
	char *response=NULL;	// farol server response
	char *entry;			// response fragment
	char parentname[1024];	// full parent name
	char att[32];			// child file attributes
	char size[32];			// child file size
	char longname[256];		// child file full name
	char shortname[256];	// child file base name

	int inc,entries;
	long result;			// farol server return value
	long is_dir,is_link;	// flags
	long margin;			// border between old and new/reused entries
	long n;

	if(file_handler==NULL)
		{
		EW_PRINT(EW_WIDGET,EW_WARN,"WDS_Widget::ShowEntry() file_handler is NULL");
		return;
		}

	GetFullPathName(parentname);

	sprintf(request,"ll %s",parentname);

//	result=farolSingleAction("localhost",request,&response);

	result=(*file_handler)("localhost",request,&response);
	if(result<0 || !response[0] || response[0]=='0' || atol(response)<0)
		{
		printf("Error scanning %s\n",parentname);
		return;
		}

	entries=0;
	entry=response;

	// less 1 for header entry
	margin=parent_form.GetWidgetList()->GetNumberNodes()-1;

	while(entry<response+result && entry[0])
		{
		inc=0;
		sscanf(entry,"%s %s %[^\n]\n%n",att,size,shortname,&inc);

		if(inc<5 || entry>response+result )
			{
			if(entries)
				printf("Error parsing data from %s\n",parentname);
			else
				printf("Empty directory %s\n",parentname);

			return;
			}

		if(HIER_DIR_DEBUG)
			{
//			printf("entry `%s' ",entry);
			printf("-> `%s' `%s' `%s'\n",att,size,shortname);
			}

		entry+=inc;

		// try to find a matching old entry

		WDS_HierarchyFile *child=(WDS_HierarchyFile *)MatchChild(margin,shortname);

		// if no match was found, make a new widget
		if(!child)
			{
			if(HIER_DIR_DEBUG)
				printf("new entry color %d ",(int)(InterpretElement(WDS_STATE_ALL,EW_COLOR_PEN)));

			child=new WDS_HierarchyFile;

			// have child use same file handler as this
			child->SetFileHandler(file_handler);

			parent_form.AddWidget(child);
			}
		else
			{
			if(HIER_DIR_DEBUG)
				printf("re-use entry ");

			margin--;
			}

		if(HIER_DIR_DEBUG)
			printf("`%s'",shortname);

		child->hier_string.NewBuffer(shortname);

		n=parentname[strlen(parentname)-1];
		sprintf(longname,"%s%s%s",parentname, (n==':' || n=='/')? "":"/", shortname);
		child->fullname.NewBuffer(longname);

		is_dir=  (att[0]=='d');
		is_link= (att[1]=='l');

		child->attributes=((att[2]=='r')<<2) + ((att[3]=='w')<<1) + (att[4]=='x');
		child->filesize=atol(size);
		child->filetype=WDS_HIER_FILE_NULL;

		if(is_dir)
			child->filetype|=WDS_HIER_FILE_DIR;

		if(is_link)
			child->filetype|=WDS_HIER_FILE_LINK;

		child->SetShowable(is_dir);

		// set label & names
		sprintf(message,"%s %d %s%s",att,(int)(child->filesize),child->hier_string.Buffer(),is_link? " ->":"");
		child->SetLabel(message);

		if(HIER_DIR_DEBUG)
			printf("labeled `%s'\n",message);

		sprintf(message,"hier-node %s",child->hier_string.Buffer());
		child->SetName(message);
		sprintf(message,"hier-form %s",child->hier_string.Buffer());
		child->GetHierForm()->SetName(message);

		long pen_color;

		if(is_dir)
			switch(child->attributes)
				{
				case 7:
					pen_color=WDS_HIER_FILE_COLOR_DRWX;
					break;
				case 5:
					pen_color=WDS_HIER_FILE_COLOR_DR_X;
					break;
				case 0:
					pen_color=WDS_HIER_FILE_COLOR_D___;
					break;
				default:
					pen_color=WDS_HIER_FILE_COLOR_D_nn;
					break;
				}
		else
			switch(child->attributes)
				{
				case 7:
					pen_color=WDS_HIER_FILE_COLOR__RWX;
					break;
				case 5:
					pen_color=WDS_HIER_FILE_COLOR__R_X;
					break;
				case 6:
					pen_color=WDS_HIER_FILE_COLOR__RW_;
					break;
				case 4:
					pen_color=WDS_HIER_FILE_COLOR__R__;
					break;
				case 0:
					pen_color=WDS_HIER_FILE_COLOR_____;
					break;
				default:
					pen_color=WDS_HIER_FILE_COLOR___nn;
					break;
				}
		
		child->GetLocalLook()->SetElement(WDS_STATE_ALL,EW_COLOR_PEN,pen_color);

		entries++;
		}

	// remove/delete un-reused old children
	parent_form.GetWidgetList()->ToHead();			// reset list
	parent_form.GetWidgetList()->PostIncrement();	// skip past header entry

	parent_form.DeleteChildren(margin);

	free(response);
	}



/******************************************************************************
long	WDS_HierarchyFile::DefaultFileHandler(char *serverhost,
												char *request,char **response)

Keywords:
******************************************************************************/
long WDS_HierarchyFile::DefaultFileHandler(char *serverhost,char *request,char **response)
	{
	OSD_FileAttributes attributes;

	char buffer[256];
	char argument[128];
	char filename[256];
	char fullname[256];

	char drive[128];
	char path[128];
	char basename[128];

	if(*response)
		osdFree(*response);
	*response=NULL;

	request=ReadArgument(request,argument);

	if( !strcmp(argument,"ll") )
		{
		char **namelist;

		long names,name;

		if(!request[0])
			{
			EW_PRINT(EW_WIDGET,EW_ERROR,"must specify directory for ll command");
			return -2;
			}

		request=ReadArgument(request,argument);

		osdHierarchyNameDissect(argument,NULL,NULL,NULL,drive,path,basename);
		sprintf(fullname,"%s%s%s%s",drive,drive[0]?":":"",path,basename);

		if( (names=osdDirectoryScan(fullname, &namelist, NULL, NULL)) < 0 )
			return -3;

		long bytes=2,newbuffersize,buffersize=HIER_ALLOC_GRAIN;
		*response=(char *)osdMalloc(buffersize);

		strcpy(*response,"\n");
		for(name=0;name<names;name++)
			{
			if(fullname[strlen(fullname)-1]=='/')
				sprintf(filename,"%s%s",fullname,namelist[name]);
			else
				sprintf(filename,"%s/%s",fullname,namelist[name]);

			if( osdFileGetAttributes(filename,&attributes) )
				continue;

			sprintf(buffer,"%s %s\n",attributes.string,namelist[name]);

			strcat(*response,buffer);

			// expand buffer as neccessary
			bytes+=strlen(buffer);
			newbuffersize= (bytes/HIER_ALLOC_GRAIN+2)*HIER_ALLOC_GRAIN;
			if(buffersize!=newbuffersize)
				{
				buffersize=newbuffersize;
				*response=(char *)osdRealloc(*response,buffersize);
				}
			}

		osdDirectoryListFree(&namelist);
		}

	return strlen(*response)+1;
	}



/******************************************************************************
char	*WDS_HierarchyFile::ReadArgument(char *string,char *argument)

Keywords:
******************************************************************************/
char *WDS_HierarchyFile::ReadArgument(char *string,char *argument)
	{
	int inc;

	if(*string)
		sscanf(string,"%s%n",argument,&inc);

	return string+inc;
	}

