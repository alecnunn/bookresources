
/******************************************************************************

Coral Tree osd/vsprintf.c
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

#include "osd.h"

#define OSD_VSPRINTF_CHUNK	16
#define OSD_ASSIGN(b, bs, s, c)	{if (((s) - (*(b))) >= (bs)) \
{ \
(bs) = ((s) - (*(b))); \
(*(b)) = (char *) osdRealloc ((void *) (*(b)), OSD_VSPRINTF_CHUNK + (bs)); \
(s) = (*(b)) + (bs); \
(bs) += OSD_VSPRINTF_CHUNK; \
} \
*((s)++) = (c);}

#define OSD_CHECKSTR(s) {if(*str == '\0') return FALSE;}

/* To avoid ctype(3C) */
#define is_digit(c)     ((c) >= '0' && (c) <= '9')

/***********************************************************************
int strnlen(char *s, int n)

Check no more than n characters for a null.
***********************************************************************/
#if OSD_COMPILER==OSD_KR
int strnlen(s, n)
	char *s;
	int n;
#else
int strnlen(char *s, int n)
#endif
{
	char *cp;
	int m;
	cp = s;
	m = n;
	while (*cp && m)
	{
		++cp;
		--m;
	}
	return cp - s;
}

/***********************************************************************
static int skip_atoi(char **s)

***********************************************************************/
#if OSD_COMPILER==OSD_KR
static int skip_atoi(s)
	char **s;
#else
static int skip_atoi(char **s)
#endif
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD 1	       /* pad with zero */
#define SIGN    2	       /* unsigned/signed long */
#define PLUS    4	       /* show plus */
#define SPACE   8	       /* space if plus */
#define LEFT    16	      /* left justified */
#define SPECIAL 32	      /* 0x */
#define LARGE   64	      /* use 'ABCDEF' instead of 'abcdef' */

#if 0
/******************************************************************************

******************************************************************************/
#if OSD_COMPILER==OSD_KR
static void skip_ws(s)
	char **s;
#else
static void skip_ws(char **s)
#endif
{
	while(**s == ' ' || **s == '\t' || **s == '\n') (*s)++;
}

/******************************************************************************

******************************************************************************/
#if OSD_COMPILER==OSD_KR
static long is_ws(s)
	char s;
#else
static long is_ws(char s)
#endif
{
	if(s == ' ' || s == '\t' || s == '\n') return TRUE;
	return FALSE;
}
#endif

/***********************************************************************
static char * number(char *buf, int bsize, char *str, long num,
					int base, int size, int precision, int type);

***********************************************************************/
#if OSD_COMPILER==OSD_KR
static char * number(buf, bsize, str, num, base, size, precision, type)
	char **buf;
	int bsize;
	char *str;
	long num;
	int base;
	int size;
	int precision;
	int type;
#else
static char * number(char **buf, int bsize, char *str, long num,
					int base, int size, int precision, int type)
#endif
{
	char c,sign,tmp[66];
	char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
	int i;

	if (type & LARGE)
		digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++]='0';
	else
		while (num != 0)
		{ 
			tmp[i++] = digits[(int)(((unsigned long) num) % (unsigned) base)];
			num = ((unsigned long) num) / (unsigned) base;
		}
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			OSD_ASSIGN(buf, bsize, str, ' ');
	if (sign)
		OSD_ASSIGN(buf, bsize, str, sign);
	if (type & SPECIAL)
		if (base==8)
		{
			OSD_ASSIGN(buf, bsize, str, '0');
		}
		else if (base==16) {
			OSD_ASSIGN(buf, bsize, str, '0');
			OSD_ASSIGN(buf, bsize, str, digits[33]);
		}
	if (!(type & LEFT))
		while (size-- > 0)
			OSD_ASSIGN(buf, bsize, str, c);
	while (i < precision--)
		OSD_ASSIGN(buf, bsize, str, '0');
	while (i-- > 0)
		OSD_ASSIGN(buf, bsize, str, tmp[i]);
	while (size-- > 0)
		OSD_ASSIGN(buf, bsize, str, ' ');
	return str;
}

/***********************************************************************
char * osdVSPrintf(char *fmt, va_list args)

***********************************************************************/
#if OSD_COMPILER==OSD_KR
char * osdVSPrintf(fmt, args)
	char *fmt;
	va_list args;
