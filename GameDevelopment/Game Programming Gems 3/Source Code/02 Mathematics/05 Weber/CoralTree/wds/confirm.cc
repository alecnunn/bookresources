/******************************************************************************

Coral Tree wds/confirm.cc
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


#define WDS_CONFIRM_DEBUG		FALSE
#define WDS_TIME_DEBUG			FALSE

#define WDS_INVALID_WORD		FALSE	// add word "Invalid" after bad string

#define WDS_CONFIRM_BUFFERSIZE	1024



/******************************************************************************
long	WDS_String::Confirm(void)

******************************************************************************/
long WDS_String::Confirm(void)
	{
#if WDS_CONFIRM_DEBUG
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_String::Confirm() (%s)",Buffer());
#endif

	// turn off auto-highlight
	if(confirm_flags&WDS_CONFIRM_AUTO_HIGHLIGHT)
		SetHighlight(WDS_HIGHLIGHT_OFF);

	if(GetLength()>WDS_CONFIRM_BUFFERSIZE-1)
		NewLength(WDS_CONFIRM_BUFFERSIZE-1);

	if(confirm_function && ( !(confirm_flags&WDS_CONFIRM_ALLOW_EMPTY) || GetLength()) )
		{
		long result=(*confirm_function)(this,confirm_flags);

#if WDS_CONFIRM_DEBUG
		EW_PRINT(EW_WIDGET,EW_LOG,"  result: (%s) %d",Buffer(),result);
#endif

		return result;
		}


	return FALSE;
	}



/******************************************************************************
long	WDS_String::RemoveInvalid(char *buffer)

	potentially removes "Invalid" if exists
	returns TRUE if found/removed

******************************************************************************/
long WDS_String::RemoveInvalid(char *buffer)
	{
#if WDS_INVALID_WORD

	long len=strlen(buffer);
	char *match=strstr(buffer,WDS_CONFIRM_INVALID_RESPONSE);

	if(match)
		{
		len-=strlen(match);

		if(len>0 && buffer[len-1]==' ')
			len--;

		buffer[len]=0;

		return TRUE;
		}
#endif

	return FALSE;
	}



/******************************************************************************
void	WDS_String::InvalidateTo(char *buffer)

	copies string to buffer with "Invalid" appended

	checks if "Invalid" already appended

******************************************************************************/
void WDS_String::InvalidateTo(char *buffer)
	{
	CopyToBuffer(buffer);

	RemoveInvalid(buffer);

#if WDS_INVALID_WORD
	strcat(buffer," ");
	strcat(buffer,WDS_CONFIRM_INVALID_RESPONSE);
#endif

	// turn on auto-highlight
	if(confirm_flags&WDS_CONFIRM_AUTO_HIGHLIGHT)
		SetHighlight(WDS_HIGHLIGHT_BACKFILL);
	}



