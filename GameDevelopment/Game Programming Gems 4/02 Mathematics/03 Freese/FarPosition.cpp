#include "FarPosition.h"

#define NORMALIZATION   1

float FarPosition::s_segmentSize = 256.0f;


void FarPosition::Normalize()
{
#if NORMALIZATION
    if ( fabsf(m_offset.x) >= s_segmentSize )
    {
        m_segment.x  = m_segment.x + static_cast<int16>(FloatToInt(m_offset.x / s_segmentSize));
        m_offset.x = fmodf(m_offset.x, s_segmentSize);
    }

    if ( fabsf(m_offset.z) >= s_segmentSize )
    {
        m_segment.z  = m_segment.z + static_cast<int16>(FloatToInt(m_offset.z / s_segmentSize));
        m_offset.z = fmodf(m_offset.z, s_segmentSize);
    }
#endif
}


Vector3 FarPosition::GetApproximateVector() const
{
    Vector3 v = m_offset;
    v.x += m_segment.x * s_segmentSize;
    v.z += m_segment.z * s_segmentSize;

    return v;
}


void FarPosition::SetIntegerXZCoords(int32 x, int32 y)
{
    m_segment.x  = static_cast<int16>(FloatToInt(x / s_segmentSize));
    m_offset.x = fmodf(static_cast<float>(x), s_segmentSize);

    m_offset.y = 0.0f;

    m_segment.z  = static_cast<int16>(FloatToInt(y / s_segmentSize));
    m_offset.z = fmodf(static_cast<float>(y), s_segmentSize);
}


FarPosition FarPosition::FindAveragePosition( const FarPosition::VFP &positions )
{
    if ( positions.empty() )
        return FarPosition(Zero);

    FarSegment segment = positions.front().GetSegment();
    Vector3 offset(Zero);
    for ( VFP::const_iterator it = positions.begin(); it != positions.end(); ++it )
    {
        offset += it->GetRelativeVector(segment);
    }

    FarPosition average;
    average.SetSegment(segment);
    average.SetOffset(offset / (float)positions.size());
    return average;
}
