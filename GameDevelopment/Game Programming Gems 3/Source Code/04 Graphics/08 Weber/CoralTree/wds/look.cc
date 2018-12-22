/******************************************************************************

Coral Tree wds/look.cc
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



#define WDS_ELEMENT_LOOKUP_DEBUG	FALSE
#define WDS_ELEMENT_SEARCH_DEBUG	FALSE
#define WDS_LOOK_LOAD_DEBUG			FALSE
#define WDS_LOOK_CURRENT_DEBUG		FALSE
#define	WDS_LOOK_BOUNDCHECK			FALSE	// good for safety, but in critical-speed code



/******************************************************************************
		WDS_Looks::~WDS_Looks()

Keywords:
******************************************************************************/
WDS_Looks::~WDS_Looks()
	{
	WDS_LookEntry *node;
	WDS_Look *look;

	// clean up list left by RegisterLook()
	looklist.ToHead();
	while( (node=looklist.PostIncrement()) != NULL )
		{
		look=node->GetLook();

		if(look)
			delete look;
		}
	}



/******************************************************************************
void	WDS_Looks::SetElement(long state,long attribute,long set)

******************************************************************************/
void WDS_Look::SetElement(long state,long attribute,long set)
		{
#if WDS_LOOK_DEBUG
		char message[256];
#endif
		if(attribute>=0 && attribute<WDS_ELEMENT_NUMBER)
			{
			// translate old-style color mapping using negatives
#if FALSE
			if(attribute<EW_COLOR_NUMBER && set<0)
				set= (-set) | WDS_ELEMENT_MAP;
#endif

			if(state==WDS_STATE_ALL)
				{
				long m=0;
				for(m=0;m<WDS_STATES;m++)
					element[m][attribute]=set;
				}
			else if(state>=0 && state<WDS_STATES)
				element[state][attribute]=set;
#if WDS_LOOK_DEBUG
			else
				{
				sprintf(message,"SetElement(%d,%d,%d) bad state",(int)state,(int)attribute,(int)set);
				EW_PRINT(EW_WIDGET,EW_WARN,message);
				}
#endif
			}
#if WDS_LOOK_DEBUG
		else
			{
			sprintf(message,"SetElement(%d,%d,%d) bad attribute",(int)state,(int)attribute,(int)set);
			EW_PRINT(EW_WIDGET,EW_WARN,message);
			}
#endif
		}



/******************************************************************************
long	WDS_Look::GetElement(long state,long attribute)

******************************************************************************/
long WDS_Look::GetElement(long state,long attribute)
	{
#if WDS_LOOK_BOUNDCHECK
	if(attribute<0 || attribute>=WDS_ELEMENT_NUMBER)
		{
#if WDS_LOOK_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"GetElement(%d,%d) bad attribute",state,attribute);
#endif

		return WDS_ELEMENT_INHERIT;
		}

	if(state!=WDS_STATE_ALL && (state<0 || state>=WDS_STATES) )
		{
#if WDS_LOOK_DEBUG
		EW_PRINT(EW_WIDGET,EW_WARN,"GetElement(%d,%d) bad state",state,attribute);
#endif
		return WDS_ELEMENT_INHERIT;
		}
#endif

	if(state==WDS_STATE_ALL)
		{
		long value=element[0][attribute];
		long next,m=0;

		for(m=1;m<WDS_STATES;m++)
			{
			next=element[m][attribute];

			// find max (WDS_ELEMENT_INHERIT overriden by anything)

			if(WDS_ELEMENT_IS_DEFAULT(value))
				value=next;
			else if( !WDS_ELEMENT_IS_DEFAULT(next) )
				value=EW_MAX(value,element[m][attribute]);
			}

		return value;
		}
	else
		return element[state][attribute];
	}



/******************************************************************************
void	WDS_Looks::RegisterLook(WDS_Typecode typecode,GPL_ClassId class_id,char *type_name,WDS_Look *look)

	replaces if one with same typecode found

Keywords:
******************************************************************************/
void WDS_Looks::RegisterLook(WDS_Typecode typecode,GPL_ClassId class_id,char *type_name,WDS_Look *look)
	{
	WDS_Look *listed_look;

#if WDS_ELEMENT_LOOKUP_DEBUG
	printf("RegisterLook(0x%x,\"%s\",0x%x)\n",typecode,type_name,look);
#endif

	if( !typecode || (listed_look=LookupLook(typecode,0)) == NULL )
		{
		WDS_LookEntry *look_entry= new WDS_LookEntry;

		listed_look= new WDS_Look;

		look_entry->SetTypeCode(typecode);
		look_entry->SetClassID(class_id);
		look_entry->SetTypeName(type_name);
		look_entry->SetLook(listed_look);

		looklist.AppendNode(look_entry);
		}

	osdMemcpy(listed_look,look,sizeof(WDS_Look));
	}