/******************************************************************************
long	WDS_String::ConfirmDate(WDS_String *string,long flags)

	static

	for ISO 8601, international date format is YYYY-MM-DD
	otherwise US convention, format is MM-DD-YYYY

	will attempt to interpret formats of various types

	interpretation of input is usually independent of specified
		output format, except to resolve discrepencies
		example: 01-02-03 (US Jan 2, 2003 or ISO Feb 3, 2001)

	implements leap year effects including post-1700 centenary exception and 
		quadcentenary counter-exception

	doesn't implement rumored year 4000 non-leap year

	accounts for the fact that Sept 3-13, 1752 never occured
	(when they just figured out leap year exceptions and had to adjust)

******************************************************************************/
long WDS_String::ConfirmDate(WDS_String *string,long flags)
	{
	char buffer[WDS_CONFIRM_BUFFERSIZE];
	long length[3];
	int date[3];
	int preinc,inc,point=0;
	long no_rotate=FALSE;
	long iso=(flags&WDS_CONFIRM_ISO);
	long stroke=(flags&WDS_CONFIRM_STROKE);
	long leadzero=(flags&WDS_CONFIRM_LEAD_ZERO);
	long invalid=FALSE;

	// extra space or %[] won't work
	sprintf(buffer," %s",string->Buffer());
	string->RemoveInvalid(buffer);

	long m;
	for(m=0;m<3;m++)
		{
		date[m]=0;
		preinc=0;
		inc=0;

		sscanf(&buffer[point],"%*[^0-9]%n%d%n",&preinc,&date[m],&inc);
		length[m]=inc-preinc;
		point+=inc;
		}

//	printf("pre %d (%d) %d (%d) %d (%d) \n",date[0],length[0],date[1],length[1],date[2],length[2]);

	// if 1 arg, interpret without delimiters
	if(date[1]==0)
		{
		if( date[0]<100 )
			{
			// 0-100 waste
			invalid=TRUE;
			}
		else if( date[0]>12319999 || (date[0]>123199 && date[0]<1009999) || length[0]<5)
			{
			// yyyymMDD	0101-9999 123200-01009999 12320000-up
			date[2]=date[0]%100;
			date[0]/=100;
			date[1]=date[0]%100;
			date[0]/=100;

			no_rotate=TRUE;
			}
		else if(date[0]>=1010000)
			{
			// MMDDYYYY 01010000-12319999
			date[1]=date[0]/10000;
			date[0]%=10000;
			date[2]=date[1]%100;
			date[1]/=100;

			no_rotate=TRUE;
			}
		else
			{
			// YYMMDD/MMDDYY overlap 00000-123199

			// YYMMDD
			date[2]=date[0]%100;
			date[0]/=100;
			date[1]=date[0]%100;
			date[0]/=100;
			}

//		printf("one arg to %d %d %d\n",date[0],date[1],date[2]);
		}

	// if 2 args, imply year
	if(!inc && date[2]==0)
		{
		date[2]=date[1];
		date[1]=date[0];
		date[0]=0;
		}
	else if(!no_rotate)
		{
		// potentially MMDDYY
		// if 12<DD<32 *OR* YY>31 *OR* !iso and MM<13 and DD<32
		if( (date[1]>12 && date[1]<32) || date[2]>31 || (!iso && date[0]<13 && date[1]<32) )
			{
			// rotate
			m=date[2];
			date[2]=date[1];
			date[1]=date[0];
			date[0]=m;
			}
		}

	if(date[1]<1 || date[1]>12)
		invalid=TRUE;

	// 1<month<12
	date[1]=EW_BOUND(date[1],1,12);

	// assume from 2 digit date
	if(date[0]<70)
		date[0]+=2000;
	else if(date[0]<100)
		date[0]+=1900;

	long maxday=30;

	// Jan, Mar, May, July, Aug, Oct, Dec
	if( (date[1]<8 && (date[1]%2)) || (date[1]>7 && !(date[1]%2)) )
		maxday=31;
	// Feb
	else if(date[1]==2)
		{
		// NOTE: exact centuries are not leap years except every 400 years which ARE leap years
		// as a result, while 1900 and 2100 are not leap years,
		// 2000 is a regular leap year, an exception to the exception
		// 1700 and prior, all centuries were leap years

		if( !(date[0]%4) && ((date[0]%100) || !(date[0]%400) || date[0]<=1700)  )
			maxday=29;
		else
			maxday=28;
		}

	if(date[2]<1 || date[2]>maxday)
		invalid=TRUE;

	// day from 1 to 28, 29, 30, or 31
	date[2]=EW_BOUND(date[2],1,maxday);

	// Sept 1752 anomaly
	if(date[0]==1752 && date[1]==9 && date[2]>=3 && date[2]<=13)
		{
		invalid=TRUE;
		date[2]=14;
		}

	char delim= (stroke)? '/': '-';

	if(invalid && (flags&WDS_CONFIRM_MARK_INVALID) )
		{
		string->SetInvalid(TRUE);
		string->InvalidateTo(buffer);
		}
	else
		{
		string->SetInvalid(FALSE);
		if(iso)
			sprintf(buffer,leadzero? "%04d%c%02d%c%02d": "%d%c%d%c%d",date[0],delim,date[1],delim,date[2]);
		else
			sprintf(buffer,leadzero? "%02d%c%02d%c%04d": "%d%c%d%c%d",date[1],delim,date[2],delim,date[0]);
		}

	if(string->Compare(buffer))
		{
		string->NewBuffer(buffer);
		return TRUE;
		}
	else
		return FALSE;
	}



