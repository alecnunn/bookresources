// PermutationSequences.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../PermutationSequenceIndexer.h"

const char* Symbols[] = {"A","B","C"};

#define __countof(x)	(sizeof(x)/sizeof(x[0]))
 
void main()	//CPermutationSequenceIndexer Example
{
	char str[256];
	SequenceIndexer::SYMBVECTOR vec;
	SequenceIndexer::CPermutationSequenceIndexer PIndexer(__countof(Symbols));

	printf("\nConsider 3 symbols: {A, B, C};");

	printf("\n\nNumber of Permutations = %s\n",PIndexer.TotalSequenceLength().GetNumberIntoString(str));

	for(SequenceIndexer::POSITION i=0; i < PIndexer.TotalSequenceLength(); ++i)
	{
		printf("\nPermutation[%s] is = ",i.GetNumberIntoString(str));

		//compute the term from index
		vec = PIndexer.GetAt(i);	// now vec containes the symbols of the term

		//Each symbol of the term has to be looked up into the Symbols array
		printf("{%2s%2s%2s };",Symbols[vec[0]],Symbols[vec[1]],Symbols[vec[2]]);

		//Compute the index from the term and print it
		printf("\tComputed index is: %s", PIndexer.GetIndexOf(vec).GetNumberIntoString(str));	// computed value would be same as i	
	}

	printf("\n\nComputed Indexes should match the Term Indexes..\n\n");
}
