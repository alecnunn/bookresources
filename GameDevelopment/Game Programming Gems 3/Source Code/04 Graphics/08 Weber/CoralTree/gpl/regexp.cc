/******************************************************************************

Coral Tree gpl/regexp.h
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

/******************************************************************************

This file contain altered versions of source from University of Toronto.
See following copyright.

******************************************************************************/

/******************************************************************************

Copyright (c) 1986 by University of Toronto.
Written by Henry Spencer.  Not derived from licensed software.

Permission is granted to anyone to use this software for any
purpose on any computer system, and to redistribute it freely,
subject to the following restrictions:

1. The author is not responsible for the consequences of use of
      this software, no matter how awful, even if they arise
      from defects in it.

2. The origin of this software must not be misrepresented, either
      by explicit claim or by omission.

3. Altered versions must be plainly marked as such, and must not
      be misrepresented as being the original software.

******************************************************************************/



#include <gpl/regexp.h>


#define	OP(p)	(*(p))
#define	NEXT(p)	(((*((p) + 1) & 0xFF) << 8) + (*((p) + 2) & 0xFF))
#define	OPERAND(p)	((p) + 3)
#define	ISMULT(c)	('*' == (c) || '+' == (c) || '?' == (c))

/***********************************************************************
class GPL_RegularExpression
***********************************************************************/
char GPL_RegularExpression::dummy = 0;
GPL_Byte GPL_RegularExpression::magicByte = 0234;
const char GPL_RegularExpression::metaCharacters[] = "^$.[()|?+*\\";

/***********************************************************************
GPL_RegularExpression::GPL_RegularExpression(void)
***********************************************************************/
GPL_RegularExpression::GPL_RegularExpression(void)
{
	Initialize();
}

/***********************************************************************
GPL_RegularExpression::GPL_RegularExpression(const GPL_RegularExpression & re)
***********************************************************************/
GPL_RegularExpression::GPL_RegularExpression(const GPL_RegularExpression & re)
{
	Initialize();
	SetPattern(re.originalPattern);
}

/***********************************************************************
GPL_RegularExpression::GPL_RegularExpression(const GPL_String & str)
***********************************************************************/
GPL_RegularExpression::GPL_RegularExpression(const GPL_String & str)
{
	Initialize();
	SetPattern(str);
}

/***********************************************************************
GPL_RegularExpression::~GPL_RegularExpression(void)
***********************************************************************/
GPL_RegularExpression::~GPL_RegularExpression(void)
{
	Finalize();
}

/***********************************************************************
GPL_RegularExpression & GPL_RegularExpression::operator=(const GPL_RegularExpression & re)
***********************************************************************/
GPL_RegularExpression & GPL_RegularExpression::operator=(const GPL_RegularExpression & re)
{
	Finalize();
	Initialize();
	SetPattern(re.originalPattern);
	return *this;
}

/***********************************************************************
void GPL_RegularExpression::Initialize(void)
***********************************************************************/
void GPL_RegularExpression::Initialize(void)
{
	for (register unsigned long i = 0; GPL_MAXIMUM_SUBEXPRESSIONS > i; ++i)
	{
		startp[i] = (char *) 0;
		endp[i] = (char *) 0;
	}
	regmust = (char *) 0;
	regmlen = 0;
	program = (char *) 0;
	regparse = (char *) 0;
	regcode = (char *) 0;
	regsize = 0;
	reginput = (char *) 0;
	regbol = (char *) 0;
	regstartp = (char **) 0;
	regendp = (char **) 0;
	lastError = GPL_REGULAREXPRESSIONERROR_NONE;
	isAnchored = FALSE;
	startCharacter = '\0';
	subExpressionCount = 0;
	complexity = 0;
	lastStringComplexity = 0;
}

/***********************************************************************
void GPL_RegularExpression::Finalize(void)
***********************************************************************/
void GPL_RegularExpression::Finalize(void)
{
	if ((char *) 0 != program)
	{
		osdFree(program);
	}
}

/***********************************************************************
GPL_String GPL_RegularExpression::GetPattern(void) const
***********************************************************************/
GPL_String GPL_RegularExpression::GetPattern(void) const
{
	return originalPattern;
}

/***********************************************************************
GPL_String GPL_RegularExpression::GetMatch(const unsigned long index) const
***********************************************************************/
GPL_String GPL_RegularExpression::GetMatch(const unsigned long index) const
{
	GPL_String s;
	if (GPL_MAXIMUM_SUBEXPRESSIONS > index)
	{
		s.SetData(startp[index], endp[index] - startp[index]);
	}
	return s;
}

