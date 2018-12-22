

#ifndef __I_collisions_H_
#define __I_collisions_H_

#include "Vector3.h"
#include "materialpoint.h"
#include "cloth.h"
#include "SoftBody.h"

#define O_AKT 1
#define O_NAKT 2
#define O_RYS 4

extern float xmin,xmax,ymin,ymax,zmin,zmax;
extern float tlumienie;
extern float dt;

//typedef struct Obszar Obszar;

class Obszar 
{

public:
	int flaga;			// O_AKT/O_NAKT  aktywny, nieaktywny (dla korzenia)

	MaterialPoint *MaterialPoint;		// przypisany do MaterialPointu (ruchomy) jesli == NULL (nieruchomy)

	Vector3 r1;			// polozenie
	float R;			// promien
	float tlum;			// wsp. tlumienia - 1.0 brak tlumienia

	float r,g,b;

	Obszar *prawy;
};






Obszar *AlokujObszar(float promien, Vector3 r, float tlum, int flaga, MaterialPoint *p);
void DodajObszar(Obszar *korzen, float promien, Vector3 r, float tlum, int flaga, MaterialPoint *p);

void collisions(MaterialPoint *korzenP, Obszar *korzenO);
Vector3 collisionsObszary(MaterialPoint *p, Vector3 V);
void collisions(Cloth &cloth);
void collisions(SoftBody &SoftBody, Obszar *korzenO);


#endif