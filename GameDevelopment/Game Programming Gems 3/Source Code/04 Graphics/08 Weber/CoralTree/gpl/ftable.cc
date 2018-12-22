
/******************************************************************************

Coral Tree gpl/ftable.cc
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



#include "gpl.h"


#define GPL_TABLE_DEBUG				FALSE
#define GPL_TABLE_INTERPRET_DEBUG	FALSE

#define GPL_TABLE_MAXLINE		1024
#define GPL_TABLE_MAXHEADERS	1024
#define GPL_TABLE_MAXINCLUDE	32

#define GPL_TABLE_INCLUDE		"#include"
#define GPL_TABLE_DEFINE		"#define"
#define GPL_TABLE_FOR			"for"
#define GPL_TABLE_SET			"set"
#define GPL_TABLE_HEADER		"header"
#define GPL_TABLE_ROW			"row"
#define GPL_TABLE_END			"end"


/******************************************************************************
		GPL_Tables::GPL_Tables(void)

******************************************************************************/
GPL_Tables::GPL_Tables(void)
	{
	filename[0]=0;

	SetNumberRows(0);
	SetRowLength(0);
	SetCurrentTable(NULL);
	}



/******************************************************************************
long	GPL_Tables::LoadFromFile(char *filename,char *includepath)

******************************************************************************/
long GPL_Tables::LoadFromFile(char *filename,char *includepath)
	{
	return Interpret(filename,NULL,includepath);
	}



/******************************************************************************
long	GPL_Tables::ReadFromMemory(char *data)

	Never Implemented: does not work

******************************************************************************/
long GPL_Tables::ReadFromMemory(char *data)
	{
	return Interpret(NULL,data,NULL);
	}