/******************************************************************************
long	WDS_String::ConfirmTime(WDS_String *string,long flags)

	static

******************************************************************************/
long WDS_String::ConfirmTime(WDS_String *string,long flags)
	{
	char buffer[WDS_CONFIRM_BUFFERSIZE];
	char second_buffer[WDS_CONFIRM_BUFFERSIZE];
	char suffix[WDS_CONFIRM_BUFFERSIZE];
	long length[4];
	int time[3],sign[3];
	int preinc,inc,point=0;
	long leadzero=(flags&WDS_CONFIRM_LEAD_ZERO);
	long elapsed=(flags&WDS_CONFIRM_ELAPSED);
	long military=(flags&WDS_CONFIRM_24HR);
	long seconds=(flags&WDS_CONFIRM_SECONDS);
	long invalid=FALSE;

	// extra space or %[] won't work
	sprintf(buffer," %s",string->Buffer());
	string->RemoveInvalid(buffer);

	suffix[0]=0;

	long m;
	for(m=0;m<4;m++)
		{
		if(m<3)
			{
			time[m]=0;
			sign[m]=FALSE;
			}
		preinc=0;
		inc=0;

		if(m==3)
			sscanf(&buffer[point],"%n%s%n",&preinc,suffix,&inc);
		else
			sscanf(&buffer[point],"%*[^0-9-]%n%d%n",&preinc,&time[m],&inc);

		// Note: if seconds don't exist '&time[m]' returns, in which case inc isn't set and stays zero as preset
		// And as a result, prefix is still the next thing to be scanned

		length[m]=inc-preinc;
		point+=inc;

		if(m<3 && time[m]<0)
			{
			sign[m]=TRUE;
			time[m]= -time[m];
			}
		}

	if(length[0]<1 && length[1]<1 && length[2]<1)
		invalid=TRUE;

#if WDS_TIME_DEBUG
	printf("time: %s %d (%d) %s %d (%d) %s %d (%d) \"%s\" (%d) \n",
			sign[0]? "-": "+",time[0],length[0],
			sign[1]? "-": "+",time[1],length[1],
			sign[2]? "-": "+",time[2],length[2],
			suffix,length[3]);
#endif

	// HHMMSS, HHHHMMSS
	// extract SS from [0]
	if(length[1]<1 && length[2]<1 && (elapsed? (length[0]>6): (time[0]>9999)) )
		{
//		printf("A\n");

		time[2]=time[0]%100;
		time[0]=time[0]/100;

		length[2]=2;
		length[0]-=2;

		sign[2]=sign[0];
		}

	// HHMMSS or HHMM SS, HHHHMMSS or HHHHMM SS
	// extract MM from [0]
	if( (length[1]<1 || length[2]<1) && (elapsed? (length[0]>4): (time[0]>99)) )
		{
//		printf("B\n");

		// if seconds blank, shift 2nd arg to seconds
		if(length[2]<1)
			{
			time[2]=time[1];
			length[2]=length[1];
			}

		time[1]=time[0]%100;
		time[0]=time[0]/100;

		length[1]=2;
		length[0]-=2;

		sign[1]=sign[0];
		}

	// HH MMSS, HHHH MMSS
	// extract SS from [1]
	if(length[2]<1 && time[1]>99)
		{
//		printf("C\n");

		time[2]=time[1]%100;
		time[1]=time[1]/100;

		length[2]=2;
		length[1]-=2;

		sign[2]=sign[1];
		}

#if WDS_TIME_DEBUG
	printf("mid: %s %d (%d) %s %d (%d) %s %d (%d) \"%s\" (%d) \n",
			sign[0]? "-": "+",time[0],length[0],
			sign[1]? "-": "+",time[1],length[1],
			sign[2]? "-": "+",time[2],length[2],
			suffix,length[3]);
#endif

	if(suffix[0]=='P' || suffix[0]=='p')
		strcpy(suffix,"pm");
	else
		strcpy(suffix,"am");

	for(m=0;m<3;m++)
		if(sign[m])
			time[m]= -time[m];

	// seconds, minutes, not hours -> adjust for negative
	for(m=2;m>0;m--)
		while(time[m]<0)
			{
			invalid=TRUE;
			time[m]+=60;
			time[m-1]--;
			}

	if(seconds)
		{
		if(time[2]>59)
			invalid=TRUE;

		time[1]+=time[2]/60;
		time[2]%=60;
		sprintf(second_buffer,"%s%02d",military? "": ":", time[2]);
		}
	else
		second_buffer[0]=0;

	if(time[0]>23 && !elapsed)
		invalid=TRUE;

	if(time[1]>59)
		invalid=TRUE;

	time[0]+=time[1]/60;
	time[1]%=60;

	if(!elapsed)
		time[0]%=24;

	if(!military && !elapsed)
		{
		while(time[0]<0)
			{
			invalid=TRUE;

			time[0]+=12;
			suffix[0]= (suffix[0]=='p')? 'a': 'p';
			}

		if(time[0]==0)
			time[0]=12;

		while(time[0]>12)
			{
			time[0]-=12;
			suffix[0]= (suffix[0]=='p')? 'a': 'p';
			}
		}
	else if(time[0]<12)
		{
		if(suffix[0]=='p')
			time[0]+=12;

		if(!elapsed)
			while(time[0]<0)
				{
				invalid=TRUE;

				time[0]+=24;
				}
		}

#if WDS_TIME_DEBUG
	printf("result: %s %d (%d) %s %d (%d) %s %d (%d) \"%s\" (%d) \n",
			sign[0]? "-": "+",time[0],length[0],
			sign[1]? "-": "+",time[1],length[1],
			sign[2]? "-": "+",time[2],length[2],
			suffix,length[3]);
#endif

	if(invalid && (flags&WDS_CONFIRM_MARK_INVALID) )
		{
		string->SetInvalid(TRUE);
		string->InvalidateTo(buffer);
		}
	else
		{
		string->SetInvalid(FALSE);
		if(elapsed&&military)
			sprintf(buffer,leadzero? "%04d%02d%s": "%d%02d%s",time[0],time[1],second_buffer);
		else if(elapsed)
			sprintf(buffer,leadzero? "%04d:%02d%s": "%d:%02d%s",time[0],time[1],second_buffer);
		else if(military)
			sprintf(buffer,leadzero? "%02d%02d%s": "%d%02d%s",time[0],time[1],second_buffer);
		else
			sprintf(buffer,leadzero? "%02d:%02d%s%s": "%d:%02d%s%s",time[0],time[1],second_buffer,suffix);
		}

	if(string->Compare(buffer))
		{
		string->NewBuffer(buffer);
		return TRUE;
		}
	else
		return FALSE;
	}



