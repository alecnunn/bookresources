
/******************************************************************************

Coral Tree wds/getnum.cc
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

//TODO: a lot of this code is residual and messy...clean out and streamline.



#include "wds.h"



/******************************************************************************
	WDS_GetNumber::WDS_GetNumber()

Keywords:
******************************************************************************/
WDS_GetNumber::WDS_GetNumber()
{
	SetNumber(0.0);
	justification = WDS_STRING_RIGHT;
}



/******************************************************************************
void	WDS_GetNumber::RecognizeNumberChange(void)

Keywords:
******************************************************************************/
void WDS_GetNumber::RecognizeNumberChange(void)
	{
	char buffer[WDS_MAXNUMBERSTRINGLEN];

	DoubleToString(buffer,number);
	SetString(buffer);
	}



/******************************************************************************
void	WDS_GetNumber::RecognizeStringChange(void)

Keywords:
******************************************************************************/
void WDS_GetNumber::RecognizeStringChange(void)
	{
	char out_string[WDS_MAXNUMBERSTRINGLEN];
	char *buffer=string.Buffer();

	/* PROCESS STRING TO NUMBER */
	number = InterpretStringAsDouble(out_string,0,0,buffer);

//	SetNumber(StringToDouble(buffer));

	SetString(out_string);

	printf("\"%s\" -> \"%s\" %10.6f %.6G\n",buffer,out_string,atof(buffer),number);
	}



