/******************************************************************************

Coral Tree wds/pentry.cc
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


#define WDS_PENTRY_DEBUG	FALSE



/******************************************************************************
		WDS_PointerEntry::WDS_PointerEntry(void)

******************************************************************************/
WDS_PointerEntry::WDS_PointerEntry(void)
	{
	SetName("Pointer Entry");

	copying=FALSE;
	ref_type=WDS_LONG;

	SetPointer(NULL);
	}


/******************************************************************************
void	WDS_PointerEntry::SetPointer(void *set)

******************************************************************************/
void WDS_PointerEntry::SetPointer(void *set)
	{
	ref_buffer=set;

	// convienient references
	ref_long=(long *)ref_buffer;
	ref_float=(float *)ref_buffer;
	ref_char=(char *)ref_buffer;
	ref_string=(WDS_String *)ref_buffer;

	CopyValueToBuffer();
	}



/******************************************************************************
void	WDS_PointerEntry::SetPointerType(long set)

******************************************************************************/
void WDS_PointerEntry::SetPointerType(long set)
	{
	ref_type=set;

	CopyValueToBuffer();
	}


/******************************************************************************
void	WDS_PointerEntry::ChangeState(long newstate,EW_Event *event)

******************************************************************************/
void WDS_PointerEntry::ChangeState(long newstate,EW_Event *event)
	{
#if WDS_PENTRY_DEBUG
	printf("WDS_PointerEntry::ChangeState(%d)\n",newstate);
#endif

	if(!newstate)
		CopyBufferToValue();

	WDS_GetString::ChangeState(newstate,event);
	}



/******************************************************************************
void	WDS_PointerEntry::CopyValueToBuffer(void)

******************************************************************************/
void WDS_PointerEntry::CopyValueToBuffer(void)
	{
#if WDS_PENTRY_DEBUG
	printf("WDS_PointerEntry::CopyValueToBuffer() (%s)\n",GetString()->Buffer());
#endif

	if(ref_buffer==NULL)
		return;

	char message[256];

	switch(ref_type)
		{
		case WDS_STRING:
			SetString(ref_string->Buffer());
			last_string.CopyFromString(ref_string);
			break;

		case WDS_BUFFER:
			SetString(ref_char);
			last_string.NewBuffer(ref_char);
			break;

		case WDS_FLOAT:
			sprintf(message,"%.6G",*ref_float);
			SetString(message);
			last_float= *ref_float;
			break;

		default:
		case WDS_LONG:
			sprintf(message,"%d",*ref_long);
			SetString(message);
			last_long= *ref_long;
			break;
		}

	// copy back in case it was modified by WDS_GetString's confirmation function
	if(!copying)
		{
		copying=TRUE;
		CopyBufferToValue();
		copying=FALSE;
		}
	}



/******************************************************************************
void	WDS_PointerEntry::CopyBufferToValue(void)

******************************************************************************/
void WDS_PointerEntry::CopyBufferToValue(void)
	{
#if WDS_PENTRY_DEBUG
	printf("WDS_PointerEntry::CopyBufferToValue() (%s)\n",GetString()->Buffer());
#endif

	if(ref_buffer==NULL)
		return;

	switch(ref_type)
		{
		case WDS_STRING:
			ref_string->CopyFromString(GetString());
			break;

		case WDS_BUFFER:
			strcpy(ref_char,GetString()->Buffer());
			break;

		case WDS_FLOAT:
			sscanf(GetString()->Buffer(),"%f",ref_float);
			break;

		default:
		case WDS_LONG:
			sscanf(GetString()->Buffer(),"%d",ref_long);
			break;
		}

	// copy back in case it was modified by local confirmation function
	if(!copying)
		{
		copying=TRUE;
		CopyValueToBuffer();
		copying=FALSE;
		}
	}



/******************************************************************************
long	WDS_PointerEntry::ValueChanged(void)

	returns TRUE if value in pointer changed since last copied to GetString()

******************************************************************************/
long WDS_PointerEntry::ValueChanged(void)
	{
	if(ref_buffer==NULL)
		return FALSE;

	switch(ref_type)
		{
		case WDS_STRING:
			return last_string.Compare(ref_string);

		case WDS_BUFFER:
			return last_string.Compare(ref_char);

		case WDS_FLOAT:
			return (*ref_float!=last_float);

		default:
		case WDS_LONG:
			return (*ref_long!=last_long);
		}
	}



/******************************************************************************
long	WDS_PointerEntry::Resize(EW_Resize *resize)

******************************************************************************/
long WDS_PointerEntry::Resize(EW_Resize *resize)
	{
	if(ValueChanged())
		{
		CopyValueToBuffer();
		SetDirty(EW_DIRTY_COMPLETE);
		}

	return WDS_GetString::Resize(resize);
	}