/******************************************************************************
long	WDS_String::ConfirmDateTime(WDS_String *string,long flags)

	static

******************************************************************************/
long WDS_String::ConfirmDateTime(WDS_String *string,long flags)
	{
	WDS_String	local_string;

	char buffer[WDS_CONFIRM_BUFFERSIZE];
	char piece[2][WDS_CONFIRM_BUFFERSIZE];
	long fulllength;
	long pairs,skips=2;
	long invalid=FALSE;

	strcpy(buffer,string->Buffer());
	string->RemoveInvalid(buffer);

	fulllength=strlen(buffer);

	// clip trailing whitespace
	while(fulllength>0 && EW_IS_WHITESPACE(buffer[fulllength-1]))
		fulllength--;
	buffer[fulllength]=0;

	long length=fulllength;
	long mark3=fulllength;

	// pass 0: count numbers from front
	// pass 1: back up over two or three numbers

	long pass;
	for(pass=0;pass<2;pass++)
		{
		length=pass? fulllength: 0;

		if(length<0)
			length=0;

		long done=FALSE;
		long n=0;
		long inc= pass? -1: 1;
		long del= pass? -1: 0;
		long limit= pass? 0: fulllength;
		long delimited=TRUE;

		while(!done)
			{
			long oldlength=length;

			// skip over non-numbers
			while( length!=limit && (buffer[length+del]<'0' || buffer[length+del]>'9') )
				length+=inc;

//			printf("%d '%c'\n",length,buffer[length]);
			if(oldlength==length)
				delimited=FALSE;

			if(pass==0 && n==3 && delimited)
				mark3=oldlength-1;

			long count=0;

			oldlength=length;

			// skip over numbers
			while( length!=limit && (buffer[length+del]>='0' && buffer[length+del]<='9') && (count++)<2 )
				length+=inc;

			if(oldlength!=length)
				n++;

//			printf("%d %d %d %d/%d\n",pass,n,mark3,length,fulllength);

			if(pass==1 && n==skips)
				done=TRUE;

			if(length==limit)
				done=TRUE;
			}

		if(pass==0)
			{
			pairs=n;
			if(pairs>6)
				skips=3;
			}
		}

//	printf("length=least(%d,%d)\n",length,mark3);
	length=EW_MIN(length,mark3);

	long nonwhite=FALSE;
	long white_start= -1;
	long white_end= -1;

	// whitespace check
	long m;
	for(m=0;m<fulllength;m++)
		{
		if(EW_IS_WHITESPACE(buffer[m]))
			{
			if(nonwhite)
				{
				if(white_end>=0)
					white_start=fulllength;	// second whitespace
				else if(white_start<0)
					white_start=m;			// first whitespace
				}
			}
		else
			{
			nonwhite=TRUE;

			if(white_start>=0 && white_end<0)
				white_end=m-1;
			}

//		printf("white %d '%c' %d %d\n",m,buffer[m],white_start,white_end);
		}

	// if one whitespace region, split there
	if(white_start>0 && white_end<fulllength-1 && white_start<=white_end)
		length=white_start;

	strncpy(piece[0],buffer,length);
	piece[0][length]=0;
	strcpy(piece[1],&buffer[length]);

//	printf("of \"%s\" %d (%d pairs) pre \"%s\" (%d) \"%s\"\n",buffer,fulllength,pairs,piece[0],length,piece[1]);

	local_string.NewBuffer(piece[0]);
	ConfirmDate(&local_string,flags);
	if(local_string.GetInvalid())
		invalid=TRUE;
	string->RemoveInvalid(local_string.Buffer());
	sprintf(buffer,"%s ",local_string.Buffer());

	local_string.NewBuffer(piece[1]);
	ConfirmTime(&local_string,flags);
	if(local_string.GetInvalid())
		invalid=TRUE;
	string->RemoveInvalid(local_string.Buffer());
	strcat(buffer,local_string.Buffer());

	if(invalid && (flags&WDS_CONFIRM_MARK_INVALID) )
		{
		string->SetInvalid(TRUE);
		string->InvalidateTo(buffer);
		}
	else
		string->SetInvalid(FALSE);

	if(string->Compare(buffer))
		{
		string->NewBuffer(buffer);
		return TRUE;
		}
	else
		return FALSE;
	}