/******************************************************************************
WDS_Look	*WDS_Looks::LookupLook(WDS_Typecode typecode,GPL_ClassId class_id)

	returns NULL is specified typecode not found on list

Keywords:
******************************************************************************/
WDS_Look *WDS_Looks::LookupLook(WDS_Typecode typecode,GPL_ClassId class_id)
	{
	WDS_LookEntry *node;

#if WDS_ELEMENT_LOOKUP_DEBUG
	printf("LookupLook(0x%x,0x%x)\n",typecode,class_id);
#endif

	looklist.ToHead();
	while( (node=looklist.PostIncrement()) != NULL )
		{
#if WDS_ELEMENT_LOOKUP_DEBUG
		printf(" 0x%x vs 0x%x  0x%x vs 0x%x (%s)\n",
											node->GetTypeCode(),typecode,
											node->GetClassID(),class_id,
											node->GetTypeName());
#endif

		if( (typecode && node->GetTypeCode()==typecode) || (class_id && node->GetClassID()==class_id) )
			{
#if WDS_ELEMENT_LOOKUP_DEBUG
			printf(" found 0x%x\n",node->GetLook());
#endif

			return node->GetLook();
			}
		}

#if WDS_ELEMENT_LOOKUP_DEBUG
	printf(" not found\n");
#endif

	return NULL;
	}



/******************************************************************************
long	WDS_Looks::SearchForElement(WDS_Look *look,WDS_Typecode typecode,
											long value,long state,long element)

	scans through inheritance and defaults for element setting

Keywords:
******************************************************************************/
long WDS_Looks::SearchForElement(WDS_Look *look,WDS_Typecode typecode,long value,long state,long element)
	{
#if WDS_ELEMENT_SEARCH_DEBUG
	long debug= FALSE;

#if FALSE
	if(look=LookupLook(typecode,NULL))
		{
		long junk=look->GetElement(state,element);
		debug= (WDS_ELEMENT_IS_INHERIT(junk) && WDS_ELEMENT_IS_MAP(junk));
		}
#endif

	if(state== -1)
		{
		state=0;
		debug=TRUE;
		}

	if(debug)
		printf("SFE(0x%x,0x%x~%d,0x%x,%d): %d",(long)typecode,value,value&WDS_ELEMENT_MASK,state,element,value);
#endif

	if( typecode==WDS_TYPE_UNDEFINED || WDS_ELEMENT_IS_DEFAULT((long)typecode) )
		typecode=WDS_TYPE_DEFAULT;

	long inherit;

	while( typecode )
		{
		inherit=FALSE;

//		look=LookupLook(typecode);

		if(look)
			{
			if(WDS_ELEMENT_IS_DEFAULT(value))
				value=look->GetElement(state,element);
			else if(WDS_ELEMENT_IS_MAP(value))
				{
				element=(value&WDS_ELEMENT_MASK);
				value=look->GetElement(state,element);
				}
			}
			
#if WDS_ELEMENT_SEARCH_DEBUG
		else if(debug)
			printf(" => (no look 0x%x)",(long)typecode);
#endif

		if( typecode!=WDS_TYPE_DEFAULT && WDS_ELEMENT_IS_SPECIAL(value) )
			{
			if(look && WDS_ELEMENT_IS_INHERIT(value) )
				{
				typecode=(WDS_Typecode)look->GetElement(state,WDS_ELEMENT_INHERITANCE);
				inherit=TRUE;
				}
			else
				typecode=0;

			if(typecode==0 || WDS_ELEMENT_IS_DEFAULT((long)typecode) )
				typecode=WDS_TYPE_DEFAULT;
			}
		else
			{
			// done
			typecode=0;
			}
			
#if WDS_ELEMENT_SEARCH_DEBUG
		if(debug)
			{
			if(WDS_ELEMENT_IS_SPECIAL(value))
				printf(" => 0x%x (%d) [0x%x]",value,value&WDS_ELEMENT_MASK,(long)typecode);
			else
				printf(" => %d [0x%x]",value,(long)typecode);
			}
#endif

		if(typecode)
			look=GetAppropriateLook(look,state,typecode,inherit);
		};

#if WDS_ELEMENT_SEARCH_DEBUG
	if(debug)
		printf(" .\n");
#endif

	return value;
	}



