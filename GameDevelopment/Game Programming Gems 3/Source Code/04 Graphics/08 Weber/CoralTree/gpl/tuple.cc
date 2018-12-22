/******************************************************************************

Coral Tree gpl/tuple.cc
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

#include <gpl/gpl.h>

/***********************************************************************
class GPL_Tuple
***********************************************************************/
GPL_Tuple::GPL_Tuple(void)
{
}

GPL_Tuple::~GPL_Tuple(void)
{
}

GPL_Boolean GPL_Tuple::CheckMatch(char *fmt, ...)
{
	long return_val;
	va_list ap;
	va_start(ap, fmt);
	return_val = VCheckMatch(fmt, &ap);
	va_end(ap);
	return return_val;
}


/******************************************************************************
The GPL_USE_DESC_IMPLEMENTATION implementation is more elegant (code-wise)
but is currently 1 order of magnitude slower than the original implementation
that follows.
******************************************************************************/
#ifdef GPL_USE_DESC_IMPLEMENTATION
GPL_Boolean GPL_Tuple::VCheckMatch(char *fmt, va_list *ap) const
{
	GPL_Tuple tuple;
	tuple.VSet(fmt, ap);
	return tuple.Matches(*this);
}
#else
GPL_Boolean GPL_Tuple::VCheckMatch(char *fmt, va_list *ap) const
{
	long i, fl;
	long huntmode = FALSE;
	long n;

	fl = strlen(fmt);

	n = -1;

	for (i = 0; i < fl;i++)
	{
		if (fmt[i] == '*')
		{
			huntmode = TRUE;
			continue;
		}

		long match;
		while(1)
		{
			n++;
			if (n >= (long)values.GetCount()) 
			{
				return FALSE;
			}

			int fi;
			double fr;
			char *fs;
			char *fb;
			int fbl;
			char *tb;
			unsigned long tbl;
			double variance;
			GPL_Message *m;
			char *a1, *a2;
			unsigned long l1, l2;

			match = GPL_HIT;

			switch (fmt[i])
			{
				case '.':
					/* WILDCARD */
					break;
				case 'i':
					/* INT TYPE */
					if (values[n].GetType() != GPL_VALUETYPE_INTEGER)
					{
						match = GPL_TMISS;
					}
					break;
				case 'I':
					/* INT MATCH */
					if (values[n].GetType() != GPL_VALUETYPE_INTEGER) 
					{
						match = GPL_TMISS;
						break;
					}
					fi = (int)va_arg(*ap, int);
					if (fi != values[n].GetInteger())
					{
						match = GPL_CMISS;
					}
					break;
				case 'r':
					/* REAL TYPE */
					if (values[n].GetType() != GPL_VALUETYPE_REAL)
					{
						match = GPL_TMISS;
					}
					break;
				case 'R':
					/* REAL MATCH */
					if (values[n].GetType() != GPL_VALUETYPE_REAL) 
					{
						match = GPL_TMISS;
						break;
					}
					fr = (double)va_arg(*ap, double);
					variance = (double)va_arg(*ap, double);
					if (fabs(fr - values[n].GetReal()) > variance)
					{
						match = GPL_CMISS;
					}
					break;
				case 's':
					if (values[n].GetType() != GPL_VALUETYPE_STRING)
					{
						match = GPL_TMISS;
					}
					break;
				case 'S':
					if (values[n].GetType() != GPL_VALUETYPE_STRING) 
					{
						match = GPL_TMISS;
						break;
					}
					fs = (char *)va_arg(*ap, char *);
					if (!fs && (char *)(values[n].GetString()))
					{
						match = GPL_CMISS;
						break;
					}
					if (fs && !(char *)((values[n].GetString())))
					{
						match = GPL_CMISS;
						break;
					}
					if (!fs && !(char *)((values[n].GetString())))
					{
						break;
					}
					if (strcmp(fs, values[n].GetString()))
					{
						match = GPL_CMISS;
						break;
					}
					break;
				case 'b':
					if (values[n].GetType() != GPL_VALUETYPE_BLOCK)
					{
						match = GPL_TMISS;
					}
					break;
				case 'B':
					if (values[n].GetType() != GPL_VALUETYPE_BLOCK) 
					{
						match = GPL_TMISS; 
						break;
					}
					fb = (char *)va_arg(*ap, char *);
					fbl = (int)va_arg(*ap, int);
					tb = (char *)(values[n].GetBlock(tbl));
					if (tbl != (unsigned long)fbl)
					{
						match = GPL_CMISS;
						break;
					}
					if (fbl == 0)
					{
						break;
					}
					if (osdMemcmp((void *)fb, (void *)tb, fbl))
					{
						match = GPL_CMISS;
					}
					break;
				case 'm':
					if (values[n].GetType() != GPL_VALUETYPE_MESSAGE)
					{
						match = GPL_TMISS;
					}
					break;
				case 'M':
					if (values[n].GetType() != GPL_VALUETYPE_MESSAGE)
					{
						match = GPL_TMISS;
						break;
					}
					m = (GPL_Message *)va_arg(*ap, char *);
					a1 = m->GetBuffer(&l1);
					a2 = values[n].GetMessage().GetBuffer(&l2);
					if (l1 != l2)
					{
						match = GPL_CMISS; 
						break;
					}
					if (osdMemcmp((void *)a1, (void *)a2, l1))
					{
						match = GPL_CMISS; 
						break;
					}
					break;
				case 'n':
				case 'N':
					if (values[n].GetType() != GPL_VALUETYPE_NULL)
					{
						match = GPL_CMISS;
					}
					break;
				default:
					gplLog("GPL WARNING", "invalid format character");
					break;
			}

			if (huntmode)
			{
				if (match == GPL_CMISS)
				{
					return FALSE;
				}
				if (match == GPL_HIT) 
				{
					huntmode = FALSE;
					break;
				}
			}
			else
			{
				if ((match == GPL_CMISS) || (match == GPL_TMISS))
				{
					return FALSE;
				}
				break;
			}
		}
	}	



	if (!huntmode)
	{
		if (n >= (long)values.GetCount())
		{
			return FALSE;
		}
	}

	return TRUE;
}
#endif

