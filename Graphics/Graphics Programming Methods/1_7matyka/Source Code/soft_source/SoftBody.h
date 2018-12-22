#ifndef __I___SoftBody_H_
#define __I___SoftBody_H_

#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Vector3.h"
#include "materialpoint.h"


// generally properties of links between points


// ks,kd coefficients
#define s1ks 5000 
#define s1kd 1.0

#define s2ks 23000
#define s2kd 1.0


#define s3ks 13000			// star
#define s3kd 1.0
#define s4ks 13000			// column
#define s4kd 1.0
#define s5ks 13000			// center
#define s5kd 1.0

#define s6ks 1000
#define s6kd 2.0

// contrained lenghts
#define lc_s1 17
#define lc_s2 16
#define lc_s3 43
#define lc_s4 43
#define lc_s5 43
#define lc_s6 43




//
//

class SoftBody
{
public:

	SoftBody(Vector3 r, float _mass, char *filename,float scale) :
		mass(_mass)
	{

		// load object file

		FILE *fp;
		char workbuf[100];

		if(!(fp=fopen(filename,"r")))
		{
			printf("Unable to open %s file.\n",filename);
			exit(1);
		}


		// set numbers of end of the file

		MoveNextWord(fp,"NUMP");
		mfscanf(fp,"%s",workbuf);
		NUMP=atoi(workbuf);


		MoveNextWord(fp,"S1_num");
		mfscanf(fp,"%s",workbuf);
		S1_num=atoi(workbuf);

		MoveNextWord(fp,"S2_num");
		mfscanf(fp,"%s",workbuf);
		S2_num=atoi(workbuf);

		MoveNextWord(fp,"S3_num");
		mfscanf(fp,"%s",workbuf);
		S3_num=atoi(workbuf);

		MoveNextWord(fp,"S4_num");
		mfscanf(fp,"%s",workbuf);
		S4_num=atoi(workbuf);

		MoveNextWord(fp,"S5_num");
		mfscanf(fp,"%s",workbuf);
		S5_num=atoi(workbuf);

		MoveNextWord(fp,"S6_num");
		mfscanf(fp,"%s",workbuf);
		S6_num=atoi(workbuf);

		fclose(fp);

		// open file (second time)

		if(!(fp=fopen(filename,"r")))
		{
			printf("Unable to open %s file.\n",filename);
			exit(1);
		}



		// load parameters

		while(strcmp(workbuf,"Y"))
		{ 
			mfscanf(fp,"%s",workbuf);
		}

		mfscanf(fp,"%s",workbuf);
		ny=atoi(workbuf);

		while(strcmp(workbuf,"X"))
		{ 
			mfscanf(fp,"%s",workbuf);
		}

		mfscanf(fp,"%s",workbuf);
		nx=atoi(workbuf);



		// allocate points table

		Points = new MaterialPoint*[NUMP];
		Normals = new Vector3[NUMP];
		

		// load points

		MoveNextWord(fp,"Points:");

		float x,y,z;

		i=0;

		for(i=0;i<NUMP;i++)
		{
			mfscanf(fp,"%s",workbuf);				// x
			x=atof(workbuf);

			if((strcmp(workbuf,"Links:")==0))
				break;

			mfscanf(fp,"%s",workbuf);				// y
			y=atof(workbuf);

			mfscanf(fp,"%s",workbuf);				// z
			z=atof(workbuf);
			
			printf("%f   %f   %f\n",x,y,z);
			
			Points[i] = new MaterialPoint(P_NORM, mass,Vector3(x,y,z)*scale+r,Vector3(0,0,0));

			Normals[i] = Vector3(x,y,z);

		}


		// allocate spring 1 S1 table
		int first,second;

		S1 = new Sprezyna*[S1_num];
		S2 = new Sprezyna*[S2_num];
		S3 = new Sprezyna*[S3_num];
		S4 = new Sprezyna*[S4_num];
		S5 = new Sprezyna*[S5_num];
		S6 = new Sprezyna*[S6_num];


		// now put springs into tables, 
	

		MoveNextWord(fp,"S1:");

		for(i=0;i<S1_num;i++)
		{
			mfscanf(fp,"%s",workbuf);				// 1st
			first=atoi(workbuf);

			mfscanf(fp,"%s",workbuf);				// 1st
			second=atoi(workbuf);

			S1[i] = new Sprezyna(Points[first+1],Points[second+1],s1ks,s1kd,0,S_RYSL,lc_s1);
		}

		MoveNextWord(fp,"S2:");
		for(i=0;i<S2_num;i++)
		{
			mfscanf(fp,"%s",workbuf);				// 1st
			first=atoi(workbuf);

			mfscanf(fp,"%s",workbuf);				// 1st
			second=atoi(workbuf);

			S2[i] = new Sprezyna(Points[first+1],Points[second+1],s2ks,s2kd,0,S_RYSL,lc_s2);
		}

		MoveNextWord(fp,"S3:");
		for(i=0;i<S3_num;i++)
		{
			mfscanf(fp,"%s",workbuf);				// 1st
			first=atoi(workbuf);

			mfscanf(fp,"%s",workbuf);				// 1st
			second=atoi(workbuf);

			S3[i] = new Sprezyna(Points[first+1],Points[second+1],s3ks,s3kd,0,S_RYSL,lc_s3);
		}

		MoveNextWord(fp,"S4:");
		for(i=0;i<S4_num;i++)
		{
			mfscanf(fp,"%s",workbuf);				// 1st
			first=atoi(workbuf);

			mfscanf(fp,"%s",workbuf);				// 1st
			second=atoi(workbuf);

			S4[i] = new Sprezyna(Points[first+1],Points[second+1],s4ks,s4kd,0,S_RYSL,lc_s4);
		}

		MoveNextWord(fp,"S5:");
		for(i=0;i<S5_num;i++)
		{
			mfscanf(fp,"%s",workbuf);				// 1st
			first=atoi(workbuf);

			mfscanf(fp,"%s",workbuf);				// 1st
			second=atoi(workbuf);

			S5[i] = new Sprezyna(Points[first+1],Points[second+1],s5ks,s5kd,0,S_RYSL,lc_s5);
		}

		MoveNextWord(fp,"S6:");
		for(i=0;i<S6_num;i++)
		{
			mfscanf(fp,"%s",workbuf);				// 1st
			first=atoi(workbuf);

			mfscanf(fp,"%s",workbuf);				// 1st
			second=atoi(workbuf);

			S6[i] = new Sprezyna(Points[first+1],Points[second+1],s6ks,s6kd,0,S_RYSL,lc_s6);
		}

		fclose(fp);

		printf("ny=%d\n",ny);
		printf("nx=%d\n",nx);

}

