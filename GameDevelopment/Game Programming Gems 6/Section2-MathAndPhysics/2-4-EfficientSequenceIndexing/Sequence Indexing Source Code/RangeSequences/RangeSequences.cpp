// RangeSequences.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RangeSequenceIndexer.h"

#define __countof(x)	(sizeof(x)/sizeof(x[0]))

const char* Nature[] = {"Good", "Bad", "Ugly"};
const char* Sex[] = {"Male", "Female"};
const char* Living[] = {"Alive", "Dead"};

int main(void)
{
	char str[256];
	SequenceIndexer::SYMBVECTOR vec;
	SequenceIndexer::RANGE Ranges[] = {__countof(Nature), __countof(Sex), __countof(Living)};
	SequenceIndexer::CRangeSequenceIndexer RIndexer(Ranges, 3); // 3 because we have 3 attributes: Nature, Sex and Living

	printf("\nConsider a class having the attributes: {Nature, Sex, Living}; where");
	printf("\n\tNature\t= {Good, Bad, Ugly};");
	printf("\n\tSex\t= {Male, Female};");
	printf("\n\tLiving\t= {Alive, Dead};");

	printf("\n\nNumber of unique objects for that class can be: %s\n", RIndexer.TotalSequenceLength().GetNumberIntoString(str));

	for(SequenceIndexer::POSITION i=0; i < RIndexer.TotalSequenceLength(); ++i)
	{
		printf("\nObject[%2s] is = ",i.GetNumberIntoString(str));

		//compute the term from index
		vec = RIndexer.GetAt(i);	// now vec containes the symbols of the term

		//Each symbol of the term has to be looked up into the corresponding attribute array
		printf("{%5s,%7s,%6s};",Nature[vec[0]],Sex[vec[1]],Living[vec[2]]);

		//Compute the index from the term and print it
		printf("\tComputed index is: %s", RIndexer.GetIndexOf(vec).GetNumberIntoString(str));	// computed value would be same as i	
	}

	printf("\n\nComputed Indexes should match the Term Indexes..\n\n");


	return 0;
}

