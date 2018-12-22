
/******************************************************************************

Coral Tree osd/vsscanf.c
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

#include <osd/osd.h>

/* Those are flags in the conversion format. */
# define LONG		0x001	/* l: long or double */
# define LONGDBL	0x002	/* L: long long or long double */
# define SHORT		0x004	/* h: short */
# define SUPPRESS	0x008	/* *: suppress assignment */
# define POINTER	0x010	/* weird %p pointer (`fake hex') */
# define NOSKIP		0x020	/* do not skip blanks */
# define WIDTH		0x040	/* width was given */
# define GROUP		0x080	/* ': group numbers */
# define MALLOC		0x100	/* a: malloc strings */

# define TYPEMOD	(LONG|LONGDBL|SHORT)

#define MAXNUMLEN 64

#define nextchar() (read_in++, *input++)
#define backup(x) input--

#define silent_error() {return done;}
#define conv_error(x) {fprintf(stderr,"osdVSScanf: conversion error %d\n",x);return done;}
#define input_error() {fprintf(stderr,"osdVSScanf: input error\n");return done;}
#define unimplemented_error() {fprintf(stderr,"osdVSScanf: unimplemented feature error\n");return done;}
#define numbersize_error() {fprintf(stderr,"osdVSScanf: number too long\n");return done;}

#if OSD_COMPILER==OSD_KR
long osdIsSpace(c)
	int c;
#else
long osdIsSpace(int c)
#endif
{
	if(	c == '\f'	||
		c == '\n'	||
		c == '\r'	||
		c == '\t'	||
		c == '\v'	||
		c == ' '	)	return TRUE;
	return FALSE;
}

#if OSD_COMPILER==OSD_KR
long osdIsDigit(c)
	int c;
#else
long osdIsDigit(int c)
#endif
{
	if( c >= '0' && c <= '9' ) return TRUE;
	return FALSE;
}

#if OSD_COMPILER==OSD_KR
long osdIsXDigit(c)
	int c;
#else
long osdIsXDigit(int c)
#endif
{
	if( c >= '0' && c <= '9' ) return TRUE;
	if( c >= 'A' && c <= 'F' ) return TRUE;
	if( c >= 'a' && c <= 'f' ) return TRUE;
	return FALSE;
}

#if OSD_COMPILER==OSD_KR
void scroll_forward(ptrarg,pos,gpos)
	va_list *ptrarg;
	unsigned int pos;
	unsigned int *gpos;
#else
void scroll_forward(va_list *ptrarg, unsigned int pos, unsigned int *gpos)
#endif
{
	if(pos == 0) 
	{
		pos = ++(*gpos);
	}
	while(--pos > 0) 
	{
		(void)va_arg(*ptrarg, void *);
	}
}

#if OSD_COMPILER==OSD_KR
long osdSScanf(input, fmt)
	char *input;
	char *fmt;
#else
long osdSScanf(char *input, char *fmt,...)
#endif
{
	va_list args;
	size_t s;

	va_start(args, fmt);
	s = osdVSScanf(input, fmt, args);
	va_end(args);

	return s;
}


#if OSD_COMPILER==OSD_KR
long osdVSScanf(input, fmt, argptr)
	char *input;
	char *fmt;
	va_list argptr;
