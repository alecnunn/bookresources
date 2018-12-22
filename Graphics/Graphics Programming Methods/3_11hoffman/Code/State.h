#ifndef _STATE_H
#define _STATE_H

#include "Camera.h"
#include "Atmosphere.h"
#include "Sun.h"
#include <stdio.h>

class State {

public:
	State() { m_nFrameNum = 0;};
	~State() {};
	void SetCamera(const Camera *camera) {m_Camera = *camera;};
	void SetAtmosphere(const Atmosphere *atm) {m_Atmosphere = *atm;};
	void SetSun(const Sun *sun) {m_Sun = *sun;};
	void SetFrameNum(int n) {m_nFrameNum = n;};

	Camera *GetCamera() {return &m_Camera;}
	Atmosphere *GetAtmosphere() {return &m_Atmosphere;}
	Sun *GetSun() {return &m_Sun;}
	int GetFrameNum() {return m_nFrameNum;};

	bool Dump(FILE *f);
	bool Dump2(FILE *f);
	bool Read2(FILE *f);

	void Finalize();

    State operator + ( CONST State& ) const;
	State operator * ( float ) const;
    friend State operator * ( float , const State& );


private:
	Camera m_Camera;
	Atmosphere m_Atmosphere;
	Sun m_Sun;
	int m_nFrameNum;
};

State operator *(float f, const State &s1);


#endif
