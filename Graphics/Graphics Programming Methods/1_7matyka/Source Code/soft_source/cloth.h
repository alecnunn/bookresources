#ifndef __I___CLOTH_H_
#define __I___CLOTH_H_

#include <math.h>
#include <GL/glut.h>
#include "Vector3.h"
#include "materialpoint.h"


class Cloth
{
public:
	Cloth(int _nx, int _ny, float _ks, float _kd, int _lenght_constrain, 
		Vector3 r0, Vector3 r1, float mass) :
		nx(_nx), ny(_ny), ks(_ks), kd(_kd), lenght_constrain(_lenght_constrain)
	{
		Points = new MaterialPoint**[nx];
		for(i=0; i < nx; i++)
			Points[i] = new MaterialPoint*[ny+1];

		Springs1 = new Sprezyna**[nx];
		for(i=0; i < nx; i++)
			Springs1[i] = new Sprezyna*[ny+1];

		Springs2 = new Sprezyna**[nx];
		for(i=0; i < nx; i++)
			Springs2[i] = new Sprezyna*[ny+1];

		Springs3 = new Sprezyna**[nx];
		for(i=0; i < nx; i++)
			Springs3[i] = new Sprezyna*[ny+1];

		Springs4 = new Sprezyna**[nx];
		for(i=0; i < nx; i++)
			Springs4[i] = new Sprezyna*[ny+1];

		Springs5 = new Sprezyna**[nx];			// x
		for(i=0; i < nx; i++)
			Springs5[i] = new Sprezyna*[ny+1];

		Springs6 = new Sprezyna**[nx];				// y
		for(i=0; i < nx; i++)
			Springs6[i] = new Sprezyna*[ny+1];

		CreatePoints(r0,r1,mass);
		MakeStructuralSprings();
		MakeFlexSprings();
	}

	~Cloth()
	{
		for(i=0; i<nx; i++)
			delete [] Points[i];
     		delete [] Points;
	
		for(i=0; i<nx; i++)
			delete [] Springs1[i];
     		delete [] Springs1;

		for(i=0; i<nx; i++)
			delete [] Springs2[i];
     		delete [] Springs2;

		for(i=0; i<nx; i++)
			delete [] Springs3[i];
     		delete [] Springs3;

		for(i=0; i<nx; i++)
			delete [] Springs4[i];
     		delete [] Springs4;

		for(i=0; i<nx/2; i++)
			delete [] Springs5[i];
     		delete [] Springs5;

		for(i=0; i<nx; i++)
			delete [] Springs6[i];
     		delete [] Springs6;

	}

	void BindTextureOnIt(unsigned int _tekstura)
	{cloth_txt = _tekstura;}

	void CreatePoints(Vector3 r0,Vector3 r1, float mass);
	void MakeStructuralSprings(void);
	void MakeFlexSprings(void);
	void CalculateVertexNormals(void);


	void DisplayGL(int mode);

public:
	int nx;
	int ny;
	float ks;
	float kd;

	unsigned int cloth_txt;				// nr tekstury opengl (0 - brak)

	int i;							// licznik petli
	int j;
	int lenght_constrain;			// procentowo dopuszczalne rozciagniecie 

	MaterialPoint ***Points;
	Sprezyna ***Springs1;			// strukturalne x
	Sprezyna ***Springs2;			// strukturalne y
	Sprezyna ***Springs3;			// (na krzyz)
	Sprezyna ***Springs4;			// (na krzyz)
	Sprezyna ***Springs5;			// strukturalne 2 * x		- do nich nie robimy korekty
	Sprezyna ***Springs6;			// strukturalne 2 * y
						// mozna jeszcze dodac inne

};









#endif
