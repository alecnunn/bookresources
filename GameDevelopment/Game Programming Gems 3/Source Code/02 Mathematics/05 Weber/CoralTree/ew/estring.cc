
/******************************************************************************

Coral Tree ew/estring.cc
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


#include"ew.h"


/******************************************************************************
		EW_String::EW_String(void)

******************************************************************************/
EW_String::EW_String(void)
	{
	charsize=8;
	buffer=(char *)osdMalloc(1);
	buffer[0]=0;
	}


/******************************************************************************
		EW_String::~EW_String(void)

******************************************************************************/
EW_String::~EW_String(void)
	{
	if(buffer)
		osdFree(buffer);
	}



/******************************************************************************
void	EW_String::SetCharSize(long bits)

******************************************************************************/
void EW_String::SetCharSize(long bits)
	{
	// maybe less constrained later
	bits=8;

	charsize=bits;
	}



/******************************************************************************
unsigned int	EW_String::Length(void)

******************************************************************************/
unsigned int EW_String::Length(void)
	{
	if(buffer)
		return strlen(buffer);
	else
		return 0;
	}



/******************************************************************************
void	EW_String::NewLength(unsigned int newsize)

******************************************************************************/
void EW_String::NewLength(unsigned int newsize)
	{
	buffer=(char *)osdRealloc(buffer,newsize+1);
	buffer[newsize]=0;

	BufferChanged();
	}



/******************************************************************************
long	EW_String::NewBuffer(const char *newbuffer)

******************************************************************************/
long EW_String::NewBuffer(const char *newbuffer)
	{
	if(newbuffer)
		{
		long len=strlen(newbuffer);
		buffer=(char *)osdRealloc(buffer,len+1);

		if(buffer)
			memcpy(buffer,newbuffer,len+1);

		BufferChanged();
		}
	else if(buffer)
		NewLength(1);

	return (buffer != NULL);
	}



/******************************************************************************
void	EW_String::CopyFromString(EW_String *otherstring)

******************************************************************************/
void EW_String::CopyFromString(EW_String *otherstring)
	{
	if(otherstring)
		NewBuffer(otherstring->Buffer());
	}



/******************************************************************************
void	EW_String::CopyToBuffer(char *outbuffer)

******************************************************************************/
void EW_String::CopyToBuffer(char *outbuffer)
	{
	// NOTE: outbuffer must already be allocated to fit buffer
	// but this is cleaner than using EW_String::String(void)
	if(outbuffer)
		strcpy(outbuffer,buffer);

	}



/******************************************************************************
long	EW_String::Buffer(long position)

******************************************************************************/
long EW_String::Buffer(long position)
	{
	if( position>=0 && position<(long)Length() )
		return buffer[position];
	else
		return 0;
	}



/******************************************************************************
long	EW_String::Compare(char *otherbuffer)

******************************************************************************/
long EW_String::Compare(char *otherbuffer)
	{
	if(otherbuffer)
		return strcmp(buffer,otherbuffer);
	else
		return 1;
	}



/******************************************************************************
long	EW_String::Compare(EW_String *otherstring)

******************************************************************************/
long EW_String::Compare(EW_String *otherstring)
	{
	if(otherstring)
		return strcmp(buffer,otherstring->Buffer());
	else
		return 1;
	}



/******************************************************************************
long	EW_String::InsertChar(long position,char c)

	Some compilers cannot inline array types

Keywords:
******************************************************************************/
long EW_String::InsertChar(long position,char c)
	{
	char cstring[2];
	
	cstring[0]=c;
	cstring[1]=0;
	return InsertBuffer(position,cstring);
	}


/******************************************************************************
long	EW_String::InsertBuffer(long position,char *insertion)

******************************************************************************/
long EW_String::InsertBuffer(long position,char *insertion)
	{
	if( position>=0 && position<=(long)Length() )
		{
		long ilen=strlen(insertion);
		long oldlen=Length();

		// NOTE: extra BufferChanged() in NewLength()
		NewLength(oldlen+ilen);

		long i;
/*
		for(i=Length()+ilen;i>position;i--)
			buffer[i]=buffer[i-ilen];
*/

//		printf("oldlen=%d position+len=%d+%d\n",oldlen,position,ilen);
		for(i=oldlen;i>=position;i--)
			buffer[i+ilen]=buffer[i];

		for(i=0;i<ilen;i++)
			buffer[position+i]=insertion[i];

		BufferChanged();

		return 0;
		}
	else
		return 1;
	}



/******************************************************************************
long	EW_String::DeleteChar(long position)

******************************************************************************/
long EW_String::DeleteChar(long position)
	{
	return DeleteRange(position,1);
	}



/******************************************************************************
long	EW_String::DeleteRange(long position, long number)

******************************************************************************/
long EW_String::DeleteRange(long position, long number)
	{
	unsigned int length=Length();

	if( position>=0 && position+number<=(long)length )
		{
		long i;
		for(i=position+number;i<(long)length;i++)
			buffer[i-number]=buffer[i];

		NewLength( (unsigned int)(length-number) );

		return 0;
		}
	else
		return 1;
	}



/******************************************************************************
void	EW_String::ConvertFromNativeFormat(void)

	remove every CR (13 0x0d)

******************************************************************************/
void EW_String::ConvertFromNativeFormat(void)
	{
#if EW_WL==EW_WIN32
	long index=0;
	long length=Length();

	while(index<length)
		{
		EW_PRINT(EW_EW,EW_LOG,"from %d 0x%x",index,Buffer(index));

		if(Buffer(index)==EW_KEY_CR)
			{
			DeleteChar(index);
			length--;
			}
		else
			index++;
		}
#endif
	}



/******************************************************************************
void	EW_String::ConvertToNativeFormat(void)

	confirm a CR (13 0x0d) before every LF (10 0x0a)

******************************************************************************/
void EW_String::ConvertToNativeFormat(void)
	{
#if EW_WL==EW_WIN32
	long last=0,index=0;
	long length=Length();

	while(index<length)
		{
		if(Buffer(index)==EW_KEY_RETURN && last!=EW_KEY_CR)
			{
			InsertChar(index,EW_KEY_CR);
			index++;
			length++;
			}

		last=Buffer(index++);
		}

	index=0;
	while(index<length)
		EW_PRINT(EW_EW,EW_LOG,"to %d 0x%x",index,Buffer(index++));
#endif
	}