/******************************************************************************
long	WDS_String::ConfirmPhone(WDS_String *string,long flags)

	static

	default							up to 15 digits
	WDS_CONFIRM_NANP		(US)	aaa bbb-cccc
	WDS_CONFIRM_NANP_PRETTY	(US)	(aaa) bbb-cccc
	CCITT					(Intl)	d.eeeeeee

	All lengths constant except d from 2 to 8 digits

	character mapping
		ABC		2
		DEF		3
		GHI		4
		JKL		5
		MNO		6
		PRS		7
		TUV		8
		WXY		9

******************************************************************************/
long WDS_String::ConfirmPhone(WDS_String *string,long flags)
	{
	long mapping[26]={2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,-1,7,7,8,8,8,9,9,9,-1};

	char original[WDS_CONFIRM_BUFFERSIZE];

	// extra space or %[] won't work
	sprintf(original," %s",string->Buffer());
	string->RemoveInvalid(original);

	long length=strlen(original);
	long invalid=FALSE;

	char c,buffer[WDS_CONFIRM_BUFFERSIZE];
	int piece[3];
	long result=FALSE;
	long map= (flags&WDS_CONFIRM_MAP_CHAR);

	// grab 15 characters (up to 7 past a '.')
	long n=0,count1=0,count2=0,count3=0;
	long period=FALSE;
	long precode=TRUE;

	// read one extra digit to see if it is available (invalid condition)
	while(n<length && count1<16 && count2<8)
		{
		c=original[n];

		// . and ) are definite "area code" separators
		// space and tab are maybe's
		// - is not
		if(c=='.' || c==')')
			{
			period=TRUE;
			precode=FALSE;
			}
		else if(EW_IS_WHITESPACE(c))
			precode=FALSE;

		if(c>='A' && c<'Z')
			c-='A'-'a';

		if(map && c>='a' && c<'z' && c!='q')
			c='0'+mapping[c-'a'];

		if(c>='0' && c<='9')
			{
			buffer[count1++]=c;
			if(period)
				count2++;
			if(precode)
				count3++;
			}

		n++;
		}

	if(count1>15 || count2>7)
		{
		count1--;
		if(count2>0)
			count2--;
		if(count3>0 && precode)
			count3--;

		invalid=TRUE;
		}

	// if definite separator, and trailing zeros
	while(count1<7 || (period && count2<7) )
		{
		invalid=TRUE;

		buffer[count1++]='0';
		if(period)
			count2++;
		}

	buffer[count1]=0;

//	printf("n=%d/%d count1=%d count2=%d \"%s\"->\"%s\"\n",n,length,count1,count2,original,buffer);

	// if total<7, no protected precode
	if(count1<=7)
		count3=0;

	if(count2==0)
		count2=7;

	piece[0]=0;
	piece[1]=0;
	piece[2]=0;

	if(period==2)
		{
		n=EW_MIN(4,count2);
		n=EW_MIN(n,(long)strlen(buffer));
		}
	else
		n=EW_MIN(4,strlen(buffer));

	count1-=n;
	count2-=n;
	sscanf(&buffer[count1],"%d",&piece[2]);
	buffer[count1]=0;

	count2=EW_MAX(0,count2);

	n=EW_MIN(3,count2);
	n=EW_MIN(n,(long)strlen(buffer)-count3);
	count1-=n;
	sscanf(&buffer[count1],"%d",&piece[1]);
	buffer[count1]=0;

	long prefix=count1;

	if(flags&WDS_CONFIRM_NANP)
		count1-=3;
	else
		count1-=8;

	if(count1!=0)
		{
		invalid=TRUE;

		if(count1<0)
			count1=0;
		}

	sscanf(&buffer[count1],"%d",&piece[0]);

	if(invalid && (flags&WDS_CONFIRM_MARK_INVALID) )
		{
		string->SetInvalid(TRUE);
		string->InvalidateTo(buffer);
		}
	else
		{
		string->SetInvalid(FALSE);
		if(flags&WDS_CONFIRM_NANP)
			{
			if(prefix<3)
				prefix=3;
			if(flags&WDS_CONFIRM_PRETTY)
				sprintf(buffer,"(%03d) %03d-%04d",piece[0],piece[1],piece[2]);
			else
				sprintf(buffer,"%03d %03d-%04d",piece[0],piece[1],piece[2]);
			}
		else if(flags&WDS_CONFIRM_CCITT)
			{
			if(prefix<2)
				prefix=2;
			sprintf(buffer,"%0*d.%03d%04d",(int)prefix,piece[0],piece[1],piece[2]);
			}
		else
			sprintf(buffer,"%0*d%03d%04d",(int)prefix,piece[0],piece[1],piece[2]);
		}

//	printf("%d %d %d \"%s\"\n",piece[0],piece[1],piece[2],buffer);

	if(string->Compare(buffer))
		{
		string->NewBuffer(buffer);
		result=TRUE;
		}

	return result;
	}



