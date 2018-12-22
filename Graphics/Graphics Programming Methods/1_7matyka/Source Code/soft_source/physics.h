

#include "materialpoint.h"
#include "Vector3.h"
#include "collisions.h"
#include "cloth.h"

#ifndef __I_physics
#define __I_physics



// numeryka

Vector3 W_Euler(Vector3 , float );

void Solve_Euler(MaterialPoint *korzenP, float dt);
void Solve_MidPoint(MaterialPoint *korzenP, float dt);
void Solve_Euler(Cloth &cloth, float dt);


void Solve_MidPoint_k1(Cloth &cloth, float dt);
void Solve_MidPoint_k2(Cloth &cloth, float dt);

void Solve_MidPoint_k1(SoftBody &SoftBody, float dt);
void Solve_MidPoint_k2(SoftBody &SoftBody, float dt);


void PrzesunObszary(Obszar *korzenO);


// physics

void CalculateForces(SoftBody &SoftBody, float dt);
void CalculateForces(MaterialPoint *korzenP, Sprezyna *korzenS, Obszar *korzenO);
void CalculateForces(Cloth &cloth, float dt);
void CCC_Springs(Cloth &cloth);
void CCC_Springs(SoftBody &SoftBody);

Vector3 PrzyrostEulera(MaterialPoint *point,float dt);


extern Vector3 Graw;
extern float tlumienie;




#endif