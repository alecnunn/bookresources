
#include "stdafx.h"
#include "Anim.h"
#include "State.h"
#include "Spline.h"
#include "assert.h"

Anim::Anim()
{
	m_bDemo = true;
	m_states.clear();
	m_currentState = 0;
	m_currentFrame = 0;
	m_interpolatedFrame = new State;
	m_array = NULL;
	m_frameNums = NULL;
}

Anim::~Anim()
{
	for (int i=0; i< m_states.size(); i++)
		delete m_states[i];
	delete m_interpolatedFrame;
	if (m_array) delete [] m_array;
	if (m_frameNums) delete [] m_frameNums;
}

int Anim::GetMaxFrames()
{
	State *s = m_states[m_states.size()-1];
	return s->GetFrameNum();
};

void Anim::AdvCurState(int dir)
{
	if (dir > 0)
	{
		m_currentState++;
		if (m_currentState == m_states.size()) m_currentState--;
	}
	else
	{
		m_currentState--;
		if (m_currentState < 0) m_currentState = 0;
	}
}

float Anim::GetFracInFrames(int frameNum)
{
	int numStates = m_states.size();
	float frac;
	float inv = 1.0/(numStates-1);
	for (int i=1; i < numStates; i++)
	{
		if (m_frameNums[i] > frameNum)
		{
			frac = float(frameNum-m_frameNums[i-1])/(m_frameNums[i]-m_frameNums[i-1]);
			frac += i-1;
			frac *= inv;
			return frac;
		}
	}
	return 1.0;
}

State *Anim::GetSplineInterpolatedFrame(float x)
{
	int numStates = m_states.size();
	*m_interpolatedFrame = spline(x, numStates, m_array);
	m_interpolatedFrame->Finalize();
	return m_interpolatedFrame;
}


State *Anim::GetNextFrame()
{
	float f = GetFracInFrames(m_currentFrame);
	int num = GetMaxFrames();

	State *m_interpolatedFrame = GetSplineInterpolatedFrame(f);
	m_currentFrame++;
	if (m_currentFrame == num) 
	{
		m_currentFrame = 0;
		return NULL;
	}

	return m_interpolatedFrame;

}


void Anim::RecordState(State *state)
{ 
	m_states.push_back(state);
}


bool Anim::Dump(FILE *f)
{
	unsigned int num = m_states.size();
	fwrite(&num, sizeof(num), 1, f);
	fwrite(&m_bDemo, sizeof(m_bDemo), 1, f);
	for (int i=0; i < num; i++)
	{
		State *state = m_states[i];
		if (!(state->Dump(f)))
			return false;
	}
	return true;
}

bool Anim::Dump2(FILE *f)
{
	unsigned int num = m_states.size();
	fprintf(f, "%d ", num);
	fprintf(f, "%d\n", m_bDemo);
	for (int i=0; i < num; i++)
	{
		State *state = m_states[i];
		if (!(state->Dump2(f)))
			return false;
	}
	return true;

}

bool Anim::Read(FILE *f)
{
	unsigned int size;
	fread(&size, sizeof(size), 1, f);
	fread(&m_bDemo, sizeof(m_bDemo), 1, f);
	m_array = new State [size];
	m_frameNums = new int[size];
	for (int i=0; i < size; i++)
	{
		State *state = new State ;
		fread(state, sizeof(State), 1, f);
		RecordState(state);

		m_array[i] = (*state);
		m_frameNums[i] = state->GetFrameNum();
	}

	if (size == 0) assert(false);

	return true;
}

bool Anim::Read2(FILE *f)
{
	unsigned int size;
	fscanf(f, "%d",&size);
	fscanf(f, "%d", &m_bDemo);
	m_array = new State [size];
	m_frameNums = new int[size];
	for (int i=0; i < size; i++)
	{
		State *state = new State;
		state->Read2(f);
		RecordState(state);

		m_array[i] = (*state);
		m_frameNums[i] = state->GetFrameNum();

	}

	return true;
}