	~SoftBody()
	{
	}




// @todo: divide it to SoftBody and parser classes!!
//
//
// parse txt file functions
//
//

void testeof(FILE *fp)
{
	int a;

	a=ungetc(getc(fp),fp);
	if(a==EOF)
	{
		printf("Error with .dat file! \n");
		exit(0);
	}
}

void mfscanf(FILE *fp,char workbuf2[],char workbuf[])
{
		testeof(fp);
		fscanf(fp,workbuf2,workbuf);
}

void MoveNextWord(FILE *fp, char *word)
{
	char workbuf[100];

	while(!(strcmp(workbuf,word)==0))
		mfscanf(fp,"%s",workbuf);
}





//
// Other methods
//
//
//
//
//


	void CalculateNormals(void);
	void DisplayGL(int mode);
	void DisplayNormals(void);
	void DisplayVelocities(void);
	


public:
	int nx;
	int ny;
	int NUMP;

	int S1_num;
	int S2_num;
	int S3_num;
	int S4_num;
	int S5_num;
	int S6_num;

	int i;							
	int j;

	float mass;						// mass of material points in the ball

	MaterialPoint **Points;			// MaterialPoints of the Ball 
	Vector3 *Normals;				// normal vectors

	Sprezyna **S1;			
	Sprezyna **S2;
	Sprezyna **S3;
	Sprezyna **S4;
	Sprezyna **S5;
	Sprezyna **S6;
};

#endif
