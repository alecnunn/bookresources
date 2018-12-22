#ifndef GPL_REGEXP_H
#define GPL_REGEXP_H

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

#include <gpl/str.h>
#include <gpl/depict.h>

// GPL_MAXIMUM_SUBEXPRESSIONS must be at least 10, and no greater than 117
// or the parser will not work properly because the operation will not
// fit in eight bits.
#define GPL_MAXIMUM_SUBEXPRESSIONS	10

/***********************************************************************
enum GPL_RegularExpressionError
***********************************************************************/
enum GPL_RegularExpressionError
{
	GPL_REGULAREXPRESSIONERROR_NONE					=	0,
	GPL_REGULAREXPRESSIONERROR_SYNTAX_ERROR			=	1,
	GPL_REGULAREXPRESSIONERROR_TOO_BIG				=	2,
	GPL_REGULAREXPRESSIONERROR_OUT_OF_MEMORY		=	3,
	GPL_REGULAREXPRESSIONERROR_UNBALANCED			=	4,
	GPL_REGULAREXPRESSIONERROR_EMPTY_REPETITION		=	5,
	GPL_REGULAREXPRESSIONERROR_NESTED_REPETITION	=	6,
	GPL_REGULAREXPRESSIONERROR_INVALID_RANGE		=	7,
	GPL_REGULAREXPRESSIONERROR_TRAILING_ESCAPE		=	8,
	GPL_REGULAREXPRESSIONERROR_INTERNAL_ERROR		=	9
};


/***********************************************************************
class GPL_RegularExpression

POSIX-conformant regular expression.
***********************************************************************/
class GPL_RegularExpression : public GPL_Depictable
{
	public:
		static const char metaCharacters[];

		GPL_RegularExpression(void);
		GPL_RegularExpression(const GPL_RegularExpression & re);
		GPL_RegularExpression(const GPL_String & str);
		virtual ~GPL_RegularExpression(void);

		GPL_RegularExpression & operator=(const GPL_RegularExpression & re);
		GPL_Boolean operator==(const GPL_RegularExpression & re) const
		{
			return 0 == Compare(re);
		}
		GPL_Boolean operator!=(const GPL_RegularExpression & re) const
		{
			return 0 != Compare(re);
		}
		GPL_Boolean operator<(const GPL_RegularExpression & re) const
		{
			return 0 < Compare(re);
		}
		GPL_Boolean operator<=(const GPL_RegularExpression & re) const
		{
			return 0 <= Compare(re);
		}
		GPL_Boolean operator>(const GPL_RegularExpression & re) const
		{
			return 0 > Compare(re);
		}
		GPL_Boolean operator>=(const GPL_RegularExpression & re) const
		{
			return 0 >= Compare(re);
		}

		GPL_String GetPattern(void) const;
		GPL_RegularExpressionError GetLastError(void) const
		{
			return lastError;
		}
		GPL_Integer GetSubExpressionCount(void) const
		{
			return subExpressionCount;
		}
		GPL_Integer GetComplexity(void) const { return complexity; }
		GPL_String GetMatch(const unsigned long index = 0) const;

		GPL_Boolean SetPattern(const GPL_String & str);

		GPL_Integer Compare(const GPL_RegularExpression & re) const;

		GPL_Boolean Matches(const GPL_String & str, const unsigned long at = 0);

		// GPL_Depictable
		virtual GPL_Boolean Print(GPL_String & string,
									const GPL_Verbosity verbosity) const;
		virtual GPL_Boolean Parse(const GPL_String & string);

	private:
		typedef unsigned long	Flags;
		enum Operators
		{
								//opnd?	meaning
			GPL_END		=	0,	// no	End of program.
			GPL_BOL		=	1,	// no	Match "" at beginning of line.
			GPL_EOL		=	2,	// no	Match "" at end of line.
			GPL_ANY		=	3,	// no	Match any one character.
			GPL_ANYOF	=	4,	// str	Match any character in this string.
			GPL_ANYBUT	=	5,	// str	Match any character not in this string.
			GPL_BRANCH	=	6,	// node	Match this alternative, or the next...
			GPL_BACK	=	7,	// no	Match "", "next" ptr points backward.
			GPL_EXACTLY	=	8,	// str	Match this string.
			GPL_NOTHING	=	9,	// no	Match empty string.
			GPL_STAR	=	10,	// node	Match this (simple) thing 0 or more times.
			GPL_PLUS	=	11,	// node	Match this (simple) thing 1 or more times.
			GPL_OPEN	=	20,	// no	Mark this point in input as start of #n.
							//		OPEN + 1 is number 1, etc.
			GPL_CLOSE	=	(GPL_OPEN + GPL_MAXIMUM_SUBEXPRESSIONS)
							// no	Analogous to OPEN.
		};
		enum ParseFlags
		{
			// Worst case.
			GPL_WORST_CASE	=	0x00,
			// Known never to match null string.
			GPL_HAS_WIDTH	=	0x01,
			// Simple enough to be STAR/PLUS operand.
			GPL_SIMPLE		=	0x02,
			// Starts with * or +.
			GPL_META_START	=	0x04
		};

		static char dummy;
		static GPL_Byte magicByte;
		char *startp[GPL_MAXIMUM_SUBEXPRESSIONS];
		char *endp[GPL_MAXIMUM_SUBEXPRESSIONS];
		char *regmust;		/* Internal use only. */
		int regmlen;		/* Internal use only. */
		//char program[1];	/* Unwarranted chumminess with compiler. */
		char *program;
		char *regparse;		/* Input-scan pointer. */
		char *regcode;		/* Code-emit pointer; &regdummy = don't. */
		long regsize;		/* Code size. */
		char *reginput;		/* String-input pointer. */
		char *regbol;		/* Beginning of input, for ^ check. */
		char **regstartp;	/* Pointer to startp array. */
		char **regendp;		/* Ditto for endp. */
		GPL_String originalPattern;
		GPL_String originalString;
		GPL_RegularExpressionError lastError;
		GPL_Boolean isAnchored;
		char startCharacter;
		GPL_Integer subExpressionCount;
		unsigned long complexity;
		unsigned long lastStringComplexity;

		void Initialize(void);
		void Finalize(void);
		void IncreaseComplexity(const unsigned long offset,
								const unsigned long range,
								const unsigned long value);
		char *Parse(const GPL_Boolean parenthesized, Flags & flags);
		char *ParseBranch(Flags & flags);
		char *ParsePiece(Flags & flags);
		char *ParseAtom(Flags & flags);
		GPL_Boolean TryMatch(char *string);
		GPL_Boolean Match(char *prog);
		GPL_Integer CountRepetitions(char *p);
		void EmitByte(const GPL_Byte b);
		char *EmitNode(const GPL_Byte op);
		void InsertNode(const GPL_Byte op, char *opnd);
		static char *GetNextNode(char *p);
		static void SetOperandfulNextPointer(char *p, char *val);
		static void SetNextPointer(char *p, char *val);
		static GPL_String PrintOperation(GPL_Byte op);
};

#endif /* GPL_REGEXP_H */