/******************************************************************************
WDS_Look	*WDS_Looks::GetAppropriateLook(WDS_Look *look,long state,
											WDS_Typecode typecode,long inherit)

******************************************************************************/
WDS_Look *WDS_Looks::GetAppropriateLook(WDS_Look *look,long state,WDS_Typecode typecode,long inherit)
	{
	unsigned long counter=WDS_Widget::GetLookCounter();

	if(typecode==WDS_TYPE_DEFAULT)
		{
		unsigned long widgetcount=WDS_Widget::GetLookCountStatic();
		
		if(widgetcount==counter)
			return WDS_Widget::GetGlobalLookStoredStatic();

		inherit=FALSE;
		}

	if(!inherit || !look)
		{
#if WDS_LOOK_CURRENT_DEBUG
	printf("look 0x%x for typecode 0x%x inherit=%d\n",look,typecode,inherit);
#endif

		return LookupLook(typecode,0);
		}

	// in case of state==WDS_STATE_ALL
	if(state>=WDS_STATES)
		state=0;

	if(counter==look->GetCount(state))
		{
		// is current
		return look->GetReference(state);
		}

#if WDS_LOOK_CURRENT_DEBUG
	printf("look for typecode 0x%x not current\n",typecode);
#endif

	// not current
	WDS_Look *lookedup=LookupLook(typecode,0);

	look->SetCount(state,counter);
	look->SetReference(state,lookedup);

	return lookedup;
	}



/******************************************************************************
void	WDS_Looks::DumpHeader(void)

******************************************************************************/
void WDS_Looks::DumpHeader(void)
	{
	const long headers1=9;
	static char headerstring1[headers1][128]=
			{
			" 0=BLACK   1=MEDGREY    2=LIGHTGREY 3=WHITE    4=DARKGREY",
			" 5=PINK    6=LIGHTGREEN 7=BLUEGREEN 8=BLUE     9=GREEN",
			" 10=CYAN   11=YELLOW    12=RED      13=MAGENTA 14=OLIVE",
			" 15=PURPLE",
			" 1=seam 2=ridge 4=in 8=out 10=smooth 20=round",
			" 40=omit right 80=omit left 100=omit top 200=omit bottom",
			" 400=point 800=point vertical 1000=point back 2000=angle45",
			" 4000=label-left 8000=elliptical 10000=check 20000=plus 30000=minus",
			" .=inherited/default @=default m=mapped i=inherited/mapped c=copy ",
			};

	long m;
	for(m=0;m<headers1;m++)
		EW_PRINT(EW_WIDGET,EW_LOG,headerstring1[m]);
	}




/******************************************************************************
void	WDS_Looks::Dump(long code)

******************************************************************************/
void WDS_Looks::Dump(long code)
	{
	WDS_LookEntry *node;

	char message[256];

	sprintf(message,"WDS_Looks::Dump(0x%x) cache counter=%d",(unsigned int)code,(int)(WDS_Widget::GetLookCounter()));
	EW_PRINT(EW_WIDGET,EW_LOG,message);

	DumpHeader();

	looklist.ToHead();
	while( (node=looklist.PostIncrement()) != NULL )
		{
		node->GetLook()->Dump((long)(node->GetTypeCode()),node->GetTypeName(),NULL);
		}
	}