/******************************************************************************
long	GPL_Tables::Interpret(char *filename,char *data,char *includepath)

	read from string in data NOT IMPLEMENETED YET

******************************************************************************/
long GPL_Tables::Interpret(char *filename,char *data,char *includepath)
	{
	char message[256];

	GPL_File filestack[GPL_TABLE_MAXINCLUDE];
	long files;

	GPL_TableEntry *label=NULL;
	GPL_TableEntry *entry=NULL;
	long headermap[GPL_TABLE_MAXHEADERS];
	long entries=0;

	GPL_TableEntry rowindex;
	GPL_TableEntry columnindex;
	GPL_TableEntry newentry;
	GPL_TableEntry *replaceentry;

	char line[GPL_TABLE_MAXLINE];
	char arg[GPL_TABLE_MAXLINE];
	long byte,bytes=0;
	long index;
	long rownumber;

	files=1;

	if( !filestack[0].Open(filename,OSD_FILE_READ) )
		{
		gplLog("GPL WARNING", "GPL_Tables::LoadFromFile() cannot open file");
		return TRUE;
		}

	while(!filestack[0].EndOfFile())
		{
		if(filestack[files-1].EndOfFile())
			{
			filestack[files-1].Close();
			files--;
			}

		byte=0;
		bytes=filestack[files-1].ReadLine(line,GPL_TABLE_MAXLINE);

#if GPL_TABLE_DEBUG
		printf("line[%d]: \"%s\"\n",bytes,line);
#endif

		if(bytes<0)
			continue;

		byte+=ReadArgument(&line[byte],arg);
		if(!strcmp(arg,GPL_TABLE_DEFINE))
			{
			byte+=ReadArgument(&line[byte],arg);

			if(strlen(arg)>0)
				{
				index=MatchEntry(entries,label,arg);

				if(index<0)
					{
					entries++;
					label=(GPL_TableEntry *)osdRealloc(label,entries*sizeof(GPL_TableEntry));
					entry=(GPL_TableEntry *)osdRealloc(entry,entries*sizeof(GPL_TableEntry));

					index=entries-1;

					label[index].Reset();
					entry[index].Reset();

					label[index].SetCharStr(arg);
					}

				InterpretArgument(entries-1,label,entry,&line[byte],&entry[index]);
				}
			}
		else if(!strcmp(arg,GPL_TABLE_FOR))
			{
			// table type
			byte+=ReadArgument(&line[byte],arg);

			// table name
			byte+=ReadArgument(&line[byte],arg);

			AddOrSetTable(arg);
			}
		else if(!strcmp(arg,GPL_TABLE_END))
			{
			SetCurrentTable(NULL);
			}
		else if(!strcmp(arg,GPL_TABLE_SET))
			{
			long firstrow= -1;
			long lastrow= -1;

			// row number(s)
			byte+=ReadArgument(&line[byte],arg);
			InterpretArgument(entries,label,entry,arg,&rowindex);

			if( !strcmp(arg,"*") )
				{
				firstrow=0;
				lastrow=GetNumberRows();
				}
			else if(rowindex.GetType()==GPL_LONG)
				{
				firstrow=rowindex.GetLong();
				lastrow=firstrow+1;
				}

			if(firstrow>=0)
				{
				// column number
				byte+=ReadArgument(&line[byte],arg);
				InterpretArgument(entries,label,entry,arg,&columnindex);

				if(columnindex.GetType()==GPL_LONG)
					{
					// setting
					byte+=ReadArgument(&line[byte],arg);
					InterpretArgument(entries,label,entry,arg,&newentry);

					if( GetCurrentTable() )
						{
						for(rownumber=firstrow;(rownumber<lastrow && rownumber>=0);rownumber++)
							{
							if(GetCurrentTable()->ExpandTable(rownumber+1,columnindex.GetLong()+1))
								{
#if GPL_TABLE_DEBUG
								printf("resized to %d rows of %d\n",rownumber+1,columnindex.GetLong()+1);
#endif
								}
							replaceentry=GetCurrentTable()->GetTableEntry(rownumber,columnindex.GetLong());

							if(replaceentry)
								replaceentry->Set(&newentry);
							}
						}
					}
				}
			}
		else if(!strcmp(arg,GPL_TABLE_ROW))
			{
			long firstrow= -1;
			long lastrow= -1;

			index=0;

			// row number(s)
			byte+=ReadArgument(&line[byte],arg);
			InterpretArgument(entries,label,entry,arg,&rowindex);

			if( !strcmp(arg,"*") )
				{
				firstrow=0;
				lastrow=GetNumberRows();
				}
			else if(rowindex.GetType()==GPL_LONG)
				{
				firstrow=rowindex.GetLong();
				lastrow=firstrow+1;
				}

			if(firstrow>=0)
				{
				while( index<GetRowLength() && byte<(long)strlen(line) )
					{
					// setting
					byte+=ReadArgument(&line[byte],arg);
					InterpretArgument(entries,label,entry,arg,&newentry);

					if( GetCurrentTable() )
						{
						for(rownumber=firstrow;(rownumber<lastrow && rownumber>=0);rownumber++)
							{
							if(GetCurrentTable()->ExpandTable(rownumber+1,headermap[index]+1))
								{
#if GPL_TABLE_DEBUG
								printf("resized to %d rows of %d\n",rownumber+1,columnindex.GetLong()+1);
#endif
								}
							replaceentry=GetCurrentTable()->GetTableEntry(rownumber,headermap[index]);

							if(replaceentry)
								replaceentry->Set(&newentry);
							}
						}

					index++;
					}
				}
			}
		else if(!strcmp(arg,GPL_TABLE_HEADER))
			{
			long mapping;
			index=0;

			while( byte<(long)strlen(line) )
				{
				byte+=ReadArgument(&line[byte],arg);
				InterpretArgument(entries,label,entry,arg,&newentry);

				if(newentry.GetType()==GPL_LONG)
					{
					mapping=newentry.GetLong();

					if(mapping>=0)
						headermap[index]=mapping;
					else
						headermap[index]= -1;
					}
				else
					headermap[index]= -1;

				index++;
				}

#if GPL_TABLE_DEBUG
			long m;
			printf("headermap:");
			for(m=0;m<index;m++)
				printf("%d ",headermap[m]);
			printf("\n");
#endif

			while( index<GPL_TABLE_MAXHEADERS )
				headermap[index++]= -1;
			}
		else if(!strncmp(arg,GPL_TABLE_INCLUDE,strlen(GPL_TABLE_INCLUDE)))
			{
			OSD_FileAttributes attributes;

			char *argp=&line[byte-strlen(arg)+strlen(GPL_TABLE_INCLUDE)];

			char newfilename[256];
			char fullname[256];
			char dirname[256];
			char pattern[256];
			char piece[256];
			char cwd[256];

			sprintf(pattern,"%c%%[^%c]",argp[0],argp[0]);
			sscanf(argp,pattern,newfilename);

			if(!osdCWDGet(cwd,128))
				cwd[0]=0;

			long count=0;
			int inc;
			while(count<(long)(strlen(includepath)))
				{
				piece[0]=0;
				inc=0;
				sscanf(&includepath[count],"%[^:]%n",piece,&inc);

//				printf("piece= (%s) of (%s) %d+%d/%d\n",piece,look_path,count,inc,strlen(look_path));

				if( !strncmp(piece,"../",3) || !strcmp(piece,"..") )
					sprintf(dirname,"%s/..%s",cwd,&piece[2]);
				else if( !strncmp(piece,"./",2) || !strcmp(piece,".") )
					sprintf(dirname,"%s%s",cwd,&piece[1]);
				else
					strcpy(dirname,piece);

				sprintf(fullname,"%s/%s",dirname,newfilename);

#if GPL_TABLE_DEBUG
				printf("trying to include %d (%s)\n",files,fullname);
#endif

				if(!osdFileGetAttributes(fullname,&attributes))
					{
					if( !filestack[files].Open(fullname,OSD_FILE_READ) )
						{
						sprintf(message,"GPL_Tables::LoadFromFile() cannot open include file (%s)\n",fullname);
						gplLog("GPL WARNING", message);
						return TRUE;
						}
					else
						{
#if GPL_TABLE_DEBUG
						printf("including %d (%s)\n",files,fullname);
#endif

						files++;
						break;
						}
					}

				count+=inc+1;
				}
			}
		}

	filestack[0].Close();

#if GPL_TABLE_DEBUG
	long m,n;
	for(m=0;m<entries;m++)
		{
		switch(entry[m].GetType())
			{
			case GPL_NULLTYPE:
				printf("%3d define NULL:    %s\n",m,label[m].GetCharStr());
				break;
			case GPL_FLOAT:
				printf("%3d define float:   %s %.6G\n",m,label[m].GetCharStr(),entry[m].GetFloat());
				break;
			case GPL_LONG:
				printf("%3d define long:    %s %d 0x%x\n",m,label[m].GetCharStr(),entry[m].GetLong(),entry[m].GetLong());
				break;
			case GPL_CHARSTR:
				printf("%3d define charstr: %s %s\n",m,label[m].GetCharStr(),entry[m].GetCharStr());
				break;
			}
		}

	GPL_Table *table;
	GPL_TableEntry *tableentry;
	char buffer[256];
	tablelist.ToHead();
	while( ( table=tablelist.PostIncrement() ) != NULL)
		{
		printf("Table: %s rows=%d columns=%d\n",table->GetName(),table->GetNumberRows(),table->GetRowLength());
		for(n=0;n<table->GetNumberRows();n++)
			{
			for(m=0;m<table->GetRowLength();m++)
				{
				tableentry=table->GetTableEntry(n,m);
				if(tableentry)
					tableentry->ConvertToCharStr(buffer);
				else
					strcpy(buffer,"ERROR");

				printf("%s ",buffer);
				}

			printf("\n");
			}
		}
#endif

	osdFree(label);
	osdFree(entry);

	return FALSE;
	}