#else
char * osdVSPrintf(char *fmt, va_list args)
#endif
{
	int len;
	unsigned long num;
	double real;
	long exponent;
	int i;
	int base;
	char *str;
	char *s;

	/* flags to number() */
	int flags;

	/* width of output field */
	int field_width;

	/* min. # of digits for integers; */
	/* # of digits after decimal for reals; */
	/* max number of chars for string */
	int precision;
	int prec;

	/* 'h', 'l', or 'L' for numeric fields */
	int qualifier;

	/* Buffer for the string to be built. */
	char *buf;

	/* The size of the buffer. */
	int bsize;

	/* Initialize the buffer length to the length of the format string. */
	bsize = ((char *) 0 == fmt) ? 32 : strlen(fmt);
	buf = (char *) osdMalloc (bsize);

	for (str=buf ; *fmt ; ++fmt) {
		if (*fmt != '%') {
			OSD_ASSIGN(&buf, bsize, str, *fmt);
			continue;
		}
			
		/* process flags */
		flags = 0;
		repeat:
			++fmt;	  /* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}
		
		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;  
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					OSD_ASSIGN(&buf, bsize, str, ' ');
			OSD_ASSIGN(&buf, bsize, str, (unsigned char) va_arg(args, int));
			while (--field_width > 0)
				OSD_ASSIGN(&buf, bsize, str, ' ');
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
				{
					OSD_ASSIGN(&buf, bsize, str, ' ');
				}
			for (i = 0; i < len; ++i)
			{
				OSD_ASSIGN(&buf, bsize, str, *s++);
			}
			while (len < field_width--)
				OSD_ASSIGN(&buf, bsize, str, ' ');
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			str = number(&buf, bsize, str,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			continue;


		case 'n':
			if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = (str - buf);
			} else {
				int * ip = va_arg(args, int *);
				*ip = (str - buf);
			}
			continue;

		case 'E':
		case 'G':
			flags |= LARGE;
		case 'e':
		case 'f':
		case 'g':
			prec = (-1 == precision) ? 6 : precision;
			if (qualifier == 'L') {
				real = va_arg(args, long double);
			} else {
				real = va_arg(args, double);
			}
			exponent = (long) floor(log10(real));
#define	OSD_EXP	(*fmt == 'e' || *fmt == 'E' || ((*fmt == 'g' || *fmt == 'G') && (exponent < -4 || exponent >= ((-1 == precision) ? 6 : precision))))
			if (OSD_EXP) {
				num = (unsigned long) (real / pow(10.0, exponent));
			} else {
				num = (unsigned long) real;
			}
			str = number(&buf, bsize, str, num, 10, 1, -1, flags | ZEROPAD);
			if (OSD_EXP) {
				--prec;
				real = (real / pow(10.0, exponent)) - (double) num;
			} else {
				if (*fmt == 'g' || *fmt == 'G') {
					prec -= (1 > exponent ? 0 : exponent) + (0 == num ? 0 : 1);
				}
				real -= (double) num;
			}
			num = (unsigned long) osdRoundInteger(real * pow(10.0, prec));
			if (0 != precision || (flags & SPECIAL)) {
				OSD_ASSIGN(&buf, bsize, str, '.');
			}
			if (0 != precision) {
				str = number(&buf, bsize, str, num, 10,
								prec, prec, ZEROPAD);
			}
			if (OSD_EXP) {
				OSD_ASSIGN(&buf, bsize, str, (flags & LARGE ? 'E' : 'e'));
				str = number(&buf, bsize, str, exponent, 10, 2, 2, ZEROPAD | SIGN | PLUS);
			}
			continue;

		/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			if (*fmt != '%')
				OSD_ASSIGN(&buf, bsize, str, '%');
			if (*fmt)
			{
				OSD_ASSIGN(&buf, bsize, str, *fmt);
			}
			else
			{
				--fmt;
			}
			continue;
		}
		if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h')
			if (flags & SIGN)
				num = va_arg(args, short);
			else
				num = va_arg(args, unsigned short);
		else if (flags & SIGN)
			num = va_arg(args, int);
		else
			num = va_arg(args, unsigned int);
		str = number(&buf, bsize, str, num, base, field_width, precision, flags);
	}
	OSD_ASSIGN(&buf, bsize, str, '\0');
	return buf;
}


/***********************************************************************
char * osdSPrintf(char *fmt, ...)

***********************************************************************/
#if OSD_COMPILER==OSD_KR
char * osdSPrintf(fmt)
	char *fmt;
#else
char * osdSPrintf(char *fmt, ...)
#endif
{
	va_list args;
	char *s;

	va_start(args, fmt);
	s=osdVSPrintf(fmt,args);
	va_end(args);
	return s;
}