/***********************************************************************
GPL_Boolean GPL_RegularExpression::SetPattern(const GPL_String & string)

regcomp() in Spencer's code
***********************************************************************/
GPL_Boolean GPL_RegularExpression::SetPattern(const GPL_String & string)
{
	if (0 == string.GetLength())
	{
		lastError = GPL_REGULAREXPRESSIONERROR_SYNTAX_ERROR;
		return FALSE;
	}

	Finalize();

	// First pass to determine size and legality.
	regparse = string.GetData();
	subExpressionCount = 1;
	regsize = 0L;
	regcode = &dummy;
	EmitByte(magicByte);
	Flags flags = 0;
	if (NULL == Parse(FALSE, flags))
	{
		return FALSE;
	}

	// Small enough for pointer-storage convention?
	// Probably could be 65535L.
	if (32767L <= regsize)
	{
		lastError = GPL_REGULAREXPRESSIONERROR_TOO_BIG;
		return FALSE;
	}

	// Allocate space.
	program = (char *) osdMalloc((unsigned long) regsize);
	if (NULL == program)
	{
		lastError = GPL_REGULAREXPRESSIONERROR_OUT_OF_MEMORY;
		return FALSE;
	}

	// Second pass to emit code.
	regparse = string.GetData();
	subExpressionCount = 1;
	regcode = program;
	EmitByte(magicByte);
	if (NULL == Parse(FALSE, flags))
	{
		return FALSE;
	}

	register char *longest;
	register int len;
	// Dig out information for optimizations.
	// Worst-case defaults.
	startCharacter = '\0';
	isAnchored = FALSE;
	regmust = NULL;
	regmlen = 0;

	// Start with the first branch.
	register char *scan = program + 1;

	// Only one top-level choice.
	if (OP(GetNextNode(scan)) == GPL_END)
	{	
		scan = OPERAND(scan);

		// Starting-point info.
		if (GPL_EXACTLY == OP(scan))
		{
			startCharacter = *OPERAND(scan);
		}
		else if (GPL_BOL == OP(scan))
		{
			isAnchored = TRUE;
		}

		/*
		 * If there's something expensive in the r.e., find the
		 * longest literal string that must appear and make it the
		 * regmust.  Resolve ties in favor of later strings, since
		 * the regstart check works with the beginning of the r.e.
		 * and avoiding duplication strengthens checking.  Not a
		 * strong reason, but sufficient in the absence of others.
		 */
		if (flags & GPL_META_START)
		{
			longest = NULL;
			len = 0;
			while (NULL != scan)
			{
				if (OP(scan) == GPL_EXACTLY &&
					((int) strlen(OPERAND(scan))) >= len)
				{
					longest = OPERAND(scan);
					len = strlen(OPERAND(scan));
				}
				scan = GetNextNode(scan);
			}
			regmust = longest;
			regmlen = len;
		}
	}

	originalPattern = string;
	lastError = GPL_REGULAREXPRESSIONERROR_NONE;
	return TRUE;
}

/***********************************************************************
GPL_Integer GPL_RegularExpression::Compare(const GPL_RegularExpression & re) const
***********************************************************************/
GPL_Integer GPL_RegularExpression::Compare(const GPL_RegularExpression & re) const
{
	//!! KLUDGE
	// This is probably *NOT* sufficient.
	return complexity - re.complexity;
}