/******************************************************************************
void	WDS_Look::Dump(WDS_Typecode typecode,char *type_name,WDS_Widget *widget)

	if widget==NULL, dump this look

	if widget!=NULL, interpret the widget

******************************************************************************/
void WDS_Look::Dump(WDS_Typecode typecode,char *type_name,WDS_Widget *widget)
	{
	const long headers2=2;
	static char headerstring2[headers2][256]=
			{
"  0  1  2  3  4  5  6  7  8|     9 10 11 12 13 14 15 16 17 18 19 20 21 22  23 24 25 26         27         28 29 30 31 32 Cache",
" Un Pn Wd Bg Lt HL Sh HS Dt|Border Px Py Id Od Dp In dY Si dy Sd So Lx Ly  AM Ax Ay Inherit    Init       Union          Look       Cnt"
			};

	char message[256],buffer[256],format[16];

	long len;
	long m,n;

	sprintf(message,"Look %s0x%-8x%s typecode %s0x%-8x%s: (%s%s%s)",
				ewColorCode(3),(unsigned int)this,ewColorCode(0),
				ewColorCode(3),(unsigned int)typecode,ewColorCode(0),
				ewColorCode(3),type_name,ewColorCode(0));
	EW_PRINT(EW_WIDGET,EW_LOG,message);

	for(m=0;m<headers2;m++)
		EW_PRINT(EW_WIDGET,EW_LOG,headerstring2[m]);

	for(n=0;n<WDS_STATES;n++)
		{
		message[0]=0;

		for(m=0;m<WDS_ELEMENT_NUMBER+2;m++)
			{
			long value;

			if(m==WDS_ELEMENT_NUMBER)
				value=(long)GetReference(n);
			else if(m==WDS_ELEMENT_NUMBER+1)
				value=(long)GetCount(n);
			else if(widget)
				value=widget->InterpretElement(n,m);
			else
				value=GetElement(n,m);

#if FALSE
			sprintf(buffer," %-8x",value);
#else
			if(m==WDS_ELEMENT_BORDER)
				strcat(message,"|");

			if(m==WDS_ELEMENT_INHERITANCE || m==WDS_ELEMENT_CALLBACK_INIT || m==WDS_ELEMENT_NUMBER)
				{
				strcpy(format," 0x%-8x");
				len= -11;
				}
			else if(m==WDS_ELEMENT_BORDER)
				{
				strcpy(format,"x%-5.5x");
				len=6;
				}
			else if(m==WDS_ELEMENT_ANNOTATION_MODE)
				{
				strcpy(format," x%-2.2x");
				len=4;
				}
			else
				{
				strcpy(format,"%3d");
				len=3;
				}

			if(WDS_ELEMENT_IS_COPY(value))
				sprintf(buffer,"%2dc",(int)(value&WDS_ELEMENT_MASK));
			else if(WDS_ELEMENT_IS_MAP(value))
				{
				if(WDS_ELEMENT_IS_INHERIT(value))
					sprintf(buffer,"%2di",(int)(value&WDS_ELEMENT_MASK));
				else
					sprintf(buffer,"%2dm",(int)(value&WDS_ELEMENT_MASK));
				}
			else if(WDS_ELEMENT_IS_INHERIT(value) && WDS_ELEMENT_IS_DEFAULT(value))
				sprintf(buffer,"%*s",(int)len,".");
			else if(WDS_ELEMENT_IS_DEFAULT(value))
				sprintf(buffer,"%*s",(int)len,"@");
			else if(WDS_ELEMENT_IS_SPECIAL(value))
				sprintf(buffer,"%2d?",(int)(value&WDS_ELEMENT_MASK));
			else
				sprintf(buffer,format,value);
#endif

			strcat(message,buffer);
			}

		EW_PRINT(EW_WIDGET,EW_LOG,message);
		}


#if FALSE
	for(m=0;m<WDS_ELEMENT_NUMBER;m++)
		printf("%2d %d / %d\n",m,element[0][m],element[1][m]);
#endif
	}



/******************************************************************************
void	WDS_Looks::SaveAsC(char *filename,char *variablename)

******************************************************************************/
void WDS_Looks::SaveAsC(char *filename,char *variablename)
	{
	WDS_LookEntry *node;
	GPL_File file;
	char line[4096];
	long done=FALSE;
	long m,n;

	long number=looklist.GetNumberNodes();

	if(!file.Open(filename,OSD_FILE_WRITE|OSD_FILE_ERASE))
		return;

	sprintf(line,"static long %s[]={\n%d,%d,%d,\n\n",variablename,number,WDS_STATES,WDS_ELEMENT_NUMBER);
	file.Write(line,strlen(line));

	looklist.ToHead();
	while( (node=looklist.PostIncrement()) != NULL )
		{
		sprintf(line,"/* %s */\n0x%x,\n",node->GetTypeName(),node->GetClassID());
		file.Write(line,strlen(line));

		for(n=0;n<WDS_STATES;n++)
			{
			for(m=0;m<WDS_ELEMENT_NUMBER;m++)
				{
				sprintf(line,"0x%x,",node->GetLook()->GetElement(n,m));
				file.Write(line,strlen(line));
				}

			file.Write("\n",1);
			}

		file.Write("\n",1);
		}

	sprintf(line,"};\n");
	file.Write(line,strlen(line));

	file.Close();
	}



