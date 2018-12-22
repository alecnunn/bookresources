
/******************************************************************************

Coral Tree gpl/ftable.h
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


#define GPL_NULLTYPE	0
#define GPL_FLOAT		1
#define GPL_LONG		2
#define GPL_CHARSTR		3



/******************************************************************************
class	GPL_TableEntry

******************************************************************************/
class GPL_TableEntry
	{
	private:
		long type;

		union
			{
			long longvalue;
			float floatvalue;
			char *charvalue;
			};

	public:

				GPL_TableEntry(void)
					{
					Reset();
					};

virtual			~GPL_TableEntry(void)
					{
					SetType(GPL_NULLTYPE);
					};

		void	Reset(void)
					{
					type=0;
					};

		long	GetType(void)	{ return type; };
		void	SetType(long set)
					{
					if(set==type)
						return;

					if(type==GPL_CHARSTR && charvalue)
						osdFree(charvalue);

					type=set;

					if(set==GPL_CHARSTR)
						charvalue=NULL;
					};

		float	GetFloat(void)
					{
					if(type==GPL_FLOAT)
						return floatvalue;
					else
						return (float)(0.0);
					};
		void	SetFloat(float set)
					{
					SetType(GPL_FLOAT);
					floatvalue=set;
					};

		long	GetLong(void)
					{
					if(type==GPL_LONG)
						return longvalue;
					else
						return 0;
					};
		void	SetLong(long set)
					{
					SetType(GPL_LONG);
					longvalue=set;
					};

		char	*GetCharStr(void)
					{
					if(type==GPL_CHARSTR)
						return charvalue;
					else
						return NULL;
					};
		void	SetCharStr(char *set)
					{
					SetType(GPL_CHARSTR);
					charvalue=(char *)osdRealloc(charvalue,strlen(set)+1);
					strcpy(charvalue,set);
					};

		void	Set(GPL_TableEntry *set)
					{
					switch(set->GetType())
						{
						case GPL_FLOAT:
							SetFloat(set->GetFloat());
							break;
						case GPL_LONG:
							SetLong(set->GetLong());
							break;
						case GPL_CHARSTR:
							SetCharStr(set->GetCharStr());
							break;
						default:
							SetType(GPL_NULLTYPE);
							break;
						};
					};

		void	ConvertToCharStr(char *buffer)
					{
					switch(GetType())
						{
						case GPL_FLOAT:
							sprintf(buffer,"%.6G",GetFloat());
							break;
						case GPL_LONG:
							sprintf(buffer,"%d",(int)(GetLong()));
							break;
						case GPL_CHARSTR:
							sprintf(buffer,"%s",GetCharStr());
							break;
						default:
							sprintf(buffer,"<NULL>");
							break;
						};
					};

		void	ConvertToCharStrFormatted(char *buffer)
					{
					switch(GetType())
						{
						case GPL_FLOAT:
							sprintf(buffer,"%8.6G",GetFloat());
							break;
						case GPL_LONG:
							if(GetLong()>1000)
								sprintf(buffer,"x%-7x",(int)(GetLong()));
							else
								sprintf(buffer,"%8d",(int)(GetLong()));
							break;
						case GPL_CHARSTR:
							sprintf(buffer,"\"%6s\"",GetCharStr());
							break;
						default:
							sprintf(buffer,"<NULL>");
							break;
						};
					};
	};



/******************************************************************************
class	GPL_Table : public GPL_DoubleNode

******************************************************************************/
class GPL_Table : public GPL_DoubleNode
	{
	private:
		GPL_TableEntry	tablename;
		GPL_TableEntry	default_entry;

		GPL_TableEntry	**row;
		long			rows;
		long			rowlength;

		void			ResizeRow(long index,long length);

	public:
						GPL_Table(void)
							{
							row=(GPL_TableEntry **)NULL;
							rows=0;
							rowlength=0;
							};

						~GPL_Table(void)
							{
							// frees all allocated space
							SetNumberRows(0);
							};

		char			*GetName(void)		{ return tablename.GetCharStr(); };
		void			SetName(char *set)	{ tablename.SetCharStr(set); };

		GPL_TableEntry	*GetDefault(void)
							{ return &default_entry; };
		void			SetDefault(GPL_TableEntry *set)
							{ default_entry.Set(set); };

		GPL_TableEntry *GetTableEntry(long rowindex,long columnindex)
							{
							if(rowindex>=0 && rowindex<rows &&
									columnindex>=0 && columnindex<rowlength)
								return &row[rowindex][columnindex];
							else
								return NULL;
							};

		long			ExpandTable(long minrows,long mincolumns)
							{
							long result=FALSE;
							if(rows<minrows)
								{
								SetNumberRows(minrows);
								result=TRUE;
								}
							if(rowlength<mincolumns)
								{
								SetRowLength(mincolumns);
								result=TRUE;
								}
							return result;
							};

		long			GetNumberRows(void)		{ return rows; };
		void			SetNumberRows(long set);

		long			GetRowLength(void)		{ return rowlength; };
		void			SetRowLength(long set)
							{
							long m;

							for(m=0;m<rows;m++)
								ResizeRow(m,set);

							rowlength=set;
							};
	};



/******************************************************************************
class	GPL_TableList : public GPL_IDoubleList<GPL_Table>

******************************************************************************/
class GPL_TableList : public GPL_IDoubleList<GPL_Table>
	{
	};



/******************************************************************************
class	GPL_Tables

Keywords:
******************************************************************************/
class GPL_Tables
	{
	private:
		GPL_TableList	tablelist;

		GPL_Table		*current_table;
		GPL_TableEntry	default_entry;

		char			filename[256];
		long			stacksize;
		long			rows;
		long			rowlength;

		void		InterpretArgument(long entries,GPL_TableEntry *label,
										GPL_TableEntry *entry,char *argument,
										GPL_TableEntry *result);
		long		ReadArgument(char *line,char *argument);
		long		MatchEntry(long entries,GPL_TableEntry *entry,char *name);

		long			Interpret(char *filename,char *data,char *includepath);

	public:
						GPL_Tables(void);

		GPL_TableList	*GetTableList(void)	{ return &tablelist; };

		void			SetFilename(char *set)	{ strcpy(filename,set); };
		char			*GetFilename(void)		{ return filename; };

		long			LoadFromFile(char *filename,char *includepath);

						// not implemented yet
		long			ReadFromMemory(char *data);

		GPL_TableEntry	*GetDefault(void)
							{ return &default_entry; };
		void			SetDefault(GPL_TableEntry *set)
							{ default_entry.Set(set); };

		long			GetNumberRows(void)		{ return rows; };
		void			SetNumberRows(long set)	{ rows=set; };

		long			GetRowLength(void)		{ return rowlength; };
		void			SetRowLength(long set)	{ rowlength=set; };

		void			SetCurrentTable(GPL_Table *newtable)
							{ current_table=newtable; };
		GPL_Table		*GetCurrentTable(void)	{ return current_table; };

		GPL_Table  		 *AddOrSetTable(char *tablename);
    };