/******************************************************************************
long	WDS_String::ConfirmSSN(WDS_String *string,long flags)

	static

******************************************************************************/
long WDS_String::ConfirmSSN(WDS_String *string,long flags)
	{
	char c,inbuffer[WDS_CONFIRM_BUFFERSIZE],outbuffer[WDS_CONFIRM_BUFFERSIZE];

	// extra space or %[] won't work
	sprintf(inbuffer," %s",string->Buffer());
	string->RemoveInvalid(inbuffer);

	long count=0,incount=0,inlen=strlen(inbuffer);
	long invalid=FALSE;

	// 11th digit is an invalid condition that has to be checked for
	while(count<12)
		{
		if(incount<inlen)
			{
			// next character
			c=inbuffer[incount++];
			}
		else if(count==11)
			{
			// out of characters at the appropriate time
			c='-';
			outbuffer[count++]=0;
			}
		else
			{
			// out of characters too early: append zeros
			invalid=TRUE;
			c='0';
			}

		if(c>='0' && c<='9')
			{
			if(count==11)
				{
				// too many digits
				invalid=TRUE;
				outbuffer[count++]=0;
				}
			else
				{
				// needed digit
				outbuffer[count++]=c;
				}
			}
		else if(c!='-' && !EW_IS_WHITESPACE(c) && count<12)
			invalid=TRUE;

		if(count==3 || count==6)
			outbuffer[count++]='-';
		}

	outbuffer[count]=0;

	long result=FALSE;

	if(invalid && (flags&WDS_CONFIRM_MARK_INVALID) )
		{
		string->SetInvalid(TRUE);
		string->InvalidateTo(outbuffer);
		}
	else
		string->SetInvalid(FALSE);

	if(string->Compare(outbuffer))
		{
		string->NewBuffer(outbuffer);
		result=TRUE;
		}

	return result;
	}