GPL_Boolean GPL_Tuple::IsEmpty(void)
{
	if (values.GetCount() == 0)
	{
		return TRUE;
	}
	return FALSE;
}

GPL_Boolean GPL_Tuple::Get(char *fmt, ...)
{
	long i;
	long cnt;
	va_list ap;

	cnt = strlen(fmt);

	va_start(ap, fmt);

	for (i = 0; (i < cnt) && (i < (long)values.GetCount());i++)
	{
		switch (fmt[i])
		{
			case 'i':
				break;
			case 'I':
				int *int_ptr;
				int_ptr = va_arg(ap, int *);
				*int_ptr = values[i].GetInteger();
				break;
			case 'r':
				break;
			case 'R':
				double *real_ptr;
				real_ptr = va_arg(ap, double *);
				*real_ptr = values[i].GetReal();
				break;
			case 's':
				break;
			case 'S':
				char **string_ptr;
				string_ptr = va_arg(ap, char **);
				*string_ptr = osdStrdup((char *)(values[i].GetString()));
				break;
			case 'b':
				break;
			case 'B':
				char **block_ptr;
				long *block_len;
				unsigned long l;l=0;
				block_ptr = va_arg(ap, char **);
				block_len = va_arg(ap, long *);
				*block_ptr = (char *)(values[i].GetBlock(l));
				*block_len = l;
				break;
			case 'm':
				break;
			case 'M':
				GPL_Message *m;
				m = (GPL_Message *)va_arg(ap, char *);
				*m = values[i].GetMessage();
				break;
			case 'n':
				break;
			default:
				break;
		}
	}

	va_end(ap);

	return TRUE; //TODO return some type of info
}


