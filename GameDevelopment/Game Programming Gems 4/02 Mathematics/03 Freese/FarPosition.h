#ifndef   INCLUDED_FarPosition
#define   INCLUDED_FarPosition
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MathTypes.h"

union FarSegment
{
    struct
    {
        int16   x, z;
    };

    int32       xz;

    FarSegment() {};
    FarSegment( ZeroType ) : xz(0) {};

    bool operator==( const FarSegment& rhs ) const { return xz == rhs.xz; }
    bool operator!=( const FarSegment& rhs ) const { return xz != rhs.xz; }
};


class FarPosition
{
public:
    FarPosition() {};
    FarPosition( ZeroType ) : m_segment(Zero), m_offset(Zero) {};
    FarPosition( FarSegment segment, const Vector3& offset ) : m_segment(segment), m_offset(offset) {};

    ~FarPosition() {};

    void Normalize();

    const FarSegment& GetSegment() const { return m_segment; }
    const Vector3& GetOffset() const { return m_offset; }
    float GetY() const { return m_offset.y; }
    void SetY( float fY ) { m_offset.y = fY; }

    // use of this function is discouraged - it introduces precision approximations
    // math calculations are best done in relative, not absolute, coordinate space
    Vector3 GetApproximateVector() const;

    void SetFromVector( const Vector3& vector )
    {
        m_segment.xz = 0;
        m_offset = vector;
        Normalize();
    }

    Vector3 Subtract( const FarPosition &pos ) const;

    Vector3 GetRelativeVector( const FarSegment& segment ) const;

    void SetSegment( const FarSegment& segment ) { m_segment = segment; }
    void SetOffset( const Vector3& offset ) { m_offset = offset; }

    void Translate( const Vector3& vector )
    {
        m_offset += vector;
        Normalize();
    }

    void SetIntegerXZCoords( int x, int y);

    bool operator==( const FarPosition& rhs ) const 
    { return m_segment == rhs.m_segment && m_offset == rhs.m_offset; }

    bool operator!=( const FarPosition& rhs ) const 
    { return m_segment != rhs.m_segment || m_offset != rhs.m_offset; }

    typedef vector<FarPosition> VFP;
    static FarPosition FindAveragePosition( const VFP &positions );

protected:
    
private:
    // assignment operator okay
    // copy constructor okay
    
    FarSegment      m_segment;
    Vector3         m_offset;

    static float    s_segmentSize;
};


inline Vector3 operator-( const FarPosition &lhs, const FarPosition &rhs )
{
    return lhs.Subtract(rhs);
}


inline Vector3 FarPosition::Subtract( const FarPosition& pos ) const
{
    Vector3 r = m_offset - pos.m_offset;
    r.x += (m_segment.x - pos.m_segment.x) * s_segmentSize; 
    r.z += (m_segment.z - pos.m_segment.z) * s_segmentSize; 
    return r;
}


inline Vector3 FarPosition::GetRelativeVector( const FarSegment& segment ) const
{
    Vector3 r = m_offset;
    r.x += (m_segment.x - segment.x) * s_segmentSize; 
    r.z += (m_segment.z - segment.z) * s_segmentSize; 
    return r;
}


inline const FarPosition Lerp(const FarPosition& Start,
                                    const FarPosition& End,
                                    float frac)
{
    Vector3 dVector = End.Subtract(Start) * frac;
    FarPosition newFarPos = Start;
    newFarPos.Translate(dVector);
    return newFarPos;
}


#endif // INCLUDED_FarPosition

