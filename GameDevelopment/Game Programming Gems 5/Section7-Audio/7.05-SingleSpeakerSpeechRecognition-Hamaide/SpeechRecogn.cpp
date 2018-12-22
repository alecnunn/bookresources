// SpeechRecogn.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WavRead.h"
#include "Filter.h"
#include "FeatureExtraction.h"
#include "WordManagement.h"
#include "DTW.h"



void Usage()
{
	printf("Usage: \n\tSpeechRecogn wavsound database command  \n");
	printf("\ncommand can be add X(add the word X to the codebook) or test, that give the word recognise\n\n");
	printf("ex: SpeechRecogn aword.wav codebook.dict add aword\n");
	printf("    SpeechRecogn aword.wac codebook.dict test\n");
}


int main(int argc, char* argv[])
{

	if( (argc < 4) || (argc >5) )
	{
		Usage();
		exit(-1);
	}

	//Is there a valid command
	if( strcmp(argv[3], "add") && strcmp(argv[3],"test"))
	{
		Usage();
		exit(-1);
	}

	short *wavData;
	int wavSize;
	if(!wavOpen(argv[1],&wavData, wavSize))
	{
		printf("Invalid file %s\n",argv[1]);
		return -1;
	}

	float *resample;
	int resamplesize;

	//Now undersampling and preemphasis of the signal
	Resample(wavData, &resample,wavSize,resamplesize, true);
	float LastSample = 0;
	Preemphasis(resample,resamplesize,LastSample);

    features *wavFeature;
	int featureNumber;

	ComputeFeaturesVectors(resample,resamplesize, &wavFeature, featureNumber);	

	if(!strcmp(argv[3], "add"))
	{
		Word wordTemp;

		strcpy(wordTemp._word, argv[4]);
		wordTemp._featuresSize = featureNumber;
		wordTemp._features = wavFeature;

		AppendFeatureToFile(argv[2], wordTemp);

	}
	else
	{

		Word *WordList;
		int ListSize;
		int i;

		LoadFeatureFromFile(argv[2], &WordList, ListSize);

		Word *best = NULL;
		float score = 0;

		for(i=0; i< ListSize; i++)
		{
			float cur_score = DTW(wavFeature, featureNumber, WordList[i]._features, WordList[i]._featuresSize);
			//cur_score /= (featureNumber + WordList[i]._featuresSize);
			if( (cur_score < score) || (best ==NULL) )
			{
				best = &(WordList[i]);
				score = cur_score;
			}
		}

		printf("The word pronounced was %s ; with ha score of %f", best->_word, score);

		for(i=0; i<ListSize; i++)
		{
			delete[] WordList[i]._features;
		}
		delete[] WordList;

	}


	delete[] resample;
	delete[] wavFeature;
	return 0;
}