/******************************************************************************
long	WDS_String::ConfirmLong(WDS_String *string,long flags)

	static

******************************************************************************/
long WDS_String::ConfirmLong(WDS_String *string,long flags)
	{
	char buffer[WDS_CONFIRM_BUFFERSIZE];
	double value=0.0;

	long result=ConfirmDouble(string,flags);

	sscanf(string->Buffer(),"%lf",&value);

	long longvalue=(long)value;

	sprintf(buffer,"%d",(int)longvalue);

//	printf("LONG %s\n",buffer);

	if(string->Compare(buffer))
		{
		string->NewBuffer(buffer);
		result=TRUE;
		}

	string->SetInvalid(FALSE);
	return result;
	}



/******************************************************************************
long	WDS_String::ConfirmFloat(WDS_String *string,long flags)

	static

	constrained to 6 significant digits

******************************************************************************/
long WDS_String::ConfirmFloat(WDS_String *string,long flags)
	{
	char buffer[WDS_CONFIRM_BUFFERSIZE];
	double value=0.0;

	long result=ConfirmDouble(string,flags);

	sscanf(string->Buffer(),"%lf",&value);

	sprintf(buffer,"%.6G",value);

//	printf("FLOAT %s\n",buffer);

	if(string->Compare(buffer))
		{
		string->NewBuffer(buffer);
		result=TRUE;
		}

	string->SetInvalid(FALSE);
	return result;
	}



/******************************************************************************
long	WDS_String::ConfirmDouble(WDS_String *string,long flags)

	static

	constrained to 10 significant digits

******************************************************************************/
long WDS_String::ConfirmDouble(WDS_String *string,long flags)
	{
	char buffer[WDS_CONFIRM_BUFFERSIZE];
	char suffix[WDS_CONFIRM_BUFFERSIZE];
	double value=0.0;

	suffix[0]=0;

	// extra space or %[] won't work
	sprintf(buffer," %s",string->Buffer());
	string->RemoveInvalid(buffer);

	sscanf(buffer,"%*[^-.0-9]%lf%s",&value,suffix);

//	printf("%.10lG suffix (%s) %.10G\n",value,suffix,SuffixToFloat(suffix));

	value*=SuffixToFloat(suffix);

	sprintf(buffer,"%.10G",value);

//	printf("DOUBLE %s\n",buffer);

	string->SetInvalid(FALSE);

	if(string->Compare(buffer))
		{
		string->NewBuffer(buffer);
		return TRUE;
		}
	else
		return FALSE;
	}



/******************************************************************************
float	WDS_String::SuffixToFloat(char *buffer)

******************************************************************************/
float WDS_String::SuffixToFloat(char *buffer)
	{
	if(buffer==NULL)
		return 1.0;

	switch(buffer[0])
		{
		case 'A':			// atto
		case 'a':
			return 1E-18;
		case 'F':			// femto
		case 'f':
			return 1E-15;
		case 'p':			// pico
			return 1E-12;
		case 'N':			// nano
		case 'n':
			return 1E-9;
		case 'U':			// micro
		case 'u':
			return 1E-6;
		case 'm':			// milli
			return 1E-3;
		case 'C':			// centi
		case 'c':
		case '%':
			return 1E-2;
		case 'd':			// deci
			return 1E-1;
		case 'D':			// deca (da)
			return 1E1;
		case 'H':			// hecto
		case 'h':
			return 1E2;
		case 'K':			// kilo
		case 'k':
			return 1E3;
		case 'M':			// mega
			return 1E6;
		case 'G':			// giga
		case 'g':
			return 1E9;
		case 'T':			// tera
		case 't':
			return 1E12;
		case 'P':			// peta
			return 1E15;
		case 'X':			// exa (e)
		case 'x':
			return 1E18;
							// uta? 1E24
		}

	return 1.0;
	}
