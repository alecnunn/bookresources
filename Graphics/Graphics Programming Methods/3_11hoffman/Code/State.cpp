#include "State.h"
extern float g_fSlowFactor;

bool State::Dump(FILE *f)
{
	if (fwrite(this, sizeof(State), 1, f) != 1)
		return false;
	return true;
}

bool State::Dump2(FILE *f)
{
	fprintf(f, "%d ", m_nFrameNum);

	m_Camera.Dump2(f);
	m_Atmosphere.Dump2(f);
	m_Sun.Dump2(f);

	fprintf(f, "\n");

	return true;
}

bool State::Read2(FILE *f)
{
	fscanf(f, "%d", &m_nFrameNum);
	m_nFrameNum *= g_fSlowFactor;

	m_Camera.Read2(f);
	m_Atmosphere.Read2(f);
	m_Sun.Read2(f);


	return true;
}

void State::Finalize()
{
	m_Atmosphere.Finalize();
	m_Camera.Finalize();
	m_Sun.Finalize();
}


State State::operator + ( const State & s2) const
{
	State res;
	res.m_Atmosphere = m_Atmosphere + s2.m_Atmosphere;
	res.m_Camera = m_Camera + s2.m_Camera;
	res.m_Sun = m_Sun + s2.m_Sun;
	return res;
}

State State::operator * ( float f) const
{
	State res;
	res.m_Atmosphere = m_Atmosphere * f;
	res.m_Camera = m_Camera * f;
	res.m_Sun = m_Sun * f;
	return res;

}

State operator * ( float f, const State& s)
{
	State res;
	res.m_Atmosphere = s.m_Atmosphere * f;
	res.m_Camera = s.m_Camera * f;
	res.m_Sun = s.m_Sun * f;

	return res;

}
