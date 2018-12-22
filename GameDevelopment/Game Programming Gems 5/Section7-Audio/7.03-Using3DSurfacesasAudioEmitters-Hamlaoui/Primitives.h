#ifndef GPG5_HAMLAOUI_Primitives
#define GPG5_HAMLAOUI_Primitives

struct sVector
{
    float x, y, z;

    void GetClosest (sVector &Test, sVector &Result);

    void CrossProduct (sVector &V1, sVector &V2)
    {
	    x = V1.y * V2.z - V1.z * V2.y;
	    y = V1.z * V2.x - V1.x * V2.z;
	    z = V1.x * V2.y - V1.y * V2.x;
    }

    float DotProduct (sVector &V) { return x * V.x + y * V.y + z * V.z; }

    float Magnitude    () { return sqrtf (x * x + y * y + z * z); }
    float MagnitudeSqr () { return        x * x + y * y + z * z;  }

    void Normalize ()
    {
        float m = Magnitude ();

        if (m)
        {
            m = 1 / m;

            x *= m;
            y *= m;
            z *= m;
        }
    }

    void Set (float v)                      { x = v;   y = v;   z = v;   }
    void Set (float Vx, float Vy, float Vz) { x = Vx;  y = Vy;  z = Vz;  }
    void Set (sVector &V)                   { x = V.x; y = V.y; z = V.z; }
};

struct sSphere
{
    sVector Origin;
    float Radius;

    void GetClosest (sVector &Test, sVector &Result);
};

struct sBox
{
    sVector Origin;

    sVector Min;
    sVector Max;

    void GetClosest (sVector &Test, sVector &Result);
};

struct sLineSegment
{
    sVector Origin;
    sVector Direction;
    float   Length;

    void GetClosest (sVector &Test, sVector &Result);
};

struct sLineInfinite
{
    sVector Point;
    sVector Direction;

    void GetClosest (sVector &Test, sVector &Result);
};

#endif  // GPG5_HAMLAOUI_Primitives