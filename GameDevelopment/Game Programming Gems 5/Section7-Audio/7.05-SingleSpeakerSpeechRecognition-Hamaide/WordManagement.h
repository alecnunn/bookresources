#ifndef __WORD_MANAGEMENT_H__
#define __WORD_MANAGEMENT_H__

#include "FeatureExtraction.h"

/// structure that contains the word and his sequence of features
typedef struct struct_word
{
	char _word[64];
	int  _featuresSize;
	features *_features;
} Word;


/// manage feature to and from file
void LoadFeatureFromFile(char *filename, Word **wordVector,int &vectorSize);
void SaveFeatureToFile(char *filename, Word*wordVector, int vectorSize);
void AppendFeatureToFile(char *filename, Word &wordVector);


#endif //__WORD_MANAGEMENT_H__