/******************************************************************************
long	WDS_GetNumber::Operate(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_GetNumber::Operate(EW_Event *event)
	{
	WDS_String *strptr;

	char *text;

	long mx, my;
	long ctr;
	long truex,truey,sx,sy;
	long middle;

	GetTrueGeometry(&truex,&truey,&sx,&sy);

	if(GetWidgetState())
		{
		text = buffer.Buffer();
		strptr = &buffer;
		}
	else
		{
		text = string.Buffer();
		strptr = &string;
		}

	if( ( (middle=event->IsMiddleMousePress()) || event->IsRightMousePress()) && InRegion(event,TRUE) )
		{
		event->GetMousePosition(&mx,&my);
		ctr = strptr->Operate(GetRootWindow(),truex+depth,truey+depth,sx-2*depth,sy-2*depth,mx,my,justification);
		if(ctr >= 0 || ctr == WDS_STRING_MISS_TO_LEFT)
			{
			double dblnum;
			char out_string[WDS_MAXNUMBERSTRINGLEN];

			dblnum = InterpretStringAsDouble(out_string, ctr, middle? 1: -1, strptr->Buffer());
			fprintf(stderr,"OUT_STRING <%s>\n",out_string);

			strptr->NewBuffer(out_string);

			SetDirty(EW_DIRTY_COMPLETE);

			if(!GetWidgetState())
				{
				SetNumber(dblnum);
//				DoCallback();
				}
			}

		return 0;
		}
	else
		return WDS_GetString::Operate(event);
	}



/******************************************************************************
double	WDS_GetNumber::InterpretStringAsDouble(char *out_string,long position,
													long value,char *string)

	converts "string" to returned double and sets "out_string" to
		an appropriate new string

	"out_string" can be NULL

	a digit at position is incremented by value


Keywords:
******************************************************************************/
double WDS_GetNumber::InterpretStringAsDouble(char *out_string,long position,long value,char *string)
{
	long i,j;
	long string_len;
	long opnum = 0;
	long opnumcnt = 0;
	long sign = 0;
	long m_sign = 0;
	long decimal = FALSE;
	long size = 0;
	long m_size = 0;
	long precision = 0;
	long m_precision = 0;
	long decpos = -1;
	long m_decpos = -1;

	char modifier = 0;
	char numstr[WDS_MAXNUMBERSTRINGLEN];
	char m_numstr[WDS_MAXNUMBERSTRINGLEN];
	char signchar = 0;
	char m_signchar = 0;

	double	number_double;
	double	modval;

	if(!string)
		return 0.0;
	
	string_len = strlen(string);
	if(string_len > WDS_MAXNUMBERSTRINGLEN)
		string_len = WDS_MAXNUMBERSTRINGLEN;

	for(i=0,j=0;i<string_len;i++)
	{
		if(string[i] == ' ' || string[i] == ',' || string[i] == '\t')
		{
			/* SKIP WS AND COMMAS */
		}
		else if(string[i] >= '0' && string[i] <= '9')
		{
			if(!sign) sign = 1;

			size++;
			
			numstr[j] = string[i];
			j++;

			if(decimal) 
			{
				opnumcnt--;
				precision++;
			}
			else 
			{
				opnumcnt++;
			}

			if(i == position) opnum = opnumcnt;
		}
		else if(string[i] == '.')
		{
			if(decimal)
			{
				modifier = '.';
				i++;
				break;
			}
			else
			{
				decpos = opnumcnt;
				opnumcnt = 0;
				size++;
				numstr[j] = '.'; j++;
				decimal = TRUE;
			}
		}
		else if(string[i] == '+' || string[i] == '-')
		{
			if(sign)
			{
				modifier = string[i];
				i++;
				break;
			}
			else
			{
				size++;
				if(string[i] == '+') 
				{
					signchar = '+';
					sign = 1;
				}
				else 
				{
					signchar = '-';
					sign = -1;
				}
				if(i == position) position = WDS_STRING_MISS_TO_LEFT;
			}
		}
		else
		{
			modifier = string[i];
			i++;
			break;
		}
	}
	numstr[j] = 0;
	char *dummy;
	number_double = strtod(numstr,&dummy);
	number_double *= sign;
	if(!decimal)
	{
		decpos = opnumcnt;
	}
	if(opnum>0)
	{
		number_double += (double)value * pow(10,(double)(decpos-opnum));
	}
	else if(opnum < 0)
	{
		number_double += (double)value  / pow(10,(double)(-opnum));
	}
	else if(position == WDS_STRING_MISS_TO_LEFT)
	{
		number_double += (double)value * pow(10,(double)(decpos));
	}
	opnum = 0;
	opnumcnt = 0;
	decimal = 0;
	for(j=0;i<string_len;i++)
	{
		if(string[i] == ' ' || string[i] == ',' || string[i] == '\t')
		{
			/* SKIP WS AND COMMAS */
		}
		else if(string[i] >= '0' && string[i] <= '9')
		{
			if(!m_sign) m_sign = 1;

			m_size++;
			
			m_numstr[j] = string[i];
			j++;

			if(decimal) 
			{
				opnumcnt--;
				m_precision++;
			}
			else 
			{
				opnumcnt++;
			}

			if(i == position) opnum = opnumcnt;
		}
		else if(string[i] == '.')
		{
			if(decimal)
			{
				break;
			}
			else
			{
				m_decpos = opnumcnt;
				opnumcnt = 0;
				m_size++;
				m_numstr[j] = '.'; j++;
				decimal = TRUE;
			}
		}
		else if(string[i] == '+' || string[i] == '-')
		{
			if(m_sign)
			{
				break;
			}
			else
			{
				m_size++;
				if(string[i] == '+') 
				{
					m_signchar = '+';
					m_sign = 1;
				}
				else 
				{
					m_signchar = '-';
					m_sign = -1;
				}
			}
		}
		else
		{
			break;
		}
	}
	m_numstr[j] = 0;
	modval = strtod(m_numstr,&dummy);
	modval *= m_sign;
	if(!decimal)
	{
		m_decpos = opnumcnt;
	}
	if(opnum>0)
	{
		modval += (double)value * pow(10,(double)(m_decpos-opnum));
	}
	else if(opnum < 0)
	{
		modval += (double)value  / pow(10,(double)(-opnum));
	}

	double return_double=number_double;
	long valid_mod = FALSE;
	long valid_modval = FALSE;
	if(modifier)
	{
		switch(modifier)
		{
			case 'P':
			case 'p':
				return_double = number_double / 1000000;
				return_double = return_double / 1000000;
				valid_mod = TRUE;
				break;
			case 'N':
			case 'n':
				return_double = number_double / 1000000000;
				valid_mod = TRUE;
				break;
			case 'U':
			case 'u':
				return_double = number_double / 1000000;
				valid_mod = TRUE;
				break;
			case 'm':
				return_double = number_double / 1000;
				valid_mod = TRUE;
				break;
			case 'C':
			case 'c':
				return_double = number_double / 100;
				valid_mod = TRUE;
				break;
			case 'K':
			case 'k':
				return_double = number_double * 1000;
				valid_mod = TRUE;
				break;
			case 'M':
				return_double = number_double * 1000000;
				valid_mod = TRUE;
				break;
			case 'G':
			case 'g':
				return_double = number_double * 1000000000;
				valid_mod = TRUE;
				break;
			case 'T':
			case 't':
				return_double = number_double * 1000000;
				return_double = return_double * 1000000;
				valid_mod = TRUE;
				break;
			case 'e':
				return_double = number_double * pow(10,modval);
				valid_mod = TRUE;
				valid_modval = TRUE;
				break;
			case '*':
			case 'x':
				return_double = number_double * modval;
				valid_mod = TRUE;
				valid_modval = TRUE;
				break;
			case '/':
				if(modval == 0.0)
				{
					number_double = 0.0;
					break;
				}
				return_double = number_double / modval;
				valid_mod = TRUE;
				valid_modval = TRUE;
				break;
			case '+':
				return_double = number_double + modval;
				valid_mod = TRUE;
				valid_modval = TRUE;
				break;
			case '-':
				return_double = number_double - modval;
				valid_mod = TRUE;
				valid_modval = TRUE;
				break;
			case '^':
				if(number_double < 0 && (modval != (long)modval))
				{
					number_double = 0.0;
					break;
				}
				if(number_double == 0 && modval <= 0.0)
				{
					number_double = 0.0;
					break;
				}
				return_double = pow(number_double,modval);
				valid_mod = TRUE;
				valid_modval = TRUE;
				break;
			default:
				break;
		}
	}


	char format[64];
	if(out_string)
	{
		if(valid_mod && valid_modval)
		{
			sprintf(format,"%%.%df%c%%.%df",(int)precision,modifier,(int)m_precision);
			sprintf(out_string,format,number_double,modval);
		}
		else if(valid_mod)
		{
			sprintf(format,"%%.%df%c",(int)precision,modifier);
			sprintf(out_string,format,number_double);
		}
		else
		{
			sprintf(format,"%%.%df",(int)precision);
			sprintf(out_string,format,number_double);
		}
	}

	return return_double;
}
