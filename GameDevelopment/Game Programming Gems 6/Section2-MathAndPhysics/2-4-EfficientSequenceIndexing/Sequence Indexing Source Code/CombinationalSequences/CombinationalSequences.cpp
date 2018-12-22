// CombinationalSequences.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../CombinationalSequenceIndexer.h"

const char* Cities[] = {"Newyork","Paris","Berlin","London","Tokyo"};

#define __countof(x)	(sizeof(x)/sizeof(x[0]))

void main()	//CCombinationalSequenceIndexer Example
{
	char str[256];
	SequenceIndexer::SYMBVECTOR vec;	
	SequenceIndexer::CCombinationalSequenceIndexer CIndexer(__countof(Cities), 3); // we want to choose 3 cities out of 5

	printf("\nConsider 5 cities: {Newyork, Paris, Berlin, London, Tokyo};");

	printf("\n\nNumber of Combinations for selecting 3 out of 5 = %s\n",CIndexer.TotalSequenceLength().GetNumberIntoString(str));	

	for(SequenceIndexer::POSITION i=0; i < CIndexer.TotalSequenceLength(); ++i)
	{
		printf("\nCombination[%s] is = ",i.GetNumberIntoString(str));

		//compute the term from index
		vec = CIndexer.GetAt(i);	// now vec containes the symbols of the term

		//Each symbol of the term has to be looked up into the Cities array
		printf("{%7s,%7s,%7s};",Cities[vec[0]],Cities[vec[1]],Cities[vec[2]]);

		//Compute the index from the term and print it
		printf("\tComputed index is: %s", CIndexer.GetIndexOf(vec).GetNumberIntoString(str));	// computed value would be same as i	
	}
    printf("\n\nComputed Indexes should match the Term Indexes..\n\n");
}