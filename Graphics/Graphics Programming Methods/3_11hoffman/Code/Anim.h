#ifndef _ANIM_H
#define _ANIM_H

#include "stdafx.h"
#include <vector>

class State;

class Anim {
public:
	Anim();
	virtual ~Anim();

	State *GetNextFrame();
	State *GetState(int index) {return m_states[index];};

	void AdvCurState(int dir);
	State *GetCurState() {return m_states[m_currentState];};
	void InitForStart() {m_currentState = 0;};

	inline bool GetDemo() {return m_bDemo;};
	inline void SetDemo(bool b) {m_bDemo = b;};
	
	bool Dump(FILE *f);
	bool Dump2(FILE *f);
	bool Read(FILE *f);
	bool Read2(FILE *f);
	void RecordState(State *state);

private:	
	State *GetSplineInterpolatedFrame(float x);
	inline int GetMaxFrames();
	float GetFracInFrames(int frameNum);


private:
	bool m_bDemo;
	std::vector<State *> m_states;

	State *m_interpolatedFrame;
	int m_currentState;
	int m_currentFrame;
	State *m_array;
	int *m_frameNums;

};





#endif