/******************************************************************************
long	GPL_Tables::ReadArgument(char *line,char *argument)

******************************************************************************/
long GPL_Tables::ReadArgument(char *line,char *argument)
	{
	long quoted=FALSE;
	long lineindex=0;
	long argindex=0;

	while(line[lineindex] && (quoted || line[lineindex]!=' ' && line[lineindex]!='\t') )
		{
		argument[argindex]=line[lineindex];
		argindex++;
		lineindex++;

		if(line[lineindex-1]=='"')
			{
			if(quoted)
				break;

			quoted=TRUE;
			}
		}

	argument[argindex]=0;

	while(line[lineindex]==' ' || line[lineindex]=='\t')
		lineindex++;

	return lineindex;
	}



/******************************************************************************
long	GPL_Tables::MatchEntry(long entries,GPL_TableEntry *entry,char *name)

	returns index of matching name in entry
	returns -1 if none found

******************************************************************************/
long GPL_Tables::MatchEntry(long entries,GPL_TableEntry *entry,char *name)
	{
	long m=0;

	while(m<entries)
		{
		if( !strcmp(entry[m].GetCharStr(),name) )
			return m;
		m++;
		}

	return -1;
	}



/******************************************************************************
void	GPL_Tables::InterpretArgument(long entries,GPL_TableEntry *label,
				GPL_TableEntry *entry,char *argument,GPL_TableEntry *result)

******************************************************************************/
void GPL_Tables::InterpretArgument(long entries,GPL_TableEntry *label,GPL_TableEntry *entry,char *argument,GPL_TableEntry *result)
	{
	char matchname[1024];

	long byte=0;
	long matchbyte=0;
	long read=FALSE;
	long index;

#if GPL_TABLE_INTERPRET_DEBUG
	printf("Interpret (%s)\n",argument);
#endif

	if(argument[0]=='"')
		{
		strcpy(matchname,&argument[1]);
		matchname[strlen(matchname)-1]=0;
		result->SetCharStr(matchname);
		return;
		}

	result->SetType(GPL_NULLTYPE);

	while(argument[byte])
		{
		if( (argument[byte]>='A' && argument[byte]<='Z') ||
			(argument[byte]>='a' && argument[byte]<='z') ||
			(argument[byte]>='0' && argument[byte]<='9') ||
			argument[byte]=='.' || argument[byte]=='-' || argument[byte]=='_')
				{
				while(	(argument[byte]>='A' && argument[byte]<='Z') ||
						(argument[byte]>='a' && argument[byte]<='z') ||
						(argument[byte]>='0' && argument[byte]<='9') ||
						argument[byte]=='.' || argument[byte]=='-' || argument[byte]=='_')
					{
					matchname[matchbyte++]=argument[byte++];
					};

				matchname[matchbyte++]=0;

				if( (argument[byte]>='0' && argument[byte]<='9') || argument[byte]=='-' )
					index= -1;
				else
					index=MatchEntry(entries,label,matchname);

//	printf("\"%s\" matches %d\n",matchname,index);

				if(index<0)
					{
					if(	(matchname[0]>='0' && matchname[0]<='9') || matchname[0]=='.' || matchname[0]=='-')
						{
						float floatvalue=(float)atof(matchname);
						long longvalue=strtol(matchname,(char **)NULL,0);	// 0 = base auto-detect

						// Linux red hat 6 is messed up with atof
						if(matchname[1] == 'x' || matchname[1] == 'X')
							{
							floatvalue = 0.0;
							}

//	printf("string=(%s) float=%.6G long=%d\n",matchname,floatvalue,longvalue);

						// no fraction or is hex
						if( (float)longvalue == floatvalue || (longvalue>0 && floatvalue==0.0) )
							{
							if(result->GetType()==GPL_NULLTYPE)
								result->SetLong(longvalue);
							else if(result->GetType()==GPL_LONG)
								result->SetLong( longvalue | result->GetLong() );
							}
						else
							result->SetFloat(floatvalue);
						}
					else
						result->SetCharStr(matchname);
					}
				else if(entry[index].GetType()==GPL_FLOAT)
					{
					if(result->GetType()==GPL_NULLTYPE)
						result->SetFloat(entry[index].GetFloat());
					}
				else if(entry[index].GetType()==GPL_CHARSTR)
					{
					if(result->GetType()==GPL_NULLTYPE)
						result->SetCharStr(entry[index].GetCharStr());
					}
				else if(entry[index].GetType()==GPL_LONG)
					{
					if(result->GetType()==GPL_NULLTYPE)
						result->SetLong(entry[index].GetLong());
					else if(result->GetType()==GPL_LONG)
						result->SetLong( entry[index].GetLong() | result->GetLong() );
					}

				read=TRUE;
				matchbyte=0;
				}
		else if(argument[byte]>='|' || result->GetType()==GPL_LONG)
			{
			read=FALSE;
			byte++;
			}
		else
			byte++;
		}

#if GPL_TABLE_INTERPRET_DEBUG
	char text[128];
	result->ConvertToCharStrFormatted(text);
	printf("Result (%s)\n",text);
#endif
	}



