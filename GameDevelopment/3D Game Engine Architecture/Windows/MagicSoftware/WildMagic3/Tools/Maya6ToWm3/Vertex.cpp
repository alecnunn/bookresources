#include "Vertex.h"
#include "Wm3Math.h"

using namespace Wm3;

//----------------------------------------------------------------------------
Vertex::Vertex ()
{
    for (int i = 0; i < MAXVALUES; i++)
        m_afValue[i] = Mathf::MAX_REAL;
}
//----------------------------------------------------------------------------
Vertex::Vertex (float fPx, float fPy, float fPz,
    float fNx, float fNy, float fNz, float fR, float fG, float fB, float fA,
    float fU0, float fV0, float fU1, float fV1, float fU2, float fV2,
    float fU3, float fV3)
{
    m_afValue[ 0] = fPx;
    m_afValue[ 1] = fPy;
    m_afValue[ 2] = fPz;
    m_afValue[ 3] = fNx;
    m_afValue[ 4] = fNy;
    m_afValue[ 5] = fNz;
    m_afValue[ 6] = fR;
    m_afValue[ 7] = fG;
    m_afValue[ 8] = fB;
    m_afValue[ 9] = fA;
    m_afValue[10] = fU0;
    m_afValue[11] = fV0;
    m_afValue[12] = fU1;
    m_afValue[13] = fV1;
    m_afValue[14] = fU2;
    m_afValue[15] = fV2;
    m_afValue[16] = fU3;
    m_afValue[17] = fV3;
}
//----------------------------------------------------------------------------
Vertex::Vertex (const Vertex& rkV)
{
    memcpy(m_afValue,rkV.m_afValue,MAXVALUES*sizeof(float));
}
//----------------------------------------------------------------------------
Vertex& Vertex::operator= (const Vertex& rkV)
{
    memcpy(m_afValue,rkV.m_afValue,MAXVALUES*sizeof(float));
    return *this;
}
//----------------------------------------------------------------------------
bool Vertex::operator< (const Vertex& rkV) const
{
    for (int i = 0; i < MAXVALUES; i++)
    {
        if ( m_afValue[i] < rkV.m_afValue[i] )
            return true;

        if ( m_afValue[i] > rkV.m_afValue[i] )
            return false;
    }

    return false;
}
//----------------------------------------------------------------------------
bool Vertex::operator== (const Vertex& rkV) const
{
    for (int i = 0; i < MAXVALUES; i++)
    {
        if (m_afValue[i] != rkV.m_afValue[i])
            return false;
    }
    return true;
}
//----------------------------------------------------------------------------