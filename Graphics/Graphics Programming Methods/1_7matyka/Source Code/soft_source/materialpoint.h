
#include "Vector3.h"
#include "collisions.h"

#ifndef __I_MaterialPointY_
#define __I_MaterialPointY_

#define P_NORM 1			// normalny - physics dziala
#define P_ZAW 2				// nie Solve_uje rownan rochu dla ciala (polozenie = const)
#define P_OBSZ 4            // podczepiony opbszar kolizji
#define P_NRYS 8			// nie rysuj
#define P_WEKT 16           // dolacz Vector3

//#define S_OK 1
#define S_PEK 2
#define S_NRYS 4
#define S_RYSL 8


class MaterialPoint 
{

public:
	MaterialPoint()
	{};

	MaterialPoint(int _flaga, float _masa, Vector3 _r, Vector3 _v) :
	flaga(_flaga), masa(_masa), r(_r), v(_v), prawy(NULL), R(0)
	{};

	int flaga;
	float masa;
	
	Vector3 r;
	Vector3 v;
	Vector3 f;

	Vector3 n;		// a jak, mamy 2002 - normalna do MaterialPointu. Ha!

	Vector3 dr;
	Vector3 dv;

	double R;

	double col;

	MaterialPoint *prawy;
};


class Sprezyna
{
public:
	Sprezyna(MaterialPoint *_p1, MaterialPoint *_p2, float _ks, float _kd, float _d, int _f, int _const_lenght) :
	  MaterialPoint1(_p1), MaterialPoint2(_p2), ks(_ks), kd(_kd), length(_d), flag(_f), const_lenght(_const_lenght)
	  {
		if(!_d)
			length=(float)(_p1->r-_p2->r).length();
	  };

	Sprezyna()
	{}

	MaterialPoint *MaterialPoint1;
	MaterialPoint *MaterialPoint2;
	
	float ks;      // stala Hooke
	float kd;	    // stala wygaszania drgan
	float length;	// length sprezyny, jesli 0 to liczy sam defaultowo

	int flag;
	int	const_lenght;	// procentowo jak moze sie zmienic

	Sprezyna *prawy;

};


#endif