/******************************************************************************
GPL_Table	*GPL_Tables::AddOrSetTable(char *tablename)

	Set if it already exists
	Add new one if it doesn't (and Set, as well)

******************************************************************************/
GPL_Table *GPL_Tables::AddOrSetTable(char *tablename)
	{
	GPL_Table *table;

	tablelist.ToHead();
	while( ( table=tablelist.PostIncrement() ) != NULL)
		{
		if( !strcmp(table->GetName(),tablename) )
			{
			// found -> Set
			SetCurrentTable(table);
			return table;
			}
		}

	// not found -> Add new

	table= new GPL_Table;
	table->SetDefault(GetDefault());
	table->SetNumberRows(GetNumberRows());
	table->SetRowLength(GetRowLength());
	table->SetName(tablename);

	tablelist.AppendNode(table);

	SetCurrentTable(table);
	return table;
	}



/******************************************************************************
void	GPL_Table::ResizeRow(long index,long length)

******************************************************************************/
void GPL_Table::ResizeRow(long index,long length)
	{
	if(length<1)
		{
		if(row[index])
			osdFree(row[index]);

		row[index]=(GPL_TableEntry *)NULL;
		}
	else
		{
		row[index]=(GPL_TableEntry *)osdRealloc(row[index],length*sizeof(GPL_TableEntry));

		long m;
		for(m=rowlength;m<length;m++)
			{
			row[index][m].Reset();
			row[index][m].Set(GetDefault());
			}
		}
	}



/******************************************************************************
void	GPL_Table::SetNumberRows(long set)

******************************************************************************/
void GPL_Table::SetNumberRows(long set)
	{
	long m;
	for(m=set;m<rows;m++)
		if(row[m])
			osdFree(row[m]);

	if(set<1)
		{
		if(row)
			osdFree(row);
		row=(GPL_TableEntry **)NULL;
		}
	else
		row=(GPL_TableEntry **)osdRealloc(row,set*sizeof(GPL_TableEntry *));

	for(m=rows;m<set;m++)
		{
		row[m]=(GPL_TableEntry *)NULL;
		ResizeRow(m,GetRowLength());

		long n;
		for(n=0;n<rowlength;n++)

			row[m][n].Set(GetDefault());
		}

	rows=set;
	}
