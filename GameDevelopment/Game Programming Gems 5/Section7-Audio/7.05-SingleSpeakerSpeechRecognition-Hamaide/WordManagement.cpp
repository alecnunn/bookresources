#include "WordManagement.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void LoadFeatureFromFile(char *filename, Word **wordVector,int &vectorSize)
{
	vectorSize = 0;

	_fmode = _O_BINARY;
	FILE *input = fopen(filename, "r");

	Word temp;
	while(!feof(input))
	{
		if(!fread(&temp, sizeof(Word)-4,1, input)) break;
		fseek(input, temp._featuresSize*sizeof(features),SEEK_CUR);
		vectorSize++;
	}

	*wordVector = new Word[vectorSize];

	fseek(input, 0, SEEK_SET);

	for(int i=0; i<vectorSize; i++)
	{
		fread(&(*wordVector)[i], sizeof(Word)-4,1,input);
		
		(*wordVector)[i]._features = new features[(*wordVector)[i]._featuresSize];
		fread((*wordVector)[i]._features, sizeof(features), (*wordVector)[i]._featuresSize,input);
	}
	fclose(input);
}

void SaveFeatureToFile(char *filename, Word*wordVector, int vectorSize)
{
	FILE* output;

	_fmode = _O_BINARY;

	output = fopen(filename, "w");

	for(int i = 0; i< vectorSize; i++);
	{
		fwrite( &(wordVector[i]), sizeof(Word)-4,1,output);
		fwrite( wordVector[i]._features, sizeof(features), wordVector[i]._featuresSize, output);
	}

	fclose(output);
}

void AppendFeatureToFile(char *filename, Word &wordVector)
{
	FILE* output;

	_fmode = _O_BINARY;

	output = fopen(filename, "a+");

	fwrite(&wordVector, sizeof(Word)-4,1,output);
	fwrite(wordVector._features, sizeof(features), wordVector._featuresSize, output);

	fclose(output);
}
