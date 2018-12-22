#include "Main.h"
#include "Primitives.h"

void sVector::GetClosest (sVector &Test, sVector &Result)
{
    // A = Porigin

    Result.Set (x, y, z);
}

void sLineSegment::GetClosest (sVector &Test, sVector &Result)
{
    // D = X - Lorigin
    // d = clamp (D . Ldir, 0, Llength)
    // A = Lorigin + Ldir * d

    float   Dist;
    sVector Dir;

    Dir.x = Test.x - Origin.x;
    Dir.y = Test.y - Origin.y;
    Dir.z = Test.z - Origin.z;

    Dist = Dir.DotProduct (Direction);

    Dist = clampf (Dist, 0, Length);

    Result.x = Origin.x + Direction.x * Dist;
    Result.y = Origin.y + Direction.y * Dist;
    Result.z = Origin.z + Direction.z * Dist;
}

void sLineInfinite::GetClosest (sVector &Test, sVector &Result)
{
    // D = X - Lorigin
    // d = D . Ldir
    // A = Lorigin + Ldir * d

    float   Dist;
    sVector Dir;

    Dir.x = Test.x - Point.x;
    Dir.y = Test.y - Point.y;
    Dir.z = Test.z - Point.z;

    Dist = Dir.DotProduct (Direction);

    Result.x = Point.x + Direction.x * Dist;
    Result.y = Point.y + Direction.y * Dist;
    Result.z = Point.z + Direction.z * Dist;
}

void sSphere::GetClosest (sVector &Test, sVector &Result)
{
    // D = X - O
    // d = min (|D|,Sradius)
    //         ^
    // A = O + Dd

    sVector Dir;

    Dir.x = Test.x - Origin.x;
    Dir.y = Test.y - Origin.y;
    Dir.z = Test.z - Origin.z;

    float Dist = min (Dir.Magnitude (), Radius);

    Dir.Normalize ();

    Result.x = Origin.x + Dir.x * Dist;
    Result.y = Origin.y + Dir.y * Dist;
    Result.z = Origin.z + Dir.z * Dist;
}

void sBox::GetClosest (sVector &Test, sVector &Result)
{
    // Offset Box
    // A = clamp (X, Borigin + Bmin, Borigin + Bmax)

    Result.x = clampf (Test.x, Origin.x + Min.x, Origin.x + Max.x);
    Result.y = clampf (Test.y, Origin.y + Min.y, Origin.y + Max.y);
    Result.z = clampf (Test.z, Origin.z + Min.z, Origin.z + Max.z);
}