/***********************************************************************
GPL_Boolean GPL_RegularExpression::Matches(const GPL_String & string,
											const unsigned long at)

regexec() in Spencer's code
***********************************************************************/
GPL_Boolean GPL_RegularExpression::Matches(const GPL_String & string,
											const unsigned long at)
{
	// Be paranoid...
	if (0 == string.GetLength())
	{
		return FALSE;
	}

	// Check if the regular expression has a pattern.
	if (GPL_REGULAREXPRESSIONERROR_NONE != lastError)
	{
		return FALSE;
	}

	// Check validity of program.
	if (magicByte != *((GPL_Byte *) program))
	{
		lastError = GPL_REGULAREXPRESSIONERROR_INTERNAL_ERROR;
		return FALSE;
	}

	/* If there is a "must appear" string, look for it. */
	register char *s = string.GetData();
	if (NULL != regmust)
	{
		while (NULL != (s = strchr(s, regmust[0])))
		{
			if (0 == strncmp(s, regmust, (unsigned long) regmlen))
			{
				// Found it.
				break;
			}
			++s;
		}
		// Not present.
		if (NULL == s)
		{
			return FALSE;
		}
	}

	// Mark beginning of line for ^.
	s = string.GetData();
	regbol = s + at;

	// Simplest case:  anchored match need be tried only once.
	if (isAnchored)
	{
		return TryMatch(s);
	}

	// Unanchored match.
	if ('\0' != startCharacter)
	{
		// For each occurence of the start character...
		while (NULL != (s = strchr(s, startCharacter)))
		{
			// If the substring here matches...
			if (TryMatch(s))
			{
				// Success!
				return TRUE;
			}
			++s;
		}
	}

	// General case.
	else
	{
		do
		{
			if (TryMatch(s))
			{
				return TRUE;
			}
		} while ('\0' != *s++);
	}

	// Failure.
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_RegularExpression::Print(GPL_String & string,
									const GPL_Verbosity verbosity) const

regdump() in Spencer's code
***********************************************************************/
GPL_Boolean GPL_RegularExpression::Print(GPL_String & string,
									const GPL_Verbosity verbosity) const
{
	if (GPL_VERBOSITY_CONCISE == verbosity)
	{
		string.Append(GetPattern());
		return TRUE;
	}

	// Arbitrary non-END op.
	register char op = GPL_EXACTLY;

	// Skip the magic byte.
	register char *s = program + 1;

	// While that wasn't END last time...
	GPL_String temp;
	while (GPL_END != op)
	{
		// Where.
		temp.Printf("%2d[", s - program);
		string.Append(temp);

		// What.
		op = OP(s);
		string.Join(",", PrintOperation(op).GetData());

		// Next.
		register char *next = GetNextNode(s);
		temp.Printf("(%d)", ((NULL == next) ? 0 : (s - program) + (next - s)));
		string.Join(",", temp);

		// Advance s.
		s += 3;

		// Literal string, where present.
		if (GPL_ANYOF == op || GPL_ANYBUT == op || GPL_EXACTLY == op)
		{
			string.Join(",", "\"");
			while ('\0' != *s)
			{
				string.Append(*s);
				++s;
			}
			++s;
			string.Append("\"");
		}
		string.Append('\n');
	}
	if ('\0' != startCharacter)
	{
		temp.Printf("start '%c' ", startCharacter);
		string.Join(",", temp);
	}
	if (isAnchored)
	{
		string.Join(",", "anchored");
	}
	if (NULL != regmust)
	{
		temp.Printf("must have \"%s\"", regmust);
		string.Join(",", temp);
	}
	return TRUE;
}

/***********************************************************************
GPL_Boolean GPL_RegularExpression::Parse(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_RegularExpression::Parse(const GPL_String & string)
{
	return SetPattern(string);
}

/***********************************************************************
GPL_String GPL_RegularExpression::PrintOperation(char *op)

regprop() in Spencer's code
***********************************************************************/
GPL_String GPL_RegularExpression::PrintOperation(GPL_Byte op)
{
	GPL_String image;
	switch (op) {
	case GPL_BOL:
		image = "GPL_BOL";
		break;
	case GPL_EOL:
		image = "GPL_EOL";
		break;
	case GPL_ANY:
		image = "GPL_ANY";
		break;
	case GPL_ANYOF:
		image = "GPL_ANYOF";
		break;
	case GPL_ANYBUT:
		image = "GPL_ANYBUT";
		break;
	case GPL_BRANCH:
		image = "GPL_BRANCH";
		break;
	case GPL_EXACTLY:
		image = "GPL_EXACTLY";
		break;
	case GPL_NOTHING:
		image = "GPL_NOTHING";
		break;
	case GPL_BACK:
		image = "GPL_BACK";
		break;
	case GPL_END:
		image = "GPL_END";
		break;
	case GPL_OPEN + 1:
	case GPL_OPEN + 2:
	case GPL_OPEN + 3:
	case GPL_OPEN + 4:
	case GPL_OPEN + 5:
	case GPL_OPEN + 6:
	case GPL_OPEN + 7:
	case GPL_OPEN + 8:
	case GPL_OPEN + 9:
		image.Printf("GPL_OPEN%d", op - GPL_OPEN);
		break;
	case GPL_CLOSE + 1:
	case GPL_CLOSE + 2:
	case GPL_CLOSE + 3:
	case GPL_CLOSE + 4:
	case GPL_CLOSE + 5:
	case GPL_CLOSE + 6:
	case GPL_CLOSE + 7:
	case GPL_CLOSE + 8:
	case GPL_CLOSE + 9:
		image.Printf("GPL_CLOSE%d", op - GPL_CLOSE);
		break;
	case GPL_STAR:
		image = "GPL_STAR";
		break;
	case GPL_PLUS:
		image = "GPL_PLUS";
		break;
	default:
		image = "UNKNOWN";
		break;
	}
	return image;
}

/***********************************************************************
void GPL_RegularExpression::IncreaseComplexity(const unsigned long offset,
												const unsigned long range,
												const unsigned long value);

complexity	bits  0..31
----------	-----------
GPL_STAR,	bits 30..31
GPL_PLUS,
\?
GPL_ANY,	bits 22..29
GPL_ANYOF,
GPL_ANYBUT
GPL_EOL		bits 21..21
GPL_BOL		bits 20..20
GPL_OPEN	bits 16..19
GPL_EXACTLY	bits  0..15
***********************************************************************/
void GPL_RegularExpression::IncreaseComplexity(const unsigned long offset,
												const unsigned long range,
												const unsigned long value)
{
	register const unsigned long mask =
		0xFFFFFFFF >> (OSD_BITS_PER_WORD - range);

	// If the current value is less than the new value...
	if (((complexity >> offset) & mask) < (value & mask))
	{
		// Update the complexity.
		complexity = (complexity & (~(mask << offset))) |
						((value & mask) << offset);
	}
}

/***********************************************************************
char *GPL_RegularExpression::Parse(const GPL_Boolean parenthesized,
									Flags & flags)

reg() in Spencer's code
***********************************************************************/
char *GPL_RegularExpression::Parse(const GPL_Boolean parenthesized,
									Flags & flags)
{
	// Tentatively.
	flags = GPL_HAS_WIDTH;

	// Make an GPL_OPEN node, if parenthesized.
	register char *node = NULL;
	register GPL_Integer subExpressionIndex = 0;
	if (parenthesized)
	{
		if (subExpressionCount >= GPL_MAXIMUM_SUBEXPRESSIONS)
		{
			lastError = GPL_REGULAREXPRESSIONERROR_TOO_BIG;
			return NULL;
		}
		subExpressionIndex = subExpressionCount;
		++subExpressionCount;
		node = EmitNode(OSD_HELP_CAST(const GPL_Byte) (GPL_OPEN + subExpressionIndex));
		IncreaseComplexity(16, 4, subExpressionIndex);
	}

	// Pick up the branches, linking them together.
	Flags f = 0;
	register char *branch = ParseBranch(f);
	if (NULL == branch)
	{
		return NULL;
	}
	if (NULL != node)
	{
		// GPL_OPEN -> first.
		SetNextPointer(node, branch);
	}
	else
	{
		node = branch;
	}

	if (!(f & GPL_HAS_WIDTH))
	{
		flags &= ~GPL_HAS_WIDTH;
	}
	flags |= f & GPL_META_START;

	while ('|' == *regparse)
	{
		++regparse;
		branch = ParseBranch(f);
		if (NULL == branch)
		{
			return NULL;
		}

		// GPL_BRANCH -> GPL_BRANCH.
		SetNextPointer(node, branch);

		if (!(f & GPL_HAS_WIDTH))
		{
			flags &= ~GPL_HAS_WIDTH;
		}
		flags |= f & GPL_META_START;
	}

	// Make a closing node, and hook it on the end.
	register char *ender =
		EmitNode((parenthesized) ? (OSD_HELP_CAST(const GPL_Byte) (GPL_CLOSE + subExpressionIndex)) : GPL_END);	
	SetNextPointer(node, ender);

	// Hook the tails of the branches to the closing node.
	for (branch = node; NULL != branch; branch = GetNextNode(branch))
	{
		SetOperandfulNextPointer(branch, ender);
	}

	// Check for proper termination.
	if (parenthesized && ')' != *regparse++)
	{
		lastError = GPL_REGULAREXPRESSIONERROR_UNBALANCED;
		return NULL;
	}
	else if (!parenthesized && *regparse != '\0')
	{
		if (*regparse == ')')
		{
			lastError = GPL_REGULAREXPRESSIONERROR_UNBALANCED;
			return NULL;
		}
		else
		{
			lastError = GPL_REGULAREXPRESSIONERROR_UNBALANCED;
			return NULL;
		}
	}

	return node;
}

/***********************************************************************
char *GPL_RegularExpression::ParseBranch(Flags & flags)

regbranch() in Spencer's code
***********************************************************************/
char *GPL_RegularExpression::ParseBranch(Flags & flags)
{
	// Tentatively.
	flags = GPL_WORST_CASE;

	register char *node = EmitNode(GPL_BRANCH);
	Flags f = 0;
	register char *chain = NULL;
	register char *latest = NULL;
	while ('\0' != *regparse && '|' != *regparse && ')' != *regparse)
	{
		latest = ParsePiece(f);
		if (NULL == latest)
		{
			return NULL;
		}
		flags |= f & GPL_HAS_WIDTH;
		// If the first piece...
		if (NULL == chain)
		{
			flags |= f & GPL_META_START;
		}
		else
		{
			SetNextPointer(chain, latest);
		}
		chain = latest;
	}
	// Loop ran zero times.
	if (NULL == chain)
	{
		EmitNode(GPL_NOTHING);
	}

	return node;
}

/***********************************************************************
char *GPL_RegularExpression::ParsePiece(Flags & flags)

regpiece() in Spencer's code
***********************************************************************/
char *GPL_RegularExpression::ParsePiece(Flags & flags)
{
	Flags f = 0;
	register char *node = ParseAtom(f);
	if (NULL == node)
	{
		return NULL;
	}

	register char op = *regparse;
	if (!ISMULT(op))
	{
		flags = f;
		return node;
	}

	if (!(f & GPL_HAS_WIDTH) && '?' != op)
	{
		lastError = GPL_REGULAREXPRESSIONERROR_EMPTY_REPETITION;
		return NULL;
	}

	flags = ('+' != op) ? (GPL_WORST_CASE | GPL_META_START) : (GPL_WORST_CASE | GPL_HAS_WIDTH);

	if ('*' == op && (f & GPL_SIMPLE))
	{
		InsertNode(GPL_STAR, node);
	}
	else if ('*' == op)
	{
		// Emit x* as (x&|), where & means "self". 
		// Either x...
		InsertNode(GPL_BRANCH, node);
		// and loop...
		SetOperandfulNextPointer(node, EmitNode(GPL_BACK));
		// back...
		SetOperandfulNextPointer(node, node);
		// or...
		SetNextPointer(node, EmitNode(GPL_BRANCH));
		// null.
		SetNextPointer(node, EmitNode(GPL_NOTHING));
		IncreaseComplexity(30, 2, 3);
	}
	else if ('+' == op && (f & GPL_SIMPLE))
	{
		InsertNode(GPL_PLUS, node);
	}
	else if ('+' == op)
	{
		// Emit x+ as x(&|), where & means "self".
		// Either...
		register char *next = EmitNode(GPL_BRANCH);
		SetNextPointer(node, next);
		// loop back...
		SetNextPointer(EmitNode(GPL_BACK), node);
		// or...
		SetNextPointer(next, EmitNode(GPL_BRANCH));
		// null.
		SetNextPointer(node, EmitNode(GPL_NOTHING));
		IncreaseComplexity(30, 2, 2);
	}
	else if ('?' == op)
	{
		// Emit x? as (x|)
		// Either x...
		InsertNode(GPL_BRANCH, node);
		// or...
		SetNextPointer(node, EmitNode(GPL_BRANCH));
		// null.
		register char *next = EmitNode(GPL_NOTHING);
		SetNextPointer(node, next);
		SetOperandfulNextPointer(node, next);
		IncreaseComplexity(30, 2, 1);
	}
	++regparse;
	if (ISMULT(*regparse))
	{
		lastError = GPL_REGULAREXPRESSIONERROR_NESTED_REPETITION;
		return NULL;
	}

	return node;
}

/***********************************************************************
char *GPL_RegularExpression::ParseAtom(Flags & flags)

regatom() in Spencer's code
***********************************************************************/
char *GPL_RegularExpression::ParseAtom(Flags & flags)
{
	// Tentatively.
	flags = GPL_WORST_CASE;

	Flags f = 0;
	register char *node = NULL;
	switch (*regparse++) {
	case '^':
		node = EmitNode(GPL_BOL);
		IncreaseComplexity(20, 20, 1);
		break;

	case '$':
		node = EmitNode(GPL_EOL);
		IncreaseComplexity(21, 21, 1);
		break;

	case '.':
		flags |= GPL_HAS_WIDTH | GPL_SIMPLE;
		node = EmitNode(GPL_ANY);
		IncreaseComplexity(22, 8, 255);
		break;

	case '[':
		// Complement of range.
		if ('^' == *regparse)
		{
			node = EmitNode(GPL_ANYBUT);
			++regparse;
		}
		else
		{
			node = EmitNode(GPL_ANYOF);
		}
		if (']' == *regparse || '-' == *regparse)
		{
			EmitByte(*regparse++);
		}
		while ('\0' != *regparse && ']' != *regparse)
		{
			if ('-' == *regparse)
			{
				++regparse;
				if (']' == *regparse || '\0' == *regparse)
				{
					EmitByte('-');
				}
				else
				{
					register char from = *((GPL_Byte *) (regparse - 2)) + 1;
					register char to = *((GPL_Byte *) regparse);
					if (from > to + 1)
					{
						lastError = GPL_REGULAREXPRESSIONERROR_INVALID_RANGE;
						return NULL;
					}
					while (from <= to)
					{
						EmitByte((char) from);
						++from;
					}
					++regparse;
				}
			}
			else
			{
				EmitByte(*regparse++);
			}
		}
		EmitByte('\0');
		IncreaseComplexity(22, 8, strlen(OPERAND(node)));
		if (']' == *regparse)
		{
			++regparse;
			flags |= GPL_HAS_WIDTH | GPL_SIMPLE;
		}
		else
		{
			lastError = GPL_REGULAREXPRESSIONERROR_UNBALANCED;
		}
		break;

	case '(':
		node = Parse(TRUE, f);
		if (NULL != node)
		{
			flags |= f & (GPL_HAS_WIDTH | GPL_META_START);
		}
		break;

	case '\0':
	case '|':
	case ')':
		lastError = GPL_REGULAREXPRESSIONERROR_INTERNAL_ERROR;
		break;

	case '?':
	case '+':
	case '*':
		lastError = GPL_REGULAREXPRESSIONERROR_EMPTY_REPETITION;
		break;

	case '\\':
		if ('\0' != *regparse)
		{
			node = EmitNode(GPL_EXACTLY);
			EmitByte(*regparse++);
			EmitByte('\0');
			IncreaseComplexity(0, 16,
				((0 == lastStringComplexity) ? 0 : lastStringComplexity + 1));
			++lastStringComplexity;
			flags |= GPL_HAS_WIDTH | GPL_SIMPLE;
		}
		else
		{
			lastError = GPL_REGULAREXPRESSIONERROR_TRAILING_ESCAPE;
		}
		break;

	default:
		{
			--regparse;
			register GPL_Integer len = strcspn(regparse, metaCharacters);
			if (0 >= len)
			{
				lastError = GPL_REGULAREXPRESSIONERROR_INTERNAL_ERROR;
			}
			else
			{
				register char ender = *(regparse + len);
				if (1 < len && ISMULT(ender))
				{
					/* Back off clear of ?+* operand. */
					--len;
				}
				flags |= GPL_HAS_WIDTH;
				if (1 == len)
				{
					flags |= GPL_SIMPLE;
				}
				node = EmitNode(GPL_EXACTLY);
				while (0 < len)
				{
					EmitByte(*regparse++);
					--len;
				}
				EmitByte('\0');
				lastStringComplexity *= 1 + strlen(OPERAND(node));
				IncreaseComplexity(0, 16, lastStringComplexity);
				lastStringComplexity +=
					(0 == lastStringComplexity) ? 1 + strlen(OPERAND(node)) : 0;
			}
		}
		break;
	}

	return node;
}

/***********************************************************************
GPL_Boolean GPL_RegularExpression::TryMatch(char *string)

regtry() in Spencer's code
***********************************************************************/
GPL_Boolean GPL_RegularExpression::TryMatch(char *string)
{
	// Initialize member data.
	reginput = string;
	regstartp = startp;
	regendp = endp;

	// Initialize local data.
	register char **sp = startp;
	register char **ep = endp;
	for (register GPL_Integer i = GPL_MAXIMUM_SUBEXPRESSIONS; 0 < i; --i)
	{
		*sp++ = NULL;
		*ep++ = NULL;
	}

	// If the match succeeds...
	if (Match(program + 1))
	{
		// Save the end-points.
		startp[0] = string;
		endp[0] = reginput;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/***********************************************************************
GPL_Boolean GPL_RegularExpression::Match(char *prog)

regmatch() in Spencer's code
***********************************************************************/
GPL_Boolean GPL_RegularExpression::Match(char *prog)
{
	register char *scan = prog;
	register char *next = NULL;
	while (NULL != scan)
	{
		next = GetNextNode(scan);

		switch (OP(scan))
		{
		case GPL_BOL:
			if (reginput != regbol)
			{
				return FALSE;
			}
			break;

		case GPL_EOL:
			if ('\0' != *reginput)
			{
				return FALSE;
			}
			break;

		case GPL_ANY:
			if ('\0' == *reginput)
			{
				return FALSE;
			}
			++reginput;
			break;

		case GPL_EXACTLY:
			{
				// Inline the first character, for speed.
				register char *opnd = OPERAND(scan);
				if (*opnd != *reginput)
				{
					return FALSE;
				}
				register GPL_Integer len = strlen(opnd);
				if (1 < len && 0 != strncmp(opnd, reginput, (size_t) len))
				{
					return FALSE;
				}
				reginput += len;
			}
			break;

		case GPL_ANYOF:
			if ('\0' == *reginput || NULL == strchr(OPERAND(scan), *reginput))
			{
				return FALSE;
			}
			++reginput;
			break;

		case GPL_ANYBUT:
			if ('\0' == *reginput || NULL != strchr(OPERAND(scan), *reginput))
			{
				return FALSE;
			}
			++reginput;
			break;

		case GPL_NOTHING:
			break;

		case GPL_BACK:
			break;

		case GPL_BRANCH:
			// No choice.
			if (GPL_BRANCH != OP(next))
			{
				// Avoid recursion.
				next = OPERAND(scan);
			}
			else
			{
				register char *save;
				do
				{
					save = reginput;
					if (Match(OPERAND(scan)))
					{
						return TRUE;
					}
					reginput = save;
					scan = GetNextNode(scan);
				}
				while (NULL != scan && GPL_BRANCH == OP(scan));
				return FALSE;
			}
			break;

		case GPL_STAR:
		case GPL_PLUS:
			{
				// Lookahead to avoid useless match attempts
				// when we know what character comes next.
				register char nextch = '\0';
				if (GPL_EXACTLY == OP(next))
				{
					nextch = *OPERAND(next);
				}
				register GPL_Integer min = (GPL_STAR == OP(scan)) ? 0 : 1;
				register char *save = reginput;
				register GPL_Integer no = CountRepetitions(OPERAND(scan));
				while (no >= min)
				{
					// If it could work...
					if ('\0' == nextch || *reginput == nextch)
					{
						// Try it.
						if (Match(next))
						{
							return TRUE;
						}
					}
					// Couldn't or didn't -- back up.
					--no;
					reginput = save + no;
				}
				return FALSE;
			}
			//break; //unreachable

		case GPL_END:
			return TRUE;

		// Catches all GPL_OPENs and GPL_CLOSEs.
		default:
			if (GPL_OPEN < OP(scan) && GPL_OPEN + GPL_MAXIMUM_SUBEXPRESSIONS > OP(scan))
			{
				register GPL_Integer no = 0;
				register char *save = NULL;
				no = OP(scan) - GPL_OPEN;
				save = reginput;
				if (Match(next))
				{
					// Don't set startp if some later invocation
					// of the same parentheses already has.

					if (NULL == regstartp[no])
					{
						regstartp[no] = save;
					}
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			else if (GPL_CLOSE < OP(scan) && GPL_CLOSE + GPL_MAXIMUM_SUBEXPRESSIONS > OP(scan))
			{
				register GPL_Integer no = 0;
				register char *save = NULL;
				no = OP(scan) - GPL_CLOSE;
				save = reginput;
				if (Match(next))
				{
					// Don't set endp if some later invocation
					// of the same parentheses already has.
					if (NULL == regendp[no])
					{
						regendp[no] = save;
					}
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			lastError = GPL_REGULAREXPRESSIONERROR_INTERNAL_ERROR;
			return FALSE;
		}

		scan = next;
	}

	// We get here only if there's trouble -- normally "case GPL_END" is
	// the terminating point.
	lastError = GPL_REGULAREXPRESSIONERROR_INTERNAL_ERROR;
	return FALSE;
}

/***********************************************************************
GPL_Integer GPL_RegularExpression::CountRepetitions(char *p)

regrepeat() in Spencer's code
***********************************************************************/
GPL_Integer GPL_RegularExpression::CountRepetitions(char *p)
{
	register GPL_Integer count = 0;
	register char *scan = reginput;
	register char *opnd = OPERAND(p);
	switch (OP(p)) {
	case GPL_ANY:
		count = strlen(scan);
		scan += count;
		break;

	case GPL_EXACTLY:
		while (*opnd == *scan)
		{
			++count;
			++scan;
		}
		break;

	case GPL_ANYOF:
		while ('\0' != *scan && NULL != strchr(opnd, *scan))
		{
			++count;
			++scan;
		}
		break;

	case GPL_ANYBUT:
		while ('\0' != *scan && NULL == strchr(opnd, *scan))
		{
			++count;
			++scan;
		}
		break;

	default:
		lastError = GPL_REGULAREXPRESSIONERROR_INTERNAL_ERROR;
		// Best compromise.
		count = 0;
		break;
	}
	reginput = scan;

	return count;
}

/***********************************************************************
void GPL_RegularExpression::EmitByte(const GPL_Byte b)

regc() in Spencer's code
***********************************************************************/
void GPL_RegularExpression::EmitByte(const GPL_Byte b)
{
	if (regcode != &dummy)
	{
		*regcode++ = (char) b;
	}
	else
	{
		++regsize;
	}
}

/***********************************************************************
char *GPL_RegularExpression::EmitNode(const GPL_Byte op)

regnode() in Spencer's code
***********************************************************************/
char *GPL_RegularExpression::EmitNode(const GPL_Byte op)
{
	register char *ret = regcode;
	if (ret == &dummy)
	{
		regsize += 3;
		return ret;
	}

	// Emit the operator.
	register char *ptr = ret;
	*ptr++ = (char) op;

	// Emit a null "next" pointer.
	*ptr++ = '\0';
	*ptr++ = '\0';

	// Update the code pointer.
	regcode = ptr;

	return ret;
}

/***********************************************************************
void GPL_RegularExpression::InsertNode(const GPL_Byte op, char *opnd)

reginsert() in Spencer's code
***********************************************************************/
void GPL_RegularExpression::InsertNode(const GPL_Byte op, char *opnd)
{
	if (regcode == &dummy)
	{
		regsize += 3;
	}
	else
	{
		register char *src = regcode;
		regcode += 3;
		register char *dst = regcode;
		while (src > opnd)
		{
			*--dst = *--src;
		}

		// Op node, where operand used to be.
		register char *place = opnd;
		*place++ = (char) op;
		*place++ = '\0';
		*place = '\0';
	}
}

/***********************************************************************
char *GPL_RegularExpression::GetNextNode(register char *p)

regnext() in Spencer's code
***********************************************************************/
char *GPL_RegularExpression::GetNextNode(register char *p)
{
	if (p == &dummy)
	{
		return NULL;
	}

	register GPL_Integer offset = NEXT(p);
	if (0 == offset)
	{
		return NULL;
	}

	if (GPL_BACK == OP(p))
	{
		return p - offset;
	}
	else
	{
		return p + offset;
	}
}

/***********************************************************************
void GPL_RegularExpression::SetOperandfulNextPointer(char *p, char *val)

regoptail() in Spencer's code
***********************************************************************/
void GPL_RegularExpression::SetOperandfulNextPointer(char *p, char *val)
{
	// "Operandless" and "op != GPL_BRANCH" are synonymous in practice.
	if (NULL != p && p != &dummy && GPL_BRANCH == OP(p))
	{
		SetNextPointer(OPERAND(p), val);
	}
}

/***********************************************************************
void GPL_RegularExpression::SetNextPointer(char *p, char *val)

regtail() in Spencer's code
***********************************************************************/
void GPL_RegularExpression::SetNextPointer(char *p, char *val)
{
	if (p != &dummy)
	{
		// Find last node.
		register char *scan = p;
		register char *temp = NULL;
		while (TRUE)
		{
			temp = GetNextNode(scan);
			if (NULL == temp)
			{
				break;
			}
			scan = temp;
		}

		// Determine the offset.
		register GPL_Integer offset = 0;
		if (GPL_BACK == OP(scan))
		{
			offset = scan - val;
		}
		else
		{
			offset = val - scan;
		}

		// Set the pointer.
		*(scan + 1) = (char) ((offset >> 8) & 0377);
		*(scan + 2) = (char) (offset & 0377);
	}
}