/******************************************************************************
long	WDS_Looks::ReadFromBuffer(long *buffer)

******************************************************************************/
long WDS_Looks::ReadFromBuffer(long *buffer)
	{
	long number=buffer[0];
	long states=buffer[1];
	long elements=buffer[2];

//	printf("number=%d states=%d elements=%d\n",number,states,elements);

	long n;
	for(n=0;n<number;n++)
		{
		long base=3+n*(states*elements+1);

		GPL_ClassId class_id=buffer[base];

		WDS_Look *look=LookupLook(0,class_id);

//		printf("n=%d class_id=%d look=0x%x\n",n,class_id,look);

		if(look)
			{
			long state,element;
			for(state=0;state<states && state<WDS_STATES;state++)
				{
				long base2=base+1+state*elements;

				for(element=0;element<elements && element<WDS_ELEMENT_NUMBER;element++)
					look->SetElement(state,element,buffer[base2+element]);

				// default on missing elements
				for(element=elements;element<WDS_ELEMENT_NUMBER;element++)
					look->SetElement(state,element,WDS_ELEMENT_DEFAULT_VALUE);
				}

			// default on missing states
			for(state=states;state<WDS_STATES;state++)
				{
				long base2=base+1+state*elements;

				for(element=0;element<WDS_ELEMENT_NUMBER;element++)
					look->SetElement(state,element,WDS_ELEMENT_DEFAULT_VALUE);
				}
			}
		}

	// invalidate old look pointers
	WDS_Widget::IncrementLookCounter();

	return FALSE;
	}



/******************************************************************************
void	WDS_Looks::ResetWindow(EW_Window *window)

Keywords:
******************************************************************************/
void WDS_Looks::ResetWindow(EW_Window *window)
	{
	WDS_Look *look=LookupLook(WDS_TYPE_DEFAULT,0);
	if(look)
		{
		long color=look->GetElement(WDS_STATE_ALL,EW_COLOR_BACKGROUND);

		window->SetColor(EW_COLOR_BACKGROUND,color);
		}

	if(window->GetOpened())
		{
		window->Resize();
		window->SetDirty(EW_DIRTY_COMPLETE);
		}
	}



