/************************************************************************/
/*                                                                      */
/*                       Soft Body 3.0                                  */
/*                 by Maciej Matyka, 21.IX.2004                         */
/*                                                                      */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/*                                                                      */
/*  Made for purposes of Grame Programming Gems 5 article.              */
/*                                                                      */
/*  note: this code has been attached to article  only as               */
/*  an example of described method. No any guarrantee of anything is    */
/*  is given. Use at your own risk.                                     */
/*                                                                      */
/*  if you want to use any part of that code in your project,           */
/*  email Author at:                                                    */
/*                                                                      */
/*  maq@panoramix.ift.uni.wroc.pl                                       */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/************************************************************************/



#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


#include "CFileParser.h"

void CFileParser::parsePoints(void)
{
		FILE *fp;
		
		char workbuf[20];
		char workbuf_x[20];
		char workbuf_y[20];
		char workbuf_z[20];

		double x,y,z;

		if(!(fp = fopen(cFilename,"r")))
		{
			printf("Unable to open %s file.\n",cFilename);
			exit(1);
		}

		iNump = parseIntValue(fp,"Vertices:");
	
		fPoints = new float[iNump*3];

		seekWord(fp,"Vertex");
		seekWord(fp,"List:");
		// parse points (vertices)
		for(int i=0; i<iNump ; i++)
		{
			sprintf(workbuf,"%d:",i);
			seekWord(fp,workbuf);
			
			mfscanf(fp,"%s",workbuf_x);			
			mfscanf(fp,"%s",workbuf_y);			
			mfscanf(fp,"%s",workbuf_z);			

			x=atof(workbuf_x+2);		// +2 because of X:, Y:, Z: in 3DS file
			y=atof(workbuf_y+2);
			z=atof(workbuf_z+2);

			//Punkt(int _flaga, float _masa, Wektor _r, Wektor _v)
			fPoints[i*3+0]=x;
			fPoints[i*3+1]=y;
			fPoints[i*3+2]=z;
		}



		fclose(fp);
}



void CFileParser::parseFaces(void)
{
		FILE *fp;
		
		char workbuf[20];
		char workbuf_a[20];
		char workbuf_b[20];
		char workbuf_c[20];

		int a,b,c;

		if(!(fp = fopen(cFilename,"r")))
		{
			printf("Unable to open %s file.\n",cFilename);
			exit(1);
		}

		iNumf = parseIntValue(fp,"Faces:");
	
		fFaces = new int[iNumf*3];

		seekWord(fp,"Face");
		seekWord(fp,"list:");
		// parse points (vertices)
		for(int i=0; i<iNumf ; i++)
		{
			sprintf(workbuf,"%d:",i);
			seekWord(fp,workbuf);
			
			mfscanf(fp,"%s",workbuf_a);			
			mfscanf(fp,"%s",workbuf_b);			
			mfscanf(fp,"%s",workbuf_c);			

			a=atoi(workbuf_a+2);		// +2 because of X:, Y:, Z: in 3DS file
			b=atoi(workbuf_b+2);
			c=atoi(workbuf_c+2);

			//Punkt(int _flaga, float _masa, Wektor _r, Wektor _v)
			fFaces[i*3+0]=a;
			fFaces[i*3+1]=b;
			fFaces[i*3+2]=c;
		}

		fclose(fp);
}


	/**
	*
	* parseIntValue
	*
	* - return int value readed from file
	*
	*
	* @todo - better check for problems 
	*
	*/

int CFileParser::parseIntValue(FILE *fp, char *cField)
{
	char workbuf[100];
	seekWord(fp,cField);
	mfscanf(fp,"%s",workbuf);
	return atoi(workbuf);				// return number
}

	/*
	* testEOF
	*
	* - exit if cannot read something
	*
	*
	* @todo - 
	*/

void CFileParser::testEOF(FILE *fp)
{
	int a;
	a=ungetc(getc(fp),fp);
	if(a==EOF)
	{
		printf("Error with object file! \n");
		exit(0);
	}
}



	/*
	* mfscanf
	*
	* - fscanf with testEOF
	*
	*
	* @todo - 
	*/

void CFileParser::mfscanf(FILE *fp,char workbuf2[],char workbuf[])
{
		testEOF(fp);
		fscanf(fp,workbuf2,workbuf);
}



	/*
	* seekWord
	*
	* - search for word and move fp there
	*
	*
	* @todo - 
	*/

void CFileParser::seekWord(FILE *fp, char *word)
{
	char workbuf[100];

	while(!(strcmp(workbuf,word)==0))
		mfscanf(fp,"%s",workbuf);
}





	/**
	 * Dump3DSASC
	 *
	 * - dump object to 3DS Ascii format
	 *
	 *
	 * @todo - 
	 *
	 *                                                                      
	 */


void CFileParser::Dump3DSASC(char *cFilenameSave)
{
	FILE *fp;
	char workbuf[20];

	if(!(fp = fopen(cFilenameSave,"w")))
	{
		printf("Unable to open %s file for object dump.\n",cFilenameSave);
		exit(1);
	}


	fputs("Named object: \"softbody113\"\n",fp);
	fputs("Tri-mesh, ",fp);
	sprintf(workbuf,"Vertices: %d ",iNump);	
	fputs(workbuf, fp);
	sprintf(workbuf,"Faces: %d\n",iNumf);	
	fputs(workbuf, fp);

	
	fputs("Vertex List:\n",fp);

	for(int i=0; i<iNump ; i++)
		{
			sprintf(workbuf,"Vertex %d: ",i);	
			fputs(workbuf, fp);

			sprintf(workbuf," X:%f",fPoints[i*3+0]);	
			fputs(workbuf, fp);
			sprintf(workbuf," Y:%f",fPoints[i*3+1]);	
			fputs(workbuf, fp);
			sprintf(workbuf," Z:%f\n",fPoints[i*3+2]);	
			fputs(workbuf, fp);
		}


	fputs("Face list:\n", fp);


	for(i=0; i<iNumf ; i++)
		{
			sprintf(workbuf,"Face %d:",i);	
			fputs(workbuf, fp);

			sprintf(workbuf," A:%d",fFaces[i*3+0]);	
			fputs(workbuf, fp);
			sprintf(workbuf," B:%d",fFaces[i*3+1]);	
			fputs(workbuf, fp);
			sprintf(workbuf," C:%d",fFaces[i*3+2]);	
			fputs(workbuf, fp);	

			fputs(" AB:1 BC:1 CA:1\nSmoothing: 1\n", fp);
		}
			

	fclose(fp);
}