#else
long osdVSScanf(char *input, char *fmt, va_list argptr)
#endif
{
	va_list arg;
	register char *f=fmt;
	register size_t read_in = 0;
	register unsigned char fc;
	int skip_space = 0;
	register int c = 0;
	register size_t done = 0;
	register int flags;
	register int width;
	char *str = NULL;
	/*wchar_t *wstr = NULL;*/
	/*char **strptr = NULL;*/
	/*size_t strsize = 0;*/
	unsigned int pos;
	unsigned int gpos = 0;

	char got_dot, got_e, negative;
	int base;
	int number_signed;
	wchar_t decimal;
	wchar_t thousands;

	union
	{
		long int l;
		unsigned long int ul;
	} num;

	char *tw;
	char wp[MAXNUMLEN];
	size_t wpmax = MAXNUMLEN;
	size_t wpsize;

#define ADDW(Ch)														\
	do{																	\
		if(wpsize == wpmax)												\
		{																\
			numbersize_error();											\
		}																\
		wp[wpsize++] = (Ch);											\
	} while(0)

#define ARG(type) (														\
					pos = argpos,										\
					arg = argptr, 							\
					scroll_forward(&arg, pos, &gpos),					\
					va_arg (arg, type)									\
				)

	thousands = ',';
	decimal = '.';

	while(*f != '\0')
	{
		unsigned int argpos;
		fc = *f++;
		if(fc != '%')
		{
			if(osdIsSpace(fc))
			{
				skip_space = 1;
				continue;
			}

			c = nextchar();

			if(skip_space)
			{
				while(osdIsSpace(c));

				skip_space = 0;
			}

			if(c != fc)
			{
				backup(c);
				conv_error(1);
			}

			continue;
		}

		flags = 0;
		argpos=0;
		wpsize=0;

		if(osdIsDigit(*f))
		{
			argpos = *f++ - '0';
			while(osdIsDigit(*f)) argpos = argpos * 10 + (*f++ - '0');
			if(*f == '$') ++f;
			else
			{
				width = argpos;
				flags |= WIDTH;
				argpos = 0;
				goto got_width;
			}
		}

		while(*f == '*' || *f == '\'') switch(*f++)
		{
			case '*':
				flags |= SUPPRESS;
				break;
			case '\'':
				flags |= GROUP;
				break;
		}

		if(osdIsDigit(*f)) flags |= WIDTH;

		width = 0;
		while(osdIsDigit(*f))
		{
			width *= 10;
			width += *f++ - '0';
		}
		got_width:
		if(width == 0) width = -1;

		while(*f == 'h' || *f == 'l' || *f == 'L' || *f == 'a' || *f == 'q')
			switch(*f++)
			{
				case 'h':
					if (flags & TYPEMOD) {conv_error(2);}
					flags |= SHORT;
					break;
				case 'l':
					if (flags & (SHORT|LONGDBL)) {conv_error(3);}
					else if (flags & LONG)
					{
						flags &= ~LONG;
						flags |= LONGDBL;
					}
					else flags |= LONG;
					break;
				case 'q':
				case 'L':
					if (flags & TYPEMOD) {conv_error(4);}
					flags |= LONGDBL;
					break;
				case 'a':
					if (flags & TYPEMOD) {conv_error(5);}
					flags |= MALLOC;
					break;
			}

		if (*f == '\0') {conv_error (6);}

		fc = *f++;
		if (skip_space || (fc != '[' && fc != 'c' && fc != 'C' && fc != 'n'))
		{
			do 
			{
				if((c = nextchar ()) == 0) {input_error();}
			} while(osdIsSpace(c));
			backup(c);
			skip_space = 0;
		}

		switch(fc)
		{
			case '%':
				c = nextchar();
				if(c != fc)
				{
					backup(c);
					conv_error(7);
				}
				break;
			case 'n':
				if (!(flags & SUPPRESS))
				{
					if (flags & LONG) 
						*ARG (long int *) = read_in;
					else if (flags & SHORT) 
						*ARG (short int *) = read_in;
					else 
						*ARG (int *) = read_in;
					++done;
				}
				break;
			case 'c':
				if ((flags & LONG) == 0)
				{
					if (!(flags & SUPPRESS))
					{
						str = ARG (char *);
						if (str == NULL) 
						{
							conv_error (8);
						}
					}

					c = nextchar();
					if (c == EOF) {input_error();}

					if (width == -1) width = 1;

					if (!(flags & SUPPRESS))
					{
						do *str++ = c;
						while (--width > 0 && (c = nextchar ()) != EOF);
					}
					else while (--width > 0 && (c = nextchar ()) != EOF);

					if (width > 0) --read_in;

					if (!(flags & SUPPRESS)) ++done;

					break;
				}
			case 'C':
				unimplemented_error();
				break;
			case 's':
				if (flags & LONG) goto wide_char_string;
				if (!(flags & SUPPRESS))
				{
					str = ARG(char *);
				}
				c = nextchar();
				do
				{
					if(osdIsSpace(c))
					{
						backup(c);
						break;
					}
					if (!(flags & SUPPRESS)) *str++ = c;
				} while ((width <= 0 || --width > 0) && (c = nextchar()));
				if (!(flags & SUPPRESS))
				{
					*str = '\0';
					++done;
				}
				break;
			case 'S':
				wide_char_string:
				unimplemented_error();
				break;
			case 'x':
			case 'X':
				base = 16;
				number_signed = 0;
				goto number;
			case 'o':
				base = 8;
				number_signed = 0;
				goto number;
			case 'u':
				base = 10;
				number_signed = 0;
				goto number;
			case 'd':
				base = 10;
				number_signed = 1;
				goto number;
			case 'i':
				base = 0;
				number_signed = 1;

				number:
				c = nextchar();

				if(c == 0) input_error();

				if (c == '-' || c == '+')
				{
					ADDW(c);
					if(width > 0) --width;
					c = nextchar();
				}

				if (width != 0 && c == '0')
				{
					if(width > 0) --width;

					ADDW(c);
					c = nextchar();

					if (width != 0 && (c == 'x' || c == 'X'))
					{
						if(base == 0) base = 16;
						if(base == 16)
						{
							if(width > 0) --width;
							c = nextchar();
						}
					}
					else if(base == 0) base = 8;
				}

				if(base == 0) base = 10;

				while(c != 0 && width != 0)
				{
					if(base == 16 ? !osdIsXDigit(c) :
						((!osdIsDigit(c) || c - '0' >= base) &&
						!((flags & GROUP) && base == 10 && c == thousands)))
						break;
					ADDW(c);
					if(width > 0) --width;

					c = nextchar();
				}

				backup(c);

				if(wpsize == 0 ||
					(wpsize == 1 && (wp[0] == '+' || wp[0] == '-')))
					conv_error(9);

				ADDW('\0');

				if(flags & LONGDBL)
				{
					unimplemented_error();
				}
				else
				{
					if(number_signed) num.l = strtol(wp,&tw,base);
					else num.ul = strtoul(wp,&tw,base);
				}
				if(wp==tw) conv_error(10);

				if(!(flags & SUPPRESS))
				{
					if(!number_signed)
					{
						if(flags & LONG) 
							*ARG (unsigned long int *) = num.ul;
						else if(flags & SHORT)
							*ARG (unsigned short int *) = 
							(unsigned short int) num.ul;
						else
							*ARG (unsigned int *) = (unsigned int) num.ul;
					}
					else
					{
						if (flags & LONG)
							*ARG (long int *) = num.l;
						else if (flags & SHORT)
							*ARG (short int *) = (short int) num.l;
						else
							*ARG (int *) = (int) num.l;
					}
					++done;
				}
				break;
			case 'e':
			case 'E':
			case 'f':
			case 'g':
			case 'G':
				c = nextchar();
				if(c == 0) input_error();

				if (c == '-' || c == '+')
				{
					negative = c == '-';
					if( (c = nextchar()) == 0 ) conv_error(11);
					if(width > 0) --width;
				}
				else negative = 0;

				got_dot = got_e = 0;
				do
				{
					if(osdIsDigit(c)) ADDW(c);
					else if(got_e && wp[wpsize - 1] == 'e'
						&& (c == '-' || c == '+')) ADDW(c);
					else if (wpsize > 0 && !got_e && (c == 'e' || c == 'E'))
					{
						ADDW('e');
						got_e = got_dot = 1;
					}
					else if (c == decimal && !got_dot)
					{
						ADDW(c);
						got_dot = 1;
					}
					else if ((flags & GROUP) && c == thousands && !got_dot)
						ADDW (c);
					else
					{
						backup(c);
						break;
					}
				} while(width != 0 && (c = nextchar ()) != 0);

/*				if (wpsize == 0) conv_error (12); */
				if (wpsize == 0) silent_error ();

				ADDW ('\0');
				if (flags & LONGDBL)
				{
					unimplemented_error();
				}
				else if(flags & LONG)
				{
					double d = strtod(wp, &tw);
					if (!(flags & SUPPRESS) && tw != wp)
						*ARG (double *) = negative ? -d : d;
				}
				else
				{
					float d = (float)strtod(wp, &tw);
					if (!(flags & SUPPRESS) && tw != wp)
						*ARG (float *) = negative ? -d : d;
				}

				if (tw == wp) conv_error (13);

				if (!(flags & SUPPRESS)) ++done;

				break;

			case '[':
				unimplemented_error();
				break;
			case 'p':
				unimplemented_error();
				break;
		}
	}

	if (skip_space)
	{
		do c = nextchar();
		while (osdIsSpace(c));
		backup(c);
	}
	return done;
}