/******************************************************************************
long	WDS_Looks::LoadFromFile(char *lookname)

******************************************************************************/
long WDS_Looks::LoadFromFile(char *lookname)
	{
	static char default_path[128]=".:~";

	OSD_FileAttributes attributes;

	GPL_TableEntry default_entry;
	GPL_Tables tables;
	GPL_TableList *tablelist;
	GPL_Table *table;
	GPL_TableEntry *entry;
	WBA_Argument *argument;

	char message[1024];
	long state,element;
	long value;

	WDS_Look blank_look;
	WDS_Look *look;
	WDS_LookEntry *lookentry;

	char includepath[4096];
	char dirname[256];
	char filename[256];
	char **envp;
	const char *home=NULL;
	const char *look_path=NULL;
	char cwd[128];
	char piece[128];

	default_entry.SetLong(WDS_ELEMENT_DEFAULT_INIT);

	tables.SetDefault(&default_entry);
	tables.SetNumberRows(WDS_STATES);
	tables.SetRowLength(WDS_ELEMENT_NUMBER);

	if(!osdCWDGet(cwd,128))
		cwd[0]=0;

	if(argument=WBA_ARGS.Find("HOME"))
		home=argument->GetValue();
	if(argument=WBA_ARGS.Find("WDS_LOOK_PATH"))
		look_path=argument->GetValue();

#if WDS_LOOK_LOAD_DEBUG
	printf("HOME=(%s) WDS_LOOK_PATH=(%s)\n",home,look_path);
#endif

	if(look_path==NULL)
		look_path=default_path;

	long count=0,inc;

	// since GPL_Tables knows not of HOME, pre-filter
	long m;
	for(m=0;m<strlen(look_path);m++)
		{
		if(look_path[m]=='~')
			{
			includepath[count]=0;
			strcat(includepath,home);
			count+=strlen(home);
			}
		else
			includepath[count++]=look_path[m];
		}

	includepath[count]=0;

	count=0;
	while(count<strlen(look_path))
		{
		piece[0]=0;
		inc=0;
		sscanf(&look_path[count],"%[^:]%n",piece,&inc);

#if WDS_LOOK_LOAD_DEBUG
		printf("piece= (%s) of (%s) %d+%d/%d\n",piece,look_path,count,inc,strlen(look_path));
#endif

		if(home && ( !strncmp(piece,"~/",2) || !strcmp(piece,"~") ) )
			sprintf(dirname,"%s%s",home,&piece[1]);
		else if( !strncmp(piece,"../",3) || !strcmp(piece,"..") )
			sprintf(dirname,"%s/..%s",cwd,&piece[2]);
		else if( !strncmp(piece,"./",2) || !strcmp(piece,".") )
			sprintf(dirname,"%s%s",cwd,&piece[1]);
		else
			strcpy(dirname,piece);

		long base,style;
		for(base=0;base<2;base++)
			for(style=0;style<3;style++)
				{
				switch(style)
					{
					case 0:
						sprintf(filename,"%s/%s.laf",dirname,base? "all": lookname);
						break;
					case 1:
						sprintf(filename,"%s/_%s.laf",dirname,base? "all": lookname);
						break;
					case 2:
						sprintf(filename,"%s/.%s.laf",dirname,base? "all": lookname);
						break;
					}

#if WDS_LOOK_LOAD_DEBUG
				printf("LOAD look file (%s)\n",filename);
#endif

				if(!osdFileGetAttributes(filename,&attributes))
					{
#if WDS_LOOK_LOAD_DEBUG
					printf("FOUND\n");
#endif

					tables.LoadFromFile(filename,includepath);
					}
				}

		count+=inc+1;
		}

	// interpret tables

	tablelist=tables.GetTableList();

	tablelist->ToHead();
	while( (table=tablelist->PostIncrement()) != NULL)
		{
		// use previous entries where already established
		lookentry=FindTypeName(table->GetName());

		if(!lookentry)
			{
			// create blank entry with NULL typecode
			RegisterLook((WDS_Typecode)NULL,(GPL_ClassId)NULL,table->GetName(),&blank_look);
			lookentry=FindTypeName(table->GetName());
			}

		if(!lookentry)
			{
			sprintf(message,"could not create lookentry for (%s)",table->GetName());
			EW_PRINT(EW_WIDGET,EW_WARN,message);
			continue;
			}

		look=lookentry->GetLook();

		if(!look)
			{
			sprintf(message,"no previous look for (%s)",table->GetName());
			EW_PRINT(EW_WIDGET,EW_ERROR,message);
			}
		else
			{
			look->Reset();

			for(state=0;state<WDS_STATES;state++)
				{
				for(element=0;element<WDS_ELEMENT_NUMBER;element++)
					{
					entry=table->GetTableEntry(state,element);
					if(entry)
						{
						switch(entry->GetType())
							{
							case GPL_FLOAT:
								value=(long)entry->GetFloat();
								break;
							case GPL_LONG:
								value=entry->GetLong();
								break;
							case GPL_CHARSTR:
								WDS_LookEntry *otherlook=FindTypeName(entry->GetCharStr());

								if(otherlook)
									value=otherlook->GetTypeCode();
								break;
							}

						if(element==WDS_ELEMENT_INHERITANCE || element==WDS_ELEMENT_CALLBACK_INIT)
							{
							if(value==WDS_ELEMENT_DEFAULT_INIT)
								value=WDS_TYPE_UNDEFINED;
							}

						look->SetElement(state,element,value);
						}
					}
				}
			}
		}

	// invalidate old look pointers
	WDS_Widget::IncrementLookCounter();

	return FALSE;
	}



/******************************************************************************
WDS_LookEntry	*WDS_Looks::FindTypeName(char *name)

******************************************************************************/
WDS_LookEntry *WDS_Looks::FindTypeName(char *name)
	{
	WDS_LookEntry *entry;

//	printf("match (%s)\n",name);

	looklist.ToHead();
	while( (entry=looklist.PostIncrement()) != NULL)
		{
		if( !strcmp(name,entry->GetTypeName()) )
			{
//			printf("found 0x%x\n",entry);

			return entry;
			}
		}

//	printf("not found\n");

	return NULL;
	}



/******************************************************************************
long	WDS_Looks::AssignTypecodeToTypeName(WDS_Typecode typecode,
										GPL_ClassId class_id,char *type_name)

	associates given typecode with given typename on look list
	intended for internal use only

	returns FALSE is name not on list

******************************************************************************/
long WDS_Looks::AssignTypecodeToTypeName(WDS_Typecode typecode,GPL_ClassId class_id,char *type_name)
	{
//	printf("WDS_Looks::AssignTypecodeToTypeName(%d,\"%s\")\n",typecode,type_name);

	WDS_LookEntry *match=FindTypeName(type_name);

	if(!match)
		{
//		printf("no match\n");
		return FALSE;
		}

//	printf("match\n");

	match->SetTypeCode(typecode);
	match->SetClassID(class_id);

	return TRUE;
	}
