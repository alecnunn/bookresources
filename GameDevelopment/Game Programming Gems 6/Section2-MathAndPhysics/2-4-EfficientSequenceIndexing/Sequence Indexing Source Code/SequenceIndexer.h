#ifndef __SEQUENCEINDEXER_H__5AE0A6B6_51FC_4eb3_A590_903987739105
#define __SEQUENCEINDEXER_H__5AE0A6B6_51FC_4eb3_A590_903987739105

#include "BigNumber/BigNumberFactorial.h"

namespace SequenceIndexer
{
//
// Freqency is the number of times a symbol gets repeated.
// Typical values for Frequency could be in range [0,TextLength]
// 
// SymbVector is the array of indices of symbols
// Typical values for any value in that array could be in [0,TextLength-1]
//
// Data type of Frequency need not be same as that of Symbol Index
// A long text with many symbols would give small Frequency with big indices
// A long text with less symbols would give big Frequency with small indices
//
// Frequency is more linked with text content than with text length.
// Sum of all Frequency values == Length of Text
//
// Length of SymbVector == Length of the Text needed. (Some Symbols repeat)
// A Sequence == Enumeration of all possible SymbolVectors
//
// Value of a Sequence Position Index could vary from [0, TextLength!)
//
typedef unsigned long TEXTLENGTH;	// Can go beyond ULONG for huge files

typedef TEXTLENGTH FREQUENCY;		// Frequency Ranges From 0 to TextLength
typedef std::vector<FREQUENCY>	FREQVECTOR;

typedef unsigned long SYMBOLINDEX;
typedef std::vector<SYMBOLINDEX> SYMBVECTOR;

typedef BIGNUMBER POSITION;		// Factorial value, Needs bigger types

}

#endif